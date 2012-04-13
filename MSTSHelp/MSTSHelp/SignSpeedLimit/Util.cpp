#include "stdafx.h"
#include "Util.h"
#include <cmath>
vector<CString> g_colorToStringVect;
vector<CString> g_ESignalTypeToStringVect;
class Global
{
public:
	Global()
	{
		g_colorToStringVect.clear();
		g_colorToStringVect.push_back(L"红灯");
		g_colorToStringVect.push_back(L"不能识别的车灯");
		g_colorToStringVect.push_back(L"红黄灯");
		g_colorToStringVect.push_back(L"双黄灯");
		g_colorToStringVect.push_back(L"黄2灯");
		g_colorToStringVect.push_back(L"黄灯");
		g_colorToStringVect.push_back(L"绿黄灯");
		g_colorToStringVect.push_back(L"绿灯");
		g_colorToStringVect.push_back(L"白灯");
		g_ESignalTypeToStringVect.clear();
		g_ESignalTypeToStringVect.push_back(L"NORMAL");
		g_ESignalTypeToStringVect.push_back(L"DISTANCE");
		g_ESignalTypeToStringVect.push_back(L"REPEATER");
		g_ESignalTypeToStringVect.push_back(L"SHUNTING");
		g_ESignalTypeToStringVect.push_back(L"INFO");
	}
};
Global g_struct;
CString changeColorToString(char cLightColor)
{
	if (cLightColor < 0 || cLightColor > (char)g_colorToStringVect.size())
		return L"不能识别的车灯";

	return g_colorToStringVect[cLightColor];
}
CString changeESignalTypeToString(ESignalType signalType)
{
	if (signalType < 0 || signalType > (int)g_ESignalTypeToStringVect.size())
		return L"不能识别的信号机";

	return g_ESignalTypeToStringVect[signalType];
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
void AddSpeedPostLimit(float currentDistance, const STrackNode &node, vector<SSpeedPostLimit>& limitVect, HANDLE handle, int nDirection, STrackNode *nodePtr, int nDirectionOfItemToFind)
{
	int num = node.nTrItemNum;

	if (num > 0)
	{
		size_t *memory = new size_t[num];
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		int start = nDirection ? 0 : num - 1;
		int end = nDirection ? num : -1;
		int delta = nDirection ? 1 : -1;

		for (int i = start; i != end; i += delta)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);

			if (type == SpeedPostItem)
			{
				SSpeedPostItem speedPostItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&speedPostItem, sizeof(SSpeedPostItem));
				unsigned short subType = speedPostItem.sSpeedPostType;

				if ((subType & 7) == 2)
				{
					size_t pointer;
					ReadTrainProcess(handle, (LPCVOID)THIS_POINTER_MEM, &pointer, 4);
					pointer += 230;
					size_t memory;
					ReadTrainProcess(handle, (LPCVOID)pointer, &memory, 4);
					pointer = memory;
					ReadTrainProcess(handle, (LPCVOID)pointer, &memory, 4);

					if ((subType & 0x80) || subType & 0x20 && memory & 2 || subType & 0x40 && memory & 4)
					{
						float distanceToTrackStart;

						if (!nDirection)
						{
							distanceToTrackStart = node.fTrackNodeLength - speedPostItem.fLocationInTrackNode;
						}
						else
						{
							distanceToTrackStart = speedPostItem.fLocationInTrackNode;
						}

						if (currentDistance + distanceToTrackStart > 0)
						{
							// 判断该标志限速是否有效，根据方向判断
							if (IsSpeedPostValid(handle, speedPostItem.fAngle, speedPostItem.fLocationInTrackNode, nDirectionOfItemToFind, node, nodePtr))
							{
								float speed = speedPostItem.SpeedpostTrItemDataSecond;

								if (subType & 0x100)
									speed *= 1.609f;

								limitVect.push_back(SSpeedPostLimit(currentDistance +
								                                    distanceToTrackStart, speed));
							}
						}
					}
				}
			}
		}

		delete[]memory;
	}
}

