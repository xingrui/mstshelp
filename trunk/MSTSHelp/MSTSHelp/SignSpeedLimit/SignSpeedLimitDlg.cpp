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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSignSpeedLimitDlg 对话框




CSignSpeedLimitDlg::CSignSpeedLimitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSignSpeedLimitDlg::IDD, pParent)
	, m_textContent(_T(""))
	, m_bShowSpeedPost(FALSE)
	, m_bShowStation(FALSE)
	, m_bShowSiding(FALSE)
	, m_uForwardDistance(0)
	, m_bAutoGetData(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSignSpeedLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_textContent);
	DDX_Check(pDX, IDC_CHECK1, m_bShowSpeedPost);
	DDX_Check(pDX, IDC_CHECK2, m_bShowStation);
	DDX_Check(pDX, IDC_CHECK3, m_bShowSiding);
	DDX_Text(pDX, IDC_EDIT2, m_uForwardDistance);
	DDX_Check(pDX, IDC_CHECK4, m_bAutoGetData);
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
	ON_BN_CLICKED(IDC_BUTTON3, &CSignSpeedLimitDlg::OnBnClickedTest)
END_MESSAGE_MAP()


// CSignSpeedLimitDlg 消息处理程序

BOOL CSignSpeedLimitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
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
	m_uForwardDistance = 4;
	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

CString showTrackInfo(const STrackInfo& trackInfo)
{
	CString msg;
	msg.Format(L"nLeftNodeNum : %d, Direct %d, LeftLen : %f, SectLen : %f",
		trackInfo.nLeftNodeNum, trackInfo.nDirection, trackInfo.fNodeLeftLength, trackInfo.fSectionLeftLength);
	return msg;
}

STrackNode* GetNext(STrackNode* nodePtr, const SConnectStruct& connectStruct, const SConnectNode& connectNode, 
					int direction, int&nextDirect)
{
	if(connectNode.nType == 2)//JunctionNode
	{
		if(nodePtr == connectStruct.nodePtr1 && connectStruct.nDirect1 == direction)
		{
			if(connectNode.direction2 )
			{
				nextDirect = connectStruct.nDirect3;
				return connectStruct.nodePtr3;
			}else
			{
				nextDirect = connectStruct.nDirect2;
				return connectStruct.nodePtr2;
			}
		}else if(nodePtr == connectStruct.nodePtr2 && connectStruct.nDirect2 == direction){
			nextDirect = connectStruct.nDirect1;
			return connectStruct.nodePtr1;
		}else if(nodePtr == connectStruct.nodePtr3 && connectStruct.nDirect3 == direction){
			nextDirect = connectStruct.nDirect1;
			return connectStruct.nodePtr1;
		}
	}
	//EndNode Or can not find the pointer in the struct.
	return NULL;
}

STrackNode* GetNextNode(HANDLE handle, const STrackNode& node, STrackNode* nodePtr, int direction, int&nextDirect)
{
	SConnectNode connectNode;
	SConnectStruct connectStruct;
	STrackNode*next;
	ReadProcessMemory(handle, (void *)node.connectNodePtr1, (LPVOID)&connectNode, sizeof(SConnectNode), NULL);
	ReadProcessMemory(handle, (void *)connectNode.nodePointer, (LPVOID)&connectStruct, sizeof(SConnectStruct), NULL);
	next = GetNext(nodePtr, connectStruct, connectNode, direction, nextDirect);
	if(next)
		return next;
	ReadProcessMemory(handle, (void *)node.connectNodePtr2, (LPVOID)&connectNode, sizeof(SConnectNode), NULL);
	ReadProcessMemory(handle, (void *)connectNode.nodePointer, (LPVOID)&connectStruct, sizeof(SConnectStruct), NULL);
	next = GetNext(nodePtr, connectStruct, connectNode, direction, nextDirect);
	return next;
}

