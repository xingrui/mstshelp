#include "stdafx.h"
#include "Util.h"

bool GetTrainHandle(HANDLE &hProcess)
{
	if (hProcess)
	{
		DWORD dwState;

		if (GetExitCodeProcess(hProcess, &dwState))
		{
			if (dwState == STILL_ACTIVE)
				return true;
			else
			{
				CloseHandle(hProcess);
				hProcess  = NULL;
			}
		}
		else
		{
			CloseHandle(hProcess);
			hProcess  = NULL;
		}
	}

	HWND hWnd = FindWindow(L"TrainSim", NULL);

	if (hWnd)
	{
		DWORD nProcID;
		GetWindowThreadProcessId(hWnd, &nProcID);
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, nProcID);
		return hProcess != NULL;
	}
	else
	{
		return false;
	}
}

bool ReadPointerMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, int num, ...)
{
	LPCVOID addressPointer = lpBaseAddress;
	char *dataPointer;
	va_list x;
	va_start(x, num);

	for (int i = 0; i < num; ++i)
	{
		int y = va_arg(x, int);

		if (!ReadProcessMemory(hProcess, addressPointer, (LPVOID)&dataPointer, 4, NULL))
		{
			return false;
		}

		addressPointer = dataPointer + y;
	}

	va_end(x);

	if (!ReadProcessMemory(hProcess, addressPointer, lpBuffer, nSize, NULL))
	{
		return false;
	}

	return true;
}

void *GetTrainPointer(HANDLE hProcess)
{
	void *pointer;

	if (!ReadProcessMemory(hProcess, (void *)THIS_POINTER_MEM, &pointer, 4, NULL))
	{
		return NULL;
	}

	return pointer;
}

void AddSpeedPostLimit(float currentDistance, const STrackNode& node, vector<SSpeedPostLimit>& limitVect, HANDLE handle, int direction)
{
	int num = node.nTrItemNum;
	if(num > 0)
	{
		size_t* memory = new size_t[num];
		ReadProcessMemory(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4, NULL);
		for(int i = 0; i < num; ++i)
		{
			int type;
			ReadProcessMemory(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4, NULL);
			if(type == 8)
			{
				SSpeedPostItem speedPostItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadProcessMemory(handle, address, (LPVOID)&speedPostItem, sizeof(SSpeedPostItem), NULL);
				int masked = speedPostItem.SpeedpostTrItemDataFirst & 0xf;
				if(masked == 1 || masked == 2 || masked == 8)
				{
					float distanceToTrackStart;
					if(!direction)
					{
						distanceToTrackStart = node.fSectionLength - speedPostItem.TrItemSDataFirst;
					}else
					{
						distanceToTrackStart = speedPostItem.TrItemSDataFirst;
					}
					if(currentDistance + distanceToTrackStart > 0)
					limitVect.push_back(SSpeedPostLimit(currentDistance + 
						distanceToTrackStart, speedPostItem.SpeedpostTrItemDataSecond));
				}
			}
		}
		delete[]memory;
	}
}

void AddStationItem(float currentDistance, const STrackNode& node, vector<SStationItem>& limitVect, HANDLE handle, int direction)
{
	int num = node.nTrItemNum;
	if(num > 0)
	{
		size_t* memory = new size_t[num];
		ReadProcessMemory(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4, NULL);
		for(int i = 0; i < num; ++i)
		{
			int type;
			ReadProcessMemory(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4, NULL);
			if(type == 3)
			{
				SPlatformItem platformItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadProcessMemory(handle, address, (LPVOID)&platformItem, sizeof(SPlatformItem), NULL);
				wchar_t stationName[0x400];
				ReadProcessMemory(handle, platformItem.platformName, (LPVOID)stationName, 0x800, NULL);
				float distanceToTrackStart;
				if(!direction)
				{
					distanceToTrackStart = node.fSectionLength - platformItem.fTrItemSDataFirst;
				}else
				{
					distanceToTrackStart = platformItem.fTrItemSDataFirst;
				}
				if(distanceToTrackStart + currentDistance > 0)
					limitVect.push_back(SStationItem(distanceToTrackStart + currentDistance, stationName));
			}
		}
		delete[]memory;
	}
}

CString SpeedPostItemToString(const SSpeedPostItem& item)
{
	CString result;
	CString tmp;
	tmp.Format(L"%d %d %d\r\n", item.type, item.subType, item.unknown3);
	result += tmp;
	tmp.Format(L"%f %d\r\n", item.TrItemSDataFirst, item.TrItemSDataSecond);
	result += tmp;
	tmp.Format(L"%f %f %d %d\r\n", item.TrItemPDataFirst, item.TrItemPDataSecond, item.TrItemPDataThird,item.TrItemPDataFourth);
	result += tmp;
	tmp.Format(L"0x%X 0x%04X %d %f %f %X", item.variableData, item.SpeedpostTrItemDataFirst, item.SpeedpostTrItemDataSecond,
		item.SpeedpostTrItemDataThird, item.SpeedpostTrItemDataFourth, item.fData);
	result += tmp;
	return result;
}