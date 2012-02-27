// MSTSHelpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSTSHelp.h"
#include "MSTSHelpDlg.h"
#include "Logger.h"
#include <assert.h>
#include <math.h>
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMSTSHelpDlg �Ի���

CMSTSHelpDlg::CMSTSHelpDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CMSTSHelpDlg::IDD, pParent)
	, m_fCurrentSpeed(0)
	, m_fCurrentSpeedLimit(0)
	, m_fForwardSignalDistance(0)
	, m_fAcceleration(0)
	, m_bAutoDrive(FALSE)
	, m_bAutoGetData(TRUE)
	, m_bTopMost(FALSE)
	, m_bAutoAdjustLocation(FALSE)
	, m_bAutoEscapeMessage(FALSE)
	, m_bAutoSkipError(FALSE)
	, m_strManualLimit(_T(""))
	, m_bAutoSave(FALSE)
	, m_isConnectMode(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSTSHelpDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Check(pDX, IDC_CHECK1, m_bAutoDrive);
	DDX_Check(pDX, IDC_CHECK2, m_bAutoGetData);
	DDX_Check(pDX, IDC_CHECK3, m_bTopMost);
	DDX_Check(pDX, IDC_CHECK4, m_bAutoAdjustLocation);
	DDX_Check(pDX, IDC_CHECK5, m_bAutoEscapeMessage);
	DDX_Check(pDX, IDC_CHECK6, m_bAutoSkipError);
	DDX_Check(pDX, IDC_CHECK7, m_bAutoSave);
	DDX_Text(pDX, IDC_EDIT3, m_strManualLimit);
	DDX_Check(pDX, IDC_CHECK8, m_isConnectMode);
}

BEGIN_MESSAGE_MAP(CMSTSHelpDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMSTSHelpDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMSTSHelpDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMSTSHelpDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_CHECK1, &CMSTSHelpDlg::OnAutoDriveChanged)
	ON_BN_CLICKED(IDC_CHECK2, &CMSTSHelpDlg::OnAutoGetDataChanged)
	ON_BN_CLICKED(IDC_CHECK3, &CMSTSHelpDlg::OnTopMostChanged)
	ON_BN_CLICKED(IDC_CHECK4, &CMSTSHelpDlg::OnAutoAdjustChanged)
	ON_BN_CLICKED(IDC_CHECK5, &CMSTSHelpDlg::OnAutoEscMsg)
	ON_BN_CLICKED(IDC_CHECK6, &CMSTSHelpDlg::OnAutoSkipError)
	ON_BN_CLICKED(IDC_CHECK7, &CMSTSHelpDlg::OnAutoSave)
	ON_BN_CLICKED(IDC_CHECK8, &CMSTSHelpDlg::OnIsConnectMode)
END_MESSAGE_MAP()


// CMSTSHelpDlg ��Ϣ�������

BOOL CMSTSHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	//��ʼ����ʱ��
	m_nTimerGetData = SetTimer(AUTO_GET_DATA_TIMER, DATA_SPAN, NULL);
	//��ʼ���ֶ�����
	m_strManualLimit = "1000";
	//��ʼ����Ϸʱ��
	m_fGameTime = -1;
	m_lastSaveTime = -1;
	//��ʼ����ʾ�б�
	initControlList();
	UpdateData(false);
	m_pCurrentSchedule = NULL;
	m_hTrainProcess = NULL;
	m_bInStopThread = false;
	m_bIsProcessing = false;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMSTSHelpDlg::OnDestroy()
{
	if (m_hTrainProcess)
		CloseHandle(m_hTrainProcess);
}

BOOL CMSTSHelpDlg::PreTranslateMessage(MSG *pMsg)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMSTSHelpDlg::OnPaint()
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
HCURSOR CMSTSHelpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMSTSHelpDlg::OnBnClickedButton1()
{
	UpdateData();

	if (!GetTrainHandle(m_hTrainProcess))
	{
		//��ȡ����MSTS����,m_hTrainProcess�ᱻ�Զ��ÿգ���Ϊ���ݵ���һ������
		//����б�����
		m_pCurrentSchedule = NULL;
		m_lastSaveTime = -1;
		clearControlList();
		m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"�ȴ�MSTS����");
		return;
	}

	if (m_bAutoAdjustLocation)
		AdjustLocation();

	if (!GetTrainPointer(m_hTrainProcess))
	{
		//��ȡ����MSTS����Ŀ���
		//����б�����
		m_pCurrentSchedule = NULL;
		clearControlList();
		m_lastSaveTime = -1;
		m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"�ȴ�MSTS��������");
		return;
	}

	//��ȡ�г����е�����
	GetTrainData(m_hTrainProcess);

	if (m_lastSaveTime == -1)
	{
		m_lastSaveTime = m_fGameTime;
	}
	else
	{
		float fGameTime = m_fGameTime;

		if (fGameTime < m_lastSaveTime)
			fGameTime += 3600 * 24;

		if (fGameTime - m_lastSaveTime > 1800 && m_bAutoSave)
		{
			PressKeyToTrainWnd(VK_F2);
			m_lastSaveTime = m_fGameTime;
			CLogger::Log("Save Game Time:%d:%d:%d", m_sGameTime.m_nHour, m_sGameTime.m_nMinute, m_sGameTime.m_nSecond);
			Sleep(200);
			PressKeyToTrainWnd(VK_ESCAPE);
		}
	}

	if (m_bAutoDrive && !m_bInStopThread)
	{
		if (!m_bIsProcessing)
		{
			m_bIsProcessing = true;
			m_listLimit.clear();

			if (m_fCurrentSpeedLimit == 0 || m_cColor1 == 8 && m_cColor2 == 8)
			{
				//����Ϊ0�����߰׵�����
				if (m_fCurrentSpeed > 1E-3 || m_fCurrentSpeed < -1E-3)
				{
					m_bInStopThread = true;
					_beginthread(StopAndPressEnter, NULL, this);
				}
			}
			else if (IsTaskMode(m_hTrainProcess))
				AutoDriveTask(m_hTrainProcess);
			else
				AutoDrive(m_hTrainProcess);

			m_bIsProcessing = false;
		}
	}
}

