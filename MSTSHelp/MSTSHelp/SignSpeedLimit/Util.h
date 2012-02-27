#ifndef UTIL_H
#define UTIL_H

#define THIS_POINTER_MEM 0x7C2AC0
#define HEAD_TRACK_MEM 0x8098DC
#define TAIL_TRACK_MEM 0x809944
struct STrackNode;
struct SConnectStruct
{
	STrackNode* nodePtr1;
	int         nDirect1;
	STrackNode* nodePtr2;
	int         nDirect2;
	STrackNode* nodePtr3;
	int         nDirect3;
};
struct STDBFile;
struct SConnectNode
{
	int nType;// 2 JunctionNode || 3 EndNode
	DWORD data2;
	DWORD data3;
	int   nTrPinsFirst;
	int   nTrPinsSecond;
	SConnectStruct* nodePointer;
	int   nWorldTileX;
	int   nWorldTileY;
	int   nWorldFileUid;
	STDBFile* tdbFilePtr;
	DWORD data11;
	float X;
	float Y;
	float Z;
	float AX;
	float AY;
	float AZ;
	int   nTileX;
	int   nTileZ;
	short data;
	short direction;
	short direction2;
};
struct SSectionArray;
struct STrItemArray;
struct STrItem;
struct SAllTrItem
{
	STrItem *allItem;
	void*   ptr;
	int     nItemNum;
	DWORD   data;
};
struct STDBFile
{
	STrackNode* trackNodes;
	int TrackNodeNumber;
	int TrackNodeNumber2;
	int NodeNumMinus1;
	int nBufferSize;
	int NodeNumMinus2;
	void* pPtr;
	SAllTrItem* allTrItemPtr;
	wchar_t TDBFileName[0x400];
};
struct STrackNode
{
	DWORD data1;
	DWORD data2;
	SConnectNode* nodePtr1;
	DWORD data4;
	SConnectNode* nodePtr2;
	DWORD data6;
	SSectionArray* sectionArrayPtr;
	int   nSectionNum;
	STrItemArray*  trItemArrayPtr;
	int   nTrItemNum;
	float fSectionLength;
	void* pPtr;
	STDBFile* tdbFilePtr;
};
struct STrackSection;
struct STrackInfo
{
	STrackNode*     trackNodePtr;
	int             nLeftNodeNum;
	STrackSection*  sectionPtr;
	int             nDirection;
	float           fNodeLeftLength;
	float           fSectionLeftLength;
};//0x8098DC and 0x809944
// some other data I have not recognize them.
bool GetTrainHandle(HANDLE &hProcess);
void *GetTrainPointer(HANDLE hProcess);

#endif