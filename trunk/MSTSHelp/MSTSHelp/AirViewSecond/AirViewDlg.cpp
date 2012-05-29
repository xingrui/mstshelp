// AirViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AirView.h"
#include "AirViewDlg.h"
#include "cmath"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAirViewDlg 对话框




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
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CAirViewDlg 消息处理程序

BOOL CAirViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_hTrainProcess = NULL;
	m_currentAngle = 0;
	m_fDistance = 4000;
	m_pTrackSectionArray = new STrackSection[0x10000];
	SetTimer(0, 200, NULL);
	// TODO: 在此添加额外的初始化代码
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAirViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		int nWidth = rect.Width();
		int nHeight = rect.Height();
		CDC MemDC;
		CBitmap MemBitmap;
		MemDC.CreateCompatibleDC(NULL);
		MemBitmap.CreateCompatibleBitmap(&dc, nWidth, nHeight);
		CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
		MemDC.FillSolidRect(0, 0, nWidth, nHeight, ::GetSysColor(COLOR_3DFACE));
		DrawScale(&MemDC, rect.bottom, m_fDistance);
		SetPaintMode(&MemDC);
		DrawPathTracks(&MemDC);
		MemDC.SetMapMode(MM_TEXT);
		dc.BitBlt(0, 0, nWidth, nHeight, &MemDC, -nWidth / 2, -nHeight / 2, SRCCOPY);
		MemBitmap.DeleteObject();
		MemDC.DeleteDC();
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CAirViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CAirViewDlg::DrawArc(CDC *pDC, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	int nX1 = (int)(TIMES * x1);
	int nY1 = (int)(TIMES * y1);
	int nX2 = (int)(TIMES * x2);
	int nY2 = (int)(TIMES * y2);
	int nX3 = (int)(TIMES * x3);
	int nY3 = (int)(TIMES * y3);
	int nX4 = (int)(TIMES * x4);
	int nY4 = (int)(TIMES * y4);
	pDC->Arc(nX1, nY1 , nX2, nY2, nX3, nY3, nX4, nY4);
}
void CAirViewDlg::DrawMoveTo(CDC *pDC, float x1, float y1)
{
	int nX1 = (int)(TIMES * x1);
	int nY1 = (int)(TIMES * y1);
	pDC->MoveTo(nX1, nY1);
}
void CAirViewDlg::DrawLineTo(CDC *pDC, float x1, float y1)
{
	int nX1 = (int)(TIMES * x1);
	int nY1 = (int)(TIMES * y1);
	pDC->LineTo(nX1, nY1);
}
// 绘制比例尺
void CAirViewDlg::DrawScale(CDC *pDC, int nSize, float fMapSize)
{
	pDC->MoveTo(5, 15);
	pDC->LineTo(55, 15);
	CString str;
	float fNum = fMapSize * 50 / nSize ;

	if (fNum > 1000)
		str.Format(L"%.1f km", fNum / 1000);
	else
		str.Format(L"%.0f m", fNum);

	CRect textRect;
	textRect.top = 20;
	textRect.bottom = 40;
	textRect.left = 5;
	textRect.right = 105;
	pDC->DrawText(str, textRect, NULL);
}
void CAirViewDlg::DrawVectorNode(CDC *pDC, const SVectorNode &node, int nDirection, float startX, float startY, float currentAngle, HANDLE handle)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	int start = nDirection ? 0 : num - 1;
	int end = nDirection ? num : -1;
	int delta = nDirection ? 1 : -1;
	float fCurrentX = startX;
	float fCurrentY = startY;

	for (int i = start; i != end; i += delta)
	{
		STrackSection *pCurSection = m_pTrackSectionArray + pSectionData[i].sectionIndex;
		float fRaidus = pCurSection->fSectionCurveFirstRadius4;

		if (pCurSection->fSectionCurveSecondAngle8 == 0)
		{
			float fLength = pCurSection->fSectionSizeSecondLength0;
			DrawMoveTo(pDC, fCurrentX, fCurrentY);
			fCurrentX += fLength * cos(currentAngle);
			fCurrentY += fLength * sin(currentAngle);
			DrawLineTo(pDC, fCurrentX, fCurrentY);
		}
		else if ((pCurSection->fSectionCurveSecondAngle8 > 0) ^ nDirection)
		{
			float fRadius = pCurSection->fSectionCurveFirstRadius4;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= pCurSection->fSectionSizeSecondLength0 / fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fCurrentX, fCurrentY, fPreX, fPreY);
		}
		else
		{
			float fRadius = pCurSection->fSectionCurveFirstRadius4;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += pCurSection->fSectionSizeSecondLength0 / fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fCurrentX, fCurrentY, fPreX, fPreY);
		}
	}
}
void CAirViewDlg::SetPaintMode(CDC *pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->SetViewportOrg(rect.right / 2, rect.bottom / 2);
	CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(RGB(120, 255, 200));
	pOldBrush = pDC->SelectObject(&brush);
	int nRadius = 5;
	pDC->Ellipse(-nRadius, -nRadius, nRadius, nRadius);
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt((int)(m_fDistance * TIMES), (int)(m_fDistance * TIMES));
	pDC->SetViewportExt(rect.right, -rect.bottom);
}
void CAirViewDlg::DrawAllTracks(CDC *pDC)
{
}
void CAirViewDlg::DrawPathTracks(CDC *pDC)
{
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
			// 直轨道
			float fLength = pInfo->fEnd - pInfo->fStart;
			DrawMoveTo(pDC, fCurrentX, fCurrentY);
			fCurrentX += fLength * cos(currentAngle);
			fCurrentY += fLength * sin(currentAngle);
			DrawLineTo(pDC, fCurrentX, fCurrentY);
		}
		else if (pInfo->nDirection == 1)
		{
			// 右转
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fCurrentX, fCurrentY, fPreX, fPreY);
		}
		else
		{
			// 左转
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
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
			// 直轨道
			float fLength = pInfo->fEnd - pInfo->fStart;
			DrawMoveTo(pDC, fCurrentX, fCurrentY);
			fCurrentX += fLength * cos(currentAngle);
			fCurrentY += fLength * sin(currentAngle);
			DrawLineTo(pDC, fCurrentX, fCurrentY);
		}
		else if (pInfo->nDirection != 1)
		{
			// 右转
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fCurrentX, fCurrentY, fPreX, fPreY);
		}
		else
		{
			// 左转
			float fRadius = pInfo->fRadius;
			float fCenterX = fCurrentX, fCenterY = fCurrentY;
			float fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += (pInfo->fEnd - pInfo->fStart) / pInfo->fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fPreX, fPreY, fCurrentX, fCurrentY);
		}
	}
}

