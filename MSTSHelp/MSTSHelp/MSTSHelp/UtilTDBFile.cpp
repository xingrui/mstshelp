#include "stdafx.h"
#include "UtilTDBFile.h"
#define TRAIN_INFO_MEM 0x809890
// 火车车头的一些信息值的存放地方

#define HEAD_TRACK_MEM 0x8098DC
// 车头所在的TrackNode的地址

#define TAIL_TRACK_MEM 0x809944
// 车尾所在的TrackNode的地址
#define TASK_LIMIT_HEAD_MEM 0x809B38
#include <cmath>
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
	ReadTrainProcess(handle, (LPCVOID)0x771680, (LPVOID)fMatrix, 0x24);
	process_AY(fMatrix, fXYZ[1]);
	process_AX(fMatrix, fXYZ[0]);
	process_AZ(fMatrix, fXYZ[2]);
	return fMatrix;
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
void AddTempSpeedLimit(float currentDistance, STrackNode *nodePtr, vector<SForwardLimit>& limitVect, HANDLE handle, int direction, float fTempSpeedLimit)
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

			if (fBegin < fEnd && fBegin + currentDistance > 0)
			{
				limitVect.push_back(SForwardLimit(fBegin + currentDistance, fTempSpeedLimit));
			}
		}
	}
}
void AddSpeedPostLimit(float currentDistance, const STrackNode &node, vector<SForwardLimit>& limitVect, HANDLE handle, int nDirection, STrackNode *nodePtr, int nDirectionOfItemToFind)
{
	int num = node.nTrItemNum;

	if (num > 0)
	{
		size_t *memory = new size_t[num];
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		int start = nDirection ? 0 : num - 1;
		int end = nDirection ? num : -1;
		int delta = nDirection ? 1 : -1;
		size_t nTrainType;
		ReadTrainProcess(handle, (LPCVOID)TRAIN_INFO_MEM, &nTrainType, 4);

		for (int i = start; i != end; i += delta)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);

			if (type == 8)
			{
				SSpeedPostItem speedPostItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&speedPostItem, sizeof(SSpeedPostItem));
				unsigned short subType = speedPostItem.sSpeedPostType;

				if ((subType & 7) == 2)
				{
					if ((subType & 0x80) || subType & 0x20 && nTrainType & 2 || subType & 0x40 && nTrainType & 4)
					{
						float distanceToTrackStart = nDirection ? speedPostItem.fLocationInTrackNode : node.fTrackNodeLength - speedPostItem.fLocationInTrackNode;

						if (currentDistance + distanceToTrackStart > 0)
						{
							// 判断该标志限速是否有效，根据方向判断
							if (IsSpeedPostValid(handle, speedPostItem.fAngle, speedPostItem.fLocationInTrackNode, nDirectionOfItemToFind, node, nodePtr))
							{
								float speed = speedPostItem.SpeedpostTrItemDataSecond;

								if (subType & 0x100)
									speed *= 1.609f;

								limitVect.push_back(SForwardLimit(currentDistance +
								                                  distanceToTrackStart, speed / 3.6f));
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

	if (nBitSet & 2 || !cSignalColor || cSignalColor >= 8)
	{
		fSignalSpeed = 0;
	}

	/*** 这里面我也不知道是什么意思 结尾***/
}

void AddSignalItem(float currentDistance, const STrackNode &node, vector<SForwardLimit>& limitVect, HANDLE handle, int nDirection, int nDirectionOfItemToFind)
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

			if (type == 0)
			{
				SSignalItem signalItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&signalItem, sizeof(SSignalItem));
				SSignalType signalType;
				ReadTrainProcess(handle, signalItem.pSignalType14, (LPVOID)&signalType, sizeof(SSignalType));
				SSignalState signalState;
				ReadTrainProcess(handle, signalType.pSignalStateArray50 + signalItem.cLightColor21, (LPVOID)&signalState, sizeof(SSignalState));
				float distanceToTrackStart = nDirection ? signalItem.fLocationInTrackNode : node.fTrackNodeLength - signalItem.fLocationInTrackNode;

				if (nDirectionOfItemToFind != signalItem.cSignalItemDirection20)
					continue; // 判断方向

				if (signalType.eSignalType4 != 0) // 判断类型
				{
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

						if (fSavedDistanceToTrackStart + currentDistance > 0 && savedSignalState.fSpeedLimit >= 0)
							limitVect.push_back(SForwardLimit(fSavedDistanceToTrackStart + currentDistance, savedSignalState.fSpeedLimit));
					}

					savedSignalState = signalState;
					savedSignalItem = signalItem;
					fSavedDistanceToTrackStart = distanceToTrackStart;
				}
			}
		}

		if (savedSignalItem.fLocationInTrackNode > 0 && fSavedDistanceToTrackStart + currentDistance > 0 && savedSignalState.fSpeedLimit >= 0)
		{
			TestAndSetSignalItem(handle, savedSignalItem, savedSignalState.SIGASPF_flags, savedSignalState.fSpeedLimit);
			limitVect.push_back(SForwardLimit(fSavedDistanceToTrackStart + currentDistance, savedSignalState.fSpeedLimit));
		}

		delete[]memory;
	}
}

void GetForwardSpeedLimit(HANDLE m_hTrainProcess, vector<SForwardLimit>& limitList, float fTempLimit)
{
	STrackInfo headInfo;
	//headInfo is the information of the head of the train.
	size_t trainInfo;
	ReadTrainProcess(m_hTrainProcess, (void *)TRAIN_INFO_MEM, (LPVOID)&trainInfo, 4);
	BOOL bIsForward;

	if (trainInfo & 0x80) // Forward Or Backward
	{
		ReadTrainProcess(m_hTrainProcess, (void *)TAIL_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo));
		bIsForward = FALSE;
	}
	else
	{
		ReadTrainProcess(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo));
		bIsForward = TRUE;
	}

	float forwardLength;
	STrackNode trackNode;
	int nDirectOfHeadNode = headInfo.nDirection == bIsForward;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.trackNodePtr, (LPVOID)&trackNode, sizeof(STrackNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - trackNode.fTrackNodeLength;

	int nDirectOfNextNode = nDirectOfHeadNode;
	STrackNode *nextNodePtr = headInfo.trackNodePtr;

	while (forwardLength < 4000 && nextNodePtr)
	{
		STrackNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		STrackNode trackNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(STrackNode));
		AddTempSpeedLimit(forwardLength, currentNodePtr, limitList, m_hTrainProcess, nDirectOfCurrentNode, fTempLimit);
		AddSpeedPostLimit(forwardLength, trackNode, limitList, m_hTrainProcess, nDirectOfCurrentNode, currentNodePtr, !nDirectOfCurrentNode);
		AddSignalItem(forwardLength, trackNode, limitList, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode);
		forwardLength += trackNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}
}