void TestAndSetSignalItem(HANDLE handle, const SSignalItem &signalItem, int nValueToTest, float &fSignalSpeed)
{
	/*** 这里面我也不知道是什么意思 开始***/
	int nBitSet = 0;
	int nRetValue = 2;
	BYTE cSignalColor = signalItem.cLightColor21;

	if (cSignalColor)
	{
		if (cSignalColor == 1)
			nRetValue = 1;
	}
	else
	{
		nRetValue = 0;
	}

	if (nValueToTest & 1)
		nBitSet |= 1;

	if (signalItem.dwData1C & 0x2)
		nBitSet |= 2;

	if (signalItem.dwData1C & 0x2000)
		nBitSet |= 4;

	if (nBitSet & 4 && (!nRetValue || nBitSet & 2))
	{
		// 任务临时限速的值
		float *fTempLimitPtr;
		ReadTrainProcess(handle, (LPCVOID)TASK_LIMIT_MEM, &fTempLimitPtr, 4);
		float fTempLimit;
		ReadTrainProcess(handle, (LPCVOID)(fTempLimitPtr + 23), &fTempLimit, 4);
		fSignalSpeed = fTempLimit;
	}
	else if (nBitSet & 2 || !cSignalColor || cSignalColor >= 8)
	{
		fSignalSpeed = 0;
	}

	/*** 这里面我也不知道是什么意思 结尾***/
}

void AddSignalItem(float currentDistance, const STrackNode &node, vector<SShowSignalItem>& signalVect, HANDLE handle, int nDirection, int nDirectionOfItemToFind)
{
	int num = node.nTrItemNum;

	if (num > 0)
	{
		size_t *memory = new size_t[num];
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		int start = nDirection ? 0 : num - 1;
		int end = nDirection ? num : -1;
		int delta = nDirection ? 1 : -1;
		SSignalItem savedSignalItem;
		savedSignalItem.cLightColor21 = 0;
		savedSignalItem.fLocationInTrackNode = -1;
		float fSavedDistanceToTrackStart;
		SSignalState savedSignalState;

		for (int i = start; i != end; i += delta)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);

			if (type == EmptyOrSignalItem)
			{
				SSignalItem signalItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&signalItem, sizeof(SSignalItem));
				SSignalType signalType;
				ReadTrainProcess(handle, signalItem.pSignalType14, (LPVOID)&signalType, sizeof(SSignalType));
				SSignalState signalState;
				ReadTrainProcess(handle, signalType.pSignalStateArray50 + signalItem.cLightColor21, (LPVOID)&signalState, sizeof(SSignalState));
				float distanceToTrackStart;

				if (!nDirection)
				{
					distanceToTrackStart = node.fTrackNodeLength - signalItem.fLocationInTrackNode;
				}
				else
				{
					distanceToTrackStart = signalItem.fLocationInTrackNode;
				}

				if (nDirectionOfItemToFind != signalItem.cSignalItemDirection20)
					continue; // 判断方向

				if (signalType.eSignalType4 != NORMAL) // 判断类型
				{
					//if (distanceToTrackStart + currentDistance > 0)
					//signalVect.push_back(SShowSignalItem(signalType.eSignalType4, distanceToTrackStart + currentDistance, signalState.fSpeedLimit, signalItem.cLightColor21));
					continue;
				}

				float fDelta = signalItem.fLocationInTrackNode - savedSignalItem.fLocationInTrackNode;
				fDelta = fDelta < 0 ? -fDelta : fDelta;

				if (fDelta < 0.1)
				{
					if (signalItem.cLightColor21 > savedSignalItem.cLightColor21)
					{
						savedSignalState = signalState;
						savedSignalItem = signalItem;
						fSavedDistanceToTrackStart = distanceToTrackStart;
					}
				}
				else
				{
					if (savedSignalItem.fLocationInTrackNode > 0)
					{
						TestAndSetSignalItem(handle, savedSignalItem, savedSignalState.SIGASPF_flags, savedSignalState.fSpeedLimit);

						if (fSavedDistanceToTrackStart + currentDistance > 0)
							signalVect.push_back(SShowSignalItem(NORMAL, fSavedDistanceToTrackStart + currentDistance, savedSignalState.fSpeedLimit, savedSignalItem.cLightColor21));
					}

					savedSignalState = signalState;
					savedSignalItem = signalItem;
					fSavedDistanceToTrackStart = distanceToTrackStart;
				}
			}
		}

		if (savedSignalItem.fLocationInTrackNode > 0 && fSavedDistanceToTrackStart + currentDistance > 0)
		{
			TestAndSetSignalItem(handle, savedSignalItem, savedSignalState.SIGASPF_flags, savedSignalState.fSpeedLimit);
			signalVect.push_back(SShowSignalItem(NORMAL, fSavedDistanceToTrackStart + currentDistance, savedSignalState.fSpeedLimit, savedSignalItem.cLightColor21));
		}

		delete[]memory;
	}
}

