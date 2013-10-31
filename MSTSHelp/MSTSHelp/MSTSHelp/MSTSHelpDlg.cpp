// MSTSHelpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MSTSHelp.h"
#include "MSTSHelpDlg.h"
#include "Logger.h"
#include "UtilTDBFile.h"
#include <assert.h>
#include <math.h>
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CMSTSHelpDlg 对话框

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
	, m_station_stop(FALSE)
	, m_station_name(_T(""))
	, m_bDownFloatLimit(FALSE)
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
	DDX_Check(pDX, IDC_CHECK9, m_station_stop);
	DDX_Text(pDX, IDC_EDIT1, m_station_name);
	DDX_Check(pDX, IDC_CHECK10, m_bDownFloatLimit);
}

BEGIN_MESSAGE_MAP(CMSTSHelpDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMSTSHelpDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &CMSTSHelpDlg::OnAutoDriveChanged)
	ON_BN_CLICKED(IDC_CHECK2, &CMSTSHelpDlg::OnAutoGetDataChanged)
	ON_BN_CLICKED(IDC_CHECK3, &CMSTSHelpDlg::OnTopMostChanged)
	ON_BN_CLICKED(IDC_CHECK4, &CMSTSHelpDlg::OnAutoAdjustChanged)
	ON_BN_CLICKED(IDC_CHECK5, &CMSTSHelpDlg::OnAutoEscMsg)
	ON_BN_CLICKED(IDC_CHECK6, &CMSTSHelpDlg::OnAutoSkipError)
	ON_BN_CLICKED(IDC_CHECK7, &CMSTSHelpDlg::OnAutoSave)
	ON_BN_CLICKED(IDC_CHECK8, &CMSTSHelpDlg::OnIsConnectMode)
END_MESSAGE_MAP()


// CMSTSHelpDlg 消息处理程序

