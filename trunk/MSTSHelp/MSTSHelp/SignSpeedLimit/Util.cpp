#include "stdafx.h"
#include "Util.h"
#include <cmath>
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
			if(type == SpeedPostItem)
			{
				SSpeedPostItem speedPostItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadProcessMemory(handle, address, (LPVOID)&speedPostItem, sizeof(SSpeedPostItem), NULL);
				int masked = speedPostItem.SpeedpostTrItemDataFirst & 0x7;
				if(masked == 2)
				{
					float distanceToTrackStart;
					if(!direction)
					{
						distanceToTrackStart = node.fSectionLength - speedPostItem.fLocationInTrackNode;
					}else
					{
						distanceToTrackStart = speedPostItem.fLocationInTrackNode;
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
			if(type == PlatFormItem)
			{
				SPlatformItem platformItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadProcessMemory(handle, address, (LPVOID)&platformItem, sizeof(SPlatformItem), NULL);
				wchar_t stationName[0x400];
				ReadProcessMemory(handle, platformItem.platformName, (LPVOID)stationName, 0x800, NULL);
				float distanceToTrackStart;
				if(!direction)
				{
					distanceToTrackStart = node.fSectionLength - platformItem.fLocationInTrackNode;
				}else
				{
					distanceToTrackStart = platformItem.fLocationInTrackNode;
				}
				if(distanceToTrackStart + currentDistance > 0)
					limitVect.push_back(SStationItem(distanceToTrackStart + currentDistance, stationName));
			}else if(type == SidingItem)
			{
				SSidingItem sidingItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadProcessMemory(handle, address, (LPVOID)&sidingItem, sizeof(SSidingItem), NULL);
				wchar_t stationName[0x400];
				ReadProcessMemory(handle, sidingItem.sidingName, (LPVOID)stationName, 0x800, NULL);
				float distanceToTrackStart;
				if(!direction)
				{
					distanceToTrackStart = node.fSectionLength - sidingItem.fLocationInTrackNode;
				}else
				{
					distanceToTrackStart = sidingItem.fLocationInTrackNode;
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
	tmp.Format(L"%d %d %d\r\n", item.nType, item.nSubType, item.unknown3);
	result += tmp;
	tmp.Format(L"%f %d\r\n", item.fLocationInTrackNode, item.TrItemSDataSecond);
	result += tmp;
	tmp.Format(L"%f %f %d %d\r\n", item.TrItemPDataFirst, item.TrItemPDataSecond, item.TrItemPDataThird,item.TrItemPDataFourth);
	result += tmp;
	tmp.Format(L"0x%X 0x%04X %d %f %f %X", item.variableData, item.SpeedpostTrItemDataFirst, item.SpeedpostTrItemDataSecond,
		item.SpeedpostTrItemDataThird, item.SpeedpostTrItemDataFourth, item.fData);
	result += tmp;
	return result;
}

void process_AX(float* fArray, float AX)
{
	float fTemp;
	float sinValue = sin(AX);
	float cosValue = cos(AX);
	for(int i = 0; i < 3; ++i)
	{
		fTemp = cosValue * fArray[3 + i] + sinValue * fArray[6 + i];
		fArray[6 + i] = cosValue * fArray[6 + i] - sinValue * fArray[3 + i];
		fArray[3 + i] = fTemp;
	}
}
void process_AY(float* fArray, float AY)
{
	float fTemp;
	float sinValue = sin(AY);
	float cosValue = cos(AY);
	for(int i = 0; i < 3; ++i)
	{
		fTemp = cosValue * fArray[i] - sinValue * fArray[6 + i];
		fArray[6+i] = sinValue * fArray[i] + cosValue * fArray[6 + i];
		fArray[i] = fTemp;
	}
}
void process_AZ(float* fArray, float AZ)
{
	float fTemp;
	float sinValue = sin(AZ);
	float cosValue = cos(AZ);
	for(int i = 0; i < 3; ++i)
	{
		fTemp = cosValue * fArray[i] + sinValue * fArray[3 + i];
		fArray[3+i] = cosValue * fArray[3 + i] - sinValue * fArray[i];
		fArray[i] = fTemp;
	}
}

float* process(HANDLE handle, float* fArray, float*fXYZ)
{
	ReadProcessMemory(handle, (LPCVOID)0x771680, (LPVOID)fArray, 0x24, NULL);
	process_AY(fArray, fXYZ[1]);
	process_AX(fArray, fXYZ[0]);
	process_AZ(fArray, fXYZ[2]);
	return fArray;
}
void getXYZ(HANDLE handle, float* fArray, const STrackNode& node, int sectionNum, int nDirection)
{
	float fDistance = 0;
	size_t mem, basePtr;
	int sNum = sectionNum;
	ReadProcessMemory(handle, (LPCVOID)0x80A118, (LPVOID)&mem, 0x4, NULL);
	mem += 12; 
	ReadProcessMemory(handle, (LPCVOID)mem, (LPVOID)&basePtr, 0x4, NULL);
	while (sectionNum)
	{
		--sectionNum;
		const SSectionData* sectionPtr = node.sectionArrayPtr + sectionNum;
		short num;
		ReadProcessMemory(handle, (LPCVOID)sectionPtr, (LPVOID)&num, 0x2, NULL);
		size_t subPtr = basePtr;
		subPtr += 24 * num;
		float fNum;
		ReadProcessMemory(handle, (LPCVOID)subPtr, (LPVOID)&fNum, 0x2, NULL);
		fDistance += fNum;
	}
	SSectionData* sectionPtr = node.sectionArrayPtr + sNum;
	SSectionData sectionData;
	ReadProcessMemory(handle, (LPCVOID)sectionPtr, (LPVOID)&sectionData, sizeof(SSectionData), NULL);
	fArray[6] = sectionData.AX;
	fArray[7] = sectionData.AY;
	fArray[8] = sectionData.AZ;
	process(handle, fArray + 9, fArray + 6);
}
bool IsSpeedPostValid(HANDLE handle, float angle, int nDirection, const STrackNode& node)
{
	float tempArray[0x68];
	float fDirection[3];
	fDirection[0] = cos(angle);
	fDirection[1] = 0;
	fDirection[2] = sin(angle);
	getXYZ(handle, tempArray, node, 0, 0);
	if(nDirection)
	{
		tempArray[7] += 3.14159f;
		getXYZ(handle, tempArray, node, 0, 0);
	}
	float* fArray = tempArray + 60;
	float result = fDirection[0] * fArray[0] + fDirection[1] * fArray[1] + fDirection[2] * fArray[2];
	return result > 0;
}