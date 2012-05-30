// AirViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AirView.h"
#include "AirViewDlg.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
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

		try
		{
			if (GetTrainHandle(m_hTrainProcess) && GetTrainPointer(m_hTrainProcess))
			{
				ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, m_pTrackSectionArray, 0x10000 * sizeof(STrackSection), 2, 0xC, 0);
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)HEAD_TRACK_MEM, (LPVOID)&m_currentHeadInfo, sizeof(STrackInfo));
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8098F8, &m_currentAngle, 4);
				m_currentAngle = (float)M_PI_2 - m_currentAngle;
				DrawAllTracks(&MemDC);
				CPen pen(PS_SOLID, 1, RGB(0, 255, 0));
				CPen *oldPen = MemDC.SelectObject(&pen);
				DrawPathTracks(&MemDC);
				MemDC.SelectObject(oldPen);
			}
		}
		catch (int)
		{
		}

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
void CAirViewDlg::DrawArc(CDC *pDC, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
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
void CAirViewDlg::DrawMoveTo(CDC *pDC, double x1, double y1)
{
	int nX1 = (int)(TIMES * x1);
	int nY1 = (int)(TIMES * y1);
	pDC->MoveTo(nX1, nY1);
}
void CAirViewDlg::DrawLineTo(CDC *pDC, double x1, double y1)
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
void CAirViewDlg::calculateCurrentLocation(const SVectorNode &node, float fCurrentLocation, HANDLE handle)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	float fDistance;
	float fCurrentLength = 0;
	fDistance = fCurrentLocation;

	for (int i = 0; i < num; ++i)
	{
		STrackSection *pCurSection = m_pTrackSectionArray + pSectionData[i].sectionIndex;
		float fRaidus = pCurSection->fSectionCurveFirstRadius4;
		float fSectionLength = pCurSection->fSectionSizeSecondLength0;

		if (fDistance > fCurrentLength + fSectionLength)
		{
			fCurrentLength += fSectionLength;
		}
		else
		{
			float fRemainLength = fDistance - fCurrentLength;
			m_startLocation.fPointX = pSectionData[i].TileX2 * 2048 + pSectionData[i].X;
			m_startLocation.fPointY = pSectionData[i].TileZ2 * 2048 + pSectionData[i].Z;
			float currentAngle = (float)M_PI_2 - pSectionData[i].AY;
			float fZ = pSectionData[i].Y;

			if (pCurSection->fSectionCurveSecondAngle8 == 0)
			{
				m_startLocation.fPointX += fRemainLength * cos(currentAngle);
				m_startLocation.fPointY += fRemainLength * sin(currentAngle);
			}
			else if (pCurSection->fSectionCurveSecondAngle8 < 0)
			{
				float fRadius = pCurSection->fSectionCurveFirstRadius4;
				double fCenterX = m_startLocation.fPointX, fCenterY = m_startLocation.fPointY;
				double fPreX = m_startLocation.fPointX, fPreY = m_startLocation.fPointY;
				fCenterX -= fRadius * sin(currentAngle);
				fCenterY += fRadius * cos(currentAngle);
				currentAngle += fRemainLength / fRadius;
				m_startLocation.fPointX = fCenterX + fRadius * sin(currentAngle);
				m_startLocation.fPointY = fCenterY - fRadius * cos(currentAngle);
			}
			else
			{
				float fRadius = pCurSection->fSectionCurveFirstRadius4;
				double fCenterX = m_startLocation.fPointX, fCenterY = m_startLocation.fPointY;
				double fPreX = m_startLocation.fPointX, fPreY = m_startLocation.fPointY;
				fCenterX += fRadius * sin(currentAngle);
				fCenterY -= fRadius * cos(currentAngle);
				currentAngle -= fRemainLength / fRadius;
				m_startLocation.fPointX = fCenterX - fRadius * sin(currentAngle);
				m_startLocation.fPointY = fCenterY + fRadius * cos(currentAngle);
			}

			break;
		}
	}

	delete []pSectionData;
}
void CAirViewDlg::DrawVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));

	for (int i = 0; i != num; ++i)
	{
		double fCurrentX = pSectionData[i].TileX2 * 2048 + pSectionData[i].X - m_startLocation.fPointX;
		double fCurrentY = pSectionData[i].TileZ2 * 2048 + pSectionData[i].Z - m_startLocation.fPointY;
		double currentAngle;
		currentAngle = pSectionData[i].AY;
		currentAngle = M_PI_2 - currentAngle;
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
		else if (pCurSection->fSectionCurveSecondAngle8 < 0)
		{
			double fRadius = pCurSection->fSectionCurveFirstRadius4;
			double fCenterX = fCurrentX, fCenterY = fCurrentY;
			double fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += pCurSection->fSectionSizeSecondLength0 / fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fPreX, fPreY, fCurrentX, fCurrentY);
		}
		else
		{
			double fRadius = pCurSection->fSectionCurveFirstRadius4;
			double fCenterX = fCurrentX, fCenterY = fCurrentY;
			double fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= pCurSection->fSectionSizeSecondLength0 / fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			DrawArc(pDC, fCenterX - fRadius, fCenterY - fRadius, fCenterX + fRadius, fCenterY + fRadius,
			        fCurrentX, fCurrentY, fPreX, fPreY);
		}
	}

	delete []pSectionData;
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
	if (!GetTrainHandle(m_hTrainProcess) || !GetTrainPointer(m_hTrainProcess))
		return;

	m_setVectorNode.clear();
	std::queue<SQueueData> tmp;
	swap(m_queueVectorNode, tmp);
	SVectorNode vectorNode;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)m_currentHeadInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));
	m_setVectorNode.insert(m_currentHeadInfo.pVectorNode);
	//////////////////////////////////////////////////////////////////////
	float fDistance = m_currentHeadInfo.fLocationInNode;
	calculateCurrentLocation(vectorNode, fDistance, m_hTrainProcess);
	//////////////////////////////////////////////////////////////////////
	SQueueData queueData;
	SConnectNode connectNode;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)vectorNode.InConnectNodePtr, (LPVOID)&connectNode, sizeof(SConnectNode));

	if (connectNode.nType0 == 2)
	{
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)connectNode.nodePointer20, (LPVOID)&queueData.connectStruct, sizeof(SConnectStruct));
		queueData.pVectorNode = m_currentHeadInfo.pVectorNode;
		m_queueVectorNode.push(queueData);
	}

	DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)vectorNode.OutConnectNodePtr, (LPVOID)&connectNode, sizeof(SConnectNode));

	if (connectNode.nType0 == 2)
	{
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)connectNode.nodePointer20, (LPVOID)&queueData.connectStruct, sizeof(SConnectStruct));
		queueData.pVectorNode = m_currentHeadInfo.pVectorNode;
		m_queueVectorNode.push(queueData);
	}

	while (!m_queueVectorNode.empty())
	{
		SQueueData queueData = m_queueVectorNode.front();
		m_queueVectorNode.pop();

		for (int i = 0; i < 3; ++i)
		{
			SSubConnectStruct *pSubConnectStruct = queueData.connectStruct.subStruct + i;

			if (pSubConnectStruct->pVectorNode != queueData.pVectorNode && pSubConnectStruct->pVectorNode != NULL)
			{
				if (m_setVectorNode.find(pSubConnectStruct->pVectorNode) != m_setVectorNode.end())
					continue;

				m_setVectorNode.insert(pSubConnectStruct->pVectorNode);
				SVectorNode tmpNode;
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)pSubConnectStruct->pVectorNode, (LPVOID)&tmpNode, sizeof(SVectorNode));
				DrawVectorNode(pDC, tmpNode, m_hTrainProcess);
				SConnectNode *pConnectNode = pSubConnectStruct->nDirect ? tmpNode.OutConnectNodePtr : tmpNode.InConnectNodePtr;
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)pConnectNode, (LPVOID)&connectNode, sizeof(SConnectNode));
				SQueueData newQueueData;

				if (connectNode.nType0 == 2)
				{
					ReadTrainProcess(m_hTrainProcess, (LPCVOID)connectNode.nodePointer20, (LPVOID)&newQueueData.connectStruct, sizeof(SConnectStruct));
					newQueueData.pVectorNode = pSubConnectStruct->pVectorNode;
					m_queueVectorNode.push(newQueueData);
				}
			}
		}
	}
}
void CAirViewDlg::DrawPathTracks(CDC *pDC)
{
	if (!GetTrainHandle(m_hTrainProcess) || !GetTrainPointer(m_hTrainProcess))
		return;

	float forwardLength;
	SVectorNode vectorNode;
	int nDirectOfHeadNode = m_currentHeadInfo.nDirection;
	ReadTrainProcess(m_hTrainProcess, (void *)m_currentHeadInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));
	calculateCurrentLocation(vectorNode, m_currentHeadInfo.fLocationInNode, m_hTrainProcess);

	if (nDirectOfHeadNode)
		forwardLength = - m_currentHeadInfo.fLocationInNode;
	else
		forwardLength = m_currentHeadInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nDirectOfNextNode;
	DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
	SVectorNode *nextNodePtr = GetNextNode(m_hTrainProcess, vectorNode, m_currentHeadInfo.pVectorNode, nDirectOfHeadNode, nDirectOfNextNode);

	while (forwardLength < 8 * m_fDistance && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
		forwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

	nDirectOfHeadNode = !nDirectOfHeadNode;
	float backwardLength;

	if (nDirectOfHeadNode)
		backwardLength = - m_currentHeadInfo.fLocationInNode;
	else
		backwardLength = m_currentHeadInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nDirectOfPrevNode = nDirectOfHeadNode;
	SVectorNode *prevNodePtr = GetNextNode(m_hTrainProcess, vectorNode, m_currentHeadInfo.pVectorNode, nDirectOfHeadNode, nDirectOfPrevNode);

	while (backwardLength < 8 * m_fDistance && prevNodePtr)
	{
		SVectorNode *currentNodePtr = prevNodePtr;
		int nDirectOfCurrentNode = nDirectOfPrevNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
		backwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		prevNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfPrevNode);
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
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, m_pTrackSectionArray, 0x10000 * sizeof(STrackSection), 2, 0xC, 0);
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8098F8, &m_currentAngle, 4);
	m_currentAngle -= (float)M_PI_2;
	STrackInfo headInfo;
	ReadTrainProcess(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo));
	float forwardLength;
	SVectorNode vectorNode;
	int nDirectOfHeadNode = headInfo.nDirection;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nDirectOfNextNode = nDirectOfHeadNode;
	SVectorNode *nextNodePtr = headInfo.pVectorNode;

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
		backwardLength = headInfo.fLocationInNode - vectorNode.fTrackNodeLength;

	int nDirectOfPrevNode = nDirectOfHeadNode;
	SVectorNode *prevNodePtr = headInfo.pVectorNode;

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
	Invalidate();
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

		if (m_fDistance < 200)
			m_fDistance = 200;
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