void CMSTSHelpDlg::OnBnClickedButton2()
{
}

void CMSTSHelpDlg::OnBnClickedButton3()
{
}

void CMSTSHelpDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	UpdateData();

	switch (nIDEvent)
	{
	case AUTO_GET_DATA_TIMER:
		OnBnClickedButton1();
		break;
	case AUTO_ESC_MSG_TIMER:
	{
		int nFlag;
		CHECK(ReadProcessMemory(m_hTrainProcess, (void *)0x783A98, (LPVOID)&nFlag, 4, NULL))

		if (!nFlag)
		{
			PressKeyToTrainWnd(VK_ESCAPE);
		}

		break;
	}
	case AUTO_SKIP_ERROR_TIMER:
	{
		CWnd *wnd = GetForegroundWindow();

		if (wnd)
		{
			CString message;
			wnd->GetWindowText(message);

			if (message == L"����:�Ƿ����?")
			{
				HWND hParentWnd = ::GetParent(wnd->GetSafeHwnd());
				PressKey('Y');
			}
			else if (message == L"΢��ģ���")
			{
				PressKey(VK_ESCAPE);
			}
			else
			{
			}
		}

		break;
	}
	}
}
void CMSTSHelpDlg::clearControlList()
{
	int count = m_listCtrl.GetHeaderCtrl()->GetItemCount();

	for (int i = 0; i < count; ++i)
	{
		m_listCtrl.SetItemText(i, 1, L"");
	}
}
void CMSTSHelpDlg::initControlList()
{
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listCtrl.InsertColumn(0, _T("����"), LVCFMT_LEFT, 80);
	m_listCtrl.InsertColumn(1, _T("����"), LVCFMT_LEFT, 120);
	m_listCtrl.InsertItem(CURRENT_SPEED_ITEM, L"��ǰ�ٶ�");
	m_listCtrl.InsertItem(CURRENT_LIMIT_ITEM, L"��ǰ����");
	m_listCtrl.InsertItem(SIGNAL_DISTANCE_ITEM, L"ǰ���źž���");
	m_listCtrl.InsertItem(SIGNAL_LIMIT_ITEM, L"ǰ���ź�����");
	m_listCtrl.InsertItem(FORWARD_COLOR_ITEM, L"�ź���ɫ");
	m_listCtrl.InsertItem(CURRENT_ACCER_ITEM, L"���ٶ�ֵ");
	//m_listCtrl.InsertItem(FORWARD_TURNOFF_ITEM, L"ǰ������");
	m_listCtrl.InsertItem(CURRENT_TIME_ITEM, L"��ǰʱ��");
	m_listCtrl.InsertItem(INFORMATION_ITEM, L"��Ϣ");
	m_listCtrl.InsertItem(LOCO_TYPE_ITEM, L"��ͷ����");
	m_listCtrl.InsertItem(BREAK_NUM_ITEM, L"�ƶ�����");
	m_listCtrl.InsertItem(BREAK_LEVEL_ITEM, L"�ƶ�����");
	m_listCtrl.InsertItem(SCHEDULE_INFO_ITEM, L"ʱ�̱���Ϣ");
	m_listCtrl.InsertItem(CALCULATED_SPEED_LIMIT, L"Ŀǰ�г�����");
	m_listCtrl.InsertItem(FOWARD_STATION_NAME, L"ǰ����վ����");
	m_listCtrl.InsertItem(POWER_AND_ELECTRIC_BREAK, L"���ʺ��ƶ�ֵ");
	m_listCtrl.InsertItem(ROUTE_SPEED_LIMIT, L"��·����");
}

void CMSTSHelpDlg::AdjustLocation()
{
	HWND hWnd = ::FindWindow(L"TrainSim", NULL);
	WINDOWPLACEMENT placement;
	::GetWindowPlacement(hWnd, &placement);

	if (placement.showCmd == SW_SHOWMINIMIZED)
		return;

	CRect rectMsts;

	if (::GetWindowRect(hWnd, &rectMsts))
	{
		CRect rectMstsHelp;
		GetWindowRect(&rectMstsHelp);
		MoveWindow(rectMsts.right, rectMsts.top, rectMstsHelp.Width(), rectMstsHelp.Height());
	}
}