void AddTempSpeedLimit(float currentDistance, STrackNode *nodePtr, vector<STempSpeedLimit>& limitVect, HANDLE handle, int direction)
{
	SNode *head;
	SNode ite;
	STrackNode node;
	ReadTrainProcess(handle, (void *)nodePtr, (LPVOID)&node, sizeof(STrackNode));
	ReadTrainProcess(handle, (LPCVOID)TASK_LIMIT_HEAD_MEM, &head, 4);
	ReadTrainProcess(handle, head, &ite, sizeof(SNode));

	while (ite.next != head)
	{
		SNode *next = ite.next;
		ReadTrainProcess(handle, next, &ite, sizeof(SNode));
		STempSpeed speed;
		ReadTrainProcess(handle, (char *)ite.pointer + 32, &speed, sizeof(STempSpeed));

		if (nodePtr == speed.nodePtr)
		{
			float fBegin, fEnd;

			if (!direction)
			{
				fBegin = node.fTrackNodeLength - speed.fStart;
				fEnd = node.fTrackNodeLength - speed.fEnd;
			}
			else
			{
				fBegin = speed.fStart;
				fEnd = speed.fEnd;
			}

			if (fBegin < fEnd && fEnd + currentDistance > 0)
			{
				limitVect.push_back(STempSpeedLimit(fBegin + currentDistance, fEnd + currentDistance));
			}
		}
	}
}

void AddStationItem(float currentDistance, const STrackNode &node, vector<SStationItem>& stationVect, vector<SStationItem>& sidingVect, HANDLE handle, int nDirection)
{
	int num = node.nTrItemNum;

	if (num > 0)
	{
		size_t *memory = new size_t[num];
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		int start = nDirection ? 0 : num - 1;
		int end = nDirection ? num : -1;
		int delta = nDirection ? 1 : -1;

		for (int i = start; i != end; i += delta)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);

			if (type == PlatFormItem)
			{
				SPlatformItem platformItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&platformItem, sizeof(SPlatformItem));
				wchar_t stationName[0x400];
				ReadTrainProcess(handle, platformItem.platformName, (LPVOID)stationName, 0x800);
				float distanceToTrackStart;

				if (!nDirection)
				{
					distanceToTrackStart = node.fTrackNodeLength - platformItem.fLocationInTrackNode;
				}
				else
				{
					distanceToTrackStart = platformItem.fLocationInTrackNode;
				}

				if (distanceToTrackStart + currentDistance > 0)
					stationVect.push_back(SStationItem(distanceToTrackStart + currentDistance, stationName));
			}
			else if (type == SidingItem)
			{
				SSidingItem sidingItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&sidingItem, sizeof(SSidingItem));
				wchar_t stationName[0x400];
				ReadTrainProcess(handle, sidingItem.sidingName, (LPVOID)stationName, 0x800);
				float distanceToTrackStart;

				if (!nDirection)
				{
					distanceToTrackStart = node.fTrackNodeLength - sidingItem.fLocationInTrackNode;
				}
				else
				{
					distanceToTrackStart = sidingItem.fLocationInTrackNode;
				}

				if (distanceToTrackStart + currentDistance > 0)
					sidingVect.push_back(SStationItem(distanceToTrackStart + currentDistance, stationName));
			}
		}

		delete[]memory;
	}
}


