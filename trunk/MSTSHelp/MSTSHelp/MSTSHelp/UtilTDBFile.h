#ifndef UTIL_TDBFILE
#define UTIL_TDBFILE
#include "Util.h"
#include <vector>
using std::vector;
struct SVectorNode;
struct STDBFile;
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
struct SSubConnectStruct
{
	SVectorNode *nodePtr;
	int nDirect;
};
struct SConnectStruct
{
	SSubConnectStruct subStruct[3];
};
struct SConnectNode
{
	int nType0;// 2 JunctionNode || 3 EndNode
	DWORD data4;
	DWORD data8;
	size_t   nTrPinsFirst12;
	size_t   nTrPinsSecond16;
	SConnectStruct *nodePointer20;
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
	unsigned short direction80;
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
struct SSectionTypeData
{
	float fData[6];
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

struct SSpeedPostItem
{
	int nType;//8
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
struct STrackInfo
{
	SVectorNode     *vectorNodePtr;
	int             nCurrentSectionNum;
	SVectorSection  *sectionPtr;
	int             nDirection;
	float           fLocationInNode;
	float           fLocationInSection;
};
struct STempSpeed
{
	SVectorNode *nodePtr;
	float fStart;
	float fEnd;
};
inline float inner_product(float *fArray1, float *fArray2)
{
	return fArray1[0] * fArray2[0] + fArray1[1] * fArray2[1] + fArray1[2] * fArray2[2];
}
struct SSignalLight
{
	DWORD dwData0;
	float fPositionX;
	float fPositionY;
	float fPositionZ;
	int nLightNameIndex;
	float fRadius;
};
struct SSignalDrawState
{
	wchar_t *wcpDrawStateName;
	void *uPtr;
	int nDrawLightNum;
	float fDataC;
};
struct SSignalState
{
	int nIndexOfSignalAspect;
	float fSpeedLimit;
	int SIGASPF_flags;
};
struct SSignalType // Size 0x5C Exactly. SignalType in memory are placed as array. one by one.
{
	wchar_t *wcpSignalTypeName0;
	int eSignalType4;
	DWORD dwData8;
	DWORD dwDataC;
	SSignalLight *pSignalLightArray10;
	int nSignalLightNum14;
	SSignalDrawState *pSignalDrawStateArray18;
	int nSignalDrawStateNum1C;
	DWORD dwData20;
	DWORD dwData24;
	float fData28;
	float fData2C;
	float fData30;
	DWORD dwData34;
	DWORD dwData38;
	DWORD dwData3C;
	DWORD dwData40;
	DWORD dwData44;
	DWORD dwData48;
	DWORD dwData4C;
	SSignalState *pSignalStateArray50;
	int SignalNumClearAhead54;
	void *pData58;
};
struct SSignalItem
{
	int nType;//0
	int nTrItemIndexInTrackNode;
	int nData8;
	float fLocationInTrackNode; // ��Item��VectorNode���е�λ��
	DWORD dwData10;
	SSignalType *pSignalType14;
	DWORD dwData18;
	DWORD dwData1C; // 0x8000
	BYTE cSignalItemDirection20;
	BYTE cLightColor21;
	BYTE cData22;
	BYTE cData23;
	DWORD dwData24;
	DWORD dwData28;
	DWORD dwData2C;
};
void GetForwardSpeedLimit(HANDLE m_hTrainProcess, vector<SForwardLimit>& limitList, float);
#endif