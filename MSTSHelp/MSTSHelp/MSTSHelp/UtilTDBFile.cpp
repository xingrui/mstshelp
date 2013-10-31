#include "stdafx.h"
#include "UtilTDBFile.h"
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
void getSectionData_Modified(HANDLE handle, SProcessData &processData, const SVectorNode &node, int, int, SSectionTypeData *basePtr)
{
	processData.sectionPtr8 = node.sectionArrayPtr;
	SVectorSection sectionData;
	ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionData, sizeof(SVectorSection));
	processData.fAngle24[0] = sectionData.AX;
	processData.fAngle24[1] = sectionData.AY;
	processData.fAngle24[2] = sectionData.AZ;
	process(handle, processData.fMatrix, processData.fAngle24);
}

int AdjustAngle_Modified(HANDLE handle, SProcessData &processData, const SVectorNode &node, float fLocation, SSectionTypeData *basePtr)
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

	SVectorSection sectionData;
	ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionData, sizeof(SVectorSection));
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
bool IsSpeedPostValid(HANDLE handle, float angle, float fLocationInTrackNode, int nDirection, const SVectorNode &node, SVectorNode *nodePtr)
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
void AddTempSpeedLimit(float currentDistance, SVectorNode *nodePtr, vector<SForwardLimit>& limitVect, HANDLE handle, int direction, float fTempSpeedLimit)
{
	SNode *head;
	SNode ite;
	SVectorNode node;
	ReadTrainProcess(handle, (void *)nodePtr, (LPVOID)&node, sizeof(SVectorNode));
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
void AddSpeedPostLimit(float currentDistance, const SVectorNode &node, vector<SForwardLimit>& limitVect, HANDLE handle, int nDirection, SVectorNode *nodePtr, int nDirectionOfItemToFind)
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

	if (nBitSet & 4 && (!nRetValue || nBitSet & 2))
	{
		// 任务临时限速的值
		float *fTempLimitPtr;
		ReadTrainProcess(handle, (LPCVOID)0x809B48, &fTempLimitPtr, 4);
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

void AddSignalItem(float currentDistance, const SVectorNode &node, vector<SForwardLimit>& limitVect, HANDLE handle, int nDirection, int nDirectionOfItemToFind, float fCarriageLength)
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

						if (fSavedDistanceToTrackStart + currentDistance - fCarriageLength > 0 && savedSignalState.fSpeedLimit >= 0)
							limitVect.push_back(SForwardLimit(fSavedDistanceToTrackStart + currentDistance - fCarriageLength, savedSignalState.fSpeedLimit));
					}

					savedSignalState = signalState;
					savedSignalItem = signalItem;
					fSavedDistanceToTrackStart = distanceToTrackStart;
				}
			}
		}

		if (savedSignalItem.fLocationInTrackNode > 0 && fSavedDistanceToTrackStart + currentDistance - fCarriageLength > 0 && savedSignalState.fSpeedLimit >= 0)
		{
			TestAndSetSignalItem(handle, savedSignalItem, savedSignalState.SIGASPF_flags, savedSignalState.fSpeedLimit);
			limitVect.push_back(SForwardLimit(fSavedDistanceToTrackStart + currentDistance - fCarriageLength , savedSignalState.fSpeedLimit));
		}

		delete[]memory;
	}
}
struct SCarriageInTrackNode
{
	SVectorNode *pVectorNode0; // 所在的TrackNode指针
	float fLocationInVectorNode4; // 在TrackNode当中的位置
	void *pEngineOrWagonInConFile8;
	void *pSrvFileC;
	void *pCarriage10; // 另一个非常重要的指针，内有制动数据等重要信息
	int nCarriageMemoryIndex14; // *(*828108 + 8 * nIndexInMemory) == pCarriage10
	BYTE cDirection18; // 铁轨方向
	BYTE cData19[3];
};

int GetCarriageCount(HANDLE handle, void *pSrvFile)
{
	int carriage_count = 0;
	SNode *head;
	SNode iteNode;
	ReadTrainProcess(handle, (char *)pSrvFile + 0x12C, &head, 4);
	ReadTrainProcess(handle, head, &iteNode, sizeof(SNode));

	while (iteNode.next != head)
	{
		SNode *next = iteNode.next;
		ReadTrainProcess(handle, next, &iteNode, sizeof(SNode));
		++carriage_count;
	}

	return carriage_count;
}