void CSignSpeedLimitDlg::OnBnClickedOk()
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
	vector<SSpeedPostLimit> limitVect;
	vector<SStationItem> stationVect;
	vector<STempSpeedLimit> tempLimitVect;
	STrackInfo headInfo, tailInfo;
	//headInfo is the information of the head of the train.
	//tailInfo is the information of the tail of the train.
	CString info;
	ReadProcessMemory(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo), NULL);
	//m_textContent.Format(L"0x%X ", headInfo.trackNodePtr);
	UpdateData();
	m_textContent = L"";
	float forwardLength;
	STrackNode trackNode;
	int nDirectToFindInConnectNode = !headInfo.nDirection;
	ReadProcessMemory(m_hTrainProcess, (void *)headInfo.trackNodePtr, (LPVOID)&trackNode, sizeof(STrackNode), NULL);
	int nDirectOfNextNode;
	STrackNode* nextNodePtr = GetNextNode(m_hTrainProcess, trackNode,headInfo.trackNodePtr, nDirectToFindInConnectNode,nDirectOfNextNode);
	if(headInfo.nDirection)
	{
		forwardLength = trackNode.fSectionLength - headInfo.fNodeLeftLength;
	}else
	{
		forwardLength = headInfo.fNodeLeftLength;
	}
	AddTempSpeedLimit(forwardLength - trackNode.fSectionLength, headInfo.trackNodePtr, tempLimitVect, m_hTrainProcess, headInfo.nDirection);
	AddSpeedPostLimit(forwardLength - trackNode.fSectionLength, trackNode, limitVect, m_hTrainProcess, headInfo.nDirection);
	AddStationItem(forwardLength - trackNode.fSectionLength, trackNode, stationVect, m_hTrainProcess, headInfo.nDirection);
	//m_textContent.Format(L"0x%X %f\r\n", nextNodePtr, forwardLength);
	while (forwardLength < m_uForwardDistance * 1000 && nextNodePtr)
	{
		STrackNode* currentNodePtr = nextNodePtr;
		STrackNode trackNode;
		ReadProcessMemory(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(STrackNode), NULL);
		AddTempSpeedLimit(forwardLength, currentNodePtr, tempLimitVect, m_hTrainProcess, nDirectOfNextNode);
		AddSpeedPostLimit(forwardLength, trackNode, limitVect, m_hTrainProcess, nDirectOfNextNode);
		AddStationItem(forwardLength, trackNode, stationVect, m_hTrainProcess,nDirectOfNextNode);
		forwardLength += trackNode.fSectionLength;
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nDirectToFindInConnectNode = !nDirectOfNextNode;
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode,currentNodePtr, nDirectToFindInConnectNode,nDirectOfNextNode);
		//CString msg;
		//msg.Format(L"0x%X %f\r\n", nextNodePtr, forwardLength);
		//m_textContent += msg;
	}
	for(size_t i = 0; i < tempLimitVect.size(); ++i)
	{
		CString msg;
		msg.Format(L"%.1f %.1f\r\n", tempLimitVect[i].fDistanceStart, tempLimitVect[i].fDistanceEnd);
		m_textContent += msg;
	}
	m_textContent += L"****************************************************\r\n";
	if(m_bShowSpeedPost)
		for(size_t i = 0; i < limitVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f %d\r\n", limitVect[i].fDistance, limitVect[i].LimitNum);
			m_textContent += msg;
		}
	if(limitVect.size()!=0 && stationVect.size()!=0 && m_bShowSpeedPost && m_bShowStation)
		m_textContent += L"****************************************************\r\n";
	if(m_bShowStation)
		for(size_t i = 0; i < stationVect.size(); ++i)
		{
			CString msg;
			msg.Format(L"%.1f ", stationVect[i].fDistance);
			msg += stationVect[i].stationName;
			msg += L"\r\n";
			m_textContent += msg;
		}
	if(!nextNodePtr)
	{
		CString msg;
		msg.Format(L"%.1f END", forwardLength);
		m_textContent += msg;
	}
	UpdateData(FALSE);
	ReadProcessMemory(m_hTrainProcess, (void *)TAIL_TRACK_MEM, (LPVOID)&tailInfo, sizeof(STrackInfo), NULL);
}

void CSignSpeedLimitDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnTimer(nIDEvent);
	if(m_bAutoGetData)
		OnBnClickedOk();
}

void CSignSpeedLimitDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
}

void CSignSpeedLimitDlg::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
}

void CSignSpeedLimitDlg::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
}

void CSignSpeedLimitDlg::OnBnClickedCheck4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
}

CString baseFunc(HANDLE handle, void* ptr)
{
	CString str;
	str.Format(L"%x", ptr);
	return str;
}
CString TempSpeedFunc(HANDLE handle, void* ptr)
{
	CString str;
	char* cPtr = (char*)ptr;
	STempSpeed speed;
	ReadProcessMemory(handle, cPtr + 32, &speed, sizeof(STempSpeed), NULL);
	str.Format(L"%x %x %.1f %.1f", ptr, speed.nodePtr, speed.fStart, speed.fEnd);
	return str;
}
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
	//0x809B38
	CString strResult = IteratorList(m_hTrainProcess, (LPVOID)0x809B38, TempSpeedFunc);
	float* fTempLimitPtr;
	ReadProcessMemory(m_hTrainProcess, (LPCVOID)0x809B48, &fTempLimitPtr, 4, NULL);
	float fTempLimit;
	ReadProcessMemory(m_hTrainProcess, (LPCVOID)(fTempLimitPtr + 23), &fTempLimit, 4, NULL);
	int nType;
	ReadProcessMemory(m_hTrainProcess, (LPCVOID)0x78C390, &nType, 4, NULL);
	m_textContent = strResult;
	CString strSpeed;
	if(nType)
	{
		strSpeed.Format(L"Temp Speed Limit %.0f km\r\n", fTempLimit * 3.6);
	}else
	{
		strSpeed.Format(L"Temp Speed Limit %.0f mile\r\n", fTempLimit * 2.237);
	}
	m_textContent += strSpeed;
	UpdateData(false);
}