STrackNode *GetNext(STrackNode *nodePtr, const SConnectStruct &connectStruct, const SConnectNode &connectNode,
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

STrackNode *GetNextNode(HANDLE handle, const STrackNode &node, STrackNode *nodePtr, int direction, int &nextDirect)
{
	SConnectNode connectNode;
	SConnectStruct connectStruct;
	STrackNode *next;
	SConnectNode *connectNodePtr = direction ? node.OutConnectNodePtr : node.InConnectNodePtr;
	ReadTrainProcess(handle, (void *)connectNodePtr, (LPVOID)&connectNode, sizeof(SConnectNode));
	ReadTrainProcess(handle, (void *)connectNode.nodePointer20, (LPVOID)&connectStruct, sizeof(SConnectStruct));
	next = GetNext(nodePtr, connectStruct, connectNode, nextDirect);
	return next;
}
CString SpeedPostItemToString(const SSpeedPostItem &item)
{
	CString result;
	CString tmp;
	tmp.Format(L"%d %d %d\r\n", item.nType, item.nSubType, item.unknown3);
	result += tmp;
	tmp.Format(L"%f %d\r\n", item.fLocationInTrackNode, item.TrItemSDataSecond);
	result += tmp;
	tmp.Format(L"%f %f %d %d\r\n", item.TrItemPDataFirst, item.TrItemPDataSecond, item.TrItemPDataThird, item.TrItemPDataFourth);
	result += tmp;
	tmp.Format(L"0x%X 0x%04X %d %f %f %X", item.variableData, item.sSpeedPostType, item.SpeedpostTrItemDataSecond,
	           item.SpeedpostTrItemDataThird, item.fAngle, item.fData);
	result += tmp;
	return result;
}

void process_AX(float *fArray, float AX)
{
	float fTemp;
	float sinValue = sin(AX);
	float cosValue = cos(AX);

	for (int i = 0; i < 3; ++i)
	{
		fTemp = cosValue * fArray[3 + i] + sinValue * fArray[6 + i];
		fArray[6 + i] = cosValue * fArray[6 + i] - sinValue * fArray[3 + i];
		fArray[3 + i] = fTemp;
	}
}
void process_AY(float *fArray, float AY)
{
	float fTemp;
	float sinValue = sin(AY);
	float cosValue = cos(AY);

	for (int i = 0; i < 3; ++i)
	{
		fTemp = cosValue * fArray[i] - sinValue * fArray[6 + i];
		fArray[6+i] = sinValue * fArray[i] + cosValue * fArray[6 + i];
		fArray[i] = fTemp;
	}
}
void process_AZ(float *fArray, float AZ)
{
	float fTemp;
	float sinValue = sin(AZ);
	float cosValue = cos(AZ);

	for (int i = 0; i < 3; ++i)
	{
		fTemp = cosValue * fArray[i] + sinValue * fArray[3 + i];
		fArray[3+i] = cosValue * fArray[3 + i] - sinValue * fArray[i];
		fArray[i] = fTemp;
	}
}

float *process(HANDLE handle, float *fMatrix, float *fXYZ)
{
	ReadTrainProcess(handle, (LPCVOID)IDENTITY_MATRIX_MEM, (LPVOID)fMatrix, 0x24);
	process_AY(fMatrix, fXYZ[1]);
	process_AX(fMatrix, fXYZ[0]);
	process_AZ(fMatrix, fXYZ[2]);
	return fMatrix;
}

bool IsSpeedPostValid(HANDLE handle, float angle, float fLocationInTrackNode, int nDirection, const STrackNode &node, STrackNode *nodePtr)
{
	SProcessData processData;
	float fDirection[3];
	fDirection[0] = cos(angle);
	fDirection[1] = 0;
	fDirection[2] = sin(angle);
	size_t mem;
	SSectionTypeData *sectionTypePtr;
	ReadTrainProcess(handle, (LPCVOID)0x80A118, (LPVOID)&mem, 4);
	mem += 12;
	ReadTrainProcess(handle, (LPCVOID)mem, (LPVOID)&sectionTypePtr, 4);
	processData.nodePtr0 = nodePtr;
	getSectionData_Modified(handle, processData, node, 0, 0, sectionTypePtr);
	AdjustAngle_Modified(handle, processData, node, fLocationInTrackNode, sectionTypePtr);

	if (nDirection)
	{
		processData.fAngle24[1] += 3.14159f;
		process(handle, processData.fMatrix, processData.fAngle24);
	}

	float *fArray = processData.fMatrix + 6;
	float result = inner_product(fDirection, fArray);
	return result > 0;
}
CString IteratorList(HANDLE handle, void *headPtr, CString (*func)(HANDLE, void *))
{
	CString strDecorator = L"\r\n**********************************\r\n";
	int count = 0;
	SNode *head;
	SNode iteNode;
	CString strResult;
	strResult.Format(L"DbLinkedList with head 0x%08X Iterator Result:\r\n", headPtr);
	strResult = strDecorator + strResult;
	ReadTrainProcess(handle, headPtr, &head, 4);
	ReadTrainProcess(handle, head, &iteNode, sizeof(SNode));

	while (iteNode.next != head && count < 10000)
	{
		SNode *next = iteNode.next;
		ReadTrainProcess(handle, next, &iteNode, sizeof(SNode));
		strResult += func(handle, iteNode.pointer);
		++count;
	}

	CString strCount;
	strCount.Format(L"Total Count : %d", count);
	strResult += strCount;
	strResult += strDecorator;
	return strResult;
}

CString DefaultHandle(HANDLE handle, void *pointer)
{
	CString result;
	result.Format(L"0x%X\r\n", pointer);
	return result;
}

void getSectionData_Modified(HANDLE handle, SProcessData &processData, const STrackNode &node, int, int, SSectionTypeData *basePtr)
{
	processData.sectionPtr8 = node.sectionArrayPtr;
	SSectionData sectionData;
	ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionData, sizeof(SSectionData));
	processData.fAngle24[0] = sectionData.AX;
	processData.fAngle24[1] = sectionData.AY;
	processData.fAngle24[2] = sectionData.AZ;
	process(handle, processData.fMatrix, processData.fAngle24);
}

