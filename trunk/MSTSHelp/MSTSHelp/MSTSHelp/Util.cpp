#include "stdafx.h"
#include "Util.h"
#include "Logger.h"

void PressKey(byte c)
{
	keybd_event(c, MapVirtualKey(c, 0), 0, 0);
	Sleep(50);
	keybd_event(c, MapVirtualKey(c, 0), KEYEVENTF_KEYUP, 0);
}
void PressKeyToTrainWnd(byte c)
{
	HWND hWnd = GetForegroundWindow();
	HWND hWnd2 = FindWindow(L"TrainSim", NULL);

	if (hWnd == hWnd2)
	{
		PressKey(c);
	}
}
void *GetTrainPointer(HANDLE hProcess)
{
	void *pointer;

	if (!ReadTrainProcess(hProcess, (void *)THIS_POINTER_MEM, &pointer, 4))
	{
		CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
		return NULL;
	}

	return pointer;
}

bool IsPowered(HANDLE hProcess)
{
	float fPowered;

	if (!ReadTrainProcess(hProcess, (void *)IS_POWERED_MEM, (LPVOID)&fPowered, 4))
	{
		CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
		return false;
	}

	return fPowered != 0;
}

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

		if (!ReadTrainProcess(hProcess, addressPointer, (LPVOID)&dataPointer, 4))
		{
			CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
			return false;
		}

		addressPointer = dataPointer + y;
	}

	va_end(x);

	if (!ReadTrainProcess(hProcess, addressPointer, lpBuffer, nSize))
	{
		CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
		return false;
	}

	return true;
}

bool WritePointerMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, int num, ...)
{
	LPVOID addressPointer = lpBaseAddress;
	char *dataPointer;
	va_list x;
	va_start(x, num);

	for (int i = 0; i < num; ++i)
	{
		int y = va_arg(x, int);

		if (!ReadTrainProcess(hProcess, addressPointer, (LPVOID)&dataPointer, 4))
		{
			CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
			return false;
		}

		addressPointer = dataPointer + y;
	}

	va_end(x);

	if (!WriteProcessMemory(hProcess, addressPointer, lpBuffer, nSize, NULL))
	{
		CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
		return false;
	}

	return true;
}

Direction GetDirection(Locomotive loco, HANDLE hProcess)
{
	float fDirection;
	SIZE_T directionMem;

	switch (loco)
	{
	case Steam:
	{
		//*(*(*(*(0x829A04)+6C8)+1E4)+4)+1C
		float direction;

		if (!ReadPointerMemory(hProcess, (void *)0x829A04, (LPVOID)&direction, 4, 4, 0x6C8, 0x1E4, 0x4, 0x1C))
		{
			return Middle;
		}

		if (direction > 0.01)
			return Forward;
		else if (direction > -0.01)
			return Middle;
		else
			return Backward;
	}
	break;
	case Diesel:
		directionMem = DIRECTION_DIESEL_MEM;
		break;
	case Electric:
		directionMem = DIRECTION_ELECTRIC_MEM;
		break;
	default:
		throw new int;
		break;
	}

	if (!ReadTrainProcess(hProcess, (void *)directionMem, (LPVOID)&fDirection, 4))
	{
		CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
		return InValid;
	}

	if (fDirection == 1)
	{
		return Forward;
	}
	else if (fDirection == 0)
	{
		return Middle;
	}
	else if (fDirection == -1)
	{
		return Backward;
	}

	return InValid;
}

//列车的类型
//(((this+106)+666)+136)
Locomotive GetLocomotive(HANDLE hProcess)
{
	byte result;

	if (!ReadPointerMemory(hProcess, (void *)THIS_POINTER_MEM, (LPVOID)&result, 1, 3, 0x6A, 0x29A, 0x88))
	{
		return WrongLoco;
	}

	switch (result)
	{
	case 1:
		return Steam;
	case 2:
		return Electric;
	case 3:
		return Diesel;
	case 4:
		CLogger::Log("Locomotive type is %d", result);
		return WrongLoco;
	default:
		CLogger::Log("Locomotive type is %d", result);
		return WrongLoco;
	}
}