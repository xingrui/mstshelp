#ifndef UTIL_H
#define UTIL_H
#include <vector>
using std::vector;
#define IDENTITY_MATRIX_MEM 0x771680
#define DISTANCE_TYPE_MEM 0x78C390
#define CURRENT_TILE_X_MEM 0x79D118
#define CURRENT_TILE_Y_MEM 0x79D11C
#define IDENTITY2_MATRIX_MEM 0x7A8A20
#define THIS_POINTER_MEM 0x7C2AC0
#define TASK_LIMIT_HEAD_MEM 0x809B38
#define TASK_LIMIT_MEM 0x809B48
#define TRAIN_INFO_MEM 0x809890
#define HEAD_TRACK_MEM 0x8098DC
#define TAIL_TRACK_MEM 0x809944
struct STrackNode;

struct SSectionData
{
	short       sectionIndex;//179
	short       shapeIndex;  //32250
	DWORD       data2;  //0
	int         TileX;  //-846
	int         TileY;  //0x3618
	///////////////////////////////////////////
	int         WorldFileUiD;  //0x77
	DWORD       data6;  //0
	int         TileX2;  //-846
	int         TileZ2;  //0x3618
	///////////////////////////////////////////
	float       X;      //330.14
	float       Y;      //103.58
	float       Z;      //-152.46
	float       AX;     //0
	///////////////////////////////////////////
	float       AY;     //5.16
	float       AZ;     //0
	int       unData56; //-1, 0
	float		unData60;
};

struct SSectionTypeData
{
	float fData[6];
};

struct SSubConnectStruct
{
	STrackNode* nodePtr;
	int nDirect;
};
struct SConnectStruct
{
	SSubConnectStruct subStruct[3];
};
struct STDBFile;
struct SConnectNode
{
	int nType0;// 2 JunctionNode || 3 EndNode
	DWORD data4;
	DWORD data8;
	size_t   nTrPinsFirst12;
	//////////////////////////////////
	size_t   nTrPinsSecond16;
	SConnectStruct* nodePointer20;
	int   nWorldTileX;
	int   nWorldTileY;
	//////////////////////////////////
	int   nWorldFileUid;
	STDBFile* tdbFilePtr;
	DWORD data11;
	float X;
	//////////////////////////////////
	float Y;
	float Z;
	float AX;
	float AY;
	//////////////////////////////////
	float AZ;
	int   nTileX;
	int   nTileZ;
	short data;
	short direction;
	//////////////////////////////////
	short direction80;// I use this direction2 but I do not know whether the difference of the direction.
};
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
	DWORD data0;
	int data4;
	SConnectNode* connectNodePtr8;
	DWORD data12;
	SConnectNode* connectNodePtr16;
	DWORD data20;
	SSectionData* sectionArrayPtr24;
	int   nSectionNum28;
	STrItem**  trItemArrayPtr32;
	int   nTrItemNum36;
	float fTrackNodeLength40;
	void* pPtr44;
	STDBFile* tdbFilePtr48;
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
};
enum ItemType
{
	EmptyOrSignalItem = 0,
	PickupItem = 2,
	PlatFormItem = 3,
	SidingItem = 6,
	LevelCtItem = 7,
	SpeedPostItem = 8,
	SoundRegionItem = 10,
	CrossOverItem = 11,
};
struct SSpeedPostItem{
	int nType;//8
	int nSubType;//2     
	int unknown3;//0
	float fLocationInTrackNode;//TrItemSDataFirst
	int TrItemSDataSecond;
	float TrItemPDataFirst;
	float TrItemPDataSecond;
	int TrItemPDataThird;
	int TrItemPDataFourth;
	int variableData;
	short SpeedpostTrItemDataFirst;
	short SpeedpostTrItemDataSecond;//This is the Limit Number
	float SpeedpostTrItemDataThird;
	float fAngle;
	DWORD fData;//0
};

struct SPlatformItem
{
	int type;//3
	int subType;
	int unknown;
	float fLocationInTrackNode;//TrItemSDataFirst
	int   nTrItemSDataSecond;
	float fTrItemRDataFirst;
	float fTrItemRDataThird;
	int   nTrItemRDataFourth;
	int   nTrItemRDataFifth;
	DWORD data;
	wchar_t* platformName;
	wchar_t* stationName;
};

