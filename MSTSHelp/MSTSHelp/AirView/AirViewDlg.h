// AirViewDlg.h : ͷ�ļ�
//

#pragma once


// CAirViewDlg �Ի���
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
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void DrawTracks(CPaintDC *pDC);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
