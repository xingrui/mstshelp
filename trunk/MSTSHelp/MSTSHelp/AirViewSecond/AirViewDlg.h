// AirViewDlg.h : 头文件
//

#pragma once


#include "Util.h"
// CAirViewDlg 对话框
const int TIMES = 100;
struct SLocation
{
	float fPointX;
	float fPointY;
	SLocation(float x, float y): fPointX(x), fPointY(y) {}
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
	HANDLE m_hTrainProcess;
	float m_currentAngle;
	float m_fDistance;
	HICON m_hIcon;
	vector<SSectionInfo> m_vectSectionInfo;
	vector<SSectionInfo> m_backVectSectionInfo;
	STrackSection *m_pTrackSectionArray;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void DrawScale(CDC *pDC, int nSize, float fMapSize);
	void DrawPathTracks(CDC *pDC);
	void DrawAllTracks(CDC *pDC);
	void SetPaintMode(CDC *pDC);
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