BOOL CMSTSHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//初始化定时器
	m_nTimerGetData = SetTimer(AUTO_GET_DATA_TIMER, DATA_SPAN, NULL);
	//初始化手动限速
	m_strManualLimit = L"1000";
	//初始化游戏时间
	m_fGameTime = -1;
	m_lastSaveTime = -1;
	m_carriage_count = 0;
	//初始化显示列表
	initControlList();
	UpdateData(false);
	m_pCurrentSchedule = NULL;
	m_hTrainProcess = NULL;
	m_bInStopThread = false;
	m_bIsProcessing = false;
	m_last_vectorNodePtr = NULL;
	m_last_nSignalItemIndexInTrackNode8 = 0;
	m_bDownFloatLimit = FALSE;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
		case VK_RETURN:   //屏蔽Enter
			return true;
		case VK_ESCAPE:   //屏蔽Esc
			return true;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMSTSHelpDlg::OnPaint()
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
HCURSOR CMSTSHelpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMSTSHelpDlg::OnBnClickedButton1()
{
	UpdateData();

	if (m_bTopMost)
	{
		CWnd *train_wnd = FindWindow(L"TrainSim", NULL);

		if (NULL != train_wnd)
		{
			WINDOWPLACEMENT placement;
			train_wnd->GetWindowPlacement(&placement);

			if (placement.showCmd != SW_SHOWMINIMIZED)
			{
				train_wnd->BringWindowToTop();
				train_wnd->SetForegroundWindow();
			}
		}
	}

	if (!GetTrainHandle(m_hTrainProcess))
	{
		//获取不到MSTS进程,m_hTrainProcess会被自动置空，因为传递的是一个引用
		//清空列表数据
		m_pCurrentSchedule = NULL;
		m_lastSaveTime = -1;
		clearControlList();
		m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"等待MSTS启动");
		return;
	}

	if (m_bAutoAdjustLocation)
		AdjustLocation();

	if (!GetTrainPointer(m_hTrainProcess))
	{
		//获取不到MSTS任务的开启
		//清空列表数据
		m_pCurrentSchedule = NULL;
		m_lastSaveTime = -1;
		clearControlList();
		m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"等待MSTS任务运行");
		return;
	}

	//清空列表数据
	clearControlList();
	//获取列车运行的数据
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
			if (PressKeyToTrainWnd(VK_F2))
			{
				m_lastSaveTime = m_fGameTime;
				CLogger::Log("Save Game Time:%d:%d:%d", m_sGameTime.m_nHour, m_sGameTime.m_nMinute, m_sGameTime.m_nSecond);
				Sleep(200);
				PressKeyToTrainWnd(VK_ESCAPE);
			}
		}
	}

	if (m_bAutoDrive && !m_bInStopThread)
	{
		if (!m_bIsProcessing)
		{
			m_bIsProcessing = true;
			__try
			{
				m_vectLimit.clear();
				float fTrainBreak;
				ReadPointerMemory(m_hTrainProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&fTrainBreak, 4, 4, 0, 0x8, 0x10, 0x442);

				if (fTrainBreak != 0)
				{
					PressKeyToTrainWnd(VK_OEM_4);
				}

				if (m_fCurrentSpeedLimit == 0)
				{
					//限速为0
					// 此处删除了白灯无码，因为已经添加了铁轨末端自动停车的方法
					if (m_fCurrentSpeed > 1E-3 || m_fCurrentSpeed < -1E-3)
					{
						m_bInStopThread = true;
						_beginthread(StopAndPressEnter, NULL, this);
					}
				}
				else{
					HWND hWnd = ::FindWindow(L"TrainSim", NULL);
					HWND hForeGroundWindow = ::GetForegroundWindow();

					// 如果当前窗口不是MSTS，那么就不进行任何操作
					if (hWnd != hForeGroundWindow)
					{
						return;
					}

					if (m_isConnectMode)
					{
						int carriage_count = GetCarriageCount(m_hTrainProcess, (LPVOID)TRAIN_INFO_MEM);

						if (0 == m_carriage_count)
						{
							m_carriage_count = carriage_count;
						}
						else if (carriage_count != m_carriage_count)
						{
							m_isConnectMode = FALSE;
							m_bAutoDrive = FALSE;
							UpdateData(FALSE);
							float fZero = 0;
							WritePointerMemory(m_hTrainProcess, (LPVOID)POWER_INFO_MEM, &fZero, 4, 1, 0x8C);

							for (int i = 0; i < 10; ++i)
							{
								ApplyBreak();
								Sleep(100);
							}

							return;
						}

						if (0 == m_fForwardSignalLimit)
						{
							void *vectorNodePtr;
							int nSignalItemIndexInTrackNode8;
							CHECK(ReadTrainProcess(m_hTrainProcess, (void *)0x809AC4, (LPVOID)&vectorNodePtr, 4))
							CHECK(ReadTrainProcess(m_hTrainProcess, (void *)0x809ACC, (LPVOID)&nSignalItemIndexInTrackNode8, 4))

							if (m_last_vectorNodePtr != vectorNodePtr || m_last_nSignalItemIndexInTrackNode8 != nSignalItemIndexInTrackNode8)
							{
								m_last_vectorNodePtr = vectorNodePtr;
								m_last_nSignalItemIndexInTrackNode8 = nSignalItemIndexInTrackNode8;
								PressKeyToTrainWnd(VK_TAB);
							}
							else
							{
								if (m_sGameTime.m_nSecond % 6 == 0)
								{
									PressKeyToTrainWnd(VK_TAB);
								}

								float distance = m_fForwardSignalDistance - 40;

								if (distance < 0)
								{
									distance = 0;
								}

								m_vectLimit.push_back(SForwardLimit(distance, 0));
							}
						}
						else if (m_fForwardSignalLimit > 0 && m_fForwardSignalDistance != 0)
						{
							SForwardLimit limit;
							limit.m_fDistance = m_fForwardSignalDistance;
							limit.m_fSpeedLimit = m_fForwardSignalLimit;
							m_vectLimit.push_back(limit);
						}

						GetConnectSpeedLimit(m_hTrainProcess, m_vectLimit, m_station_stop, m_station_name);
						UpdateData(FALSE);
						AutoConnectTrain(m_hTrainProcess);
						return;
					}

					if (IsTaskMode(m_hTrainProcess))
					{
						AutoDriveTask(m_hTrainProcess);
					}
					else
					{
						AutoDrive(m_hTrainProcess);
					}

				}
			}
			__finally
			{
				m_bIsProcessing = false;
			}
		}
	}
}

void CMSTSHelpDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	UpdateData();

	switch (nIDEvent)
	{
	case AUTO_GET_DATA_TIMER:

		try
		{
			OnBnClickedButton1();
		}
		catch (int)
		{
			clearControlList();
			m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"获取数据失败");
		}

		break;
	case AUTO_ESC_MSG_TIMER:
	{
		int nFlag;

		if (ReadProcessMemory(m_hTrainProcess, (void *)PAUSE_BY_MSG_MEM, (LPVOID)&nFlag, 4, NULL) && !nFlag)
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

			if (message == L"错误:是否继续?")
			{
				HWND hParentWnd = ::GetParent(wnd->GetSafeHwnd());
				PressKey('Y');
			}
			else if (message == L"微软模拟火车")
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
	int count = m_listCtrl.GetItemCount();

	for (int i = 0; i < count; ++i)
	{
		m_listCtrl.SetItemText(i, 1, L"");
	}

	m_station_name.Empty();
	UpdateData(FALSE);
}
void CMSTSHelpDlg::initControlList()
{
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listCtrl.InsertColumn(0, _T("属性"), LVCFMT_LEFT, 80);
	m_listCtrl.InsertColumn(1, _T("内容"), LVCFMT_LEFT, 120);
	m_listCtrl.InsertItem(CURRENT_SPEED_ITEM, L"当前速度");
	m_listCtrl.InsertItem(CURRENT_LIMIT_ITEM, L"当前限速");
	m_listCtrl.InsertItem(SIGNAL_DISTANCE_ITEM, L"前方信号距离");
	m_listCtrl.InsertItem(SIGNAL_LIMIT_ITEM, L"前方信号限速");
	m_listCtrl.InsertItem(FORWARD_COLOR_ITEM, L"信号颜色");
	m_listCtrl.InsertItem(CURRENT_ACCER_ITEM, L"加速度值");
	m_listCtrl.InsertItem(CURRENT_TIME_ITEM, L"当前时间");
	m_listCtrl.InsertItem(INFORMATION_ITEM, L"信息");
	m_listCtrl.InsertItem(LOCO_TYPE_ITEM, L"车头类型");
	m_listCtrl.InsertItem(BREAK_NUM_ITEM, L"制动数据");
	m_listCtrl.InsertItem(BREAK_LEVEL_ITEM, L"制动级别");
	m_listCtrl.InsertItem(SCHEDULE_INFO_ITEM, L"时刻表信息");
	m_listCtrl.InsertItem(CALCULATED_SPEED_LIMIT, L"目前列车限速");
	m_listCtrl.InsertItem(FOWARD_STATION_NAME, L"前方车站名称");
	m_listCtrl.InsertItem(POWER_AND_ELECTRIC_BREAK, L"功率和制动值");
	m_listCtrl.InsertItem(ROUTE_SPEED_LIMIT, L"线路限速");
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

		// 不是任务模式(或者是任务模式 没有前方到站信息) 退出循环
		if (!GetCurrentSchedule(pDlg->m_hTrainProcess, NULL, schedule))
			break;

		// 列车已经到站 退出循环
		if (schedule.m_fActualArrivalTime != 0)
			break;

		PressKeyToTrainWnd(VK_RETURN);
		Sleep(500);
	}

	pDlg->m_bInStopThread = false;
	return;
}