void CMSTSHelpDlg::StopAndPressEnter(void *pThis)
{
	CMSTSHelpDlg *pDlg = (CMSTSHelpDlg *)pThis;
	pDlg->m_bInStopThread = true;

	for ( int i = 0; i < 25; ++i)
	{
		pDlg->ApplyBreak();
		Sleep(100);
	}

	for ( int i = 0; i < 14; ++i)
	{
		SSchedule schedule;
		Sleep(500);

		if (GetCurrentSchedule(pDlg->m_hTrainProcess, NULL, schedule))
		{
			if (schedule.m_fActualArrivalTime == 0)
				PressKeyToTrainWnd(VK_RETURN);
			else
				break;
		}
	}

	pDlg->m_bInStopThread = false;
	return;
}

//ֱ�Ӳ�����-0.1m/s*s�ļ��ٶ��������ǰӦ�����ٶ��٣�Ȼ�����������п���
//������ǰ�Ĺ��ʺ��ƶ�ֵ
void CMSTSHelpDlg::AdjustPowerAndBreak()
{
	CString str;
	float fCalculatedSpeedLimit = m_fCurrentSpeedLimit;
	list<SForwardLimit>::iterator ite = m_listLimit.begin();
	bool bInBreaking = false;

	while (ite != m_listLimit.end())
	{
		if (ite->m_fDistance < 1.0F && ite->m_fSpeedLimit == 0)
		{
			if (m_fCurrentSpeed > 1e-3f)
			{
				m_bInStopThread = true;
				_beginthread(StopAndPressEnter, NULL, this);
			}

			return;
		}

		if (ite->m_fSpeedLimit > fCalculatedSpeedLimit)
			break;

		float fSpeedLimit = sqrt(ite->m_fSpeedLimit * ite->m_fSpeedLimit + 2 * m_fExpectedAccerlate * ite->m_fDistance);

		if (fSpeedLimit < 1.0)
			fSpeedLimit = 1.0;

		if (fCalculatedSpeedLimit > fSpeedLimit)
		{
			//����������ٱȵ�ǰ����ҪС�����Ӧ�ô����ƶ�״̬
			fCalculatedSpeedLimit = fSpeedLimit;
			bInBreaking = true;
		}

		++ite;
	}

	LPCTSTR pStr = m_strManualLimit.GetBuffer(100);
	float fManualSpeedLimit;
	swscanf_s(pStr, L"%f", &fManualSpeedLimit);
	m_strManualLimit.ReleaseBuffer(100);
	m_strManualLimit.Format(L"%.1f", fManualSpeedLimit);
	UpdateData(FALSE);

	if (fCalculatedSpeedLimit * 3.6 > fManualSpeedLimit)
		fCalculatedSpeedLimit = fManualSpeedLimit / 3.6F;

	str.Format(L"%.1f", fCalculatedSpeedLimit * 3.6F);
	m_listCtrl.SetItemText(CALCULATED_SPEED_LIMIT, 1, str);

	//���ݵ�ǰ�ٶȺ�������������ٶȵ������ʺ��ƶ���ֵ
	if (m_fCurrentSpeed > fCalculatedSpeedLimit)
	{
		//��������0
		if(m_fCurrentPower > 1E-3)
			PressKeyToTrainWnd('A');

		//���ٶ�Ϊ���ٽ�һ��
		if (m_fAcceleration > -1E-3)
			PressKeyToTrainWnd('A');
		else if (m_fAcceleration > -0.3F)
		{
			// ���٣���Ҫ����ɲ���ͼ�С����
			if ((m_fCurrentSpeed + m_fAcceleration * 3) > fCalculatedSpeedLimit)
			{
				if(m_loco == Electric)
					PressKeyToTrainWnd('A');
				else
					ApplyBreak();
			}
		}

		//�ж��Ƿ�Ҫ��ɲ�����д���

		if (m_fCurrentSpeed > 1.3 * fCalculatedSpeedLimit + 3)
		{
			//���س��٣�ɲ�����㹻�ˣ�ҲҪ��������ɲ���ٶ�
			ApplyBreak();
		}else if((m_fCurrentSpeed > fCalculatedSpeedLimit + 1.38) 
			|| m_fCurrentSpeed > 1.1 * fCalculatedSpeedLimit)
		{
			//���ٽ϶࣬ɲ�����ٶȲ���
			if(m_fAcceleration > -0.4)
				ApplyBreak();
		}
		else if (m_fCurrentSpeed < 1.2 * fCalculatedSpeedLimit && m_fAcceleration < -0.45F)
		{
			ReleaseBreak();
		}

		else if (m_loco == Electric && m_fAcceleration < -0.45F && m_fBreakNum == 1)
		{
			ReleaseBreak();
		}

		return;
	}

	if (m_fAcceleration < (fCalculatedSpeedLimit - m_fCurrentSpeed) / 50)
	{
		if(m_fCurrentSpeed < 0.1)
		{
			for (int i = 0; i < 20; ++i)
				PressKeyToTrainWnd(VK_OEM_4);
		}

		// �г����ٶȹ�С����Ҫ����
		for (int i = 0; i < 20; ++i)
			ReleaseBreak();

		if (!bInBreaking || m_fCurrentSpeed < 1.0F)
			ReleaseElectricBreak();

		if (m_fCurrentPower < 1)
		{
			/*DWORD cbCodeSize = ((LPBYTE)AfterMyFunc - (LPBYTE)MyFunc);
			PDWORD pCodeRemote = (PDWORD)VirtualAllocEx(m_hTrainProcess, 0, cbCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			::WriteProcessMemory(m_hTrainProcess, pCodeRemote, &MyFunc, cbCodeSize, NULL);
			CString msg;
			msg.Format(L"%x %d", pCodeRemote, pCodeRemote);
			MessageBox(msg);
			HANDLE hThread = CreateRemoteThread(m_hTrainProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pCodeRemote, NULL, 0, NULL);
			DWORD h;
			if(hThread)
			{
				::WaitForSingleObject(hThread, INFINITE);
				::GetExitCodeThread(hThread, &h);
				TRACE("run and return %d", h);
				::CloseHandle(hThread);
			}
			::VirtualFreeEx(m_hTrainProcess, pCodeRemote, cbCodeSize, MEM_RELEASE);*/
			PressKeyToTrainWnd('D');
		}
	}
	else if (m_fAcceleration > (fCalculatedSpeedLimit - m_fCurrentSpeed) / 25)
	{
		// ���ٹ��죬��Ҫ�ʵ����ټ��ٶ�
		PressKeyToTrainWnd('A');
	}
}

