#ifndef UTIL_H
#define UTIL_H
#define PAUSE_BY_MSG_MEM		0x783A98
#define VIEW_MODE_MEM			0x799E3C
#define CONNECT_DIS_MEM			0x79C798
#define POWER_INFO_MEM			0x7B6440
#define THIS_POINTER_MEM		0x7C2AC0
#define CURRENT_SCHEDULE_MEM	0x8098C0
#define FORWARD_TURNOFF_MEM		0x8098EC
#define BREAK_INFO_MEM			0x8099BC
#define CUR_SPEED_MEM			0x8099C8
#define ACCER_MEM				0x8099D0
#define LIGHT_COLOR_MEM_2		0x809A54
#define SPEED_LIMIT_MEM			0x809AB0
#define LIGHT_COLOR_MEM			0x809AC0
#define SIG_DISTANT_MEM			0x809AD4
#define FOWARD_LIMIT_MEM		0x809ADC
#define F_GAME_TIME_MEM			0x809B08
#define ROUTE_SPEED_LIMIT_MEM	0x809B48
#define FOWARD_STATION_DIS_MEM	0x809B70
#define TR_ITEM_ARRAY_MEM		0x80A038
#define DIRECTION_DIESEL_MEM	0x80A56C
#define DIRECTION_ELECTRIC_MEM	0x80A628
#define IS_POWERED_MEM			0x80A640
#define S_GAME_TIME_MEM			0x80ACC4

#define AIR_BREAK_RELEASE VK_OEM_1
#define AIR_BREAK_APPLY VK_OEM_7

//#define IS_TASK_MODE_BIT 0x80979B
#include <string>
enum Locomotive
{
	Steam = 0,
	Diesel = 1,
	Electric = 2,
	WrongLoco = 3,
};

enum Direction
{
	Forward = 0,
	Middle = 1,
	Backward = 2,
	InValid = 3,
};

struct SGameTime
{
	int m_nSecond;
	int m_nMinute;
	int m_nHour;
};

struct SSchedule
{
	int m_nPlatformStartID;
	float m_fArrivalTime;
	float m_fActualArrivalTime;
	float m_fDepartTime;
	float m_fActualDepartTime;
	float m_fDistanceDownPath;
	float m_fUnknownData;
	DWORD m_nSkipCount;
	DWORD data6;
	float m_fEfficiency;
	SSchedule(): m_fDepartTime(-1) {}
	CString toString()const
	{
		CString str;
		str.Format(L"%d\t%1.1f\t%1.1f\t%1.1f\t%1.1f\t%1.0f\t%1.1f\t%x\t%d\t%0.4f", m_nPlatformStartID, m_fArrivalTime, m_fActualArrivalTime, m_fDepartTime, m_fActualDepartTime, m_fDistanceDownPath,
		           m_fUnknownData, m_nSkipCount, data6, m_fEfficiency);
		return str;
	}
	std::string toSTLString()const
	{
		if (m_fDepartTime < 0)
		{
			return "Invalid Schedule";
		}

		char cArray[200];
		sprintf_s(cArray, 200, "%d\t%1.1f\t%1.1f\t%1.1f\t%1.1f\t%1.0f\t%1.1f\t%x\t%d\t%0.4f", m_nPlatformStartID, m_fArrivalTime, m_fActualArrivalTime, m_fDepartTime, m_fActualDepartTime, m_fDistanceDownPath,
		          m_fUnknownData, m_nSkipCount, data6, m_fEfficiency);
		return std::string(cArray);
	}
};

struct SList
{
	SList *m_next;
	SList *m_pre;
	void *m_data;
};

struct SHead
{
	SList *head;
	SSchedule *currentSchedule;
};
inline BOOL ReadTrainProcess(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	if(!ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, NULL))
		throw 1;
	return TRUE;
}
bool ReadPointerMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, int num, ...);
bool WritePointerMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, int num, ...);
bool GetTrainHandle(HANDLE &hProcess);
Direction GetDirection(Locomotive loco, HANDLE hProcess);

//列车的类型
//(((this+106)+666)+136)
Locomotive GetLocomotive(HANDLE hProcess);
void *GetTrainPointer(HANDLE hProcess);
bool IsPowered(HANDLE hProcess);
void PressKey(byte c);
void PressKeyToTrainWnd(byte c);

#endif