//直接采用以-0.1m/s*s的加速度算出来当前应该限速多少，然后根据这个进行控制
//调整当前的功率和制动值
void CMSTSHelpDlg::AdjustPowerAndBreak()
{
	CString str;
	float fCalculatedSpeedLimit = m_fCurrentSpeedLimit;

	if (m_bDownFloatLimit)
	{
		fCalculatedSpeedLimit -= 0.3f;
	}

	if (fCalculatedSpeedLimit < 0)
	{
		fCalculatedSpeedLimit = 0;
	}

	vector<SForwardLimit>::iterator ite = m_vectLimit.begin();
	bool bInBreaking = false;

	while (ite != m_vectLimit.end())
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
		{
			++ite;
			continue;
		}

		if (ite->m_fDistance > 3000)
		{
			++ite;
			continue;
		}

		float fSpeedLimit = sqrt(ite->m_fSpeedLimit * ite->m_fSpeedLimit + 2 * m_fExpectedAccerlate * ite->m_fDistance);

		if (fSpeedLimit < 1.0)
			fSpeedLimit = 1.0;

		if (fCalculatedSpeedLimit > fSpeedLimit)
		{
			//算出来的限速比当前限速要小，因此应该处于制动状态
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

	str.Format(L"%.3f", fCalculatedSpeedLimit * 3.6F);
	m_listCtrl.SetItemText(CALCULATED_SPEED_LIMIT, 1, str);

	//根据当前速度和算出来的限速速度调整功率和制动的值
	if (m_fCurrentSpeed > fCalculatedSpeedLimit)
	{
		//降功率至0
		if (m_fCurrentPower > 1E-3)
			PressKeyToTrainWnd('A');

		//加速度为正再降一次
		if (m_fAcceleration > -1E-3)
			PressKeyToTrainWnd('A');
		else if (m_fAcceleration > -0.3F)
		{
			// 超速，需要进行刹车和减小功率
			if ((m_fCurrentSpeed + m_fAcceleration * 3) > fCalculatedSpeedLimit)
			{
				if (m_loco == Electric)
					PressKeyToTrainWnd('A');
				else
					ApplyBreak();
			}
		}

		if (m_fCurrentSpeed > fCalculatedSpeedLimit + 2 / 3.6f)
		{
			float fZero = 0;
			WritePointerMemory(m_hTrainProcess, (LPVOID)POWER_INFO_MEM, &fZero, 4, 1, 0x8C);
		}

		//判断是否要对刹车进行处理

		if (m_fCurrentSpeed > 1.3 * fCalculatedSpeedLimit + 3)
		{
			//严重超速，刹车就算够了，也要继续增大刹车速度
			ApplyBreak();
		}
		else if ((m_fCurrentSpeed > fCalculatedSpeedLimit + 1.38)
		         || m_fCurrentSpeed > 1.1 * fCalculatedSpeedLimit)
		{
			//超速较多，刹车加速度不够
			if (m_fAcceleration > -0.4)
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
		// 列车加速度过小，需要加速
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
		// 加速过快，需要适当减少加速度
		PressKeyToTrainWnd('A');
	}
}

CString CMSTSHelpDlg::changeBitsLevelToString(int nBitsLevel)
{
	CString strLevel;

	if (nBitsLevel & (nBitsLevel - 1) || !nBitsLevel)
	{
		strLevel = L"错误的制动级别数据";
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
		strColor = L"红灯";
		break;
	case 2:
		strColor = L"红黄灯";
		break;
	case 3:
		strColor = L"双黄灯";
		break;
	case 4:
		strColor = L"黄2灯";
		break;
	case 5:
		strColor = L"黄灯";
		break;
	case 6:
		strColor = L"绿黄灯";
		break;
	case 7:
		strColor = L"绿灯";
		break;
	case 8:
		strColor = L"白灯";
		break;
	default:
		strColor = L"不能识别的车灯";
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
		strLocoType = L"蒸汽机";
		break;
	case Diesel:
		strLocoType = L"内燃机";
		break;
	case Electric:
		strLocoType = L"电力机";
		break;
	default:
		strLocoType = L"无法识别的车头";
		break;
	}

	return strLocoType;
}

float CMSTSHelpDlg::timeMinus(float a, float b)
{
	float result = a - b;

	if (result < -12 * 3600)
		return result + 24 * 3600;
	else if (result > 12 * 3600)
		return result - 24 * 3600;
	else return result;
}

void CMSTSHelpDlg::GetTrainData(HANDLE hProcess)
{
	//float fForwardTurnOff;
	int nLevelBit;
	CString strColor;
	m_fTaskTempSpeedLimit = GetTaskTempSpeedLimit(hProcess);
	CHECK(ReadTrainProcess(hProcess, (void *)CUR_SPEED_MEM, (LPVOID)&m_fCurrentSpeed, 4))
	CHECK(ReadTrainProcess(hProcess, (void *)SPEED_LIMIT_MEM, (LPVOID)&m_fCurrentSpeedLimit, 4))
	BOOL bIsInTaskLimit;
	CHECK(ReadTrainProcess(hProcess, (void *)0x809AAC, (LPVOID)&bIsInTaskLimit, 4))
	m_fCurrentSpeedLimit = bIsInTaskLimit ? m_fTaskTempSpeedLimit : m_fCurrentSpeedLimit;
	CHECK(ReadTrainProcess(hProcess, (void *)FOWARD_LIMIT_MEM, (LPVOID)&m_fForwardSignalLimit, 4))
	CHECK(ReadTrainProcess(hProcess, (void *)SIG_DISTANT_MEM, (LPVOID)&m_fForwardSignalDistance, 4))
	void *signal_pointer;
	CHECK(ReadTrainProcess(hProcess, (void *)0x809AC4, (LPVOID)&signal_pointer, 4))

	if (NULL == signal_pointer)
	{
		// 没有前方信号 为了方便处理，直接将前方信号灯限速置为-1
		m_fForwardSignalLimit = -1;
	}

	CHECK(ReadTrainProcess(hProcess, (void *)S_GAME_TIME_MEM, (LPVOID)&m_sGameTime, 12))
	CHECK(ReadTrainProcess(hProcess, (void *)F_GAME_TIME_MEM, (LPVOID)&m_fGameTime, 4));
	CString strTime;
	strTime.Format(L"%02d:%02d:%02d", m_sGameTime.m_nHour, m_sGameTime.m_nMinute, m_sGameTime.m_nSecond);
	CHECK(ReadTrainProcess(hProcess, (void *)LIGHT_COLOR_MEM, (LPVOID)&m_cColor1, 1))
	strColor = changeColorToString(m_cColor1);
	strColor += L" : ";
	CHECK(ReadTrainProcess(hProcess, (void *)LIGHT_COLOR_MEM_2, (LPVOID)&m_cColor2, 1))
	strColor += changeColorToString(m_cColor2);
	CHECK(ReadTrainProcess(hProcess, (void *)ACCER_MEM, (LPVOID)&m_fAcceleration, 4))
	//CHECK(ReadTrainProcess(hProcess, (void *)FORWARD_TURNOFF_MEM, (LPVOID)&fForwardTurnOff, 4, NULL))
	CHECK(ReadPointerMemory(hProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&m_fBreakNum, 4, 4, 0, 0x8, 0x10, 0x24C))
	CHECK(ReadPointerMemory(hProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&nLevelBit, 4, 4, 0, 0x8, 0x10, 0x248))
	CString str;
	str.Format(L"%.3f", m_fCurrentSpeed * 3.6F);
	m_listCtrl.SetItemText(CURRENT_SPEED_ITEM, 1, str);
	str.Format(L"%.3f", m_fCurrentSpeedLimit * 3.6F);
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
	m_listCtrl.SetItemText(INFORMATION_ITEM, 1, L"成功获得数据");
	str.Format(L"%.2f", m_fBreakNum);
	m_listCtrl.SetItemText(BREAK_NUM_ITEM, 1, str);
	str = changeBitsLevelToString(nLevelBit);
	m_listCtrl.SetItemText(BREAK_LEVEL_ITEM, 1, str);
	float fNum2;
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)POWER_INFO_MEM, &m_fCurrentPower, 4, 1, 0x8C))
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)POWER_INFO_MEM, &fNum2, 4, 1, 0x32C))
	str.Format(L"%.3f %.3f", m_fCurrentPower, fNum2);
	m_listCtrl.SetItemText(POWER_AND_ELECTRIC_BREAK, 1, str);
	float fRouteLimit;
	CHECK(ReadPointerMemory(hProcess, (LPCVOID)ROUTE_SPEED_LIMIT_MEM, (LPVOID)&fRouteLimit, 4, 1, 0x64))
	str.Format(L"%.1f", fRouteLimit * 3.6);
	m_listCtrl.SetItemText(ROUTE_SPEED_LIMIT, 1, str);
	SSchedule schedule;

	if (GetCurrentSchedule(hProcess, NULL, schedule))
	{
		//仅仅获取数据，不改变成员变量的信息。
		float fCurrentTime, fDistance;
		CHECK(ReadTrainProcess(hProcess, (void *)F_GAME_TIME_MEM, (LPVOID)&fCurrentTime, 4))
		CHECK(ReadTrainProcess(hProcess, (void *)FOWARD_STATION_DIS_MEM, (LPVOID)&fDistance, 4))
		ShowScheduleInfo(schedule, fCurrentTime, fDistance);
	}
}


