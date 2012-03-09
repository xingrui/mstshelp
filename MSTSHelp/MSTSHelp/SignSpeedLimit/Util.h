#ifndef UTIL_H
#define UTIL_H
#include <vector>
using std::vector;
#define THIS_POINTER_MEM 0x7C2AC0
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
	int         WorldFileUiD;  //0x77
	DWORD       data6;  //0
	int         TileX2;  //-846
	int         TileZ2;  //0x3618
	float       X;      //330.14
	float       Y;      //103.58
	float       Z;      //-152.46
	float       AX;     //0
	float       AY;     //5.16
	float       AZ;     //0
	float       unData; //-1, 0
	float		unData2;
};
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
	short direction2;// I use this direction2 but I do not know whether the difference of the direction.
};
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
	SConnectNode* connectNodePtr1;
	DWORD data4;
	SConnectNode* connectNodePtr2;
	DWORD data6;
	SSectionData* sectionArrayPtr;
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
	float SpeedpostTrItemDataFourth;
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
	int   LimitNum;
	SSpeedPostLimit(float dis, int num):fDistance(dis),LimitNum(num){}
};

struct SStationItem
{
	float fDistance;
	CString stationName;
	SStationItem(float dis, CString sName):fDistance(dis),stationName(sName){}
};

void AddSpeedPostLimit(float currentDistance, const STrackNode& node, vector<SSpeedPostLimit>& limitVect, HANDLE, int direction);
void AddStationItem(float currentDistance, const STrackNode& node, vector<SStationItem>& limitVect, HANDLE handle, int direction);
CString SpeedPostItemToString(const SSpeedPostItem& item);
bool GetTrainHandle(HANDLE &hProcess);
void *GetTrainPointer(HANDLE hProcess);
void process_AX(float* fArray, float AX);
void process_AY(float* fArray, float AY);
void process_AZ(float* fArray, float AZ);

#endif