// SignSpeedLimitDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SignSpeedLimit.h"
#include "SignSpeedLimitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSignSpeedLimitDlg 对话框




CSignSpeedLimitDlg::CSignSpeedLimitDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CSignSpeedLimitDlg::IDD, pParent)
	, m_textContent(_T(""))
	, m_bShowSpeedPost(FALSE)
	, m_bShowStation(FALSE)
	, m_bShowSiding(FALSE)
	, m_bAutoGetData(FALSE)
	, m_bShowTaskLimit(FALSE)
	, m_uForwardDistance(0)
	, m_bShowSignal(FALSE)
	, m_bShowTrackInfo(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSignSpeedLimitDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_textContent);
	DDX_Text(pDX, IDC_EDIT2, m_uForwardDistance);
	DDX_Check(pDX, IDC_CHECK1, m_bShowSpeedPost);
	DDX_Check(pDX, IDC_CHECK2, m_bShowStation);
	DDX_Check(pDX, IDC_CHECK3, m_bShowSiding);
	DDX_Check(pDX, IDC_CHECK4, m_bAutoGetData);
	DDX_Check(pDX, IDC_CHECK5, m_bShowTaskLimit);
	DDX_Check(pDX, IDC_CHECK6, m_bShowSignal);
	DDX_Check(pDX, IDC_CHECK7, m_bShowTrackInfo);
}

BEGIN_MESSAGE_MAP(CSignSpeedLimitDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSignSpeedLimitDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK1, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK2, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK3, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK4, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK5, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK6, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK7, &CSignSpeedLimitDlg::OnBnClickedCheck)
END_MESSAGE_MAP()


// CSignSpeedLimitDlg 消息处理程序

BOOL CSignSpeedLimitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu *pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);

		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码
	m_hTrainProcess = NULL;
	SetTimer(0, 1000, NULL);
	m_bShowSpeedPost = TRUE;
	m_bShowStation = TRUE;
	m_bShowTaskLimit = TRUE;
	m_bShowSiding = TRUE;
	m_bShowSignal = TRUE;
	m_bShowTrackInfo = TRUE;
	m_bAutoGetData = FALSE;
	m_uForwardDistance = 4;
	m_textContent = L"";
	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CSignSpeedLimitDlg::OnDestroy()
{
	if (m_hTrainProcess)
		CloseHandle(m_hTrainProcess);
}

BOOL CSignSpeedLimitDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:   //屏蔽Enter
			return true;
		case VK_ESCAPE:   //屏蔽Esc
			return true;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
void CSignSpeedLimitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSignSpeedLimitDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSignSpeedLimitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSignSpeedLimitDlg::OnGetData()
{
	vector<SSpeedPostLimit> limitVect;
	vector<SStationItem> stationVect;
	vector<SStationItem> sidingVect;
	vector<STempSpeedLimit> tempLimitVect;
	vector<SShowSignalItem> signalVect;
	vector<SSectionInfo> sectionVect;
	vector<SSectionInfo> backSectionVect;
	vector<SSpeedPostLimit> backLimitVect;
	vector<SStationItem> backStationVect;
	vector<SStationItem> backSidingVect;
	vector<SShowSignalItem> backSignalVect;
	STrackInfo headInfo;
	//headInfo is the information of the head of the train.
	m_textContent = L"";
	size_t trainInfo;
	ReadTrainProcess(m_hTrainProcess, (void *)TRAIN_INFO_MEM, (LPVOID)&trainInfo, 4);
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

	while (forwardLength < m_uForwardDistance * 1000 && nextNodePtr)
	{
		SVectorNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		AddTempSpeedLimit(forwardLength, currentNodePtr, tempLimitVect, m_hTrainProcess, nDirectOfCurrentNode);
		AddSpeedPostLimit(forwardLength, vectorNode, limitVect, m_hTrainProcess, nDirectOfCurrentNode, currentNodePtr, !nDirectOfCurrentNode);
		AddStationItem(forwardLength, vectorNode, stationVect, sidingVect, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode);
		AddSignalItem(forwardLength, vectorNode, signalVect, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode);
		AddSectionInfo(forwardLength, vectorNode, sectionVect, m_hTrainProcess, nDirectOfCurrentNode, !nDirectOfCurrentNode, pSection);
		CString res;
		res.Format(L"0X%X 0X%X %.1f %.1f ", currentNodePtr, vectorNode.pPtr44, forwardLength, vectorNode.fTrackNodeLength + forwardLength);
		//m_textContent += res;

		if (vectorNode.pPtr44 != NULL)
		{
			SNode node;
			ReadTrainProcess(m_hTrainProcess, vectorNode.pPtr44, &node, sizeof(SNode));

			if (node.pointer == vectorNode.pPtr44)
			{
#if 0
				ReadTrainProcess(m_hTrainProcess, node.next, &node, sizeof(SNode));
				SCarriageInTrackNode carriageInTrackNode;
				ReadTrainProcess(m_hTrainProcess, node.pointer, &carriageInTrackNode, sizeof(SCarriageInTrackNode));
				wchar_t srvFileName[0x100];
				wchar_t *pSrvFileName;
				// pSrvFileC may be NULL pointer.
				ReadTrainProcess(m_hTrainProcess, (char *)carriageInTrackNode.pSrvFileC + 0x10, &pSrvFileName, 4);
				ReadTrainProcess(m_hTrainProcess, pSrvFileName, &srvFileName, 0x200);
				int nCarriageType;
				ReadTrainProcess(m_hTrainProcess, (char *)carriageInTrackNode.pEngineOrWagonInConFile8 + 8, &nCarriageType, 4);
				wchar_t name[0x20];
				int dest = nCarriageType == 1 ? 0xD4 : 0x94;
				ReadTrainProcess(m_hTrainProcess, (char *)carriageInTrackNode.pEngineOrWagonInConFile8 + dest, name, 0x40);
				//m_textContent += srvFileName;
#endif
			}
		}

		//m_textContent += "\r\n";
		/*if(node.pointer != node.next)
		{
			CString res;
			res.Format(L"%.1f\r\n", forwardLength);
			m_textContent += res;
		}*/
		forwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

	/*** Copy And Modify Of Previous Code Start **/
	nDirectOfHeadNode = !nDirectOfHeadNode;
	float backwardLength;

	if (nDirectOfHeadNode)
		backwardLength = - headInfo.fLocationInNode;
	else
		backwardLength = headInfo.fLocationInNode - trackNode.fTrackNodeLength;

	int nDirectOfPrevNode = nDirectOfHeadNode;
	SVectorNode *prevNodePtr = headInfo.vectorNodePtr;

	while (backwardLength < 1000 && prevNodePtr)
	{
		SVectorNode *currentNodePtr = prevNodePtr;
		int nDirectOfCurrentNode = nDirectOfPrevNode;
		SVectorNode vectorNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&vectorNode, sizeof(SVectorNode));
		AddSpeedPostLimit(backwardLength, vectorNode, backLimitVect, m_hTrainProcess, nDirectOfCurrentNode, currentNodePtr, nDirectOfCurrentNode);
		AddStationItem(backwardLength, vectorNode, backStationVect, backSidingVect, m_hTrainProcess, nDirectOfCurrentNode, nDirectOfCurrentNode);
		AddSignalItem(backwardLength, vectorNode, backSignalVect, m_hTrainProcess, nDirectOfCurrentNode, nDirectOfCurrentNode);
		AddSectionInfo(backwardLength, vectorNode, backSectionVect, m_hTrainProcess, nDirectOfCurrentNode, nDirectOfCurrentNode, pSection);
		backwardLength += vectorNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		prevNodePtr = GetNextNode(m_hTrainProcess, vectorNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfPrevNode);
	}

	/*** Copy And Modify Of Previous Code End **/
	int nIsKiloMeter;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)DISTANCE_TYPE_MEM, &nIsKiloMeter, 4);

	if (m_bShowTaskLimit)
	{
		float *fTempLimitPtr;
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)TASK_LIMIT_MEM, &fTempLimitPtr, 4);
		float fTempLimit;
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)(fTempLimitPtr + 23), &fTempLimit, 4);
		CString strSpeed;

		if (nIsKiloMeter)
		{
			strSpeed.Format(L"任务临时限速 %.0f 公里/时\r\n", fTempLimit * 3.6);
		}
		else
		{
			strSpeed.Format(L"任务临时限速 %.0f 英里/时\r\n", fTempLimit * 2.237);
		}

		m_textContent += strSpeed;

		for (size_t i = 0; i < tempLimitVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f %.1f\r\n", tempLimitVect[i].fDistanceStart, tempLimitVect[i].fDistanceEnd);
			m_textContent += msg;
		}

		m_textContent += L"****************************************************\r\n";
	}

	int nOffset = bIsForward ? 0x62 : 0x66;
	float fCarriageLength;
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)THIS_POINTER_MEM, &fCarriageLength, 4, 3, nOffset, 0x94, 0x400);

	if (m_bShowSpeedPost)
	{
		CString temp;
		float fRate = 1.0f;

		if (nIsKiloMeter)
		{
			temp = L"标志限速(公里/时)\r\n";
		}
		else
		{
			temp = L"标志限速(英里/时)\r\n";
			fRate = 1.609f;
		}

		m_textContent += temp;

		for (size_t i = backLimitVect.size(); i > 0; )
		{
			--i;
			CString msg;
			msg.Format(L"%.1f %.0f\r\n", -backLimitVect[i].fDistance - fCarriageLength, backLimitVect[i].fLimitNum / fRate);
			m_textContent += msg;
		}

		for (size_t i = 0; i < limitVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f %.0f\r\n", limitVect[i].fDistance - fCarriageLength, limitVect[i].fLimitNum / fRate);
			m_textContent += msg;
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (m_bShowStation)
	{
		m_textContent += L"车站名称\r\n";
		/*for (size_t i = backStationVect.size() ; i > 0;)
		{
			--i;
			m_textContent += getStationString(backStationVect[i]);
		}*/

		for (size_t i = 0; i < stationVect.size(); ++i)
		{
			m_textContent += getStationString(stationVect[i]);
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (m_bShowSiding)
	{
		m_textContent += L"侧线名称\r\n";
		/*for (size_t i = backSidingVect.size(); i > 0;)
		{
			--i;
			m_textContent += getStationString(backSidingVect[i]);
		}*/

		for (size_t i = 0; i < sidingVect.size(); ++i)
		{
			m_textContent += getStationString(sidingVect[i]);
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (m_bShowSignal)
	{
		m_textContent += L"信号\r\n";

		for (size_t i = backSignalVect.size(); i > 0;)
		{
			--i;
			CString msg;
			msg.Format(L"%s %.1f ", changeESignalTypeToString(backSignalVect[i].eSignalType), -backSignalVect[i].fDistance - fCarriageLength / 2);

			if (backSignalVect[i].eSignalType == NORMAL)
			{
				CString strSignalLimit;
				strSignalLimit.Format(L"限速%.1f ", backSignalVect[i].fSignalSpeed * 3.6f);
				msg += strSignalLimit;
			}

			msg += changeColorToString(backSignalVect[i].nLightColor);
			msg += L"\r\n";
			m_textContent += msg;
		}

		for (size_t i = 0; i < signalVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%s %.1f ", changeESignalTypeToString(signalVect[i].eSignalType), signalVect[i].fDistance - fCarriageLength / 2);

			if (signalVect[i].eSignalType == NORMAL)
			{
				CString strSignalLimit;
				strSignalLimit.Format(L"限速%.1f ", signalVect[i].fSignalSpeed * 3.6f);
				msg += strSignalLimit;
			}

			msg += changeColorToString(signalVect[i].nLightColor);
			msg += L"\r\n";
			m_textContent += msg;
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (m_bShowTrackInfo)
	{
		/*for (size_t i = backSectionVect.size(); i > 0;)
		{
			--i;
			CString msg;
			msg.Format(L"%.1f %.1f ", -backSectionVect[i].fEnd, -backSectionVect[i].fStart);
			m_textContent += msg;
			m_textContent += getTrackSectionString(backSectionVect[i]);
		}*/
		for (size_t i = 0; i < sectionVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f %.1f ", sectionVect[i].fStart, sectionVect[i].fEnd);
			m_textContent += msg;
			m_textContent += getTrackSectionString(sectionVect[i]);
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (!nextNodePtr)
	{
		CString msg;
		msg.Format(L"%.1f 铁轨尽头", forwardLength);
		m_textContent += msg;
	}
}

void CSignSpeedLimitDlg::OnBnClickedOk()
{
	UpdateData();

	if (!GetTrainHandle(m_hTrainProcess))
	{
		m_textContent = L"等待MSTS启动";
		UpdateData(FALSE);
		return;
	}

	if (!GetTrainPointer(m_hTrainProcess))
	{
		m_textContent = L"等待MSTS任务运行";
		UpdateData(FALSE);
		return;
	}

	try
	{
		OnGetData();
	}
	catch (int)
	{
		m_textContent = L"获取数据失败";
	}

	UpdateData(FALSE);
}

void CSignSpeedLimitDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnTimer(nIDEvent);

	if (m_bAutoGetData)
		OnBnClickedOk();
}

void CSignSpeedLimitDlg::OnBnClickedCheck()
{
	UpdateData();
}
