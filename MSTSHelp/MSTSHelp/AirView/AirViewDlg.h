// AirViewDlg.h : ͷ�ļ�
//

#pragma once


#include "..\SignSpeedLimit\Util.h"
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
	HANDLE m_hTrainProcess;
	HICON m_hIcon;
	vector<SSectionInfo> m_vectSectionInfo;
	vector<SSectionInfo> m_backVectSectionInfo;

	// ���ɵ���Ϣӳ�亯��
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