float GetConnectDistance(float currentDistance, const SVectorNode &node, HANDLE handle, int nDirection, SVectorNode *nodePtr, void *pTrain)
{
	// currentDistance is the location add half carriage.
	SNode *head = (SNode *)node.pPtr44;
	SNode ite;
	float distance = LONG_DISTANCE;

	if (NULL == head)
	{
		return distance;
	}

	ReadTrainProcess(handle, (LPCVOID)head, &ite, sizeof(SNode));
	float carriageSize;

	while (ite.next != head)
	{
		SNode *next = ite.next;
		ReadTrainProcess(handle, next, &ite, sizeof(SNode));
		SCarriageInTrackNode carriageInTrackNode;
		memset(&carriageInTrackNode, '\0', sizeof(carriageInTrackNode));
		ReadTrainProcess(handle, (char *)ite.pointer, &carriageInTrackNode, sizeof(SCarriageInTrackNode));

		if (carriageInTrackNode.pSrvFileC == (void *)TRAIN_INFO_MEM)
		{
			// This carriage is belongs to this train.
			continue;
		}

		float distanceToTrackStart = nDirection ? carriageInTrackNode.fLocationInVectorNode4 : node.fTrackNodeLength - carriageInTrackNode.fLocationInVectorNode4;
		float tmp_distance = distanceToTrackStart + currentDistance;
		struct SEngineOrWagonInConFile
		{
			void *pWagFile0;
			void *pEngFile4;
			int nCarriageType8; // 1 Eng 3 Wag
			int nWagNameHashValueC; // 0
			int nEngNamehashValue10;
			wchar_t wcDirectoryName14[0x20];
			wchar_t wcUnknown54[0x20];
			wchar_t wcWagonDataFirst94[0x20];
			wchar_t wcEngineDataFirstD4[0x20];
		};
		SEngineOrWagonInConFile engine;
		ReadTrainProcess(handle, (char *)carriageInTrackNode.pEngineOrWagonInConFile8, &engine, sizeof(engine));
		void *pBase = NULL != engine.pWagFile0 ? carriageInTrackNode.pEngineOrWagonInConFile8 : (char *)carriageInTrackNode.pCarriage10 + 0x94;
		ReadPointerMemory(handle, pBase, &carriageSize, sizeof(carriageSize),
		                  1, 0x400);

		if (tmp_distance < distance && tmp_distance > 0)
		{
			distance = tmp_distance - carriageSize / 2;
		}
	}

	return distance;
}

float GetConnectDistance(HANDLE m_hTrainProcess, float fDistance)
{
	float res_distance = LONG_DISTANCE;
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
	SVectorNode vectorNode;
	int nDirectOfHeadNode = headInfo.nDirection == bIsForward;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.vectorNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nOffset = bIsForward ? 0x62 : 0x66;
	float fCarriageLength;
	void *pTrain;
	ReadProcessMemory(m_hTrainProcess, (LPCVOID)THIS_POINTER_MEM, &pTrain, sizeof(void *), NULL);
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)THIS_POINTER_MEM, &fCarriageLength, 4, 3, nOffset, 0x94, 0x400);
	fCarriageLength /= 2;
	int nDirectOfNextNode = nDirectOfHeadNode;
	SVectorNode *nextNodePtr = headInfo.vectorNodePtr;

	while (forwardLength < fDistance && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode trackNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(SVectorNode));
		float ret = GetConnectDistance(forwardLength, trackNode, m_hTrainProcess, nDirectOfCurrentNode, currentNodePtr, pTrain);

		if (ret < LONG_DISTANCE)
		{
			res_distance = ret - fCarriageLength;
			break;
		}

		forwardLength += trackNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

	return res_distance;
}

