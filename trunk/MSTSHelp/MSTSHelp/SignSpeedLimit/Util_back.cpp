#include "stdafx.h"
#include "Util_back.h"
#include <cmath>
float* process30(HANDLE handle, float* fMatrix, float*fXYZ)
{
	ReadTrainProcess(handle, (LPCVOID)IDENTITY2_MATRIX_MEM, (LPVOID)fMatrix, 0x30);
	process_AY(fMatrix, fXYZ[1]);
	process_AX(fMatrix, fXYZ[0]);
	process_AZ(fMatrix, fXYZ[2]);
	return fMatrix;
}

float* sub_5B3DF7(float* fTempArray, float a, float b, float c)
{
	fTempArray[0] = a;
	fTempArray[1] = b;
	fTempArray[2] = c;
	return fTempArray;
}
float* sub_5B3F03(float* fArray, float* fMatrix, float* fArray2)
{
	fArray2[0] = fMatrix[0] * fArray[0] + fMatrix[3] * fArray[1] + fMatrix[6] * fArray[2] + fMatrix[9];
	fArray2[1] = fMatrix[1] * fArray[0] + fMatrix[4] * fArray[1] + fMatrix[7] * fArray[2] + fMatrix[10];
	fArray2[2] = fMatrix[2] * fArray[0] + fMatrix[5] * fArray[1] + fMatrix[8] * fArray[2] + fMatrix[11];
	return fArray2;
}
float* sub_5B3EB5(float* fArray, float* fMatrix, float* fArray2)
{
	sub_5B3F03(fArray, fMatrix, fArray2);
	fArray[0] = fArray2[0];
	fArray[1] = fArray2[1];
	fArray[2] = fArray2[2];
	return fArray2;
}

float* sub_5B3E21(float* fArray1, float* fArray2, float* fArray3)
{
	fArray1[0] += fArray2[0];
	fArray1[1] += fArray2[1];
	fArray1[2] += fArray2[2];
	fArray3[0] = fArray1[0];
	fArray3[1] = fArray1[1];
	fArray3[2] = fArray1[2];
	return fArray3;
}


void getSectionData(HANDLE handle, SProcessData& processData, const STrackNode& node, int sectionNum, float* fArray, SSectionTypeData* basePtr)
{
	float fDistance = 0;
	processData.nSectionNum4 = sectionNum;
	processData.sectionPtr8 = node.sectionArrayPtr + sectionNum;
	processData.nData12 = 1;
	processData.fDistanceFromSectionStart20 = 0;
	while (sectionNum)
	{
		--sectionNum;
		const SSectionData* sectionPtr = node.sectionArrayPtr + sectionNum;
		unsigned short sectionIndex;
		ReadTrainProcess(handle, (LPCVOID)sectionPtr, (LPVOID)&sectionIndex, 2);
		float fLength;
		ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex), (LPVOID)&fLength, 4);
		fDistance += fLength;
	}
	processData.fDistanceFromNodeStart16 = fDistance;
	SSectionData* sectionPtr = processData.sectionPtr8;
	SSectionData sectionData;
	ReadTrainProcess(handle, (LPCVOID)sectionPtr, (LPVOID)&sectionData, sizeof(SSectionData));
	int nCurrentTileX, nCurrentTileY;
	ReadTrainProcess(handle, (LPCVOID)CURRENT_TILE_X_MEM, (LPVOID)&nCurrentTileX, 4);
	ReadTrainProcess(handle, (LPCVOID)CURRENT_TILE_Z_MEM, (LPVOID)&nCurrentTileY, 4);
	processData.fXYZ72[0] = (sectionData.TileX2 - nCurrentTileX) * 2048.0f + sectionData.X;
	processData.fXYZ72[2] = (sectionData.TileZ2 - nCurrentTileY) * 2048.0f + sectionData.Z;
	processData.fXYZ72[1] = sectionData.Y;
	processData.fAngle24[0] = sectionData.AX;
	processData.fAngle24[1] = sectionData.AY;
	processData.fAngle24[2] = sectionData.AZ;
	processData.nData88 = 0;
	processData.nData92 = 0;
	processData.fData100 = -1.0;
	process(handle, processData.fMatrix, processData.fAngle24);
	if(fArray)
	{
		if(inner_product(fArray, processData.fMatrix + 6))
		{
			processData.nData12 = processData.nData12 != 1;
			processData.nData92 = -processData.nData92;
			processData.fAngle24[1] += 3.14159f;
			process(handle, processData.fMatrix, processData.fAngle24);
		}
	}
}


