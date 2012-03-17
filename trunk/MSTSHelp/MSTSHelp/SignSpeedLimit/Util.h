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
	DWORD data4; // ��֪����ʲô��˼
	DWORD data8; // ��֪����ʲô��˼
	size_t   nTrPinsFirst12; // TrPins�ĵ�һ���� ����1
	size_t   nTrPinsSecond16; // TrPins�ĵڶ������� JunctionNode��2 EndNode��0
	SConnectStruct* nodePointer20; // �Բۣ�����ָ�룬���Űɣ�����������ǽṹ��
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
	unsigned short direction80;//������ݷǳ���Ҫ���������������������������ݣ�ֻ����JunctionNode��ʱ����Ч
	// ���ǵ����ǵ�JunctionNode�����TrPins���ݰɣ����ǵõ�ǰ���TDB�ļ����ܿ���
	// JunctionNode����������� ������� TrPin ( ConnectNodeIndex Direction ) 
	// ����������Ǵ洢��nodePointer20ָ��(���ṹ��ĵ���������)ָ�����������ġ�
	// ��һ��TrPin�ǵ����һ�����ڷ��򣬵ڶ���TrPin�͵�����TrPin�ǿ���ѡ��ġ�
	// Ҳ����˵���JunctionNode�������ɵ�һ�����ӵ��ڶ�����Ҳ�������ɵ�һ�����ӵ�������
	// ���direction80Ϊ0�� ��ʾ�������ӵڶ���TrPin�����direction80Ϊ1����ʾ�������ӵ�����TrPin
	// ϣ�����������㹻�����ˡ�
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
	float fLocationInTrackNode; // ��Item��VectorNode���е�λ��
	int TrItemSDataSecond;
	float TrItemPDataFirst;
	float TrItemPDataSecond;
	int TrItemPDataThird;
	int TrItemPDataFourth;
	int variableData;
	unsigned short sSpeedPostType; // ����Ǹñ�־���ٵ�����
	// ������ͺܸ��ӣ��ص����һ�°ɡ�
	// 1. & ���ǰ�λ�� ( sSpeedPostType & 7) �Ľ����2��ʱ�����Item���Ǳ�־����
	// �����������ֵ�ͱ�ʾ����̱�֮��ġ�
	// 2. sSpeedPostType  & 0x80 Ϊ1��ʾ������ٶԿͳ���������Ч
	// 3. sSpeedPostType  & 0x20 Ϊ1��ʾ������ٶԿͳ���Ч (*0x809890)&6 Ϊ2��ʾ��ǰ�����ǿͳ�
	// 4. sSpeedPostType  & 0x40 Ϊ1��ʾ������ٶԻ�����Ч (*0x809890)&6 Ϊ4��ʾ��ǰ�����ǻ���
	// 5. sSpeedPostType  & 0x100 Ϊ1��ʾ���ٵĵ�λΪӢ��/Сʱ��Ϊ0��ʾ���ٵĵ�λΪ����/Сʱ
	unsigned short SpeedpostTrItemDataSecond;// ����Ǹñ�־���ٵ���ֵ
	float SpeedpostTrItemDataThird;
	float fAngle; // ����Ǹñ�־���������ԭʼ����ϵ��XZƽ��������ת�ĽǶ�
	DWORD fData;//0
};

struct SPlatformItem
{
	int type;//3
	int subType;
	int unknown;
	float fLocationInTrackNode; // ��Item��VectorNode���е�λ��
	int   nTrItemSDataSecond;   // TrItemSData�ĵڶ�������
	float fTrItemRDataFirst;    // ͬ����ģ�����˼���
	float fTrItemRDataThird;
	int   nTrItemRDataFourth;
	int   nTrItemRDataFifth;
	DWORD data;
	wchar_t* platformName;      // ָ��վ̨����
	wchar_t* stationName;       // ָ��վ����
};

struct SSidingItem
{
	int type; // 6
	int subType;
	int unknown;
	float fLocationInTrackNode; // ��Item��VectorNode���е�λ��
	int   nTrItemSDataSecond;
	wchar_t* sidingName;   // ָ����ߵ�����
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