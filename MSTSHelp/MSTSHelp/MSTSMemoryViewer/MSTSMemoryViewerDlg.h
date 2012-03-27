// MSTSMemoryViewerDlg.h : 头文件
//

#pragma once


// CMSTSMemoryViewerDlg 对话框
class CMSTSMemoryViewerDlg : public CDialog
{
	// 构造
public:
	CMSTSMemoryViewerDlg(CWnd *pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_MSTSMEMORYVIEWER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;
	HANDLE m_hTrainProcess;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strDBListHead;
public:
	afx_msg void OnBnClickedButton1();
public:
	CString m_textContent;
public:
	afx_msg void OnBnClickedOk();
};
