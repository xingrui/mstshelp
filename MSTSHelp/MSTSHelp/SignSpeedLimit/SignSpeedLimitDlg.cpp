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
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CSignSpeedLimitDlg �Ի���




CSignSpeedLimitDlg::CSignSpeedLimitDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CSignSpeedLimitDlg::IDD, pParent)
	, m_textContent(_T(""))
	, m_bShowSpeedPost(FALSE)
	, m_bShowStation(FALSE)
	, m_bShowSiding(FALSE)
	, m_bAutoGetData(FALSE)
	, m_bShowTaskLimit(FALSE)
	, m_uForwardDistance(0)
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
	ON_BN_CLICKED(IDC_CHECK1, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK2, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK3, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK4, &CSignSpeedLimitDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK5, &CSignSpeedLimitDlg::OnBnClickedCheck)
END_MESSAGE_MAP()


// CSignSpeedLimitDlg ��Ϣ��������

BOOL CSignSpeedLimitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// ��������...���˵������ӵ�ϵͳ�˵��С�
	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	m_hTrainProcess = NULL;
	SetTimer(0, 1000, NULL);
	m_bShowSpeedPost = TRUE;
	m_bShowStation = TRUE;
	m_bShowTaskLimit = TRUE;
	m_bShowSiding = TRUE;
	m_bAutoGetData = FALSE;
	m_uForwardDistance = 4;
	m_textContent = L"";
	UpdateData(FALSE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
		case VK_RETURN:   //����Enter
			return true;
		case VK_ESCAPE:   //����Esc
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

// �����Ի���������С����ť������Ҫ����Ĵ���
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
			strSpeed.Format(L"������ʱ���� %.0f ����/ʱ\r\n", fTempLimit * 3.6);
		}
		else
		{
			strSpeed.Format(L"������ʱ���� %.0f Ӣ��/ʱ\r\n", fTempLimit * 2.237);
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
			temp = L"ǰ����־����(����/ʱ)\r\n";
		}
		else
		{
			temp = L"ǰ����־����(Ӣ��/ʱ)\r\n";
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
		m_textContent += L"ǰ����վ����\r\n";

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
		m_textContent += L"ǰ����������\r\n";

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
		msg.Format(L"%.1f ���쾡ͷ", forwardLength);
		m_textContent += msg;
	}
}

void CSignSpeedLimitDlg::OnBnClickedOk()
{
	UpdateData();

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

	try
	{
		OnGetData();
	}
	catch (int)
	{
		m_textContent = L"��ȡ����ʧ��";
	}

	UpdateData(FALSE);
}

void CSignSpeedLimitDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	CDialog::OnTimer(nIDEvent);

	if (m_bAutoGetData)
		OnBnClickedOk();
}

void CSignSpeedLimitDlg::OnBnClickedCheck()
{
	UpdateData();
}