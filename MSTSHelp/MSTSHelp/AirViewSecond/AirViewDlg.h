// AirViewDlg.h : 头文件
//

#pragma once


#include "Util.h"
#include <queue>
#include <set>
using std::set;
using std::queue;
// CAirViewDlg 对话框
const int TIMES = 100;
struct SLocation
{
	double fPointX;
	double fPointY;
	SLocation(double x = 0, double y = 0): fPointX(x), fPointY(y) {}
};
struct SQueueData
{
	SConnectStruct connectStruct;
	SVectorNode *pVectorNode;
};
struct SDrawUnit
{
	int nType;
	double fromX;
	double fromY;
	double toX;
	double toY;
	double centerX;
	double centerY;
	double radius;
};
struct SSavedData
{
	void *pTrain;
	STrackSection *pTrackSectionArray;
	vector<SDrawUnit> vectDrawUnit;
};
class CAirViewDlg : public CDialog
{
	// 构造
public:
	CAirViewDlg(CWnd *pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_AIRVIEW_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV 支持


	// 实现
protected:
	SSavedData m_savedData;
	void InitSavedData()
	{
		m_savedData.pTrain = NULL;
		m_savedData.vectDrawUnit.clear();
	}
	HANDLE m_hTrainProcess;
	float m_fDistance;
	HICON m_hIcon;
	vector<SSectionInfo> m_vectSectionInfo;
	vector<SSectionInfo> m_backVectSectionInfo;
	STrackInfo m_currentHeadInfo;
	SLocation m_startLocation;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	BOOL GetHandleAndPrepareData();
	void DrawScale(CDC *pDC, int nSize, float fMapSize);
	void DrawPathTracks(CDC *pDC);
	void DrawAllTracks(CDC *pDC);
	void DrawUnits(CDC *pDC);
	void DrawAllTracksByTDBFile(CDC *pDC);
	void GetAllTracksDataByTDBFile();
	void GetVectorNodeData(const SVectorNode &node, HANDLE handle);
	void DrawAllAITracks(CDC *pDC);
	void SetPaintMode(CDC *pDC);
	void CalculateCurrentLocation(const SVectorNode &node, float fCurrentLocation, HANDLE handle);
	bool DrawVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle);
	void DrawPointInVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle, float fLocation, CString strName);
	void DrawArc(CDC *pDC, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
	void DrawMoveTo(CDC *pDC, double x1, double y1);
	void DrawLineTo(CDC *pDC, double x1, double y1);
	void GetDataAndPaint();
	void GetTrackData();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
};
