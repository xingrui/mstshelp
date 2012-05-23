// AirViewDlg.h : 头文件
//

#pragma once


#include "..\SignSpeedLimit\Util.h"
// CAirViewDlg 对话框
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
	HICON m_hIcon;
	vector<SSectionInfo> m_vectSectionInfo;
	vector<SSectionInfo> m_backVectSectionInfo;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void DrawTracks(CPaintDC *pDC);
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
};
