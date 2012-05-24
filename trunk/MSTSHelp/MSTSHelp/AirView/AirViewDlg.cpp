// AirViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AirView.h"
#include "AirViewDlg.h"
#include "cmath"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAirViewDlg �Ի���




CAirViewDlg::CAirViewDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CAirViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAirViewDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAirViewDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CAirViewDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAirViewDlg ��Ϣ�������

BOOL CAirViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	m_hTrainProcess = NULL;
	m_currentAngle = 0;
	SetTimer(0, 100, NULL);
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAirViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		DrawTracks(&dc);
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CAirViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAirViewDlg::DrawTracks(CPaintDC *pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt(4000, 4000);
	pDC->SetViewportExt(rect.right, -rect.bottom);
	pDC->SetViewportOrg(rect.right / 2, rect.bottom / 2);
	CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(RGB(120, 255, 200));
	pOldBrush = pDC->SelectObject(&brush);
	int nRadius = 8 * 4000 / rect.right;
	pDC->Ellipse(-nRadius, -nRadius, nRadius, nRadius);//CRect()Ϊ��Ҫ����Բ����Ӿ���
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	float currentAngle = -m_currentAngle;

	if (m_vectSectionInfo.empty())
		return;

	SSectionInfo info = m_vectSectionInfo[0];
	info.fEnd = 0;
	m_vectSectionInfo[0].fStart = 0;
	float fCurrentX = 0, fCurrentY = 0;

	for (size_t i = 0; i < m_vectSectionInfo.size(); ++i)
	{
		SSectionInfo *pInfo = &m_vectSectionInfo[i];

		if (pInfo->nDirection == 0)
		{
			// ֱ���
			float fLength = pInfo->fEnd - pInfo->fStart;
			pDC->MoveTo(fCurrentX, fCurrentY);
			fCurrentX += fLength * cos(currentAngle);
			fCurrentY += fLength * sin(currentAngle);
			pDC->LineTo(fCurrentX, fCurrentY);
		}
		else if (pInfo->nDirection == 1)
		{
			// ��ת
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			pDC->Arc(fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			         fCurrentX, fCurrentY, fPreX, fPreY);
		}
		else
		{
			// ��ת
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			pDC->Arc(fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			         fPreX, fPreY, fCurrentX, fCurrentY);
		}
	}

	currentAngle = -m_currentAngle + 3.1415926f;
	fCurrentX = 0, fCurrentY = 0;

	for (size_t i = 0; i <= m_backVectSectionInfo.size(); ++i)
	{
		SSectionInfo *pInfo;

		if (i == 0)
			pInfo = &info;
		else
			pInfo = &m_backVectSectionInfo[i - 1];

		if (pInfo->nDirection == 0)
		{
			// ֱ���
			float fLength = pInfo->fEnd - pInfo->fStart;
			pDC->MoveTo(fCurrentX, fCurrentY);
			fCurrentX += fLength * cos(currentAngle);
			fCurrentY += fLength * sin(currentAngle);
			pDC->LineTo(fCurrentX, fCurrentY);
		}
		else if (pInfo->nDirection != 1)
		{
			// ��ת
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			pDC->Arc(fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			         fCurrentX, fCurrentY, fPreX, fPreY);
		}
		else
		{
			// ��ת
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			pDC->Arc(fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			         fPreX, fPreY, fCurrentX, fCurrentY);
		}
	}
}

void CAirViewDlg::GetDataAndPaint()
{
	if (!GetTrainHandle(m_hTrainProcess))
	{
		return;
	}

	if (!GetTrainPointer(m_hTrainProcess))
	{
		return;
	}

	GetTrackData();
	Invalidate();
}

void CAirViewDlg::GetTrackData()
{
	m_vectSectionInfo.clear();
	m_backVectSectionInfo.clear();
	STrackInfo headInfo;
	//headInfo is the information of the head of the train.
	size_t trainInfo;
	ReadTrainProcess(m_hTrainProcess, (void *)TRAIN_INFO_MEM, (LPVOID)&trainInfo, 4);
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8098F8, &m_currentAngle, 4);
	m_currentAngle -= 1.57f;
	BOOL bIsForward;

	if (trainInfo & 0x80) // Forward Or Backward
	{
		ReadTrainProcess(m_hTrainProcess, (void *)TAIL_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo));
		bIsForward = FALSE;
	}
	else
	{
		ReadTrainProcess(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo));
		bIsForward = TRUE;
	}

	float forwardLength;
	SVectorNode trackNode;
	int nDirectOfHeadNode = headInfo.nDirection == bIsForward;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.vectorNodePtr, (LPVOID)&trackNode, sizeof(SVectorNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - trackNode.fTrackNodeLength;

	int nDirectOfNextNode = nDirectOfHeadNode;
	SVectorNode *nextNodePtr = headInfo.vectorNodePtr;
	STrackSection *pSection;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, &pSection, 4, 1, 0xC);

	while (forwardLength < 4 * 1000 && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		AddSectionInfo(forwardLength, vectorNode, m_vectSectionInfo, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode, pSection);
		forwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

	nDirectOfHeadNode = !nDirectOfHeadNode;
	float backwardLength;

	if (nDirectOfHeadNode)
		backwardLength = - headInfo.fLocationInNode;
	else
		backwardLength = headInfo.fLocationInNode - trackNode.fTrackNodeLength;

	int nDirectOfPrevNode = nDirectOfHeadNode;
	SVectorNode *prevNodePtr = headInfo.vectorNodePtr;

	while (backwardLength < 4000 && prevNodePtr)
	{
		SVectorNode *currentNodePtr = prevNodePtr;
		int nDirectOfCurrentNode = nDirectOfPrevNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		AddSectionInfo(backwardLength, vectorNode, m_backVectSectionInfo, m_hTrainProcess, nDirectOfCurrentNode, nDirectOfCurrentNode, pSection);
		backwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		prevNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfPrevNode);
	}
}

void CAirViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	Invalidate();
	UpdateWindow();
}

void CAirViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnTimer(nIDEvent);
	GetDataAndPaint();
}

void CAirViewDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_hTrainProcess)
		CloseHandle(m_hTrainProcess);
}

void CAirViewDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDataAndPaint();
}