void CMSTSHelpDlg::ShowScheduleInfo(const SSchedule &schedule, float fCurrentTime, float fDistance)
{
	CString str;
	wchar_t stationName[30];
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)TR_ITEM_ARRAY_MEM, stationName, 30, 6, 0xC, 0x20, 0, 4 * schedule.m_nPlatformStartID, 0x28, 0))
	m_listCtrl.SetItemText(FOWARD_STATION_NAME, 1, stationName);

	if (schedule.m_fActualArrivalTime != 0)
		str.Format(L"%.1f 秒后出发", timeMinus(schedule.m_fDepartTime, fCurrentTime));
	else
		str.Format(L"%.0f米 %.1f秒", fDistance, timeMinus(schedule.m_fArrivalTime, fCurrentTime));

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
		CHECK(ReadPointerMemory(hProcess, (LPCVOID)POWER_INFO_MEM, &fNum1, 4, 1, 0x8C))

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

bool CMSTSHelpDlg::UpdateScheduleInfo(float &fNextStationDistance)
{
	SSchedule schedule;
	SSchedule *pSchedule;

	if (!GetCurrentSchedule(m_hTrainProcess, &pSchedule, schedule))
		return false;

	if (NULL == m_pCurrentSchedule)
	{
		//当前的时刻表不是合法的，更新时刻表
		m_pCurrentSchedule = pSchedule;
		m_currentSchedule = schedule;
		CLogger::Log("Schedule Changed because NULL %s", m_currentSchedule.toSTLString().c_str());
	}
	else if (m_pCurrentSchedule == pSchedule)
	{
		//时刻表未发生变化，更新数据内容
		m_currentSchedule = schedule;
	}
	else
	{
		//时刻表发生了变化，需要考虑是否使用新的时刻表
		if (timeMinus(m_currentSchedule.m_fDepartTime, m_fGameTime) < 0)
		{
			//当前时刻表出发时间已过
			if (m_currentSchedule.m_fActualArrivalTime != 0)
			{
				//当前时刻表出发时间已过，而且已经停过车, 更新时刻表
				CLogger::Log("Schedule Changed because Update old %s new %s", schedule.toSTLString().c_str(), m_currentSchedule.toSTLString().c_str());
				m_currentSchedule = schedule;
				m_pCurrentSchedule = pSchedule;
			}
			else
			{
				//当前时刻表出发时间已过，但是尚未停过车,按正常停车处理，不更新时刻表
			}
		}
		else
		{
			//当前时刻表出发时间未过，使用旧时刻表，并更新旧时刻表当中的内容
			CHECK(ReadTrainProcess(m_hTrainProcess, (void *)m_pCurrentSchedule, (LPVOID)&m_currentSchedule, sizeof(schedule)))
			fNextStationDistance = 0;
		}
	}

	return true;
}

