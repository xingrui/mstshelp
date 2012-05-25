#ifndef UTIL_H
#define UTIL_H
#include <vector>
using std::vector;
#define IDENTITY_MATRIX_MEM 0x771680
// 单位矩阵[1,0,0,0,1,0,0,0,1]其实没有必要从这里读，直接在程序中初始化就可以

#define DISTANCE_TYPE_MEM 0x78C390
// 距离单位的类型 1为公里 0为英里

#define CURRENT_TILE_X_MEM 0x79D118
#define CURRENT_TILE_Z_MEM 0x79D11C
// 这两个数据程序中没有用到，应该是当前的X值和Z值，线路设计中经常看到

#define IDENTITY2_MATRIX_MEM 0x7A8A20
// 又是一个矩阵[1,0,0,0,1,0,0,0,1,0,0,0]

#define THIS_POINTER_MEM 0x7C2AC0
// 很重要的指针，此处为空表示没有任务运行，不为空表示任务正在运行

#define TASK_LIMIT_HEAD_MEM 0x809B38
// 任务中的临时限速的链表表头，就是F4左边用红色线显示的那个

#define TASK_LIMIT_MEM 0x809B48
// 任务的临时限速值，线路限速值等信息的存放地方

#define TRAIN_INFO_MEM 0x809890
// 火车车头的一些信息值的存放地方

#define HEAD_TRACK_MEM 0x8098DC
// 车头所在的TrackNode的地址

#define TAIL_TRACK_MEM 0x809944
// 车尾所在的TrackNode的地址
struct SVectorNode;

struct SVectorSection
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
	SVectorNode *nodePtr;
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
	SConnectStruct *nodePointer20; // 卧槽，又是指针，等着吧，下面继续又是结构体
	int   nWorldTileX;
	int   nWorldTileY;
	int   nWorldFileUid;
	STDBFile *tdbFilePtr;
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
	void   *ptr;
	int     nItemNum;
};
struct STDBFile
{
	SVectorNode *trackNodes;
	int TrackNodeNumber;
	int TrackNodeNumber2;
	int NodeNumMinus1;
	int nBufferSize;
	int NodeNumMinus2;
	void *pPtr;
	SAllTrItem *allTrItemPtr;
	wchar_t TDBFileName[0x400];
};
struct SVectorNode
{
	DWORD data0;
	int data4;
	SConnectNode *InConnectNodePtr;
	DWORD data12;
	SConnectNode *OutConnectNodePtr;
	DWORD data20;
	SVectorSection *sectionArrayPtr;
	int   nSectionNum;
	STrItem  **trItemArrayPtr;
	int   nTrItemNum;
	float fTrackNodeLength;
	void *pPtr44;
	STDBFile *tdbFilePtr;
};
struct STrackInfo
{
	SVectorNode     *vectorNodePtr;
	int             nCurrentSectionNum;
	SVectorSection  *sectionPtr;
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
struct SSectionInfo
{
	float fStart;
	float fEnd;
	int nDirection;
	float fRadius;
	float fAngle;
	SSectionInfo(float start, float end, int direction, float radius, float angle): fStart(start), fEnd(end), nDirection(direction), fRadius(radius), fAngle(angle) {};
};

struct SProcessData
{
	const SVectorNode *nodePtr0;
	int nSectionNum4;
	SVectorSection *sectionPtr8;
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
struct SNode
{
	SNode *next;
	SNode *prev;
	void *pointer;
};
struct STrackSection
{
	float fSectionSizeSecondLength0; // This value is 0 when this section is not straight, so this value is calculated by data in SectionCurve
	float fSectionCurveFirstRadius4;
	float fSectionCurveSecondAngle8;
	float fSectionSizeFirstGaugeC;
	float fSectionSkew10; // a degree value much like the SectionCurve describing the direction of the preceding SectionSize. As with curves you need to define skewed TrackSections in pairs with the negative skew first. This makes for a straight path with a slant, not very usefull.
	float fSectionFlags14;
};
void AddSectionInfo(float currentDistance, const SVectorNode &node, vector<SSectionInfo>& sectionVect, HANDLE handle, int nDirection, int nDirectionOfItemToFind, STrackSection *pSection);

bool ReadPointerMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, int num, ...);
bool GetTrainHandle(HANDLE &hProcess);
void *GetTrainPointer(HANDLE hProcess);
SVectorNode *GetNext(SVectorNode *nodePtr, const SConnectStruct &connectStruct, const SConnectNode &connectNode,
                     int &nextDirect);
SVectorNode *GetNextNode(HANDLE handle, const SVectorNode &node, SVectorNode *nodePtr, int direction, int &nextDirect);


void getSectionData(HANDLE handle, SProcessData &processData, const SVectorNode &node, int sectionNum, float *fArray, SSectionTypeData *basePtr);

inline void ReadTrainProcess(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	if (!ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, NULL))
		throw 1;
}

#endif