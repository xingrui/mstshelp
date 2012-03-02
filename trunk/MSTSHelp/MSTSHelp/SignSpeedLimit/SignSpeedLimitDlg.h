// SignSpeedLimitDlg.h : ͷ�ļ�
//

#pragma once

#include "Util.h"
// CSignSpeedLimitDlg �Ի���
class CSignSpeedLimitDlg : public CDialog
{
// ����
public:
	CSignSpeedLimitDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SIGNSPEEDLIMIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	HANDLE m_hTrainProcess;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
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
};
