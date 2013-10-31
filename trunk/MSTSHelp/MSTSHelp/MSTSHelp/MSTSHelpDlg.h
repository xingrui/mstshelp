// MSTSHelpDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "Util.h"
#include <vector>
#include <list>
using namespace std;
// CMSTSHelpDlg 对话框
class CMSTSHelpDlg : public CDialog
{
	// 构造
public:
	CMSTSHelpDlg(CWnd *pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_MSTSHELP_DIALOG };
	enum { CURRENT_SPEED_ITEM = 0,
	       CURRENT_LIMIT_ITEM = 1,
	       SIGNAL_DISTANCE_ITEM = 2,
	       SIGNAL_LIMIT_ITEM = 3,
	       FORWARD_COLOR_ITEM = 4,
	       CURRENT_ACCER_ITEM = 5,
	       CURRENT_TIME_ITEM = 6,
	       INFORMATION_ITEM = 7,
	       LOCO_TYPE_ITEM = 8,
	       BREAK_NUM_ITEM = 9,
	       BREAK_LEVEL_ITEM = 10,
	       SCHEDULE_INFO_ITEM = 11,
	       CALCULATED_SPEED_LIMIT = 12,
	       FOWARD_STATION_NAME = 13,
	       POWER_AND_ELECTRIC_BREAK = 14,
	       ROUTE_SPEED_LIMIT = 15,
	       //FORWARD_TURNOFF_ITEM = 6,
	     };
	enum
	{
		AUTO_GET_DATA_TIMER = 10,
		AUTO_ESC_MSG_TIMER = 11,
		AUTO_SKIP_ERROR_TIMER = 12,
	} TIMER_TYPE;

	static const int DATA_SPAN = 1000;
	float m_fExpectedAccerlate;
protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV 支持

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
	BOOL PreTranslateMessage(MSG *pMsg);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnAutoDriveChanged();
	afx_msg void OnAutoGetDataChanged();
	afx_msg void OnTopMostChanged();
	afx_msg void OnAutoAdjustChanged();
	afx_msg void OnAutoEscMsg();
	afx_msg void OnAutoSkipError();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void ApplyBreak();
	void ReleaseBreak();
	void ReleaseElectricBreak();
public:
	void AdjustLocation();
	void AdjustPowerAndBreak();
	void AutoConnectTrain(HANDLE hProcess);
	void AutoDrive(HANDLE hProcess);
	void AutoDriveTask(HANDLE hProcess);
	void PrepareDataAndControlTrain(HANDLE hProcess);
private:
	void GetTrainData(HANDLE hProcess);
	static BOOL IsTaskMode(HANDLE hProcess);
	bool UpdateScheduleInfo(float &fNextStationDistance);
	void ShowScheduleInfo(const SSchedule &schedule, float fCurrentTime, float fDistance);
private:
	void clearControlList();
	void initControlList();
	CString changeBitsLevelToString(int nBitsLevel);
public:
	//static methods
	static CString changeColorToString(char c);
	static CString changeLocoTypeToString(Locomotive loco);
	static void changeDirection(HANDLE hProcess, Direction direction);
	static void StopAndPressEnter(void *pThis);
	static bool GetCurrentSchedule(HANDLE hProcess, SSchedule **pSchedule, SSchedule &schedule);
	static bool MakePowered(HANDLE hProcess, bool isPowered);
	static float timeMinus(float a, float b);
private:
	//Game Data
	float m_fCurrentSpeed;
	float m_fCurrentSpeedLimit;
	float m_fForwardSignalDistance;
	float m_fForwardSignalLimit;
private:
	float m_fAcceleration;
	char m_cColor1, m_cColor2;
private:
	float m_fBreakNum;
	int m_nBreakLevel;
	SGameTime m_sGameTime;
	float m_fGameTime;
	float m_fTaskTempSpeedLimit;
private:
	//Schedule information
	vector<SSchedule> m_vectSchedule;
	SSchedule *m_pCurrentSchedule;
	SSchedule m_currentSchedule;
	float m_lastSaveTime;
	int m_carriage_count;
public:
	bool m_bInStopThread;
	void *m_last_vectorNodePtr;
	int m_last_nSignalItemIndexInTrackNode8;
private:
	HANDLE m_hTrainProcess;
private:
	//Timer
	UINT_PTR m_nTimerGetData;
	UINT_PTR m_nTimerEscMsg;
	UINT_PTR m_nTimerSkipError;
private:
	CListCtrl m_listCtrl;
	// DDX_VARIABLE
	BOOL m_bTopMost;
	BOOL m_bAutoSkipError;
	BOOL m_bAutoEscapeMessage;
	BOOL m_bAutoAdjustLocation;
	BOOL m_bAutoDrive;
	BOOL m_bAutoGetData;
	BOOL m_bAutoSave;
	CString m_strManualLimit;
	afx_msg void OnAutoSave();
	Locomotive m_loco;
private:
	vector<SForwardLimit> m_vectLimit;
	bool m_bIsProcessing;
	float m_fCurrentPower;
	BOOL m_isConnectMode;
	afx_msg void OnIsConnectMode();
	BOOL m_station_stop;
	CString m_station_name;
	BOOL m_bDownFloatLimit;
};

#define CHECK(A) if(!A){CLogger::Log("Read Memory Failed in %s line:%d", __FUNCTION__, __LINE__);}
