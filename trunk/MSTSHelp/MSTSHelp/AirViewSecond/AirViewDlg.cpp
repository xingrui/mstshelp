// AirViewDlg.cpp : ʵ���ļ�
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
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
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
	m_fDistance = 4000;
	InitSavedData();
	m_savedData.pTrackSectionArray = new STrackSection[0x10000];
	SetTimer(0, 200, NULL);
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL CAirViewDlg::GetHandleAndPrepareData()
{
	if (m_hTrainProcess)
	{
		DWORD dwState;

		if (GetExitCodeProcess(m_hTrainProcess, &dwState))
		{
			if (dwState == STILL_ACTIVE)
			{
				void *pTrain = GetTrainPointer(m_hTrainProcess);

				if (pTrain == NULL)
					return FALSE;
				else if (pTrain == m_savedData.pTrain)
				{
					ReadTrainProcess(m_hTrainProcess, (LPCVOID)HEAD_TRACK_MEM, (LPVOID)&m_currentHeadInfo, sizeof(STrackInfo));
					return TRUE;
				}
				else
				{
					m_savedData.pTrain = pTrain;
					ReadTrainProcess(m_hTrainProcess, (LPCVOID)HEAD_TRACK_MEM, (LPVOID)&m_currentHeadInfo, sizeof(STrackInfo));
					ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, m_savedData.pTrackSectionArray, 0x10000 * sizeof(STrackSection), 2, 0xC, 0);
					m_savedData.vectDrawUnit.clear();
					GetAllTracksDataByTDBFile();
					return TRUE;
				}
			}
			else
			{
				CloseHandle(m_hTrainProcess);
				m_hTrainProcess  = NULL;
			}
		}
		else
		{
			CloseHandle(m_hTrainProcess);
			m_hTrainProcess  = NULL;
		}
	}

	HWND hWnd = ::FindWindow(L"TrainSim", NULL);

	if (hWnd)
	{
		DWORD nProcID;
		GetWindowThreadProcessId(hWnd, &nProcID);
		m_hTrainProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, nProcID);

		if (!m_hTrainProcess)
		{
			return FALSE;
		}
		else
		{
			void *pTrain = GetTrainPointer(m_hTrainProcess);

			if (pTrain)
			{
				m_savedData.pTrain = pTrain;
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)HEAD_TRACK_MEM, (LPVOID)&m_currentHeadInfo, sizeof(STrackInfo));
				ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, m_savedData.pTrackSectionArray, 0x10000 * sizeof(STrackSection), 2, 0xC, 0);
				m_savedData.vectDrawUnit.clear();
				GetAllTracksDataByTDBFile();
				return TRUE;
			}
			else
				return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
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
			if (GetHandleAndPrepareData())
			{
				SVectorNode vectorNode;
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)m_currentHeadInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));
				float fDistance = m_currentHeadInfo.fLocationInNode;
				CalculateCurrentLocation(vectorNode, fDistance, m_hTrainProcess);
				DrawUnits(&MemDC);
				CPen pen(PS_SOLID, 1, RGB(0, 255, 0));
				CPen *pOldPen = MemDC.SelectObject(&pen);
				DrawPathTracks(&MemDC);
				MemDC.SelectObject(pOldPen);
				pen.DeleteObject();
				CBrush brush, *pOldBrush;
				brush.CreateSolidBrush(RGB(255, 0, 0));
				pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
				pOldBrush = MemDC.SelectObject(&brush);
				pOldPen = MemDC.SelectObject(&pen);
				CGdiObject *pObject = MemDC.SelectStockObject(ANSI_FIXED_FONT);
				MemDC.SetBkMode(TRANSPARENT);
				DrawAllAITracks(&MemDC);
				MemDC.SelectObject(pOldPen);
				MemDC.SelectObject(pOldBrush);
				brush.DeleteObject();
				pen.DeleteObject();
			}
			else
			{
				m_savedData.pTrain = NULL;
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
// ���Ʊ�����
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
void CAirViewDlg::CalculateCurrentLocation(const SVectorNode &node, float fCurrentLocation, HANDLE handle)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	float fDistance;
	float fCurrentLength = 0;
	fDistance = fCurrentLocation;

	for (int i = 0; i < num; ++i)
	{
		STrackSection *pCurSection = m_savedData.pTrackSectionArray + pSectionData[i].sectionIndex;
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
bool CAirViewDlg::DrawVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	double fCurrentX = 0;
	double fCurrentY = 0;

	for (int i = 0; i != num; ++i)
	{
		fCurrentX = pSectionData[i].TileX2 * 2048 + pSectionData[i].X - m_startLocation.fPointX;
		fCurrentY = pSectionData[i].TileZ2 * 2048 + pSectionData[i].Z - m_startLocation.fPointY;
		double currentAngle;
		currentAngle = pSectionData[i].AY;
		currentAngle = M_PI_2 - currentAngle;
		STrackSection *pCurSection = m_savedData.pTrackSectionArray + pSectionData[i].sectionIndex;
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
	float minDistance = m_fDistance < 10000 ? 10000 : m_fDistance;
	return fCurrentX * fCurrentX + fCurrentY * fCurrentY < 64 * minDistance * minDistance;
}
void CAirViewDlg::DrawPointInVectorNode(CDC *pDC, const SVectorNode &node, HANDLE handle, float fLocation, CString strName)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	float fCurrentLength = 0;

	for (int i = 0; i != num; ++i)
	{
		STrackSection *pCurSection = m_savedData.pTrackSectionArray + pSectionData[i].sectionIndex;
		float fRaidus = pCurSection->fSectionCurveFirstRadius4;
		float fSectionLength = pCurSection->fSectionSizeSecondLength0;

		if (fLocation > fCurrentLength + fSectionLength)
		{
			fCurrentLength += fSectionLength;
		}
		else
		{
			float fRemainLength = fLocation - fCurrentLength;
			double fCurrentX = pSectionData[i].TileX2 * 2048 + pSectionData[i].X - m_startLocation.fPointX;
			double fCurrentY = pSectionData[i].TileZ2 * 2048 + pSectionData[i].Z - m_startLocation.fPointY;
			float currentAngle = (float)M_PI_2 - pSectionData[i].AY;
			float fZ = pSectionData[i].Y;

			if (pCurSection->fSectionCurveSecondAngle8 == 0)
			{
				fCurrentX += fRemainLength * cos(currentAngle);
				fCurrentY += fRemainLength * sin(currentAngle);
			}
			else if (pCurSection->fSectionCurveSecondAngle8 < 0)
			{
				float fRadius = pCurSection->fSectionCurveFirstRadius4;
				double fCenterX = fCurrentX, fCenterY = fCurrentY;
				double fPreX = fCurrentX, fPreY = fCurrentY;
				fCenterX -= fRadius * sin(currentAngle);
				fCenterY += fRadius * cos(currentAngle);
				currentAngle += fRemainLength / fRadius;
				fCurrentX = fCenterX + fRadius * sin(currentAngle);
				fCurrentY = fCenterY - fRadius * cos(currentAngle);
			}
			else
			{
				float fRadius = pCurSection->fSectionCurveFirstRadius4;
				double fCenterX = fCurrentX, fCenterY = fCurrentY;
				double fPreX = fCurrentX, fPreY = fCurrentY;
				fCenterX += fRadius * sin(currentAngle);
				fCenterY -= fRadius * cos(currentAngle);
				currentAngle -= fRemainLength / fRadius;
				fCurrentX = fCenterX - fRadius * sin(currentAngle);
				fCurrentY = fCenterY + fRadius * cos(currentAngle);
			}

			CRect rect;
			GetClientRect(&rect);
			float fEllipse = 5 * m_fDistance / rect.Height();
			int nX1 = (int)((fCurrentX - fEllipse) * TIMES);
			int nY1 = (int)((fCurrentY - fEllipse) * TIMES);
			int nX2 = (int)((fCurrentX + fEllipse) * TIMES);
			int nY2 = (int)((fCurrentY + fEllipse) * TIMES);
			pDC->Ellipse(nX1, nY1, nX2, nY2);
			pDC->TextOut(nX2, nY2, strName);
			break;
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
void CAirViewDlg::DrawAllAITracks(CDC *pDC)
{
	int count = 0;
	SNode *head;
	SNode iteNode;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x809AF8, &head, 4);
	ReadTrainProcess(m_hTrainProcess, head, &iteNode, sizeof(SNode));

	if (iteNode.pointer != head)
	{
		return;
	}

	while (iteNode.next != head && count < 10000)
	{
		SNode *next = iteNode.next;
		ReadTrainProcess(m_hTrainProcess, next, &iteNode, sizeof(SNode));
		size_t pVectorNode;
		float fLocation;
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0x4C, &pVectorNode, 4);
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0x5C, &fLocation, 4);
		size_t pbackVectorNode;
		float fbackLocation;
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0xB4, &pbackVectorNode, 4);
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0xC4, &fbackLocation, 4);
		size_t pWCTrain_Config;
		wchar_t trainTrips[0x100];
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0x10, &pWCTrain_Config, 4);
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)pWCTrain_Config, trainTrips, 0x40);
		wchar_t trainTrips2[0x100];
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0x8, &pWCTrain_Config, 4);
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)pWCTrain_Config, trainTrips2, 0x40);
		CString strOutput = trainTrips2;
		//strOutput += L" : ";
		//strOutput += trainTrips2;
		CString result;
		result.Format(L"0x%X %s\r\n", pVectorNode, trainTrips);
		DWORD dwData;
		ReadTrainProcess(m_hTrainProcess, (char *)iteNode.pointer + 0x144, &dwData, 4);

		if (pVectorNode && dwData)
		{
			SVectorNode vectorNode;
			ReadTrainProcess(m_hTrainProcess, (LPCVOID)pVectorNode, &vectorNode, sizeof(SVectorNode));
			DrawPointInVectorNode(pDC, vectorNode, m_hTrainProcess, fLocation, strOutput);
			ReadTrainProcess(m_hTrainProcess, (LPCVOID)pbackVectorNode, &vectorNode, sizeof(SVectorNode));
			DrawPointInVectorNode(pDC, vectorNode, m_hTrainProcess, fbackLocation, strOutput);
			//DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
		}

		++count;
	}

	CString strCount;
	strCount.Format(L"Total Count : %d", count);
}
void CAirViewDlg::DrawAllTracks(CDC *pDC)
{
	set<SVectorNode *> setVectorNode;
	queue<SQueueData> queueVectorNode;
	SVectorNode vectorNode;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)m_currentHeadInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));
	setVectorNode.insert(m_currentHeadInfo.pVectorNode);
	SQueueData queueData;
	SConnectNode connectNode;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)vectorNode.InConnectNodePtr, (LPVOID)&connectNode, sizeof(SConnectNode));

	if (connectNode.nType0 == 2)
	{
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)connectNode.nodePointer20, (LPVOID)&queueData.connectStruct, sizeof(SConnectStruct));
		queueData.pVectorNode = m_currentHeadInfo.pVectorNode;
		queueVectorNode.push(queueData);
	}

	DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)vectorNode.OutConnectNodePtr, (LPVOID)&connectNode, sizeof(SConnectNode));

	if (connectNode.nType0 == 2)
	{
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)connectNode.nodePointer20, (LPVOID)&queueData.connectStruct, sizeof(SConnectStruct));
		queueData.pVectorNode = m_currentHeadInfo.pVectorNode;
		queueVectorNode.push(queueData);
	}

	while (!queueVectorNode.empty())
	{
		SQueueData queueData = queueVectorNode.front();
		queueVectorNode.pop();

		for (int i = 0; i < 3; ++i)
		{
			SSubConnectStruct *pSubConnectStruct = queueData.connectStruct.subStruct + i;

			if (pSubConnectStruct->pVectorNode != queueData.pVectorNode && pSubConnectStruct->pVectorNode != NULL)
			{
				if (setVectorNode.find(pSubConnectStruct->pVectorNode) != setVectorNode.end())
					continue;

				setVectorNode.insert(pSubConnectStruct->pVectorNode);
				SVectorNode tmpNode;
				ReadTrainProcess(m_hTrainProcess, (LPCVOID)pSubConnectStruct->pVectorNode, (LPVOID)&tmpNode, sizeof(SVectorNode));

				if (DrawVectorNode(pDC, tmpNode, m_hTrainProcess))
				{
					SConnectNode *pConnectNode = pSubConnectStruct->nDirect ? tmpNode.OutConnectNodePtr : tmpNode.InConnectNodePtr;
					ReadTrainProcess(m_hTrainProcess, (LPCVOID)pConnectNode, (LPVOID)&connectNode, sizeof(SConnectNode));
					SQueueData newQueueData;

					if (connectNode.nType0 == 2)
					{
						ReadTrainProcess(m_hTrainProcess, (LPCVOID)connectNode.nodePointer20, (LPVOID)&newQueueData.connectStruct, sizeof(SConnectStruct));
						newQueueData.pVectorNode = pSubConnectStruct->pVectorNode;
						queueVectorNode.push(newQueueData);
					}
				}
			}
		}
	}
}
void CAirViewDlg::DrawUnits(CDC *pDC)
{
	vector<SDrawUnit>::iterator it = m_savedData.vectDrawUnit.begin();

	while (it != m_savedData.vectDrawUnit.end())
	{
		if (it->nType == 1)
		{
			DrawMoveTo(pDC, it->fromX - m_startLocation.fPointX, it->fromY - m_startLocation.fPointY);
			DrawLineTo(pDC, it->toX - m_startLocation.fPointX, it->toY - m_startLocation.fPointY);
		}
		else
		{
			DrawArc(pDC, it->centerX - it->radius - m_startLocation.fPointX,
			        it->centerY - it->radius - m_startLocation.fPointY,
			        it->centerX + it->radius - m_startLocation.fPointX,
			        it->centerY + it->radius - m_startLocation.fPointY,
			        it->fromX - m_startLocation.fPointX,
			        it->fromY - m_startLocation.fPointY,
			        it->toX - m_startLocation.fPointX,
			        it->toY - m_startLocation.fPointY);
		}

		++it;
	}
}
void CAirViewDlg::DrawAllTracksByTDBFile(CDC *pDC)
{
	SVectorNode vectorNode;
	struct STDBFilePart
	{
		SVectorNode **ppTrackNodePtrArray0;
		int nTrackNodeNumber4;
	};
	STDBFilePart tdbFile;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A038, &tdbFile, sizeof(STDBFilePart), 2, 0xC, 0);
	size_t *ppVectorNode = new size_t[tdbFile.nTrackNodeNumber4];
	ReadTrainProcess(m_hTrainProcess, tdbFile.ppTrackNodePtrArray0, ppVectorNode, 4 * tdbFile.nTrackNodeNumber4);

	for (int i = 0; i < tdbFile.nTrackNodeNumber4; ++i)
	{
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)ppVectorNode[i], &vectorNode, sizeof(SVectorNode));

		if (vectorNode.data0 == 1)
			DrawVectorNode(pDC, vectorNode, m_hTrainProcess);
	}

	delete[] ppVectorNode;
}
void CAirViewDlg::GetAllTracksDataByTDBFile()
{
	SVectorNode vectorNode;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)m_currentHeadInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));
	struct STDBFilePart
	{
		SVectorNode **ppTrackNodePtrArray0;
		int nTrackNodeNumber4;
	};
	STDBFilePart tdbFile;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A038, &tdbFile, sizeof(STDBFilePart), 2, 0xC, 0);
	size_t *ppVectorNode = new size_t[tdbFile.nTrackNodeNumber4];
	ReadTrainProcess(m_hTrainProcess, tdbFile.ppTrackNodePtrArray0, ppVectorNode, 4 * tdbFile.nTrackNodeNumber4);

	for (int i = 0; i < tdbFile.nTrackNodeNumber4; ++i)
	{
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)ppVectorNode[i], &vectorNode, sizeof(SVectorNode));

		if (vectorNode.data0 == 1)
			GetVectorNodeData(vectorNode, m_hTrainProcess);
	}

	delete[] ppVectorNode;
}
void CAirViewDlg::GetVectorNodeData(const SVectorNode &node, HANDLE handle)
{
	int num = node.nSectionNum;
	SVectorSection *pSectionData = new SVectorSection[num];
	ReadTrainProcess(handle, (LPCVOID)node.sectionArrayPtr, pSectionData, num * sizeof(SVectorSection));
	double fCurrentX = 0;
	double fCurrentY = 0;
	SDrawUnit drawUnit;

	for (int i = 0; i != num; ++i)
	{
		fCurrentX = pSectionData[i].TileX2 * 2048 + pSectionData[i].X;
		fCurrentY = pSectionData[i].TileZ2 * 2048 + pSectionData[i].Z;
		double currentAngle;
		currentAngle = pSectionData[i].AY;
		currentAngle = M_PI_2 - currentAngle;
		STrackSection *pCurSection = m_savedData.pTrackSectionArray + pSectionData[i].sectionIndex;
		float fRaidus = pCurSection->fSectionCurveFirstRadius4;

		if (pCurSection->fSectionCurveSecondAngle8 == 0)
		{
			drawUnit.nType = 1;
			float fLength = pCurSection->fSectionSizeSecondLength0;
			drawUnit.fromX = fCurrentX;
			drawUnit.fromY = fCurrentY;
			fCurrentX += fLength * cos(currentAngle);
			fCurrentY += fLength * sin(currentAngle);
			drawUnit.toX = fCurrentX;
			drawUnit.toY = fCurrentY;
		}
		else if (pCurSection->fSectionCurveSecondAngle8 < 0)
		{
			drawUnit.nType = 2;
			double fRadius = pCurSection->fSectionCurveFirstRadius4;
			double fCenterX = fCurrentX, fCenterY = fCurrentY;
			double fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX -= fRadius * sin(currentAngle);
			fCenterY += fRadius * cos(currentAngle);
			currentAngle += pCurSection->fSectionSizeSecondLength0 / fRadius;
			fCurrentX = fCenterX + fRadius * sin(currentAngle);
			fCurrentY = fCenterY - fRadius * cos(currentAngle);
			drawUnit.centerX = fCenterX;
			drawUnit.centerY = fCenterY;
			drawUnit.fromX = fPreX;
			drawUnit.fromY = fPreY;
			drawUnit.toX = fCurrentX;
			drawUnit.toY = fCurrentY;
			drawUnit.radius = fRadius;
		}
		else
		{
			drawUnit.nType = 2;
			double fRadius = pCurSection->fSectionCurveFirstRadius4;
			double fCenterX = fCurrentX, fCenterY = fCurrentY;
			double fPreX = fCurrentX, fPreY = fCurrentY;
			fCenterX += fRadius * sin(currentAngle);
			fCenterY -= fRadius * cos(currentAngle);
			currentAngle -= pCurSection->fSectionSizeSecondLength0 / fRadius;
			fCurrentX = fCenterX - fRadius * sin(currentAngle);
			fCurrentY = fCenterY + fRadius * cos(currentAngle);
			drawUnit.centerX = fCenterX;
			drawUnit.centerY = fCenterY;
			drawUnit.fromX = fCurrentX;
			drawUnit.fromY = fCurrentY;
			drawUnit.toX = fPreX;
			drawUnit.toY = fPreY;
			drawUnit.radius = fRadius;
		}

		m_savedData.vectDrawUnit.push_back(drawUnit);
	}

	delete []pSectionData;
}
void CAirViewDlg::DrawPathTracks(CDC *pDC)
{
	float forwardLength;
	SVectorNode vectorNode;
	int nDirectOfHeadNode = m_currentHeadInfo.nDirection;
	ReadTrainProcess(m_hTrainProcess, (void *)m_currentHeadInfo.pVectorNode, (LPVOID)&vectorNode, sizeof(SVectorNode));

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
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80A118, m_savedData.pTrackSectionArray, 0x10000 * sizeof(STrackSection), 2, 0xC, 0);
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
		AddSectionInfo(forwardLength, vectorNode, vectSectionInfo, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode, m_savedData.pTrackSectionArray);
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
		AddSectionInfo(backwardLength, vectorNode, backVectSectionInfo, m_hTrainProcess, nDirectOfCurrentNode, nDirectOfCurrentNode, m_savedData.pTrackSectionArray);
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnTimer(nIDEvent);
	Invalidate();
}

void CAirViewDlg::OnDestroy()
{
	CDialog::OnDestroy();
	delete[] m_savedData.pTrackSectionArray;

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
