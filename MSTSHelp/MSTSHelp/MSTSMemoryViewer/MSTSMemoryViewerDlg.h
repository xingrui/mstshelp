// MSTSMemoryViewerDlg.h : ͷ�ļ�
//

#pragma once


// CMSTSMemoryViewerDlg �Ի���
class CMSTSMemoryViewerDlg : public CDialog
{
	// ����
public:
	CMSTSMemoryViewerDlg(CWnd *pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_MSTSMEMORYVIEWER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;
	HANDLE m_hTrainProcess;

	// ���ɵ���Ϣӳ�亯��
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