CString CMSTSHelpDlg::changeBitsLevelToString(int nBitsLevel)
{
	CString strLevel;

	if (nBitsLevel & (nBitsLevel - 1) || !nBitsLevel)
	{
		strLevel = L"������ƶ���������";
		m_nBreakLevel = -1;
	}
	else
	{
		m_nBreakLevel = 0;

		while (nBitsLevel)
		{
			nBitsLevel >>= 1;
			++m_nBreakLevel;
		}

		strLevel.Format(L"%d", m_nBreakLevel);
	}

	return strLevel;
}
CString  CMSTSHelpDlg::changeColorToString(char cLightColor)
{
	CString strColor;

	switch (cLightColor)
	{
	case 0:
		strColor = "���";
		break;
	case 2:
		strColor = "��Ƶ�";
		break;
	case 3:
		strColor = "˫�Ƶ�";
		break;
	case 4:
		strColor = "��2��";
		break;
	case 5:
		strColor = "�Ƶ�";
		break;
	case 6:
		strColor = "�̻Ƶ�";
		break;
	case 7:
		strColor = "�̵�";
		break;
	case 8:
		strColor = "�׵�";
		break;
	default:
		strColor = "����ʶ��ĳ���";
		break;
	}

	return strColor;
}
CString  CMSTSHelpDlg::changeLocoTypeToString(Locomotive loco)
{
	CString strLocoType;

	switch (loco)
	{
	case Steam:
		strLocoType = "������";
		break;
	case Diesel:
		strLocoType = "��ȼ��";
		break;
	case Electric:
		strLocoType = "������";
		break;
	default:
		strLocoType = "�޷�ʶ��ĳ�ͷ";
		break;
	}

	return strLocoType;
}

bool CMSTSHelpDlg::lessThan(float a, float b)
{
	float result = a - b;

	if (result < -12 * 3600)
		return false;
	else if (result > 12 * 3600)
		return true;
	else return result < 0;
}