CString AddStationItem(float currentDistance, const SVectorNode &node, vector<SForwardLimit>& limitList, HANDLE handle, int nDirection)
{
	CString stationName;
	int num = node.nTrItemNum;

	if (num > 0)
	{
		size_t *memory = new size_t[num];
		ReadTrainProcess(handle, (LPCVOID)node.trItemArrayPtr, (LPVOID)memory, num * 4);
		int start = !nDirection ? 0 : num - 1;
		int end = !nDirection ? num : -1;
		int delta = !nDirection ? 1 : -1;

		for (int i = start; i != end; i += delta)
		{
			int type;
			ReadTrainProcess(handle, (LPCVOID)(*(memory + i)), (LPVOID)&type, 4);

			if (type == PlatFormItem)
			{
				SPlatformItem platformItem;
				const void *address = (LPCVOID) * (memory + i);
				ReadTrainProcess(handle, address, (LPVOID)&platformItem, sizeof(SPlatformItem));
				wchar_t tmp_stationName[0x400];
				ReadTrainProcess(handle, platformItem.wcpPlatformName28, (LPVOID)tmp_stationName, 0x800);
				float distanceToTrackStart = nDirection ? platformItem.fLocationInVectorNode : node.fTrackNodeLength - platformItem.fLocationInVectorNode;
				SForwardLimit limit;
				limit.m_fDistance = distanceToTrackStart + currentDistance;
				limit.m_fSpeedLimit = 0;
				limitList.push_back(limit);
				stationName.AppendFormat(L"%.1f:", limit.m_fDistance);
				stationName += tmp_stationName;
				break;
				//if (distanceToTrackStart + currentDistance > 0)
				//{
				//	float fEndPosition;
				//	ReadPointerMemory(handle, (LPCVOID)0x80A038, &fEndPosition, 4, 5, 0xC, 0x20, 0, 4 * platformItem.nPlatformAnotherSideIndex34, 0xC);
				//	int nStationNum;
				//	ReadPointerMemory(handle, (LPCVOID)0x80A038, &nStationNum, 4, 5, 0xC, 0x20, 0, 4 * platformItem.nPlatformAnotherSideIndex34, 0x34);
				//	float fEndPositionToTrackStart = nDirection ? fEndPosition : node.fTrackNodeLength - fEndPosition;
				//
				//}
			}
		}

		delete[]memory;
	}

	return stationName;
}

void GetConnectSpeedLimit(HANDLE m_hTrainProcess, vector<SForwardLimit>& limitList, BOOL stationStop, CString &stationName)
{
	stationName.Empty();
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
	SVectorNode vectorNode;
	int nDirectOfHeadNode = headInfo.nDirection == bIsForward;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.vectorNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nOffset = bIsForward ? 0x62 : 0x66;
	float fCarriageLength;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)THIS_POINTER_MEM, &fCarriageLength, 4, 3, nOffset, 0x94, 0x400);
	fCarriageLength /= 2;
	int nDirectOfNextNode = nDirectOfHeadNode;
	SVectorNode *nextNodePtr = headInfo.vectorNodePtr;
	vector<SForwardLimit> station_limit;

	while (forwardLength < 4000 && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode trackNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(SVectorNode));

		if (stationName.IsEmpty())
		{
			stationName = AddStationItem(forwardLength, trackNode, station_limit, m_hTrainProcess, nDirectOfCurrentNode);
		}

		forwardLength += trackNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

	if (stationStop)
	{
		limitList.insert(limitList.end(), station_limit.begin(), station_limit.end());
	}

	if (forwardLength < 4000)
		if (forwardLength > 20)
			limitList.push_back(SForwardLimit(forwardLength - 20, 0));
		else
			limitList.push_back(SForwardLimit(0, 0));
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
	SVectorNode vectorNode;
	int nDirectOfHeadNode = headInfo.nDirection == bIsForward;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.vectorNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nOffset = bIsForward ? 0x62 : 0x66;
	float fCarriageLength;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)THIS_POINTER_MEM, &fCarriageLength, 4, 3, nOffset, 0x94, 0x400);
	fCarriageLength /= 2;
	int nDirectOfNextNode = nDirectOfHeadNode;
	SVectorNode *nextNodePtr = headInfo.vectorNodePtr;

	while (forwardLength < 4000 && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode trackNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(SVectorNode));
		AddTempSpeedLimit(forwardLength, currentNodePtr, limitList, m_hTrainProcess, nDirectOfCurrentNode, fTempLimit);
		AddSpeedPostLimit(forwardLength, trackNode, limitList, m_hTrainProcess, nDirectOfCurrentNode, currentNodePtr, !nDirectOfCurrentNode);
		AddSignalItem(forwardLength, trackNode, limitList, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode, fCarriageLength);
		forwardLength += trackNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

	if (forwardLength < 4000)
		if (forwardLength > 50)
			limitList.push_back(SForwardLimit(forwardLength - 50, 0));
		else
			limitList.push_back(SForwardLimit(0, 0));
}