void CAirViewDlg::GetDataAndPaint()
{
	m_vectSectionInfo.clear();
	m_backVectSectionInfo.clear();

	if (GetTrainHandle(m_hTrainProcess) && GetTrainPointer(m_hTrainProcess))
	{
		try
		{
			GetTrackData();
		}
		catch (int)
		{
		}
	}

	Invalidate();
}

void CAirViewDlg::GetTrackData()
{
	m_vectSectionInfo.clear();
	m_backVectSectionInfo.clear();
	vector<SSectionInfo> vectSectionInfo;
	vector<SSectionInfo> backVectSectionInfo;
	STrackSection *pSection;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, &pSection, 4, 1, 0xC);
	ReadTrainProcess(m_hTrainProcess, pSection, m_pTrackSectionArray, 0x10000 * sizeof(STrackSection));
	STrackInfo headInfo;
	//headInfo is the information of the head of the train.
	size_t trainInfo;
	ReadTrainProcess(m_hTrainProcess, (void *)TRAIN_INFO_MEM, (LPVOID)&trainInfo, 4);
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8098F8, &m_currentAngle, 4);
	m_currentAngle -= 1.57f;
	ReadTrainProcess(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo));
	float forwardLength;
	SVectorNode trackNode;
	int nDirectOfHeadNode = headInfo.nDirection;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.vectorNodePtr, (LPVOID)&trackNode, sizeof(SVectorNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - trackNode.fTrackNodeLength;

	int nDirectOfNextNode = nDirectOfHeadNode;
	SVectorNode *nextNodePtr = headInfo.vectorNodePtr;

	while (forwardLength < 8 * m_fDistance && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		AddSectionInfo(forwardLength, vectorNode, vectSectionInfo, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode, m_pTrackSectionArray);
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

	while (backwardLength < 8 * m_fDistance && prevNodePtr)
	{
		SVectorNode *currentNodePtr = prevNodePtr;
		int nDirectOfCurrentNode = nDirectOfPrevNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		AddSectionInfo(backwardLength, vectorNode, backVectSectionInfo, m_hTrainProcess, nDirectOfCurrentNode, nDirectOfCurrentNode, m_pTrackSectionArray);
		backwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		prevNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfPrevNode);
	}

	m_vectSectionInfo.swap(vectSectionInfo);
	m_backVectSectionInfo.swap(backVectSectionInfo);
}

void CAirViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	Invalidate();
	UpdateWindow();
}

void CAirViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnTimer(nIDEvent);
	GetDataAndPaint();
}

void CAirViewDlg::OnDestroy()
{
	CDialog::OnDestroy();
	delete[] m_pTrackSectionArray;

	if (m_hTrainProcess)
		CloseHandle(m_hTrainProcess);
}

void CAirViewDlg::OnBnClickedOk()
{
	GetDataAndPaint();
}

BOOL CAirViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		m_fDistance /= 1.5;

		if (m_fDistance < 1000)
			m_fDistance = 1000;
	}
	else
	{
		m_fDistance *= 1.5;

		if (m_fDistance > 400000)
			m_fDistance = 400000;
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CAirViewDlg::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}