void CMSTSHelpDlg::GetTrainData(HANDLE hProcess)
{
	//float fForwardTurnOff;
	int nLevelBit;
	CString strColor;
	CHECK(ReadProcessMemory(hProcess, (void *)CUR_SPEED_MEM, (LPVOID)&m_fCurrentSpeed, 4, NULL))
	CHECK(ReadProcessMemory(hProcess, (void *)SPEED_LIMIT_MEM, (LPVOID)&m_fCurrentSpeedLimit, 4, NULL))
	CHECK(ReadProcessMemory(hProcess, (void *)FOWARD_LIMIT_MEM, (LPVOID)&m_fForwardSignalLimit, 4, NULL))
	CHECK(ReadProcessMemory(hProcess, (void *)SIG_DISTANT_MEM, (LPVOID)&m_fForwardSignalDistance, 4, NULL))
	CHECK(ReadProcessMemory(hProcess, (void *)S_GAME_TIME_MEM, (LPVOID)&m_sGameTime, 12, NULL))
	CHECK(ReadProcessMemory(hProcess, (void *)F_GAME_TIME_MEM, (LPVOID)&m_fGameTime, 4, NULL));
	CString strTime;
	strTime.Format(L"%02d:%02d:%02d", m_sGameTime.m_nHour, m_sGameTime.m_nMinute, m_sGameTime.m_nSecond);
	CHECK(ReadProcessMemory(hProcess, (void *)LIGHT_COLOR_MEM, (LPVOID)&m_cColor1, 1, NULL))
	strColor = changeColorToString(m_cColor1);
	strColor += " : ";
	CHECK(ReadProcessMemory(hProcess, (void *)LIGHT_COLOR_MEM_2, (LPVOID)&m_cColor2, 1, NULL))
	strColor += changeColorToString(m_cColor2);
	CHECK(ReadProcessMemory(hProcess, (void *)ACCER_MEM, (LPVOID)&m_fAcceleration, 4, NULL))
	//CHECK(ReadProcessMemory(hProcess, (void *)FORWARD_TURNOFF_MEM, (LPVOID)&fForwardTurnOff, 4, NULL))
	CHECK(ReadPointerMemory(hProcess, (LPCVOID)0x8099BC, (LPVOID)&m_fBreakNum, 4, 5, 0, 0, 0x8, 0x10, 0x24C))
	CHECK(ReadPointerMemory(hProcess, (LPCVOID)0x8099BC, (LPVOID)&nLevelBit, 4, 5, 0, 0, 0x8, 0x10, 0x248))
	CString str;
	str.Format(L"%.1f", m_fCurrentSpeed * 3.6F);
	m_listCtrl.SetItemText(CURRENT_SPEED_ITEM, 1, str);
	str.Format(L"%.1f", m_fCurrentSpeedLimit * 3.6F);
	m_listCtrl.SetItemText(CURRENT_LIMIT_ITEM, 1, str);
	str.Format(L"%.1f", m_fForwardSignalDistance);
	m_listCtrl.SetItemText(SIGNAL_DISTANCE_ITEM, 1, str);
	str.Format(L"%.1f", m_fForwardSignalLimit * 3.6F);
	m_listCtrl.SetItemText(SIGNAL_LIMIT_ITEM, 1, str);
	m_listCtrl.SetItemText(FORWARD_COLOR_ITEM, 1, strColor);
	str.Format(L"%.3f", m_fAcceleration);
	m_listCtrl.SetItemText(CURRENT_ACCER_ITEM, 1, str);
	//str.Format(L"%f", fForwardTurnOff);
	//m_listCtrl.SetItemText(FORWARD_TURNOFF_ITEM, 1, str);
	m_listCtrl.SetItemText(CURRENT_TIME_ITEM, 1, strTime);
	m_loco = GetLocomotive(hProcess);

	switch (m_loco)
	{
	case Steam:
		m_fExpectedAccerlate = 0.1F;
		break;
	case Diesel:
		m_fExpectedAccerlate = 0.15F;
		break;
	case Electric:
		m_fExpectedAccerlate = 0.2F;
		break;
	}

	CString strLocoType = changeLocoTypeToString(m_loco);
	m_listCtrl.SetItemText(LOCO_TYPE_ITEM, 1, strLocoType);
	m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"�ɹ��������");
	str.Format(L"%.2f", m_fBreakNum);
	m_listCtrl.SetItemText(BREAK_NUM_ITEM, 1, str);
	str = changeBitsLevelToString(nLevelBit);
	m_listCtrl.SetItemText(BREAK_LEVEL_ITEM, 1, str);
	float fNum2;
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x7B6440, &m_fCurrentPower, 4, 1, 0x8C))
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x7B6440, &fNum2, 4, 1, 0x32C))
	str.Format(L"%.3f %.3f", m_fCurrentPower, fNum2);
	m_listCtrl.SetItemText(POWER_AND_ELECTRIC_BREAK, 1, str);
	float fRouteLimit;
	CHECK(ReadProcessMemory(hProcess, (LPCVOID)ROUTE_SPEED_LIMIT_MEM, (LPVOID)&fRouteLimit, 4, NULL))
	str.Format(L"%.1f", fRouteLimit * 3.6);
	m_listCtrl.SetItemText(ROUTE_SPEED_LIMIT, 1, str);
	SSchedule schedule;

	if (GetCurrentSchedule(hProcess, NULL, schedule))
	{
		//������ȡ���ݣ����ı��Ա��������Ϣ��
		float fCurrentTime, fDistance;
		CHECK(ReadProcessMemory(hProcess, (void *)0x809B08, (LPVOID)&fCurrentTime, 4, NULL))
		CHECK(ReadProcessMemory(hProcess, (void *)0x809B70, (LPVOID)&fDistance, 4, NULL))
		ShowScheduleInfo(schedule, fCurrentTime, fDistance);
	}
}


void CMSTSHelpDlg::ShowScheduleInfo(const SSchedule &schedule, float fCurrentTime, float fDistance)
{
	CString str;
	wchar_t stationName[30];
	ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x80a038, stationName, 30, 6, 0xC, 0x20, 0, 4 * schedule.m_nPlatformStartID, 0x28, 0);
	m_listCtrl.SetItemText(FOWARD_STATION_NAME, 1, stationName);

	if (schedule.m_fActualArrivalTime != 0)
		str.Format(L"%.1f ������", schedule.m_fDepartTime - fCurrentTime);
	else
		str.Format(L"%.0f�� %.1f��", fDistance, schedule.m_fArrivalTime - fCurrentTime);

	m_listCtrl.SetItemText(SCHEDULE_INFO_ITEM, 1, str);
}

