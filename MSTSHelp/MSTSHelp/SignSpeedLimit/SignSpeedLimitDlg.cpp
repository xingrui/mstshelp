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
	, m_uForwardDistance(0)
	, m_bAutoGetData(FALSE)
	, m_bShowTaskLimit(FALSE)
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
}

BEGIN_MESSAGE_MAP(CSignSpeedLimitDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSignSpeedLimitDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK1, &CSignSpeedLimitDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CSignSpeedLimitDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CSignSpeedLimitDlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &CSignSpeedLimitDlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &CSignSpeedLimitDlg::OnBnClickedCheck5)
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
	m_uForwardDistance = 4;
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

CString DefaultHandle(HANDLE handle, void *pointer)
{
	CString result;
	result.Format(L"0x%X\r\n", pointer);
	return result;
}

CString AITrainHandle(HANDLE handle, void *pointer)
{
	size_t mem;
	wchar_t name[0x20];
	ReadTrainProcess(handle, (char *)pointer + 0x11C, &mem, 4);
	ReadTrainProcess(handle, (LPCVOID)(mem + 8), name, 0x40);
	ReadTrainProcess(handle, (char *)pointer + 76, &mem, 4);
	size_t mem2;
	wchar_t trainTrips[0x20];
	ReadTrainProcess(handle, (char *)pointer + 8, &mem2, 4);
	ReadTrainProcess(handle, (LPCVOID)mem2, trainTrips, 0x40);
	CString result;
	result.Format(L"0x%X %s %s\r\n", mem, name, trainTrips);

	if (mem == NULL)
	{
		return CString();
	}
	else
	{
		return result;
	}
}

CString showAllCarriage(HANDLE handle)
{
	CString strResult = IteratorList(handle, (LPVOID)0x8099BC, DefaultHandle);
	return strResult;
}

CString showAllAITrain(HANDLE handle)
{
	CString strResult = IteratorList(handle, (LPVOID)0x809AF8, AITrainHandle);
	return strResult;
}

void CSignSpeedLimitDlg::OnGetData()
{
	vector<SSpeedPostLimit> limitVect;
	vector<SStationItem> stationVect;
	vector<SStationItem> sidingVect;
	vector<STempSpeedLimit> tempLimitVect;
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
	STrackNode trackNode;
	int nDirectOfHeadNode = headInfo.nDirection == bIsForward;
	ReadTrainProcess(m_hTrainProcess, (void *)headInfo.trackNodePtr, (LPVOID)&trackNode, sizeof(STrackNode));

	if (nDirectOfHeadNode)
		forwardLength = - headInfo.fLocationInNode;
	else
		forwardLength = headInfo.fLocationInNode - trackNode.fTrackNodeLength;

	int nDirectOfNextNode = nDirectOfHeadNode;
	STrackNode *nextNodePtr = headInfo.trackNodePtr;

	while (forwardLength < m_uForwardDistance * 1000 && nextNodePtr)
	{
		STrackNode *currentNodePtr = nextNodePtr;
		int nDirectOfCurrentNode = nDirectOfNextNode;
		STrackNode trackNode;
		ReadTrainProcess(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(STrackNode));
		AddTempSpeedLimit(forwardLength, currentNodePtr, tempLimitVect, m_hTrainProcess, nDirectOfCurrentNode);
		AddSpeedPostLimit(forwardLength, trackNode, limitVect, m_hTrainProcess, nDirectOfCurrentNode, currentNodePtr);
		AddStationItem(forwardLength, trackNode, stationVect, sidingVect, m_hTrainProcess, nDirectOfCurrentNode);
		forwardLength += trackNode.fTrackNodeLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode, currentNodePtr, nDirectOfCurrentNode, nDirectOfNextNode);
	}

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

	if (m_bShowSpeedPost)
	{
		CString temp;
		float fRate = 1.0f;

		if (nIsKiloMeter)
		{
			temp = L"前方标志限速(公里/时)\r\n";
		}
		else
		{
			temp = L"前方标志限速(英里/时)\r\n";
			fRate = 1.609f;
		}

		m_textContent += temp;

		for (size_t i = 0; i < limitVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f %.0f\r\n", limitVect[i].fDistance, limitVect[i].fLimitNum / fRate);
			m_textContent += msg;
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (m_bShowStation)
	{
		m_textContent += L"前方车站名称\r\n";

		for (size_t i = 0; i < stationVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f ", stationVect[i].fDistance);
			msg += stationVect[i].stationName;
			msg += L"\r\n";
			m_textContent += msg;
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (m_bShowSiding)
	{
		m_textContent += L"前方边线名称\r\n";

		for (size_t i = 0; i < sidingVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f ", sidingVect[i].fDistance);
			msg += sidingVect[i].stationName;
			msg += L"\r\n";
			m_textContent += msg;
		}

		m_textContent += L"****************************************************\r\n";
	}

	if (!nextNodePtr)
	{
		CString msg;
		msg.Format(L"%.1f 铁轨尽头", forwardLength);
		m_textContent += msg;
	}

	//m_textContent += showAllCarriage(m_hTrainProcess);

	/*wchar_t name[0x100];
	size_t mem;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8099B0, &mem, 4);
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)(mem + 8), name, 200);
	m_textContent = name;
	m_textContent += L"\r\n";*/
	m_textContent += showAllAITrain(m_hTrainProcess);
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

void CSignSpeedLimitDlg::OnBnClickedCheck1()
{
	UpdateData();
}

void CSignSpeedLimitDlg::OnBnClickedCheck2()
{
	UpdateData();
}

void CSignSpeedLimitDlg::OnBnClickedCheck3()
{
	UpdateData();
}

void CSignSpeedLimitDlg::OnBnClickedCheck4()
{
	UpdateData();
}


void CSignSpeedLimitDlg::OnBnClickedCheck5()
{
	UpdateData();
}

CString TempSpeedFunc(HANDLE handle, void *ptr)
{
	CString str;
	char *cPtr = (char *)ptr;
	STempSpeed speed;
	ReadTrainProcess(handle, cPtr + 32, &speed, sizeof(STempSpeed));
	str.Format(L"%x %x %.1f %.1f", ptr, speed.nodePtr, speed.fStart, speed.fEnd);
	return str;
}

void CSignSpeedLimitDlg::GetTrainData(void *startLocation)
{
}

//Remain This Method For Test
void CSignSpeedLimitDlg::OnBnClickedTest()
{
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

	CString strResult = IteratorList(m_hTrainProcess, (LPVOID)TASK_LIMIT_HEAD_MEM, TempSpeedFunc);
	float *fTempLimitPtr;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)TASK_LIMIT_MEM, &fTempLimitPtr, 4);
	float fTempLimit;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)(fTempLimitPtr + 23), &fTempLimit, 4);
	int nType;
	ReadTrainProcess(m_hTrainProcess, (LPCVOID)DISTANCE_TYPE_MEM, &nType, 4);
	m_textContent = strResult;
	CString strSpeed;

	if (nType)
		strSpeed.Format(L"Temp Speed Limit %.0f km\r\n", fTempLimit * 3.6);
	else
		strSpeed.Format(L"Temp Speed Limit %.0f mile\r\n", fTempLimit * 2.237);

	m_textContent += strSpeed;
	UpdateData(false);
}
