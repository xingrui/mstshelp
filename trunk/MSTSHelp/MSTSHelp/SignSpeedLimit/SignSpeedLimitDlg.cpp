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
	, m_ItemNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSignSpeedLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_textContent);
	DDX_Text(pDX, IDC_EDIT2, m_ItemNum);
	DDV_MinMaxInt(pDX, m_ItemNum, 0, 10000);
}

BEGIN_MESSAGE_MAP(CSignSpeedLimitDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSignSpeedLimitDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CSignSpeedLimitDlg::OnBnClickedButton1)
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
	ReadProcessMemory(handle, (void *)node.nodePtr1, (LPVOID)&connectNode, sizeof(SConnectNode), NULL);
	ReadProcessMemory(handle, (void *)connectNode.nodePointer, (LPVOID)&connectStruct, sizeof(SConnectStruct), NULL);
	next = GetNext(nodePtr, connectStruct, connectNode, direction, nextDirect);
	if(next)
		return next;
	ReadProcessMemory(handle, (void *)node.nodePtr2, (LPVOID)&connectNode, sizeof(SConnectNode), NULL);
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
	STrackInfo headInfo, tailInfo;
	CString info;
	ReadProcessMemory(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo), NULL);
	m_textContent.Format(L"0x%X ", headInfo.trackNodePtr);
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
		//AddSpeedPostLimit(forwardLength - trackNode.fSectionLength, trackNode, limitVect, m_hTrainProcess);
	}
	AddSpeedPostLimit(forwardLength - trackNode.fSectionLength, trackNode, limitVect, m_hTrainProcess, headInfo.nDirection);
	AddStationItem(forwardLength - trackNode.fSectionLength, trackNode, stationVect, m_hTrainProcess, headInfo.nDirection);
	m_textContent.Format(L"0x%X %f\r\n", nextNodePtr, forwardLength);
	while (forwardLength < 4000 && nextNodePtr)
	{
		STrackNode* currentNodePtr = nextNodePtr;
		STrackNode trackNode;
		ReadProcessMemory(m_hTrainProcess, (void *)currentNodePtr, (LPVOID)&trackNode, sizeof(STrackNode), NULL);
		AddSpeedPostLimit(forwardLength, trackNode, limitVect, m_hTrainProcess, nDirectOfNextNode);
		AddStationItem(forwardLength, trackNode, stationVect, m_hTrainProcess,nDirectOfNextNode);
		forwardLength += trackNode.fSectionLength;
		//if(!nDirectOfNextNode)
		//{
			//AddSpeedPostLimit(forwardLength, trackNode, limitVect, m_hTrainProcess);
		//}
		/************************************************************************/
		/* Get Next Node Pointer                                                */
		/************************************************************************/
		nDirectToFindInConnectNode = !nDirectOfNextNode;
		nextNodePtr = GetNextNode(m_hTrainProcess, trackNode,currentNodePtr, nDirectToFindInConnectNode,nDirectOfNextNode);
		CString msg;
		msg.Format(L"0x%X %f\r\n", nextNodePtr, forwardLength);
		m_textContent += msg;
	}
	for(size_t i = 0; i < limitVect.size(); ++i)
	{
		CString msg;
		msg.Format(L"%f %d\r\n", limitVect[i].fDistance, limitVect[i].LimitNum);
		m_textContent += msg;
	}
	for(size_t i = 0; i < stationVect.size(); ++i)
	{
		CString msg;
		msg.Format(L"%f ", stationVect[i].fDistance);
		msg += stationVect[i].stationName;
		msg += L"\r\n";
		m_textContent += msg;
	}
	UpdateData(FALSE);
	ReadProcessMemory(m_hTrainProcess, (void *)TAIL_TRACK_MEM, (LPVOID)&tailInfo, sizeof(STrackInfo), NULL);
}

void CSignSpeedLimitDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnTimer(nIDEvent);
	OnBnClickedOk();
}

void CSignSpeedLimitDlg::OnBnClickedButton1()
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
	SSpeedPostItem item;
	UpdateData();
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80a038,&item, sizeof(SSpeedPostItem), 5, 0xC, 0x20, 0, 4 * m_ItemNum, 0x0);
	m_textContent.Format(L"0x%X ", 4*m_ItemNum);
	m_textContent += SpeedPostItemToString(item);
	UpdateData(FALSE);
}