void CMSTSHelpDlg::changeDirection(HANDLE hProcess, Direction direction)
{
	CLogger::Log(__FUNCTION__);
	Locomotive loco = GetLocomotive(hProcess);
	Direction currentDirection = GetDirection(loco, hProcess);

	if (currentDirection == direction)
		return;

	float fNum1;

	for (int i = 0; i < 20; ++i)
	{
		CHECK(ReadPointerMemory(hProcess, (LPCVOID)0x7B6440, &fNum1, 4, 1, 0x8C))

		if (fNum1 == 0)
			break;

		PressKeyToTrainWnd('A');
		Sleep(100);
	}

	int pressNum = loco == Steam ? 50 : 1;

	switch (direction)
	{
	case Forward:

		if (currentDirection == Backward)
			for (int i = 0; i < pressNum * 2; ++i)
				PressKeyToTrainWnd('W');
		else
		{
			for (int i = 0; i < pressNum; ++i)
				PressKeyToTrainWnd('W');
		}

		break;
	case Middle:

		if (currentDirection == Forward)
		{
			while (currentDirection == Forward)
			{
				PressKeyToTrainWnd('S');
				Sleep(100);
				Direction currentDirection = GetDirection(loco, hProcess);
			}
		}
		else
		{
			while (currentDirection == Backward)
			{
				PressKeyToTrainWnd('W');
				Sleep(100);
				Direction currentDirection = GetDirection(loco, hProcess);
			}
		}

		break;
	case Backward:

		if (currentDirection == Forward)
			for (int i = 0; i < pressNum * 2; ++i)
				PressKeyToTrainWnd('S');
		else
			for (int i = 0; i < pressNum; ++i)
				PressKeyToTrainWnd('S');

		break;
	}
}

bool CMSTSHelpDlg::MakePowered(HANDLE hProcess, bool bIsPowered)
{
	Locomotive loco = GetLocomotive(hProcess);

	switch (loco)
	{
	case Electric:

		if (bIsPowered ^ IsPowered(hProcess))
		{
			float fNum = bIsPowered;
			//WriteProcessMemory(hProcess, (void *)IS_POWERED_MEM, (LPVOID)&fNum, 4, NULL);
			PressKeyToTrainWnd('P');
			return true;
		}

		break;
	default:
		break;
	}

	return false;
}

