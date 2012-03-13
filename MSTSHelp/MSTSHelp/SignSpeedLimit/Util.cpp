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
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		for(int i = 0; i < num; ++i)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);
			if(type == SpeedPostItem)
			{
				SSpeedPostItem speedPostItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&speedPostItem, sizeof(SSpeedPostItem));
				short subType = speedPostItem.SpeedpostTrItemDataFirst;
				if((subType & 7) == 2)
				{
					size_t pointer;
					ReadTrainProcess(handle, (LPCVOID)THIS_POINTER_MEM, &pointer, 4);
					pointer += 230;
					size_t memory;
					ReadTrainProcess(handle, (LPCVOID)pointer, &memory, 4);
					pointer = memory;
					ReadTrainProcess(handle, (LPCVOID)pointer, &memory, 4);
					if((subType & 0x80) || subType & 0x20 && memory & 2 || subType & 0x40 && memory & 4)
					{
						if(IsSpeedPostValid(handle, speedPostItem.fAngle, speedPostItem.fLocationInTrackNode, !direction, node))
						{
							float distanceToTrackStart;
							if(!direction)
							{
								distanceToTrackStart = node.fSectionLength - speedPostItem.fLocationInTrackNode;
							}else
							{
								distanceToTrackStart = speedPostItem.fLocationInTrackNode;
							}
							float speed = speedPostItem.SpeedpostTrItemDataSecond;
							if(subType & 0x100)
								speed *= 1.609f;
							if(currentDistance + distanceToTrackStart > 0)
								limitVect.push_back(SSpeedPostLimit(currentDistance + 
								distanceToTrackStart, speed));
						}
					}
				}
			}
		}
		delete[]memory;
	}
}

void AddTempSpeedLimit(float currentDistance, STrackNode* nodePtr, vector<STempSpeedLimit>& limitVect, HANDLE handle, int direction)
{
	void* ite, *head;
	STrackNode node;
	ReadTrainProcess(handle, (void *)nodePtr, (LPVOID)&node, sizeof(STrackNode));
	ReadTrainProcess(handle, (LPCVOID)TASK_LIMIT_HEAD_MEM, &head, 4);
	ReadTrainProcess(handle, head, &ite, 4);
	while(ite != head)
	{
		void* data;
		ReadTrainProcess(handle, (DWORD*)ite + 2, &data, 4);
		STempSpeed speed;
		ReadTrainProcess(handle, (char*)data + 32, &speed, sizeof(STempSpeed));
		if(nodePtr == speed.nodePtr)
		{
			float fBegin, fEnd;
			if(!direction)
			{
				fBegin = node.fSectionLength - speed.fStart;
				fEnd = node.fSectionLength - speed.fEnd;
			}else
			{
				fBegin = speed.fStart;
				fEnd = speed.fEnd;
			}
			if(fBegin < fEnd && fEnd + currentDistance > 0)
			{
				limitVect.push_back(STempSpeedLimit(fBegin + currentDistance, fEnd + currentDistance));
			}
		}
		void* next;
		ReadTrainProcess(handle, ite, &next, 4);
		ite = next;
	}
}

void AddStationItem(float currentDistance, const STrackNode& node, vector<SStationItem>& stationVect, vector<SStationItem>& sidingVect, HANDLE handle, int direction)
{
	int num = node.nTrItemNum;
	if(num > 0)
	{
		size_t* memory = new size_t[num];
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		for(int i = 0; i < num; ++i)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);
			if(type == PlatFormItem)
			{
				SPlatformItem platformItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&platformItem, sizeof(SPlatformItem));
				wchar_t stationName[0x400];
				ReadTrainProcess(handle, platformItem.platformName, (LPVOID)stationName, 0x800);
				float distanceToTrackStart;
				if(!direction)
				{
					distanceToTrackStart = node.fSectionLength - platformItem.fLocationInTrackNode;
				}else
				{
					distanceToTrackStart = platformItem.fLocationInTrackNode;
				}
				if(distanceToTrackStart + currentDistance > 0)
					stationVect.push_back(SStationItem(distanceToTrackStart + currentDistance, stationName));
			}else if(type == SidingItem)
			{
				SSidingItem sidingItem;
				const void* address = (LPCVOID)*(memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&sidingItem, sizeof(SSidingItem));
				wchar_t stationName[0x400];
				ReadTrainProcess(handle, sidingItem.sidingName, (LPVOID)stationName, 0x800);
				float distanceToTrackStart;
				if(!direction)
				{
					distanceToTrackStart = node.fSectionLength - sidingItem.fLocationInTrackNode;
				}else
				{
					distanceToTrackStart = sidingItem.fLocationInTrackNode;
				}
				if(distanceToTrackStart + currentDistance > 0)
					sidingVect.push_back(SStationItem(distanceToTrackStart + currentDistance, stationName));
			}
		}
		delete[]memory;
	}
}


