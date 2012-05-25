#ifndef UTIL_H
#define UTIL_H
#include <vector>
using std::vector;
#define IDENTITY_MATRIX_MEM 0x771680
// ��λ����[1,0,0,0,1,0,0,0,1]��ʵû�б�Ҫ���������ֱ���ڳ����г�ʼ���Ϳ���

#define DISTANCE_TYPE_MEM 0x78C390
// ���뵥λ������ 1Ϊ���� 0ΪӢ��

#define CURRENT_TILE_X_MEM 0x79D118
#define CURRENT_TILE_Z_MEM 0x79D11C
// ���������ݳ�����û���õ���Ӧ���ǵ�ǰ��Xֵ��Zֵ����·����о�������

#define IDENTITY2_MATRIX_MEM 0x7A8A20
// ����һ������[1,0,0,0,1,0,0,0,1,0,0,0]

#define THIS_POINTER_MEM 0x7C2AC0
// ����Ҫ��ָ�룬�˴�Ϊ�ձ�ʾû���������У���Ϊ�ձ�ʾ������������

#define TASK_LIMIT_HEAD_MEM 0x809B38
// �����е���ʱ���ٵ������ͷ������F4����ú�ɫ����ʾ���Ǹ�

#define TASK_LIMIT_MEM 0x809B48
// �������ʱ����ֵ����·����ֵ����Ϣ�Ĵ�ŵط�

#define TRAIN_INFO_MEM 0x809890
// �𳵳�ͷ��һЩ��Ϣֵ�Ĵ�ŵط�

#define HEAD_TRACK_MEM 0x8098DC
// ��ͷ���ڵ�TrackNode�ĵ�ַ

#define TAIL_TRACK_MEM 0x809944
// ��β���ڵ�TrackNode�ĵ�ַ
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
	DWORD data4; // ��֪����ʲô��˼
	DWORD data8; // ��֪����ʲô��˼
	size_t   nTrPinsFirst12; // TrPins�ĵ�һ���� ����1
	size_t   nTrPinsSecond16; // TrPins�ĵڶ������� JunctionNode��2 EndNode��0
	SConnectStruct *nodePointer20; // �Բۣ�����ָ�룬���Űɣ�����������ǽṹ��
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