void CMSTSHelpDlg::UpdateScheduleInfo(float &fNextStationDistance)
{
	SSchedule schedule;
	SSchedule *pSchedule;
	GetCurrentSchedule(m_hTrainProcess, &pSchedule, schedule);

	if (NULL == m_pCurrentSchedule)
	{
		//��ǰ��ʱ�̱��ǺϷ��ģ�����ʱ�̱�
		m_pCurrentSchedule = pSchedule;
		m_currentSchedule = schedule;
		CLogger::Log("Schedule Changed because NULL %s", m_currentSchedule.toSTLString().c_str());
	}
	else if (m_pCurrentSchedule == pSchedule)
	{
		//ʱ�̱�δ�����仯��������������
		m_currentSchedule = schedule;
	}
	else
	{
		//ʱ�̱����˱仯����Ҫ�����Ƿ�ʹ���µ�ʱ�̱�
		if (lessThan(m_currentSchedule.m_fDepartTime, m_fGameTime))
		{
			//��ǰʱ�̱����ʱ���ѹ�
			if (m_currentSchedule.m_fActualArrivalTime != 0)
			{
				//��ǰʱ�̱����ʱ���ѹ��������Ѿ�ͣ����, ����ʱ�̱�
				CLogger::Log("Schedule Changed because Update old %s new %s", schedule.toSTLString().c_str(), m_currentSchedule.toSTLString().c_str());
				m_currentSchedule = schedule;
				m_pCurrentSchedule = pSchedule;
			}
			else
			{
				//��ǰʱ�̱����ʱ���ѹ���������δͣ����,������ͣ������������ʱ�̱�
			}
		}
		else
		{
			//��ǰʱ�̱����ʱ��δ����ʹ�þ�ʱ�̱������¾�ʱ�̱��е�����
			CHECK(ReadProcessMemory(m_hTrainProcess, (void *)m_pCurrentSchedule, (LPVOID)&m_currentSchedule, sizeof(schedule), NULL))
			fNextStationDistance = 0;
		}
	}
}
void CMSTSHelpDlg::AutoDriveTask(HANDLE hProcess)
{
	HWND hWnd = ::FindWindow(L"TrainSim", NULL);
	HWND hForeGroundWindow = ::GetForegroundWindow();

	// �����ǰ���ڲ���MSTS����ô�Ͳ������κβ���
	if (hWnd != hForeGroundWindow)
		return;

	if(m_isConnectMode)
	{
		int mode;
		CHECK(ReadProcessMemory(hProcess, (void*)0x799E3C, (LPVOID)&mode, 4, NULL))
		PressKeyToTrainWnd('6');
		if(mode == 8)
		{
			float distance;
			CHECK(ReadProcessMemory(hProcess, (void*)0x79C798, (LPVOID)&distance, 4, NULL))
			if(distance < 0)
			{
				SForwardLimit limit;
				limit.m_fDistance = 20;
				limit.m_fSpeedLimit = 1.0;
				m_listLimit.push_back(limit);
			}else
			{
				SForwardLimit limit;
				if(distance < 10)
					limit.m_fDistance = 0;
				else
					limit.m_fDistance = distance  / 3;
				limit.m_fSpeedLimit = 1.0;
				m_listLimit.push_back(limit);
			}////////////////////////////////////////////////////////////////////////////////
			Direction currentDirection = GetDirection(m_loco, hProcess);

			if (currentDirection == Middle || currentDirection == Backward)
			{
				changeDirection(hProcess, Forward);
				// �ı�ǰ�����򣬴�ʱ���г��������Ѿ������˱仯����˼���ִ��û��ʲô���塣
				return;
			}

			if (MakePowered(hProcess, true))
			{
				// ��Դ״�������˱仯����Ҫ��������
				return;
			}
			AdjustPowerAndBreak();
			return;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//���������ģʽ����Ҫ��֪ǰ����վ����Ϣ
	float fNextStationDistance;
	CHECK(ReadProcessMemory(hProcess, (void *)0x809B70, (LPVOID)&fNextStationDistance, 4, NULL))
	UpdateScheduleInfo(fNextStationDistance);
	ShowScheduleInfo(m_currentSchedule, m_fGameTime, fNextStationDistance);
	//���������ʱ�̱�ֱ�ӽ���һ��ɸѡ

	if (m_currentSchedule.m_fActualArrivalTime == 0)
	{
		//����û�е���ǰ��վ̨�����
		SForwardLimit limit;
		limit.m_fDistance = fNextStationDistance;
		limit.m_fSpeedLimit = 0;
		m_listLimit.push_back(limit);
	}
	else if (m_currentSchedule.m_fActualArrivalTime != 0 && lessThan(m_fGameTime, m_currentSchedule.m_fDepartTime))
	{
		//�����Ѿ�����վ̨����û�е�����ʱ������
		if (m_fCurrentSpeed > 1E-3)
			ApplyBreak();

		return;
	}
	else
	{
		//�Ѿ�����վ̨����Ҳ�Ѿ����������ʱ��
	}

	////////////////////////////////////////////////////////////////////////////////
	Direction currentDirection = GetDirection(m_loco, hProcess);

	if (currentDirection == Middle || currentDirection == Backward)
	{
		changeDirection(hProcess, Forward);
		// �ı�ǰ�����򣬴�ʱ���г��������Ѿ������˱仯����˼���ִ��û��ʲô���塣
		return;
	}

	if (MakePowered(hProcess, true))
	{
		// ��Դ״�������˱仯����Ҫ��������
		return;
	}

	if (m_fForwardSignalLimit == 0)
	{
		if (m_fForwardSignalDistance < 50)
		{
			// ǰ������Ϊ0�����Ҿ���С��50��ֱ��ͣ��
			if (m_fCurrentSpeed > 1E-3)
			{
				m_bInStopThread = true;
				_beginthread(StopAndPressEnter, NULL, this);
			}
			else
			{
				// ��TAB������ͨ��
				if (m_sGameTime.m_nSecond % 6 == 0)
					PressKeyToTrainWnd(VK_TAB);
			}

			return;
		}
		else
		{
			SForwardLimit limit;
			limit.m_fDistance = m_fForwardSignalDistance -= 40;
			limit.m_fSpeedLimit = 0;
			m_listLimit.push_back(limit);
		}
	}
	else if (m_fForwardSignalLimit > 0)
	{
		SForwardLimit limit;
		limit.m_fDistance = m_fForwardSignalDistance;
		limit.m_fSpeedLimit = m_fForwardSignalLimit;
		m_listLimit.push_back(limit);
	}
	else
	{
		//ǰ������Ϊ-1����ʾû��ǰ���ź�����
	}

	AdjustPowerAndBreak();
}

void CMSTSHelpDlg::AutoDrive(HANDLE hProcess)
{
	HWND hWnd = ::FindWindow(L"TrainSim", NULL);
	HWND hForeGroundWindow = ::GetForegroundWindow();

	// �����ǰ���ڲ���MSTS����ô�Ͳ������κβ���
	if (hWnd != hForeGroundWindow)
		return;

	// Ϊ�����г���׼��
	Direction currentDirection = GetDirection(m_loco, hProcess);

	// ����ǰ������
	if (currentDirection == Middle || currentDirection == Backward)
	{
		changeDirection(hProcess, Forward);
		// �ı�ǰ�����򣬴�ʱ���г��������Ѿ������˱仯����˼���ִ��û��ʲô���塣
		return;
	}

	// ���õ�Դ״��
	if (MakePowered(hProcess, true))
	{
		// ��Դ״�������˱仯����Ҫ��������
		return;
	}

	//�����г����е�״�����������ʺ��ƶ�
	if (m_fForwardSignalLimit >= 0)
	{
		SForwardLimit limit;
		limit.m_fDistance = m_fForwardSignalDistance;
		limit.m_fSpeedLimit = m_fForwardSignalLimit;
		m_listLimit.push_back(limit);
	}

	AdjustPowerAndBreak();
}

void CMSTSHelpDlg::ApplyBreak()
{
	int nLevelBit;
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x8099BC, (LPVOID)&nLevelBit, 4, 5, 0, 0, 0x8, 0x10, 0x248));
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x8099BC, (LPVOID)&m_fBreakNum, 4, 5, 0, 0, 0x8, 0x10, 0x24C));
	changeBitsLevelToString(nLevelBit);

	if (m_nBreakLevel == -1)
		PressKeyToTrainWnd(AIR_BREAK_APPLY);
	else if (m_nBreakLevel < 8 || m_fBreakNum != 1)
		PressKeyToTrainWnd(AIR_BREAK_APPLY);
}

