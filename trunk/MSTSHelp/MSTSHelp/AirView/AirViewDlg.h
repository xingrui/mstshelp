// AirViewDlg.h : ͷ�ļ�
//

#pragma once


#include "Util.h"
// CAirViewDlg �Ի���
const int TIMES = 100;
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

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void DrawScale(CDC *pDC, int nSize, float fMapSize);
	void DrawTracks(CDC *pDC);
	void DrawArc(CDC *pDC, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
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