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
	unsigned short       sectionIndex;//179
	unsigned short       shapeIndex;  //32250
	DWORD       data4;  //0
	int         TileX;  //-846
	int         TileY;  //0x3618
	int         WorldFileUiD;  //0x77
	DWORD       data20;  //0
	int         TileX2;  //-846
	int         TileZ2;  //0x3618
	float       X;      //330.14
	float       Y;      //103.58
	float       Z;      //-152.46
	float       AX;     //0
	float       AY;     //5.16
	float       AZ;     //0
	int         unData56; //-1, 0
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
	DWORD data4; // 不知道是什么意思
	DWORD data8; // 不知道是什么意思
	size_t   nTrPinsFirst12; // TrPins的第一数据 总是1
	size_t   nTrPinsSecond16; // TrPins的第二个数据 JunctionNode是2 EndNode是0
	SConnectStruct* nodePointer20; // 卧槽，又是指针，等着吧，下面继续又是结构体
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
	unsigned short data;
	unsigned short direction;
	unsigned short direction80;//这个数据非常重要，这个是真正决定道岔走向的数据，只在是JunctionNode的时候有效
	// 还记得我们的JunctionNode里面的TrPins数据吧，不记得到前面的TDB文件介绍看看
	// JunctionNode里面会有三个 这个东西 TrPin ( ConnectNodeIndex Direction ) 
	// 这个东西就是存储在nodePointer20指针(本结构体的第六个数据)指向的内容里面的。
	// 第一个TrPin是道岔的一个出口方向，第二个TrPin和第三个TrPin是可以选择的。
	// 也就是说这个JunctionNode可以是由第一个连接到第二个，也可以是由第一个连接到第三个
	// 如果direction80为0， 表示道岔连接第二个TrPin，如果direction80为1，表示道岔连接第三个TrPin
	// 希望我描述得足够明白了。
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
	SConnectNode* InConnectNodePtr;
	DWORD data12;
	SConnectNode* OutConnectNodePtr;
	DWORD data20;
	SSectionData* sectionArrayPtr;
	int   nSectionNum;
	STrItem**  trItemArrayPtr;
	int   nTrItemNum;
	float fTrackNodeLength;
	void* pPtr44;
	STDBFile* tdbFilePtr;
};
struct STrackInfo
{
	STrackNode*     trackNodePtr;
	int             nCurrentSectionNum;
	SSectionData*  sectionPtr;
	int             nDirection;
	float           fLocationInNode;
	float           fLocationInSection;
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
	ItemType nType;//8
	int nSubType; //2     
	int unknown3; //0
	float fLocationInTrackNode; // 该Item在VectorNode当中的位置
	int TrItemSDataSecond;
	float TrItemPDataFirst;
	float TrItemPDataSecond;
	int TrItemPDataThird;
	int TrItemPDataFourth;
	int variableData;
	unsigned short sSpeedPostType; // 这个是该标志限速的类型
	// 这个类型很复杂，重点介绍一下吧。
	// 1. & 就是按位与 ( sSpeedPostType & 7) 的结果是2的时候，这个Item才是标志限速
	// 如果是其他数值就表示是里程表之类的。
	// 2. sSpeedPostType  & 0x80 为1表示这个限速对客车货车都有效
	// 3. sSpeedPostType  & 0x20 为1表示这个限速对客车有效 (*0x809890)&6 为2表示当前车辆是客车
	// 4. sSpeedPostType  & 0x40 为1表示这个限速对货车有效 (*0x809890)&6 为4表示当前车辆是货车
	// 5. sSpeedPostType  & 0x100 为1表示限速的单位为英里/小时，为0表示限速的单位为公里/小时
	unsigned short SpeedpostTrItemDataSecond;// 这个是该标志限速的数值
	float SpeedpostTrItemDataThird;
	float fAngle; // 这个是该标志限速相对于原始坐标系在XZ平面上面旋转的角度
	DWORD fData;//0
};

struct SPlatformItem
{
	int type;//3
	int subType;
	int unknown;
	float fLocationInTrackNode; // 该Item在VectorNode当中的位置
	int   nTrItemSDataSecond;   // TrItemSData的第二个数据
	float fTrItemRDataFirst;    // 同上面的，顾名思义吧
	float fTrItemRDataThird;
	int   nTrItemRDataFourth;
	int   nTrItemRDataFifth;
	DWORD data;
	wchar_t* platformName;      // 指向站台名称
	wchar_t* stationName;       // 指向车站名称
};

struct SSidingItem
{
	int type; // 6
	int subType;
	int unknown;
	float fLocationInTrackNode; // 该Item在VectorNode当中的位置
	int   nTrItemSDataSecond;
	wchar_t* sidingName;   // 指向边线的名称
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
struct SNode
{
	SNode* next;
	SNode* prev;
	void* pointer;
};
void AddTempSpeedLimit(float currentDistance, STrackNode* node, vector<STempSpeedLimit>& limitVect, HANDLE handle, int direction);
void AddSpeedPostLimit(float currentDistance, const STrackNode& node, vector<SSpeedPostLimit>& limitVect, HANDLE, int direction, STrackNode*);
void AddStationItem(float currentDistance, const STrackNode& node, vector<SStationItem>& stationVect, vector<SStationItem>& sidingVect, HANDLE handle, int direction);

CString SpeedPostItemToString(const SSpeedPostItem& item);
bool GetTrainHandle(HANDLE &hProcess);
void *GetTrainPointer(HANDLE hProcess);
STrackNode* GetNext(STrackNode* nodePtr, const SConnectStruct& connectStruct, const SConnectNode& connectNode, 
					int&nextDirect);
STrackNode* GetNextNode(HANDLE handle, const STrackNode& node, STrackNode* nodePtr, int direction, int&nextDirect);

void process_AX(float* fArray, float AX);
void process_AY(float* fArray, float AY);
void process_AZ(float* fArray, float AZ);
float* process(HANDLE handle, float* fMatrix, float*fXYZ);
float* process30(HANDLE handle, float* fMatrix, float*fXYZ);

bool IsSpeedPostValid(HANDLE handle, float angle, float fLocationInTrackNode, int nDirection, const STrackNode& node, STrackNode* nodePtr);
CString IteratorList(HANDLE handle, void* head, CString (*func)(HANDLE, void*));
void getSectionData_Modified(HANDLE handle, SProcessData& processData, const STrackNode& node,int, int , SSectionTypeData* basePtr);
int AdjustAngle_Modified(HANDLE handle, SProcessData& processData, const STrackNode& node, float fLocation, SSectionTypeData* basePtr);

void getSectionData(HANDLE handle, SProcessData& processData, const STrackNode& node, int sectionNum, float* fArray, SSectionTypeData* basePtr);
int AdjustAngle(HANDLE handle, SProcessData& processData, const STrackNode& node, float fLocation, SSectionTypeData* basePtr);

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
		trackInfo.nCurrentSectionNum, trackInfo.nDirection, trackInfo.fLocationInNode, trackInfo.fLocationInSection);
	return msg;
}

#endif