STrackNode* GetPrevNode(HANDLE handle, SProcessData& processData, SConnectNode* connectNodePtr, int nDirection, SSectionTypeData* basePtr)
{
	SConnectNode connectNode;
	ReadTrainProcess(handle, connectNodePtr, &connectNode, sizeof(SConnectNode));
	STrackNode* prevNode = NULL;
	size_t nTemp;
	SConnectStruct connectStruct;
	ReadTrainProcess(handle, (void *)connectNode.nodePointer20, (LPVOID)&connectStruct, sizeof(SConnectStruct));

	if(connectNode.nType0 == 2)
	{
		for(size_t i = 0; i < connectNode.nTrPinsSecond16 + connectNode.nTrPinsFirst12; ++i)
		{
			if(connectStruct.subStruct[i].nodePtr == processData.nodePtr0
				&& connectStruct.subStruct[i].nDirect != nDirection)
			{
				if(i >= processData.nData12)
				{
					nTemp = 0;
				}else
				{
					nTemp = connectNode.direction80 + connectNode.nTrPinsFirst12;
				}
				if(nTemp >= connectNode.nTrPinsSecond16 + connectNode.nTrPinsFirst12)
					prevNode = NULL;
				else
					prevNode = connectStruct.subStruct[nTemp].nodePtr;
				break;
			}
		}
	}else if(connectNode.nType0 == 3)
	{
		prevNode = NULL;
	}else
	{
		nTemp = connectStruct.subStruct[0].nodePtr == processData.nodePtr0;
		prevNode = connectStruct.subStruct[nTemp].nodePtr;
		if(!prevNode)
			return NULL;
	}
	if(!prevNode)
		return NULL;
	int nDirect = connectStruct.subStruct[nTemp].nDirect;
	processData.nodePtr0 = prevNode;
	STrackNode trackNode;
	ReadTrainProcess(handle, prevNode, &trackNode, sizeof(STrackNode));
	if(nDirect == 1)
	{
		if(nDirection != nDirect)
			processData.nData12 = !processData.nData12;
		processData.fDistanceFromNodeStart16 = 0;
		processData.nSectionNum4 = 0;
		processData.sectionPtr8 = trackNode.sectionArrayPtr + processData.nSectionNum4;
		processData.fDistanceFromSectionStart20 = 0;
	}else
	{
		if(nDirection != nDirect)
			processData.nData12 = !processData.nData12;

		processData.fDistanceFromNodeStart16 = trackNode.fTrackNodeLength;
		processData.nSectionNum4 = trackNode.nSectionNum - 1;
		processData.sectionPtr8 = trackNode.sectionArrayPtr + processData.nSectionNum4;
		unsigned short sectionIndex;
		ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex, 2);
		SSectionTypeData* sectionTypePtr = basePtr + sectionIndex;
		float fLength;
		ReadTrainProcess(handle, (LPCVOID)sectionTypePtr, (LPVOID)&fLength, 4);
		processData.fDistanceFromSectionStart20 = fLength;
	}
	return prevNode;
}

