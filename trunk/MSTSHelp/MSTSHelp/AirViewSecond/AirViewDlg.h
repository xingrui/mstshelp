// AirViewDlg.h : ͷ�ļ�
//

#pragma once


#include "Util.h"
#include <queue>
#include <set>
using std::set;
using std::queue;
// CAirViewDlg �Ի���
const int TIMES = 100;
struct SLocation
{
	float fPointX;
	float fPointY;
	SLocation(float x = 0, float y = 0): fPointX(x), fPointY(y) {}
};
struct SQueueData
{
	SConnectStruct connectStruct;
	SVectorNode *pVectorNode;
	SLocation location;
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
	HANDLE m_hTrainProcess;
	float m_currentAngle;
	float m_fDistance;
	HICON m_hIcon;
	vector<SSectionInfo> m_vectSectionInfo;
	vector<SSectionInfo> m_backVectSectionInfo;
	STrackSection *m_pTrackSectionArray;
	queue<SQueueData> m_queueVectorNode;
	set<SVectorNode *> m_setVectorNode;
	STrackInfo m_currentHeadInfo;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void DrawScale(CDC *pDC, int nSize, float fMapSize);
	void DrawPathTracks(CDC *pDC);
	void DrawAllTracks(CDC *pDC);
	void SetPaintMode(CDC *pDC);
	SLocation calculateCurrentLocation(const SVectorNode &node, float fCurrentLocation, HANDLE handle);
	SLocation DrawVectorNode(CDC *pDC, const SVectorNode &node, int nDirection,  const SLocation &startLocation, HANDLE handle);
	void DrawArc(CDC *pDC, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void DrawMoveTo(CDC *pDC, float x1, float y1);
	void DrawLineTo(CDC *pDC, float x1, float y1);
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