int AdjustAngle_Modified(HANDLE handle, SProcessData &processData, const STrackNode &node, float fLocation, SSectionTypeData *basePtr)
{
	float fRemainDistance = fLocation;
	unsigned short sectionIndex;
	ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex, 2);
	float fCurrentSectionLength;
	ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex), (LPVOID)&fCurrentSectionLength, 4);
	int nCurrentNum = 0;

	while (fRemainDistance > fCurrentSectionLength)
	{
		if (++nCurrentNum >= node.nSectionNum)
			break;

		fRemainDistance -= fCurrentSectionLength;
		processData.sectionPtr8 = node.sectionArrayPtr + nCurrentNum;
		ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex, 2);
		ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex), (LPVOID)&fCurrentSectionLength, 4);
	}

	SSectionData sectionData;
	ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionData, sizeof(SSectionData));
	SSectionTypeData *typePtr = basePtr + sectionData.sectionIndex;
	processData.fAngle24[0] = sectionData.AX;
	processData.fAngle24[1] = sectionData.AY;
	processData.fAngle24[2] = sectionData.AZ;
	SSectionTypeData typeData;
	ReadTrainProcess(handle, typePtr, &typeData, sizeof(SSectionTypeData));

	if (typeData.fData[1] != 0)
	{
		float fNum = fRemainDistance / typeData.fData[1];

		if (typeData.fData[2] < 0)
		{
			processData.fAngle24[1] -= fNum;
		}
		else
		{
			processData.fAngle24[1] += fNum;
		}
	}

	process(handle, processData.fMatrix, processData.fAngle24);
	return 1;
}