int AdjustAngle(HANDLE handle, SProcessData& processData, const STrackNode& node, float fLocation, SSectionTypeData* basePtr)
{
	int result;
	if(processData.nData12)
	{
		processData.fDistanceFromNodeStart16 += fLocation;
		processData.fDistanceFromSectionStart20 += fLocation;
	}else
	{
		processData.fDistanceFromNodeStart16 -= fLocation;
		processData.fDistanceFromSectionStart20 -= fLocation;
	}
	if(processData.fDistanceFromNodeStart16 < 0)
	{
		SConnectNode* connectNode = node.InConnectNodePtr;
		float fDistance = processData.fDistanceFromNodeStart16;
		size_t nFlag = processData.nData12;
		if(GetPrevNode(handle, processData, connectNode, 0, basePtr))
		{
			if(processData.nData12 !=1 )
			{
				if(processData.nData12 == nFlag)
				{
					fDistance = -fDistance;
				}
			}else if(processData.nData12 != nFlag)
				fDistance = -fDistance;
			result = AdjustAngle(handle, processData, node, fDistance, basePtr);
		}else
		{
			processData.fDistanceFromNodeStart16 = 0;
			processData.fDistanceFromSectionStart20 = 0;
			processData.nSectionNum4 = 0;
			processData.sectionPtr8 = node.sectionArrayPtr + processData.nSectionNum4;
			result = 0;
		}
		return result;
	}
	result = 1;
	if(processData.fDistanceFromNodeStart16 > node.fTrackNodeLength)
	{
		SConnectNode* connectNode = node.OutConnectNodePtr;
		float fDistanceRemain = processData.fDistanceFromNodeStart16 - node.fTrackNodeLength;
		size_t nTemp = processData.nData12;
		if(GetPrevNode(handle, processData, connectNode, 1, basePtr))
		{
			if(processData.nData12 != 1 || processData.nData12 == nTemp)
			{
				if(!processData.nData12 && processData.nData12 == nTemp)
					fDistanceRemain = -fDistanceRemain;
			}else
			{
				fDistanceRemain = -fDistanceRemain;
			}
			return AdjustAngle(handle, processData, node, fDistanceRemain, basePtr);
		}else
		{
			processData.fDistanceFromNodeStart16 = node.fTrackNodeLength;
			processData.nSectionNum4 = node.nSectionNum - 1;
			processData.sectionPtr8 = node.sectionArrayPtr + processData.nSectionNum4;
			unsigned short sectionIndex;
			ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex, 2);
			float fLength;
			ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex), (LPVOID)&fLength, 4);
			processData.fDistanceFromSectionStart20 = fLength;
			result = 0;
		}
	}else
	{
		float fDistance = processData.fDistanceFromSectionStart20;
		unsigned short sectionIndex;
		ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex, 2);
		float fLength;
		ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex), (LPVOID)&fLength, 4);
		if((processData.nData12 != 1 || fLocation < 0.0000001) && (processData.nData12 || fLocation > -0.0000001))
		{
			while(fDistance < 0)
			{
				int nCurrentNum = processData.nSectionNum4 - 1;
				if(nCurrentNum < 0)
					break;
				if(nCurrentNum >= node.nSectionNum)
					break;
				processData.nSectionNum4 = nCurrentNum;
				processData.sectionPtr8 = node.sectionArrayPtr + nCurrentNum;
				unsigned short sectionIndex;
				ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex, 2);
				float fLength;
				ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex), (LPVOID)&fLength, 4);
				fDistance += fLength;
			}
		}else
		{
			while (fDistance > fLength)
			{
				int nCurrentNum = processData.nSectionNum4 + 1;
				if(nCurrentNum < 0)
					break;
				if(nCurrentNum >= node.nSectionNum)
					break;
				processData.nSectionNum4 = nCurrentNum;
				fDistance -= fLength;
				processData.sectionPtr8 = node.sectionArrayPtr + nCurrentNum;
				unsigned short sectionIndex2;
				ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionIndex2, 2);
				ReadTrainProcess(handle, (LPCVOID)(basePtr + sectionIndex2), (LPVOID)&fLength, 4);
			}
		}
		processData.fDistanceFromSectionStart20 = fDistance;
	}
	SSectionData sectionData;
	ReadTrainProcess(handle, (LPCVOID)processData.sectionPtr8, (LPVOID)&sectionData, sizeof(SSectionData));
	SSectionTypeData* typePtr = basePtr + sectionData.sectionIndex;
	int nCurrentTileX, nCurrentTileZ;
	ReadTrainProcess(handle, (LPCVOID)CURRENT_TILE_X_MEM, (LPVOID)&nCurrentTileX, 4);
	ReadTrainProcess(handle, (LPCVOID)CURRENT_TILE_Z_MEM, (LPVOID)&nCurrentTileZ, 4);
	processData.fXYZ72[0] = (sectionData.TileX2 - nCurrentTileX) * 2048.0f + sectionData.X;
	processData.fXYZ72[1] = sectionData.Y;
	processData.fXYZ72[2] = (sectionData.TileZ2 - nCurrentTileZ) * 2048.0f + sectionData.Z;
	processData.fAngle24[0] = sectionData.AX;
	processData.fAngle24[1] = sectionData.AY;
	processData.fAngle24[2] = sectionData.AZ;
	if(!processData.nData12)
		processData.fAngle24[1] -= 3.14159f;
	SSectionTypeData typeData;
	ReadTrainProcess(handle, typePtr, &typeData, sizeof(SSectionTypeData));
	float fTempArray[3];
	float fTransformMatrix[12];
	float fArray2[3];
	float fTempResult[3];
	if(typeData.fData[1] == 0)
	{
		if(typeData.fData[4] == 0)
		{
			sub_5B3DF7(fTempArray, 0, 0, processData.fDistanceFromSectionStart20);
			process30(handle, fTransformMatrix, &sectionData.AX);
			sub_5B3EB5(fTempArray, fTransformMatrix, fArray2);
			sub_5B3E21(processData.fXYZ72, fTempArray, fTempResult);
			processData.nData92 = sectionData.AX;
			processData.nData88 = 0;
		}else
		{
			process30(handle, fTransformMatrix, &sectionData.AX);
			sub_5B3DF7(fTempArray, 0, 0, processData.fDistanceFromSectionStart20);
			process30(handle, fTransformMatrix, &sectionData.AX);
			sub_5B3EB5(fTempArray, fTransformMatrix, fArray2);
			sub_5B3E21(processData.fXYZ72, fTempArray, fTempResult);
			processData.nData92 = sectionData.AX;
			processData.nData88 = 0;
		}
	}else
	{
		float fNum = processData.fDistanceFromSectionStart20 / typeData.fData[1];
		float sinValue = sin(fNum);
		float cosValue = cos(fNum);
		float fSin = sinValue * typeData.fData[1];
		float fCos = typeData.fData[1] - cosValue * typeData.fData[1];
		sub_5B3DF7(fTempArray,fCos, 0, fSin);
		if(typeData.fData[2] < 0)
		{
			fTempArray[0] = -fTempArray[0];
			processData.fAngle24[1] -= fNum;
		}
		else
		{
			processData.fAngle24[1] += fNum;
		}
		process30(handle, fTransformMatrix, &sectionData.AX);
		sub_5B3EB5(fTempArray, fTransformMatrix, fTempResult);
		sub_5B3E21(processData.fXYZ72, fTempArray, fTempResult);
		processData.nData92 = sectionData.AX;
		processData.nData88 = sectionData.unData56;
		processData.fData100 = sectionData.unData60;
	}
	process(handle, processData.fMatrix, processData.fAngle24);
	return result;
}