void CMSTSHelpDlg::ReleaseBreak()
{
	int nLevelBit;
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x8099BC, (LPVOID)&nLevelBit, 4, 5, 0, 0, 0x8, 0x10, 0x248));
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x8099BC, (LPVOID)&m_fBreakNum, 4, 5, 0, 0, 0x8, 0x10, 0x24C));
	changeBitsLevelToString(nLevelBit);

	if (m_nBreakLevel == -1)
		PressKeyToTrainWnd(AIR_BREAK_RELEASE);
	else if (m_nBreakLevel > 5 || m_fBreakNum != 1)
		PressKeyToTrainWnd(AIR_BREAK_RELEASE);
}

void CMSTSHelpDlg::ReleaseElectricBreak()
{
	if (m_loco == Electric)
	{
		float fBreak;

		for (int i = 0; i < 20; ++i)
		{
			ReadPointerMemory(m_hTrainProcess, (LPCVOID)0x7B6440, &fBreak, 4, 1, 0x32C);

			if (fBreak == 0)
				break;

			PressKeyToTrainWnd('D');
			Sleep(100);
		}
	}
}

bool CMSTSHelpDlg::GetCurrentSchedule(HANDLE hProcess, SSchedule **pSchedule, SSchedule &schedule)
{
	SHead head;
	CHECK(ReadProcessMemory(hProcess, (void *)0x8098C0, (LPVOID)&head, sizeof(SHead), NULL))

	if (head.currentSchedule != NULL)
	{
		if (pSchedule)
			*pSchedule = head.currentSchedule;

		CHECK(ReadProcessMemory(hProcess, (void *)head.currentSchedule, (LPVOID)&schedule, sizeof(schedule), NULL))
		return true;
	}

	return false;
}

BOOL CMSTSHelpDlg::IsTaskMode(HANDLE hProcess)
{
	//�Ƿ���ǰ����վ��Ϣ
	SHead head;
	SList node;
	CHECK(ReadProcessMemory(hProcess, (void *)0x8098C0, (LPVOID)&head, sizeof(SHead), NULL))
	CHECK(ReadProcessMemory(hProcess, (void *)head.head, (LPVOID)&node, sizeof(SList), NULL))
	return node.m_next != head.head;
}

void CMSTSHelpDlg::GetAllSchedule(HANDLE hProcess)
{
	SHead head;

	if (!ReadProcessMemory(hProcess, (void *)0x8098C0, (LPVOID)&head, sizeof(SHead), NULL))
	{
		CLogger::Log("Read Memory Failed in %s", __FUNCTION__);
		return;
	}
	else
	{
		CString str;
		str.Format(L"%X %X", head.head, head.currentSchedule);
		MessageBox(str);
	}

	void *pNode = head.head;
	CString output;

	if (head.currentSchedule != NULL)
	{
		SList node;
		CHECK(ReadProcessMemory(hProcess, (void *)pNode, (LPVOID)&node, sizeof(SList), NULL))
		CString str;
		str.Format(L"%X %X %X\n", node.m_next, node.m_pre, node.m_data);
		output += str;
		m_vectSchedule.clear();

		while (node.m_next != head.head)
		{
			pNode = node.m_next;
			CHECK(ReadProcessMemory(hProcess, pNode, (LPVOID)&node, sizeof(SList), NULL))
			str.Format(L"%X %X %X\n", node.m_next, node.m_pre, node.m_data);
			output += str;
			SSchedule schedule;
			CHECK(ReadProcessMemory(hProcess, node.m_data, (LPVOID)&schedule, sizeof(SSchedule), NULL))
			m_vectSchedule.push_back(schedule);
		}

		MessageBox(output);
	}

	CString strSchedule;

	for (size_t i = 0; i < m_vectSchedule.size(); ++i)
	{
		strSchedule += m_vectSchedule[i].toString() + L"\n";
	}

	MessageBox(strSchedule);
}
void CMSTSHelpDlg::OnAutoDriveChanged()
{
	UpdateData();

	if (m_bAutoDrive)
	{
		m_bAutoGetData = true;
		UpdateData(false);
	}
}

void CMSTSHelpDlg::OnAutoGetDataChanged()
{
	UpdateData();

	if (m_bAutoDrive)
	{
		m_bAutoDrive = false;
		UpdateData(false);
	}
}

void CMSTSHelpDlg::OnTopMostChanged()
{
	UpdateData();

	if (m_bTopMost)
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void CMSTSHelpDlg::OnAutoAdjustChanged()
{
	UpdateData();
}


void CMSTSHelpDlg::OnAutoEscMsg()
{
	UpdateData();

	if (m_bAutoEscapeMessage)
	{
		m_nTimerEscMsg = SetTimer(AUTO_ESC_MSG_TIMER, DATA_SPAN / 2, NULL);
	}
	else
	{
		KillTimer(m_nTimerEscMsg);
	}
}

void CMSTSHelpDlg::OnAutoSkipError()
{
	UpdateData();

	if (m_bAutoSkipError)
	{
		m_nTimerSkipError = SetTimer(AUTO_SKIP_ERROR_TIMER, DATA_SPAN / 2, NULL);
	}
	else
	{
		KillTimer(m_nTimerSkipError);
	}
}

void CMSTSHelpDlg::OnAutoSave()
{
	UpdateData();
}

void CMSTSHelpDlg::OnIsConnectMode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
}