void CMSTSHelpDlg::PrepareDataAndControlTrain(HANDLE hProcess)
{
	////////////////////////////////////////////////////////////////////////////////
	Direction currentDirection = GetDirection(m_loco, hProcess);

	if (currentDirection == Middle || currentDirection == Backward)
	{
		changeDirection(hProcess, Forward);
		// 改变前进方向，此时的列车数据是已经发生了变化，因此继续执行没有什么意义。
		return;
	}

	if (MakePowered(hProcess, true))
	{
		// 电源状况发生了变化，需要更新数据
		return;
	}

	if (m_fForwardSignalLimit == 0)
	{
		if (m_fForwardSignalDistance < 50)
		{
			// 前方限速为0，而且距离小于50，直接停车
			if (m_fCurrentSpeed > 1E-3)
			{
				m_bInStopThread = true;
				_beginthread(StopAndPressEnter, NULL, this);
			}
			else
			{
				// 按TAB键申请通过
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
			m_vectLimit.push_back(limit);
		}
	}
	else if (m_fForwardSignalLimit > 0)
	{
		SForwardLimit limit;
		limit.m_fDistance = m_fForwardSignalDistance;
		limit.m_fSpeedLimit = m_fForwardSignalLimit;
		m_vectLimit.push_back(limit);
	}
	else
	{
		//前方限速为-1，表示没有前方信号限速
	}

	GetForwardSpeedLimit(m_hTrainProcess, m_vectLimit, m_fTaskTempSpeedLimit);

	if (m_bDownFloatLimit)
	{
		for (size_t i = 0; i < m_vectLimit.size(); ++i)
		{
			if (m_vectLimit[i].m_fSpeedLimit > 0.3f)
			{
				m_vectLimit[i].m_fSpeedLimit -= 0.3f;
			}
		}
	}

	AdjustPowerAndBreak();
}

void CMSTSHelpDlg::AutoConnectTrain(HANDLE hProcess)
{
	//GetConnectDistance(hProcess, 1000);
	//PressKeyToTrainWnd('6');
	//int mode;
	//CHECK(ReadTrainProcess(hProcess, (void *)VIEW_MODE_MEM, (LPVOID)&mode, 4))
	//if (mode == 8)
	//{
	float distance = GetConnectDistance(hProcess, 1000);

	if (distance < 0)
	{
		SForwardLimit limit;
		limit.m_fDistance = 20;
		limit.m_fSpeedLimit = 1.0;
		m_vectLimit.push_back(limit);
	}
	else
	{
		SForwardLimit limit;

		if (distance < 10)
		{
			limit.m_fDistance = 0;
		}
		else if (distance < 60)
		{
			limit.m_fDistance = distance  / 2;
		}
		else if (distance < 260)
		{
			float scale = (distance - 60) / 200;
			limit.m_fDistance = distance  / (2 - scale);
		}
		else
		{
			limit.m_fDistance = distance;
		}

		limit.m_fSpeedLimit = 1.0;
		m_vectLimit.push_back(limit);
	}

	Direction currentDirection = GetDirection(m_loco, hProcess);

	if (Middle == currentDirection)
	{
		changeDirection(hProcess, Forward);
		// 改变前进方向，此时的列车数据是已经发生了变化，因此继续执行没有什么意义。
		return;
	}

	if (MakePowered(hProcess, true))
	{
		// 电源状况发生了变化，需要更新数据
		return;
	}

	//此处的代码并没有区分向前和向后的情况，因为想实现向后挂车

	if (m_fCurrentSpeed < 0)
	{
		m_fCurrentSpeed = -m_fCurrentSpeed;
		m_fAcceleration = -m_fAcceleration;
	}

	AdjustPowerAndBreak();
	//}
}

void CMSTSHelpDlg::AutoDriveTask(HANDLE hProcess)
{
	///////////////////////////////////////////////////////////////////////////////
	//如果是任务模式，需要得知前方到站的信息
	float fNextStationDistance;
	CHECK(ReadTrainProcess(hProcess, (void *)FOWARD_STATION_DIS_MEM, (LPVOID)&fNextStationDistance, 4))

	if (UpdateScheduleInfo(fNextStationDistance))
	{
		// 是任务模式 而且前方有合法的到站信息
		ShowScheduleInfo(m_currentSchedule, m_fGameTime, fNextStationDistance);
		//在这里根据时刻表直接进行一次筛选

		if (m_currentSchedule.m_fActualArrivalTime == 0)
		{
			//处理没有到达前方站台的情况
			SForwardLimit limit;
			limit.m_fDistance = fNextStationDistance;
			limit.m_fSpeedLimit = 0;
			m_vectLimit.push_back(limit);
		}
		else if (m_currentSchedule.m_fActualArrivalTime != 0 && timeMinus(m_fGameTime, m_currentSchedule.m_fDepartTime) < 0)
		{
			//处理已经到达站台但还没有到出发时间的情况
			if (m_fCurrentSpeed > 1E-3)
				ApplyBreak();
			else if (m_fCurrentPower != 0)
				for (int i = 0; i < 10; ++i)
					PressKeyToTrainWnd('A');
			else if (timeMinus(m_currentSchedule.m_fDepartTime, m_fGameTime) < 20)
				for (int i = 0; i < 30; ++i)
					ReleaseBreak();

			return;
		}
		else
		{
			//已经到达站台而且也已经到达出发的时间
		}
	}
	else
	{
		CLogger::Log("task mode but no valid forward station info.");
	}

	PrepareDataAndControlTrain(hProcess);
}

void CMSTSHelpDlg::AutoDrive(HANDLE hProcess)
{
	PrepareDataAndControlTrain(hProcess);
}

void CMSTSHelpDlg::ApplyBreak()
{
	int nLevelBit;
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&nLevelBit, 4, 4, 0, 0x8, 0x10, 0x248));
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&m_fBreakNum, 4, 4, 0, 0x8, 0x10, 0x24C));
	changeBitsLevelToString(nLevelBit);

	if (m_nBreakLevel == -1)
		PressKeyToTrainWnd(AIR_BREAK_APPLY);
	else if (m_nBreakLevel < 8 || m_fBreakNum != 1)
		PressKeyToTrainWnd(AIR_BREAK_APPLY);
}

