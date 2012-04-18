// SignSpeedLimitDlg.h : ͷ�ļ�
//

#pragma once

#include "Util.h"
// CSignSpeedLimitDlg �Ի���
class CSignSpeedLimitDlg : public CDialog
{
	// ����
public:
	CSignSpeedLimitDlg(CWnd *pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_SIGNSPEEDLIMIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;
	HANDLE m_hTrainProcess;
	// ���ɵ���Ϣӳ�亯��
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
	afx_msg void OnBnClickedCheck();
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	CString m_textContent;
public:
	BOOL m_bShowSpeedPost;
	BOOL m_bShowStation;
	BOOL m_bShowSiding;
	BOOL m_bAutoGetData;
	BOOL m_bShowTaskLimit;
	BOOL m_bShowSignal;
public:
	UINT m_uForwardDistance;
public:
	void OnGetData();
public:
	BOOL m_bShowTrackInfo;
};
