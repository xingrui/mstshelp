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
	STrackNode *nodePtr;
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
	DWORD   data;
};
struct STDBFile
{
	STrackNode *trackNodes;
	int TrackNodeNumber;
	int TrackNodeNumber2;
	int NodeNumMinus1;
	int nBufferSize;
	int NodeNumMinus2;
	void *pPtr;
	SAllTrItem *allTrItemPtr;
	wchar_t TDBFileName[0x400];
};
struct STrackNode
{
	DWORD data0;
	int data4;
	SConnectNode *InConnectNodePtr;
	DWORD data12;
	SConnectNode *OutConnectNodePtr;
	DWORD data20;
	SSectionData *sectionArrayPtr;
	int   nSectionNum;
	STrItem  **trItemArrayPtr;
	int   nTrItemNum;
	float fTrackNodeLength;
	void *pPtr44;
	STDBFile *tdbFilePtr;
};
struct STrackInfo
{
	STrackNode     *trackNodePtr;
	int             nCurrentSectionNum;
	SSectionData  *sectionPtr;
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
struct SSpeedPostItem
{
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

struct SSignalItem
{
	ItemType nType;//0
	int nTrItemIndexInTrackNode;
	int nData8;
	float fLocationInTrackNode; // 该Item在VectorNode当中的位置
	DWORD dwData10;
	void *ptr14;
	DWORD dwData18;
	DWORD dwData1C; // 0x8000
	BYTE bData20[4];
	int nItemDirection;
	DWORD dwData28;
	DWORD dwData2C;
};

struct SPlatformItem
{
	int type;//3
	int nTrItemIndexInTrackNode;
	int unknown;
	float fLocationInTrackNode; // 该Item在VectorNode当中的位置
	int   nTrItemSDataSecond;   // TrItemSData的第二个数据
	float fTrItemRDataFirst;    // 同上面的，顾名思义吧
	float fTrItemRDataThird;
	int   nTrItemRDataFourth;
	int   nTrItemRDataFifth;
	DWORD data;
	wchar_t *platformName;      // 指向站台名称
	wchar_t *stationName;       // 指向车站名称
};

struct SSidingItem
{
	int type; // 6
	int nTrItemIndexInTrackNode;
	int unknown;
	float fLocationInTrackNode; // 该Item在VectorNode当中的位置
	int   nTrItemSDataSecond;
	wchar_t *sidingName;   // 指向边线的名称
	int   nSidingTrItemDataFirst;
	int   nSidingTrItemDataSecond;
};

struct SSpeedPostLimit
{
	float fDistance;
	float   fLimitNum;
	SSpeedPostLimit(float dis, float num): fDistance(dis), fLimitNum(num) {}
};

struct STempSpeedLimit
{
	float fDistanceStart;
	float fDistanceEnd;
	STempSpeedLimit(float f1, float f2): fDistanceStart(f1), fDistanceEnd(f2) {}
};

struct SStationItem
{
	float fDistance;
	CString stationName;
	SStationItem(float dis, CString sName): fDistance(dis), stationName(sName) {}
};

struct SShowSignalItem
{
	float fDistance;
	float fSignalSpeed;
	int nLightColor;
	SShowSignalItem(float dis, float signalSpeed, int color): fDistance(dis), fSignalSpeed(signalSpeed), nLightColor(color) {}
};

struct SProcessData
{
	const STrackNode *nodePtr0;
	int nSectionNum4;
	SSectionData *sectionPtr8;
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
	STrackNode *nodePtr;
	float fStart;
	float fEnd;
};
struct SNode
{
	SNode *next;
	SNode *prev;
	void *pointer;
};
struct S80A9D8
{
	DWORD dwData0;
	SNode *WagFileDBList4;
	DWORD dwData8;
	SNode *EngFileDBListC;
	DWORD dwData10;
	SNode *ConFileDBList14;
};
CString changeColorToString(char cLightColor);
struct SWagFile // Size C0C
{
	DWORD dwMagicData0; // 0x773230
	DWORD dwMagicData4; // same with strangeData;
	wchar_t nameExceptExtendedName8[0x20]; // SS9_0195 占用2*0x20 = 0x40个BYTE
	wchar_t nameWagonShape48[0x20]; // 占用2*0x20 = 0x40个BYTE
	BYTE nLocomotiveType88[4]; //0x88 sub_608800() 1 客车 2 货车
	DWORD dwData8C; //
	int nCarriageCnt90; // sub_5A5EB1()
	SNode *dbLinkedList94; // sub_5A5EB1() 数据指针的类型为 sub_5A5EB1() 每一个车厢对应一个节点
	float fMaxSpeed98; // sub_5A5EB1()
	DWORD dwData9C;
	DWORD dwDataA0;
	DWORD dwDataA4;
	DWORD dwDataA8;
	DWORD dwDataAC;
	DWORD dwDataB0;
	DWORD dwDataB4;
	DWORD dwDataB8;
	DWORD dwDataBC;
	DWORD dwDataC0;
	DWORD dwDataC4;
	DWORD dwDataC8;
	DWORD dwDataCC;
	DWORD dwDataD0;
	DWORD dwDataD4;
	DWORD dwDataD8;
	DWORD dwDataDC;
	DWORD dwDataE0;
	DWORD dwDataE4;
	DWORD dwDataE8;
	DWORD dwDataEC;
	DWORD dwDataF0;
	DWORD dwDataF4;
	DWORD dwDataF8;
	DWORD dwDataFC;
	DWORD dwData100;
	DWORD dwData104;
	DWORD dwData108;
	DWORD dwData10C;
	DWORD dwData110;
	DWORD dwData114;
	DWORD dwData118;
	DWORD dwData11C;
	DWORD dwData120;
	DWORD dwData124;
	DWORD dwData128;
	DWORD dwData12C;
	DWORD dwData130;
	DWORD dwData134;
	DWORD dwData138;
	DWORD dwData13C;
	DWORD dwData140;
	DWORD dwData144;
	DWORD dwData148;
	DWORD dwData14C;
	DWORD dwData150;
	DWORD dwData154;
	DWORD dwData158;
	DWORD dwData15C;
	DWORD dwData160;
	DWORD dwData164;
	DWORD dwData168;
	DWORD dwData16C;
	DWORD dwData170;
	DWORD dwData174;
	DWORD dwData178;
	DWORD dwData17C;
	DWORD dwData180;
	DWORD dwData184;
	DWORD dwData188;
	DWORD dwData18C;
	DWORD dwData190;
	DWORD dwData194;
	DWORD dwData198;
	DWORD dwData19C;
	DWORD dwData1A0;
	DWORD dwData1A4;
	DWORD dwData1A8;
	DWORD dwData1AC;
	DWORD dwData1B0;
	DWORD dwData1B4;
	DWORD dwData1B8;
	DWORD dwData1BC;
	DWORD dwData1C0;
	DWORD dwData1C4;
	DWORD dwData1C8;
	DWORD dwData1CC;
	DWORD dwData1D0;
	DWORD dwData1D4;
	DWORD dwData1D8;
	DWORD dwData1DC;
	DWORD dwData1E0;
	DWORD dwData1E4;
	DWORD dwData1E8;
	DWORD dwData1EC;
	DWORD dwData1F0;
	DWORD dwData1F4;
	DWORD dwData1F8;
	DWORD dwData1FC;
	DWORD dwData200;
	DWORD dwData204;
	DWORD dwData208;
	DWORD dwData20C;
	DWORD dwData210;
	DWORD dwData214;
	DWORD dwData218;
	DWORD dwData21C;
	DWORD dwData220;
	DWORD dwData224;
	DWORD dwData228;
	DWORD dwData22C;
	DWORD dwData230;
	DWORD dwData234;
	DWORD dwData238;
	DWORD dwData23C;
	DWORD dwData240;
	DWORD dwData244;
	DWORD dwData248;
	DWORD dwData24C;
	DWORD dwData250;
	DWORD dwData254;
	DWORD dwData258;
	DWORD dwData25C;
	DWORD dwData260;
	DWORD dwData264;
	DWORD dwData268;
	DWORD dwData26C;
	DWORD dwData270;
	DWORD dwData274;
	DWORD dwData278;
	DWORD dwData27C;
	DWORD dwData280;
	DWORD dwData284;
	DWORD dwData288;
	DWORD dwData28C;
	DWORD dwData290;
	DWORD dwData294;
	DWORD dwData298;
	DWORD dwData29C;
	DWORD dwData2A0;
	DWORD dwData2A4;
	DWORD dwData2A8;
	DWORD dwData2AC;
	DWORD dwData2B0;
	DWORD dwData2B4;
	DWORD dwData2B8;
	DWORD dwData2BC;
	DWORD dwData2C0;
	DWORD dwData2C4;
	DWORD dwData2C8;
	DWORD dwData2CC;
	DWORD dwData2D0;
	DWORD dwData2D4;
	DWORD dwData2D8;
	DWORD dwData2DC;
	DWORD dwData2E0;
	DWORD dwData2E4;
	DWORD dwData2E8;
	DWORD dwData2EC;
	DWORD dwData2F0;
	DWORD dwData2F4;
	DWORD dwData2F8;
	DWORD dwData2FC;
	DWORD dwData300;
	DWORD dwData304;
	DWORD dwData308;
	DWORD dwData30C;
	DWORD dwData310;
	DWORD dwData314;
	DWORD dwData318;
	DWORD dwData31C;
	DWORD dwData320;
	DWORD dwData324;
	DWORD dwData328;
	DWORD dwData32C;
	DWORD dwData330;
	DWORD dwData334;
	DWORD dwData338;
	DWORD dwData33C;
	DWORD dwData340;
	DWORD dwData344;
	DWORD dwData348;
	DWORD dwData34C;
	DWORD dwData350;
	DWORD dwData354;
	DWORD dwData358;
	DWORD dwData35C;
	DWORD dwData360;
	DWORD dwData364;
	DWORD dwData368;
	DWORD dwData36C;
	DWORD dwData370;
	DWORD dwData374;
	DWORD dwData378;
	DWORD dwData37C;
	DWORD dwData380;
	DWORD dwData384;
	DWORD dwData388;
	DWORD dwData38C;
	DWORD dwData390;
	DWORD dwData394;
	DWORD dwData398;
	DWORD dwData39C;
	DWORD dwData3A0;
	DWORD dwData3A4;
	DWORD dwData3A8;
	DWORD dwData3AC;
	DWORD dwData3B0;
	DWORD dwData3B4;
	DWORD dwData3B8;
	DWORD dwData3BC;
	DWORD dwData3C0;
	DWORD dwData3C4;
	DWORD dwData3C8;
	float fBuffers_Spring_StiffnessFirst3CC;
	float fBuffers_Spring_StiffnessSecond3D0;
	float fBuffers_Spring_DampingFirst3D4;
	float fBuffers_Spring_DampingSecond3D8;
	DWORD dwData3DC; // 0
	DWORD dwData3E0; // 0
	DWORD dwData3E4; // 0
	DWORD dwData3E8; // 0
	float fBuffers_Center3EC;
	float fBuffers_Radius3F0;
	float fBuffers_Angle3F4;
	float fCarriageWidth3F8;
	float fCarriageHeight3FC;
	float fCarriageLength400; //0x400 sub_608800() 很可能是车厢的长度
	DWORD dwData404;
	DWORD dwData408;
	DWORD dwData40C;
	float fCentreOfGravityX410;
	float fCentreOfGravityY414;
	float fCentreOfGravityZ418;
	DWORD dwData41C;
	DWORD dwData420;
	DWORD dwData424;
	DWORD dwData428;
	DWORD dwData42C;
	DWORD dwData430;
	DWORD dwData434;
	DWORD dwData438;
	DWORD dwData43C;
	DWORD dwData440;
	float fCarriageWeight444; //0x444 sub_608800() 很可能是车厢的重量
	float fWheelRadius448; // 车轮的半径
	DWORD dwData44C;
	DWORD dwData450;
	DWORD dwData454;
	float fNumWheels458;
	DWORD dwData45C;
	DWORD dwData460;
	DWORD dwData464;
	float Adhesion1_468;
	float Adhesion2_46C;
	float Adhesion3_470;
	float Adhesion4_474;
	DWORD dwData478;
	DWORD dwData47C;
	DWORD dwData480;
	DWORD dwData484;
	DWORD dwData488;
	DWORD dwData48C;
	DWORD dwData490;
	DWORD dwData494;
	DWORD dwData498;
	DWORD dwData49C;
	DWORD dwData4A0;
	DWORD dwData4A4;
	DWORD dwData4A8;
	DWORD dwData4AC;
	DWORD dwData4B0;
	DWORD dwData4B4;
	DWORD dwData4B8;
	DWORD dwData4BC;
	DWORD dwData4C0;
	DWORD dwData4C4;
	float fData4C8;
	float fDerailRailHeight4CC;
	float fDerailBufferForce4D0;
	float fDerailRailForce4D4;
	DWORD dwData4D8;
	DWORD dwData4DC;
	DWORD dwData4E0;
	DWORD dwData4E4;
	DWORD dwData4E8;
	wchar_t namePassengerCabinFile4EC[0x20];
	DWORD dwData52C;
	DWORD dwData530;
	DWORD dwData534;
	DWORD dwData538;
	DWORD dwData53C;
	DWORD dwData540;
	DWORD dwData544;
	DWORD dwData548;
	DWORD dwData54C;
	DWORD dwData550;
	DWORD dwData554;
	DWORD dwData558;
	DWORD dwData55C;
	DWORD dwData560;
	DWORD dwData564;
	DWORD dwData568;
	DWORD dwData56C;
	DWORD dwData570;
	DWORD dwData574;
	DWORD dwData578;
	DWORD dwData57C;
	DWORD dwData580;
	DWORD dwData584;
	DWORD dwData588;
	DWORD dwData58C;
	DWORD dwData590;
	DWORD dwData594;
	DWORD dwData598;
	DWORD dwData59C;
	DWORD dwData5A0;
	DWORD dwData5A4;
	DWORD dwData5A8;
	DWORD dwData5AC;
	DWORD dwData5B0;
	DWORD dwData5B4;
	DWORD dwData5B8;
	DWORD dwData5BC;
	DWORD dwData5C0;
	DWORD dwData5C4;
	DWORD dwData5C8;
	DWORD dwData5CC;
	DWORD dwData5D0;
	DWORD dwData5D4;
	DWORD dwData5D8;
	DWORD dwData5DC;
	DWORD dwData5E0;
	DWORD dwData5E4;
	DWORD dwData5E8;
	DWORD dwData5EC;
	DWORD dwData5F0;
	DWORD dwData5F4;
	DWORD dwData5F8;
	DWORD dwData5FC;
	DWORD dwData600;
	DWORD dwData604;
	DWORD dwData608;
	DWORD dwData60C;
	DWORD dwData610;
	DWORD dwData614;
	DWORD dwData618;
	float fPassengerCabinHeadPosX61C;
	float fPassengerCabinHeadPosY620;
	float fPassengerCabinHeadPosZ624;
	DWORD dwData628;
	DWORD dwData62C;
	DWORD dwData630;
	DWORD dwData634;
	DWORD dwData638;
	DWORD dwData63C;
	wchar_t InsideSound640[0x20];
	DWORD dwData680;
	DWORD dwData684;
	DWORD dwData688;
	DWORD dwData68C;
	DWORD dwData690;
	DWORD dwData694;
	DWORD dwData698;
	DWORD dwData69C;
	DWORD dwData6A0;
	DWORD dwData6A4;
	DWORD dwData6A8;
	DWORD dwData6AC;
	DWORD dwData6B0;
	DWORD dwData6B4;
	DWORD dwData6B8;
	DWORD dwData6BC;
	DWORD dwData6C0;
	DWORD dwData6C4;
	DWORD dwData6C8;
	DWORD dwData6CC;
	DWORD dwData6D0;
	DWORD dwData6D4;
	DWORD dwData6D8;
	DWORD dwData6DC;
	DWORD dwData6E0;
	DWORD dwData6E4;
	DWORD dwData6E8;
	DWORD dwData6EC;
	DWORD dwData6F0;
	DWORD dwData6F4;
	DWORD dwData6F8;
	DWORD dwData6FC;
	DWORD dwData700;
	DWORD dwData704;
	DWORD dwData708;
	DWORD dwData70C;
	DWORD dwData710;
	DWORD dwData714;
	DWORD dwData718;
	DWORD dwData71C;
	DWORD dwData720;
	DWORD dwData724;
	DWORD dwData728;
	DWORD dwData72C;
	DWORD dwData730;
	DWORD dwData734;
	DWORD dwData738;
	DWORD dwData73C;
	DWORD dwData740;
	DWORD dwData744;
	DWORD dwData748;
	DWORD dwData74C;
	DWORD dwData750;
	DWORD dwData754;
	DWORD dwData758;
	DWORD dwData75C;
	DWORD dwData760;
	DWORD dwData764;
	DWORD dwData768;
	DWORD dwData76C;
	DWORD dwData770;
	DWORD dwData774;
	DWORD dwData778;
	DWORD dwData77C;
	DWORD dwData780;
	DWORD dwData784;
	DWORD dwData788;
	DWORD dwData78C;
	DWORD dwData790;
	DWORD dwData794;
	DWORD dwData798;
	DWORD dwData79C;
	DWORD dwData7A0;
	DWORD dwData7A4;
	DWORD dwData7A8;
	DWORD dwData7AC;
	DWORD dwData7B0;
	DWORD dwData7B4;
	DWORD dwData7B8;
	DWORD dwData7BC;
	DWORD dwData7C0;
	DWORD dwData7C4;
	DWORD dwData7C8;
	DWORD dwData7CC;
	DWORD dwData7D0;
	DWORD dwData7D4;
	DWORD dwData7D8;
	DWORD dwData7DC;
	DWORD dwData7E0;
	DWORD dwData7E4;
	DWORD dwData7E8;
	DWORD dwData7EC;
	DWORD dwData7F0;
	DWORD dwData7F4;
	DWORD dwData7F8;
	DWORD dwData7FC;
	DWORD dwData800;
	DWORD dwData804;
	DWORD dwData808;
	DWORD dwData80C;
	DWORD dwData810;
	DWORD dwData814;
	DWORD dwData818;
	DWORD dwData81C;
	DWORD dwData820;
	DWORD dwData824;
	DWORD dwData828;
	DWORD dwData82C;
	DWORD dwData830;
	DWORD dwData834;
	DWORD dwData838;
	DWORD dwData83C;
	DWORD dwData840;
	DWORD dwData844;
	DWORD dwData848;
	DWORD dwData84C;
	DWORD dwData850;
	DWORD dwData854;
	DWORD dwData858;
	DWORD dwData85C;
	DWORD dwData860;
	DWORD dwData864;
	DWORD dwData868;
	DWORD dwData86C;
	DWORD dwData870;
	DWORD dwData874;
	DWORD dwData878;
	DWORD dwData87C;
	DWORD dwData880;
	DWORD dwData884;
	DWORD dwData888;
	DWORD dwData88C;
	DWORD dwData890;
	DWORD dwData894;
	DWORD dwData898;
	DWORD dwData89C;
	DWORD dwData8A0;
	DWORD dwData8A4;
	DWORD dwData8A8;
	DWORD dwData8AC;
	DWORD dwData8B0;
	DWORD dwData8B4;
	DWORD dwData8B8;
	DWORD dwData8BC;
	DWORD dwData8C0;
	DWORD dwData8C4;
	DWORD dwData8C8;
	DWORD dwData8CC;
	DWORD dwData8D0;
	DWORD dwData8D4;
	DWORD dwData8D8;
	DWORD dwData8DC;
	DWORD dwData8E0;
	DWORD dwData8E4;
	DWORD dwData8E8;
	DWORD dwData8EC;
	DWORD dwData8F0;
	DWORD dwData8F4;
	DWORD dwData8F8;
	DWORD dwData8FC;
	DWORD dwData900;
	DWORD dwData904;
	DWORD dwData908;
	DWORD dwData90C;
	DWORD dwData910;
	DWORD dwData914; // 0x914 sub_5D5C50() type
	float fMaxBrakeForce918; // 0x918 sub_5DB099()
	float fMaxHandBrakeForce91C;
	float fNumOfHandBrakeLeverSteps920;
	DWORD dwData924;
	float fData928;
	DWORD dwData92C;
	float fData930;
	float fEmergencyBrakeResMaxPressure934;
	float fData938;
	DWORD dwData93C;
	DWORD dwData940;
	float fTripleValveRatio944;
	float fMaxReleaseRate948;
	float fMaxApplicationRate94C;
	float fMaxAuxilaryChargingRate950;
	float fBrakeCylinderPressureForMaxBrakeBrakeForce954;
	float fData958;
	float fEmergencyResCapacity95C;
	float fEmergencyResChargingRate960;
	float fData964; // 15.00 K152-K55
	float fData968; // 1.50
	float fData96C; // 50.00
	float fData970; // 70.00
	float fData974; // 70.00
	float fData978; // 100.00
	float fData97C; // 25.00
	float fData980; // 20.00
	float fData984; // 40.00
	float fData988; // 2.00
	float fData98C; // 2.03
	float fData990; // 110.00
	DWORD dwData994;
	DWORD dwData998;
	DWORD dwData99C;
	wchar_t soundFileName9A0[0x60];
	DWORD dwDataA60[0x6B];
};
struct SEngFile // Size 0xD2C
{
	DWORD dwData0; // 0x773244
	DWORD dwData4;
	wchar_t engFileName8[0x20];
	wchar_t name48[0x20];
	BYTE nLocomotiveType88[4]; // 火车头的类型
	DWORD dwData8C;
	DWORD dwData90;
	DWORD dwData94;
	DWORD dwData98;
	DWORD dwData9C;
	DWORD dwDataA0;
	DWORD dwDataA4;
	DWORD dwDataA8;// A8 估计是一个bitvector 推断原因：sub_60CD5C()函数
	DWORD dwDataAC;
	DWORD dwDataB0;
	DWORD dwDataB4;
	DWORD dwDataB8;
	DWORD dwDataBC;
	DWORD dwDataC0;
	DWORD dwDataC4;
	DWORD dwDataC8;
	DWORD dwDataCC;
	DWORD dwDataD0;
	DWORD dwDataD4;
	DWORD dwDataD8;
	DWORD dwDataDC;
	DWORD dwDataE0;
	DWORD dwDataE4;
	DWORD dwDataE8;
	DWORD dwDataEC;
	DWORD dwDataF0;
	DWORD dwDataF4;
	DWORD dwDataF8;
	DWORD dwDataFC;
	DWORD dwData100;
	DWORD dwData104;
	short dwData108;
	float fMaxSpeed10A; // 车厢的最高速度 ------>STrainController.fMaxSpeed1F8
	DWORD dwData10E;
	DWORD dwData112;
	DWORD dwData116;
	DWORD dwData11A;
	DWORD dwData11E;
	DWORD dwData122;
	DWORD dwData126;
	DWORD dwData12A;
	DWORD dwData12E;
	DWORD dwData132;
	DWORD dwData136;
	DWORD dwData13A;
	DWORD dwData13E;
	DWORD dwData142;
	DWORD dwData146;
	DWORD dwData14A;
	DWORD dwData14E;
	DWORD dwData152;
	DWORD dwData156;
	DWORD dwData15A;
	DWORD dwData15E;
	DWORD dwData162;
	DWORD dwData166;
	DWORD dwData16A;
	DWORD dwData16E;
	DWORD dwData172;
	DWORD dwData176;
	DWORD dwData17A;
	DWORD dwData17E;
	DWORD dwData182;
	DWORD dwData186;
	DWORD dwData18A;
	DWORD dwData18E;
	DWORD dwData192;
	DWORD dwData196;
	DWORD dwData19A;
	DWORD dwData19E;
	DWORD dwData1A2;
	DWORD dwData1A6;
	DWORD dwData1AA;
	DWORD dwData1AE;
	DWORD dwData1B2;
	DWORD dwData1B6;
	DWORD dwData1BA;
	DWORD dwData1BE;
	DWORD dwData1C2;
	DWORD dwData1C6;
	DWORD dwData1CA;
	DWORD dwData1CE;
	DWORD dwData1D2;
	DWORD dwData1D6;
	DWORD dwData1DA;
	DWORD dwData1DE;
	DWORD dwData1E2;
	DWORD dwData1E6;
	DWORD dwData1EA;
	DWORD dwData1EE;
	DWORD dwData1F2;
	DWORD dwData1F6;
	DWORD dwData1FA;
	DWORD dwData1FE;
	DWORD dwData202;
	DWORD dwData206;
	DWORD dwData20A;
	DWORD dwData20E;
	DWORD dwData212;
	DWORD dwData216;
	DWORD dwData21A;
	DWORD dwData21E;
	DWORD dwData222;
	DWORD dwData226;
	DWORD dwData22A;
	DWORD dwData22E;
	DWORD dwData232;
	DWORD dwData236;
	DWORD dwData23A;
	DWORD dwData23E;
	DWORD dwData242;
	DWORD dwData246;
	DWORD dwData24A;
	DWORD dwData24E;
	DWORD dwData252;
	DWORD dwData256;
	DWORD dwData25A;
	DWORD dwData25E;
	DWORD dwData262;
	DWORD dwData266;
	DWORD dwData26A;
	DWORD dwData26E;
	DWORD dwData272;
	DWORD dwData276;
	DWORD dwData27A;
	DWORD dwData27E;
	DWORD dwData282;
	DWORD dwData286;
	DWORD dwData28A;
	DWORD dwData28E;
	DWORD dwData292;
	DWORD dwData296;
	DWORD dwData29A;
	DWORD dwData29E;
	DWORD dwData2A2;
	DWORD dwData2A6;
	DWORD dwData2AA;
	DWORD dwData2AE;
	DWORD dwData2B2;
	DWORD dwData2B6;
	DWORD dwData2BA;
	DWORD dwData2BE;
	DWORD dwData2C2;
	DWORD dwData2C6;
	DWORD dwData2CA;
	DWORD dwData2CE;
	DWORD dwData2D2;
	DWORD dwData2D6;
	DWORD dwData2DA;
	DWORD dwData2DE;
	DWORD dwData2E2;
	DWORD dwData2E6;
	DWORD dwData2EA;
	DWORD dwData2EE;
	DWORD dwData2F2;
	DWORD dwData2F6;
	DWORD dwData2FA;
	DWORD dwData2FE;
	DWORD dwData302;
	DWORD dwData306;
	DWORD dwData30A;
	DWORD dwData30E;
	DWORD dwData312;
	DWORD dwData316;
	DWORD dwData31A;
	DWORD dwData31E;
	DWORD dwData322;
	DWORD dwData326;
	DWORD dwData32A;
	DWORD dwData32E;
	DWORD dwData332;
	DWORD dwData336;
	DWORD dwData33A;
	DWORD dwData33E;
	DWORD dwData342;
	DWORD dwData346;
	DWORD dwData34A;
	DWORD dwData34E;
	DWORD dwData352;
	DWORD dwData356;
	DWORD dwData35A;
	DWORD dwData35E;
	DWORD dwData362;
	DWORD dwData366;
	DWORD dwData36A;
	DWORD dwData36E;
	DWORD dwData372;
	DWORD dwData376;
	DWORD dwData37A;
	DWORD dwData37E;
	DWORD dwData382;
	DWORD dwData386;
	DWORD dwData38A;
	DWORD dwData38E;
	DWORD dwData392;
	DWORD dwData396;
	DWORD dwData39A;
	DWORD dwData39E;
	DWORD dwData3A2;
	DWORD dwData3A6;
	DWORD dwData3AA;
	DWORD dwData3AE;
	DWORD dwData3B2;
	DWORD dwData3B6;
	DWORD dwData3BA;
	DWORD dwData3BE;
	DWORD dwData3C2;
	DWORD dwData3C6;
	DWORD dwData3CA;
	DWORD dwData3CE;
	DWORD dwData3D2;
	DWORD dwData3D6;
	DWORD dwData3DA;
	DWORD dwData3DE;
	DWORD dwData3E2;
	DWORD dwData3E6;
	DWORD dwData3EA;
	DWORD dwData3EE;
	DWORD dwData3F2;
	DWORD dwData3F6;
	DWORD dwData3FA;
	DWORD dwData3FE;
	DWORD dwData402;
	DWORD dwData406;
	DWORD dwData40A;
	DWORD dwData40E;
	DWORD dwData412;
	DWORD dwData416;
	DWORD dwData41A;
	DWORD dwData41E;
	float fDynamicBrakesMaximumEffectiveSpeed422;
	float fDynamicBrakesMaximumForce426;
	float fDynamicBrakesResistorCurrentLimit42A;
	float fDynamicBrakesCutInSpeed42E;
	float fDynamicBrakesMaxAirBrakePressure432;
	float fDynamicBrakesFadingSpeed436;
	float fDynamicBrakesDelayTimeBeforeEngaging43A;
	float fDynamicBrakesMaximumSpeedForFadeOut43E;
	float fDynamicBrakesEffectAtMaximumFadeOut442;
	float fDynamicBrakesHigherSpeedCurveExponent446;
	float fDynamicBrakesLowerSpeedCurveExponent44A;
	float fDynamicBrakesNumberOfControllerNotches44E;
	float fBreakLevelArray452TO4F6rt[42]; // 0x452------0x4EA 一共42个float值，每个值对应到了ENG文件当中每个级别
	DWORD dwData4FA[0x20C];
	short sDataD2A;
};
struct SEngineOrWagonInConFile
{
	SWagFile *wagFilePtr0;
	SEngFile *engFilePtr4;
	DWORD dwData8; // 1
	DWORD dwDataC; // 0
	DWORD strangeData10; // 0x566064A
	wchar_t EngineDataSecond14[0x40];
	wchar_t wagFileName94[0x20];
	wchar_t EngineDataFirstD4[0x60];
};

void AddTempSpeedLimit(float currentDistance, STrackNode *node, vector<STempSpeedLimit>& limitVect, HANDLE handle, int direction);
void AddSpeedPostLimit(float currentDistance, const STrackNode &node, vector<SSpeedPostLimit>& limitVect, HANDLE, int direction, STrackNode *);
void AddStationItem(float currentDistance, const STrackNode &node, vector<SStationItem>& stationVect, vector<SStationItem>& sidingVect, HANDLE handle, int direction);
void AddSignalItem(float currentDistance, const STrackNode &node, vector<SShowSignalItem>& signalVect, HANDLE, int direction);

bool ReadPointerMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, int num, ...);
CString SpeedPostItemToString(const SSpeedPostItem &item);
bool GetTrainHandle(HANDLE &hProcess);
void *GetTrainPointer(HANDLE hProcess);
STrackNode *GetNext(STrackNode *nodePtr, const SConnectStruct &connectStruct, const SConnectNode &connectNode,
                    int &nextDirect);
STrackNode *GetNextNode(HANDLE handle, const STrackNode &node, STrackNode *nodePtr, int direction, int &nextDirect);

void process_AX(float *fArray, float AX);
void process_AY(float *fArray, float AY);
void process_AZ(float *fArray, float AZ);
float *process(HANDLE handle, float *fMatrix, float *fXYZ);
float *process30(HANDLE handle, float *fMatrix, float *fXYZ);

bool IsSpeedPostValid(HANDLE handle, float angle, float fLocationInTrackNode, int nDirection, const STrackNode &node, STrackNode *nodePtr);
CString IteratorList(HANDLE handle, void *head, CString (*func)(HANDLE, void *));
CString DefaultHandle(HANDLE handle, void *pointer);
void getSectionData_Modified(HANDLE handle, SProcessData &processData, const STrackNode &node, int, int , SSectionTypeData *basePtr);
int AdjustAngle_Modified(HANDLE handle, SProcessData &processData, const STrackNode &node, float fLocation, SSectionTypeData *basePtr);

void getSectionData(HANDLE handle, SProcessData &processData, const STrackNode &node, int sectionNum, float *fArray, SSectionTypeData *basePtr);
int AdjustAngle(HANDLE handle, SProcessData &processData, const STrackNode &node, float fLocation, SSectionTypeData *basePtr);

inline void ReadTrainProcess(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	if (!ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, NULL))
		throw 1;
}

inline float inner_product(float *fArray1, float *fArray2)
{
	return fArray1[0] * fArray2[0] + fArray1[1] * fArray2[1] + fArray1[2] * fArray2[2];
}


inline CString showTrackInfo(const STrackInfo &trackInfo)
{
	CString msg;
	msg.Format(L"nLeftNodeNum : %d, Direct %d, LeftLen : %f, SectLen : %f",
	           trackInfo.nCurrentSectionNum, trackInfo.nDirection, trackInfo.fLocationInNode, trackInfo.fLocationInSection);
	return msg;
}




#endif