void CMSTSHelpDlg::ReleaseBreak()
{
	int nLevelBit;
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&nLevelBit, 4, 4, 0, 0x8, 0x10, 0x248));
	CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)BREAK_INFO_MEM, (LPVOID)&m_fBreakNum, 4, 4, 0, 0x8, 0x10, 0x24C));
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
			CHECK(ReadPointerMemory(m_hTrainProcess, (LPCVOID)POWER_INFO_MEM, &fBreak, 4, 1, 0x32C))

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
	CHECK(ReadTrainProcess(hProcess, (void *)CURRENT_SCHEDULE_MEM, (LPVOID)&head, sizeof(SHead)))

	if (head.currentSchedule != NULL)
	{
		if (pSchedule)
			*pSchedule = head.currentSchedule;

		CHECK(ReadTrainProcess(hProcess, (void *)head.currentSchedule, (LPVOID)&schedule, sizeof(schedule)))
		return true;
	}

	return false;
}

BOOL CMSTSHelpDlg::IsTaskMode(HANDLE hProcess)
{
	//是否有前方到站信息
	SHead head;
	SNode node;
	CHECK(ReadTrainProcess(hProcess, (void *)CURRENT_SCHEDULE_MEM, (LPVOID)&head, sizeof(SHead)))
	CHECK(ReadTrainProcess(hProcess, (void *)head.head, (LPVOID)&node, sizeof(SNode)))
	return node.next != head.head;
}

void CMSTSHelpDlg::OnAutoDriveChanged()
{
	UpdateData();

	if (m_bAutoDrive)
	{
		m_bAutoGetData = true;
		UpdateData(false);
	}
	else
	{
		m_isConnectMode = FALSE;
		UpdateData(false);
	}
}

void CMSTSHelpDlg::OnAutoGetDataChanged()
{
	UpdateData();

	if (m_bAutoDrive)
	{
		m_bAutoDrive = false;
		m_isConnectMode = FALSE;
		UpdateData(false);
	}
}

void CMSTSHelpDlg::OnTopMostChanged()
{
	UpdateData();
	/*if (m_bTopMost)
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}*/
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
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	if (m_isConnectMode)
	{
		m_bAutoDrive = TRUE;
		m_bAutoGetData = TRUE;
		UpdateData(FALSE);
		m_carriage_count = 0;
	}
}
