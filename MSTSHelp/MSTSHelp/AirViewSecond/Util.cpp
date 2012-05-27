#include "stdafx.h"
#include "Util.h"
#include <cmath>
vector<CString> g_colorToStringVect;
vector<CString> g_ESignalTypeToStringVect;
vector<CString> g_stationTypeToStringVect;
CString getTrackSectionString(const SSectionInfo &info)
{
	CString temp, msg;

	if (info.nDirection == 0)
	{
		msg += L"直轨道";
	}
	else if (info.nDirection == 1)
	{
		msg += L"右转 半径为";
		temp.Format(L"%.1f", info.fRadius);
		msg += temp;
	}
	else
	{
		msg += L"左转 半径为";
		temp.Format(L"%.1f", info.fRadius);
		msg += temp;
	}

	temp.Format(L" 坡度 %.5f", info.fAngle * 180 / 3.1415926f);
	msg += temp;
	msg += L"\r\n";
	return msg;
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


void *GetTrainPointer(HANDLE hProcess)
{
	void *pointer;

	if (!ReadProcessMemory(hProcess, (void *)THIS_POINTER_MEM, &pointer, 4, NULL))
	{
		return NULL;
	}

	return pointer;
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
		ReadTrainProcess(hProcess, addressPointer, (LPVOID)&dataPointer, 4);
		addressPointer = dataPointer + y;
	}

	va_end(x);
	ReadTrainProcess(hProcess, addressPointer, lpBuffer, nSize);
	return true;
}

void AddSectionInfo(float currentDistance, const SVectorNode &node, vector<SSectionInfo>& sectionVect, HANDLE handle, int nDirection, int nDirectionOfItemToFind, STrackSection *pSection)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	int start = nDirection ? 0 : num - 1;
	int end = nDirection ? num : -1;
	int delta = nDirection ? 1 : -1;
	float fCurrentLength = 0;

	for (int i = start; i != end; i += delta)
	{
		STrackSection *pCurSection = pSection + pSectionData[i].sectionIndex;
		STrackSection curSection;
		ReadTrainProcess(handle, pCurSection, &curSection, sizeof(STrackSection));
		float fRaidus = curSection.fSectionCurveFirstRadius4;
		int nAngle = -1;

		if (curSection.fSectionCurveSecondAngle8 == 0)
			nAngle = 0;
		else if ((curSection.fSectionCurveSecondAngle8 > 0) ^ nDirectionOfItemToFind)
			nAngle = 1;

		float fDis = currentDistance + fCurrentLength;
		fDis += (nDirection != nDirectionOfItemToFind) * curSection.fSectionSizeSecondLength0;
		float fHeightAngle = nDirectionOfItemToFind ? -pSectionData[i].AX : pSectionData[i].AX;

		if (fDis > 0)
			sectionVect.push_back(SSectionInfo(currentDistance + fCurrentLength, currentDistance + fCurrentLength + curSection.fSectionSizeSecondLength0, nAngle, fRaidus, fHeightAngle));

		fCurrentLength += curSection.fSectionSizeSecondLength0;
	}

	delete[] pSectionData;
}
SVectorNode *GetNext(SVectorNode *nodePtr, const SConnectStruct &connectStruct, const SConnectNode &connectNode,
                     int &nextDirect)
{
	if (connectNode.nType0 == 2) //JunctionNode
	{
		if (nodePtr == connectStruct.subStruct[0].nodePtr)
		{
			nextDirect = connectStruct.subStruct[1 + connectNode.direction80].nDirect;
			return connectStruct.subStruct[1 + connectNode.direction80].nodePtr;
		}
		else if (nodePtr == connectStruct.subStruct[1].nodePtr)
		{
			nextDirect = connectStruct.subStruct[0].nDirect;
			return connectStruct.subStruct[0].nodePtr;
		}
		else if (nodePtr == connectStruct.subStruct[2].nodePtr)
		{
			nextDirect = connectStruct.subStruct[0].nDirect;
			return connectStruct.subStruct[0].nodePtr;
		}
	}

	//EndNode Or can not find the pointer in the struct.
	return NULL;
}

SVectorNode *GetNextNode(HANDLE handle, const SVectorNode &node, SVectorNode *nodePtr, int direction, int &nextDirect)
{
	SConnectNode connectNode;
	SConnectStruct connectStruct;
	SVectorNode *next;
	SConnectNode *connectNodePtr = direction ? node.OutConnectNodePtr : node.InConnectNodePtr;
	ReadTrainProcess(handle, (void *)connectNodePtr, (LPVOID)&connectNode, sizeof(SConnectNode));
	ReadTrainProcess(handle, (void *)connectNode.nodePointer20, (LPVOID)&connectStruct, sizeof(SConnectStruct));
	next = GetNext(nodePtr, connectStruct, connectNode, nextDirect);
	return next;
}
