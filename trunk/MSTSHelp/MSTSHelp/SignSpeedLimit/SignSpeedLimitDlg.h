// SignSpeedLimitDlg.h : 头文件
//

#pragma once

#include "Util.h"
// CSignSpeedLimitDlg 对话框
class CSignSpeedLimitDlg : public CDialog
{
// 构造
public:
	CSignSpeedLimitDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SIGNSPEEDLIMIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HANDLE m_hTrainProcess;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
	BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	CString m_textContent;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnBnClickedButton1();
public:
	BOOL m_bShowSpeedPost;
public:
	BOOL m_bShowStation;
public:
	BOOL m_bShowSiding;
public:
	UINT m_uForwardDistance;
public:
	afx_msg void OnBnClickedCheck1();
public:
	afx_msg void OnBnClickedCheck2();
public:
	afx_msg void OnBnClickedCheck3();
public:
	BOOL m_bAutoGetData;
public:
	afx_msg void OnBnClickedCheck4();
public:
	afx_msg void OnBnClickedTest();
public:
	BOOL m_bShowTaskLimit;
public:
	afx_msg void OnBnClickedCheck5();
	void OnGetData();
	void GetTrainData(void* startLocation);
};
