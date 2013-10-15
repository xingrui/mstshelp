// AirViewDlg.h : ͷ�ļ�
//

#pragma once


#include "Util.h"
#include <queue>
#include <set>
using std::set;
using std::queue;
// CAirViewDlg �Ի���
extern int TIMES;
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
	// ����
public:
	CAirViewDlg(CWnd *pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_AIRVIEW_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	SSavedData m_savedData;
	HANDLE m_hTrainProcess;
	float m_fMapSize;
	HICON m_hIcon;
	STrackInfo m_currentHeadInfo;
	SLocation m_startLocation;
	SLocation m_BaseLocation;
	SLocation m_mapOffset;
	SLocation m_oldOffset;
	RECTL m_BoundsRect;
	HENHMETAFILE m_EnhMetaFile;
	CPoint m_startPoint;
	bool m_in_drag;

	void InitSavedData()
	{
		m_savedData.pTrain = NULL;
		m_savedData.vectDrawUnit.clear();
	}
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	BOOL GetHandleAndPrepareData();
	void DrawScale(CDC *pDC, int nSize, float fMapSize);
	void DrawPathTracks(CDC *pDC);
	void DrawAllTracks(CDC *pDC);
	void DrawUnits(CDC *pDC);
	void DrawAllTracksByTDBFile(CDC *pDC);
	CRect GetTrackBoundRect();
	void UpdateBoundRect(const SVectorNode &node, CRect &rect);
	void GetAllTracksDataByTDBFile();
	void GetMetaFileHandleByTDBFile();
	void GetVectorNodeData(const SVectorNode &node, HANDLE handle);
	void DrawAllAITracks(CDC *pDC);
	void SetPaintMode(CDC *pDC);
	void CalculateCurrentLocation(const SVectorNode &node, float fCurrentLocation, HANDLE handle);
	bool DrawVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle);
	bool DrawVectorNodeInMetaFile(CDC *pDC, const SVectorNode &node, HANDLE handle);
	void DrawPointInVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle, float fLocation, CString strName);
	void DrawArc(CDC *pDC, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
	void DrawMoveTo(CDC *pDC, double x1, double y1);
	void DrawLineTo(CDC *pDC, double x1, double y1);
	void GetDataAndPaint();
	void GetTrackData();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BOOL PreTranslateMessage(MSG *pMsg);
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
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};