STrackNode* GetNext(STrackNode* nodePtr, const SConnectStruct& connectStruct, const SConnectNode& connectNode, 
					int direction, int&nextDirect)
{
	if(connectNode.nType == 2)//JunctionNode
	{
		if(nodePtr == connectStruct.nodePtr1 && connectStruct.nDirect1 == direction)
		{
			if(connectNode.direction2 )
			{
				nextDirect = connectStruct.nDirect3;
				return connectStruct.nodePtr3;
			}else
			{
				nextDirect = connectStruct.nDirect2;
				return connectStruct.nodePtr2;
			}
		}else if(nodePtr == connectStruct.nodePtr2 && connectStruct.nDirect2 == direction){
			nextDirect = connectStruct.nDirect1;
			return connectStruct.nodePtr1;
		}else if(nodePtr == connectStruct.nodePtr3 && connectStruct.nDirect3 == direction){
			nextDirect = connectStruct.nDirect1;
			return connectStruct.nodePtr1;
		}
	}
	//EndNode Or can not find the pointer in the struct.
	return NULL;
}

STrackNode* GetNextNode(HANDLE handle, const STrackNode& node, STrackNode* nodePtr, int direction, int&nextDirect)
{
	SConnectNode connectNode;
	SConnectStruct connectStruct;
	STrackNode*next;
	ReadTrainProcess(handle, (void *)node.connectNodePtr1, (LPVOID)&connectNode, sizeof(SConnectNode));
	ReadTrainProcess(handle, (void *)connectNode.nodePointer, (LPVOID)&connectStruct, sizeof(SConnectStruct));
	next = GetNext(nodePtr, connectStruct, connectNode, direction, nextDirect);
	if(next)
		return next;
	ReadTrainProcess(handle, (void *)node.connectNodePtr2, (LPVOID)&connectNode, sizeof(SConnectNode));
	ReadTrainProcess(handle, (void *)connectNode.nodePointer, (LPVOID)&connectStruct, sizeof(SConnectStruct));
	next = GetNext(nodePtr, connectStruct, connectNode, direction, nextDirect);
	return next;
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
		item.SpeedpostTrItemDataThird, item.fAngle, item.fData);
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
	ReadTrainProcess(handle, (LPCVOID)IDENTITY_MATRIX_MEM, (LPVOID)fArray, 0x24);
	process_AY(fArray, fXYZ[1]);
	process_AX(fArray, fXYZ[0]);
	process_AZ(fArray, fXYZ[2]);
	return fArray;
}
void getSectionData(HANDLE handle, SProcessData& processData, const STrackNode& node, int sectionNum, float* fArray)
{
	float fDistance = 0;
	size_t mem, basePtr;
	processData.nodePtr0 = NULL;
	processData.nSectionNum4 = sectionNum;
	processData.sectionPtr8 = node.sectionArrayPtr + sectionNum;
	processData.nData12 = 1;
	processData.fData20 = 0;
	int sNum = sectionNum;
	ReadTrainProcess(handle, (LPCVOID)0x80A118, (LPVOID)&mem, 0x4);
	mem += 12; 
	ReadTrainProcess(handle, (LPCVOID)mem, (LPVOID)&basePtr, 0x4);
	while (sectionNum)
	{
		--sectionNum;
		const SSectionData* sectionPtr = node.sectionArrayPtr + sectionNum;
		short num;
		ReadTrainProcess(handle, (LPCVOID)sectionPtr, (LPVOID)&num, 0x2);
		size_t subPtr = basePtr;
		subPtr += 24 * num;
		float fNum;
		ReadTrainProcess(handle, (LPCVOID)subPtr, (LPVOID)&fNum, 0x4);
		fDistance += fNum;
	}
	processData.fDistance16 = fDistance;
	SSectionData* sectionPtr = processData.sectionPtr8;
	SSectionData sectionData;
	ReadTrainProcess(handle, (LPCVOID)sectionPtr, (LPVOID)&sectionData, sizeof(SSectionData));
	int dword_79D118, dword_79D11C;
	ReadTrainProcess(handle, (LPCVOID)0x79D118, (LPVOID)&dword_79D118, 4);
	ReadTrainProcess(handle, (LPCVOID)0x79D11C, (LPVOID)&dword_79D11C, 4);
	processData.fXYZ72[0] = (sectionData.TileX2 - dword_79D118) * 2048.0f + sectionData.X;
	processData.fXYZ72[2] = (sectionData.TileZ2 - dword_79D11C) * 2048.0f + sectionData.Z;
	processData.fXYZ72[1] = sectionData.Y;
	processData.fAXYZ24[0] = sectionData.AX;
	processData.fAXYZ24[1] = sectionData.AY;
	processData.fAXYZ24[2] = sectionData.AZ;
	processData.nData88 = 0;
	processData.nData92 = 0;
	processData.fData100 = -1.0;
	process(handle, processData.fMatrix, processData.fAXYZ24);
	if(fArray)
	{
		if(inner_product(fArray, processData.fMatrix + 6))
		{
			processData.nData12 = processData.nData12 != 1;
			processData.nData92 = -processData.nData92;
			processData.fAXYZ24[1] += 3.14159f;
			process(handle, processData.fMatrix, processData.fAXYZ24);
		}
	}
}
/*int functionName(HANDLE handle, SProcessData& processData, const STrackNode& node, float fLocation)
{
	int result;
	int nTemp = 0;
	if(processData.nData12)
	{
		processData.fDistance16 += fLocation;
		processData.fData20 += fLocation;
	}else
	{
		processData.fDistance16 -= fLocation;
		processData.fData20 -= fLocation;
	}
	if(processData.fDistance16 < 0)
	{
		STrItem** itemPtr = node.trItemArrayPtr;
		float fDistance = processData.fDistance16;
		int nFlag = processData.nData12;
		if(someFunction(handle, processData, itemPtr, 0))
		{
			if(processData.nData12 !=1 )
			{
				if(processData.nData12 == nFlag)
				{
					fDistance = -fDistance;
				}
			}else if(processData.nData12 != nFlag)
				fDistance = -fDistance;
			result = functionName(handle, processData, node, fDistance);
		}else
		{
			processData.fDistance16 = 0;
			processData.fData20 = 0;
			processData.nSectionNum4 = 0;
			processData.sectionPtr8 = node.sectionArrayPtr;
		}
	}
	return result;
}*/
bool IsSpeedPostValid(HANDLE handle, float angle, float fLocationInTrackNode, int nDirection, const STrackNode& node)
{
	SProcessData processData;
	float fDirection[3];
	fDirection[0] = cos(angle);
	fDirection[1] = 0;
	fDirection[2] = sin(angle);
	getSectionData(handle, processData, node, 0, 0);
	if(nDirection)
	{
		processData.nData12 = processData.nData12 != 1;
		processData.nData92 = -processData.nData92;
		processData.fAXYZ24[1] += 3.14159f;
		process(handle, processData.fMatrix, processData.fAXYZ24);
	}
	float* fArray = processData.fMatrix + 6;
	float result = inner_product(fDirection, fArray);
	return result > 0;
}
CString IteratorList(HANDLE handle, void* headPtr, CString (*func)(HANDLE, void*))
{
	//head 0x809B38 temp speed limit
	void* ite, *head;
	CString strResult = L"Iterator Result:\r\n";
	ReadTrainProcess(handle, headPtr, &head, 4);
	ReadTrainProcess(handle, head, &ite, 4);
	while(ite != head)
	{
		void* data;
		ReadTrainProcess(handle, (DWORD*)ite + 2, &data, 4);
		strResult += func(handle, data);
		strResult += "\r\n";
		void* next;
		ReadTrainProcess(handle, ite, &next, 4);
		ite = next;
	}
	return strResult;
}
/*int someFunction(HANDLE handle, SProcessData& processData, STrItem**itemPtr, int num)
{
	return 0;
}*/