struct SSidingItem
{
	int type; // 6
	int subType;
	int unknown;
	float fLocationInTrackNode;//TrItemSDataFirst
	int   nTrItemSDataSecond;
	wchar_t* sidingName;
	int   nSidingTrItemDataFirst;
	int   nSidingTrItemDataSecond;
};

struct SSpeedPostLimit
{
	float fDistance;
	float   fLimitNum;
	SSpeedPostLimit(float dis, float num):fDistance(dis),fLimitNum(num){}
};

struct STempSpeedLimit
{
	float fDistanceStart;
	float fDistanceEnd;
	STempSpeedLimit(float f1, float f2):fDistanceStart(f1), fDistanceEnd(f2){}
};

struct SStationItem
{
	float fDistance;
	CString stationName;
	SStationItem(float dis, CString sName):fDistance(dis),stationName(sName){}
};

struct SProcessData
{
	const STrackNode* nodePtr0;
	int nSectionNum4;
	SSectionData* sectionPtr8;
	size_t nData12;
	float fDistanceFromNodeStart16;
	float fDistanceFromSectionStart20;
	float fAngle24[3];
	float fMatrix[9];
	float fXYZ72[3];// 72 76 80
	DWORD nData84;
	int nData88;
	float nData92;
	DWORD nData96;
	float fData100;
};

struct STempSpeed
{
	STrackNode* nodePtr;
	float fStart;
	float fEnd;
};
void AddTempSpeedLimit(float currentDistance, STrackNode* node, vector<STempSpeedLimit>& limitVect, HANDLE handle, int direction);
void AddSpeedPostLimit(float currentDistance, const STrackNode& node, vector<SSpeedPostLimit>& limitVect, HANDLE, int direction, STrackNode*);
void AddStationItem(float currentDistance, const STrackNode& node, vector<SStationItem>& stationVect, vector<SStationItem>& sidingVect, HANDLE handle, int direction);

CString SpeedPostItemToString(const SSpeedPostItem& item);
bool GetTrainHandle(HANDLE &hProcess);
void *GetTrainPointer(HANDLE hProcess);
STrackNode* GetNext(STrackNode* nodePtr, const SConnectStruct& connectStruct, const SConnectNode& connectNode, 
					int direction, int&nextDirect);
STrackNode* GetNextNode(HANDLE handle, const STrackNode& node, STrackNode* nodePtr, int direction, int&nextDirect);

void process_AX(float* fArray, float AX);
void process_AY(float* fArray, float AY);
void process_AZ(float* fArray, float AZ);
float* process(HANDLE handle, float* fMatrix, float*fXYZ);
float* process30(HANDLE handle, float* fMatrix, float*fXYZ);

bool IsSpeedPostValid(HANDLE handle, float angle, float fLocationInTrackNode, int nDirection, const STrackNode& node, STrackNode* nodePtr);
CString IteratorList(HANDLE handle, void* head, CString (*func)(HANDLE, void*));
void getSectionData_Modified(HANDLE handle, SProcessData& processData, const STrackNode& node, SSectionTypeData* basePtr);
int AdjustAngle_Modified(HANDLE handle, SProcessData& processData, const STrackNode& node, float fLocation, SSectionTypeData* basePtr);

inline void ReadTrainProcess(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	if(!ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, NULL))
		throw 1;

}

inline float inner_product(float* fArray1, float* fArray2)
{
	return fArray1[0] * fArray2[0] + fArray1[1]*fArray2[1]+fArray1[2]*fArray2[2];
}


inline CString showTrackInfo(const STrackInfo& trackInfo)
{
	CString msg;
	msg.Format(L"nLeftNodeNum : %d, Direct %d, LeftLen : %f, SectLen : %f",
		trackInfo.nLeftNodeNum, trackInfo.nDirection, trackInfo.fNodeLeftLength, trackInfo.fSectionLeftLength);
	return msg;
}

#endif