// SignSpeedLimitDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SignSpeedLimit.h"
#include "SignSpeedLimitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CSignSpeedLimitDlg �Ի���




CSignSpeedLimitDlg::CSignSpeedLimitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSignSpeedLimitDlg::IDD, pParent)
	, m_textContent(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSignSpeedLimitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_textContent);
}

BEGIN_MESSAGE_MAP(CSignSpeedLimitDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSignSpeedLimitDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSignSpeedLimitDlg ��Ϣ�������

BOOL CSignSpeedLimitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_hTrainProcess = NULL;
	SetTimer(0, 1000, NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSignSpeedLimitDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	if(connectNode.nType == 2)
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
	}else if(connectNode.nType == 3)
	{
		if(connectStruct.nDirect1 == direction)
			return NULL;
	}
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
		m_textContent = L"�ȴ�MSTS����";
		UpdateData(FALSE);
		return;
	}

	if (!GetTrainPointer(m_hTrainProcess))
	{
		m_textContent = L"�ȴ�MSTS��������";
		UpdateData(FALSE);
		return;
	}
	STrackInfo headInfo, tailInfo;
	CString info;
	ReadProcessMemory(m_hTrainProcess, (void *)HEAD_TRACK_MEM, (LPVOID)&headInfo, sizeof(STrackInfo), NULL);
	info += showTrackInfo(headInfo);
	//info.Format(L"section length : %f", trackNode.fSectionLength);
	m_textContent.Format(L"0x%X ", headInfo.trackNodePtr);
	STrackNode* nextNode = headInfo.trackNodePtr;
	int nextDirect = !headInfo.nDirection;
	for(int i = 0; i < 5; ++i)
	{
		STrackNode trackNode;
		ReadProcessMemory(m_hTrainProcess, (void *)nextNode, (LPVOID)&trackNode, sizeof(STrackNode), NULL);
		int direct;
		nextNode = GetNextNode(m_hTrainProcess, trackNode,nextNode, nextDirect,direct);
		nextDirect = !direct;
		CString msg;
		msg.Format(L"0x%X ", nextNode);
		m_textContent += msg;
	}
	UpdateData(FALSE);
	info+=L"\n";
	ReadProcessMemory(m_hTrainProcess, (void *)TAIL_TRACK_MEM, (LPVOID)&tailInfo, sizeof(STrackInfo), NULL);
	info += showTrackInfo(tailInfo);

	//MessageBox(info);
}

void CSignSpeedLimitDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnTimer(nIDEvent);
	OnBnClickedOk();
}
