// MSTSMemoryViewerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSTSMemoryViewer.h"
#include "MSTSMemoryViewerDlg.h"
#include "../SignSpeedLimit/Util.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMSTSMemoryViewerDlg �Ի���




CMSTSMemoryViewerDlg::CMSTSMemoryViewerDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CMSTSMemoryViewerDlg::IDD, pParent)
	, m_strDBListHead(_T(""))
	, m_textContent(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSTSMemoryViewerDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_strDBListHead);
	DDX_Text(pDX, IDC_EDIT1, m_textContent);
}

BEGIN_MESSAGE_MAP(CMSTSMemoryViewerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMSTSMemoryViewerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CMSTSMemoryViewerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMSTSMemoryViewerDlg ��Ϣ�������

BOOL CMSTSMemoryViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMSTSMemoryViewerDlg::OnPaint()
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
HCURSOR CMSTSMemoryViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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


CString TempSpeedFunc(HANDLE handle, void *ptr)
{
	CString str;
	char *cPtr = (char *)ptr;
	STempSpeed speed;
	ReadTrainProcess(handle, cPtr + 32, &speed, sizeof(STempSpeed));
	str.Format(L"%x %x %.1f %.1f", ptr, speed.nodePtr, speed.fStart, speed.fEnd);
	return str;
}

CString showAllTaskLimit(HANDLE handle)
{
	CString strResult = IteratorList(handle, (LPVOID)TASK_LIMIT_HEAD_MEM, TempSpeedFunc);
	float *fTempLimitPtr;
	ReadTrainProcess(handle, (LPCVOID)TASK_LIMIT_MEM, &fTempLimitPtr, 4);
	float fTempLimit;
	ReadTrainProcess(handle, (LPCVOID)(fTempLimitPtr + 23), &fTempLimit, 4);
	int nType;
	ReadTrainProcess(handle, (LPCVOID)DISTANCE_TYPE_MEM, &nType, 4);
	CString strSpeed;

	if (nType)
		strSpeed.Format(L"Temp Speed Limit %.0f km\r\n", fTempLimit * 3.6);
	else
		strSpeed.Format(L"Temp Speed Limit %.0f mile\r\n", fTempLimit * 2.237);

	strResult += strSpeed;
	return strResult;
}

CString showContentIn80A038(HANDLE handle)
{
	CString strResult;
	size_t mem;
	ReadTrainProcess(handle, (LPCVOID)0x80A038, &mem, 4);
	SNode *nodes[2];
	ReadTrainProcess(handle, (LPCVOID)mem, nodes, 8);
	strResult += IteratorList(handle, (LPVOID)nodes[0], DefaultHandle);
	strResult += IteratorList(handle, (LPVOID)nodes[1], DefaultHandle);
	return strResult;
}

void CMSTSMemoryViewerDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		wchar_t headName[0x100];
		wchar_t tailName[0x100];
		size_t mem;
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8099AC, &mem, 4);
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)(mem + 8), headName, 200);
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)0x8099B0, &mem, 4);
		ReadTrainProcess(m_hTrainProcess, (LPCVOID)(mem + 8), tailName, 200);
		m_textContent.Format(L"headName : %s\r\ntailName : %s\r\n", headName, tailName);
		m_textContent += showAllCarriage(m_hTrainProcess);
		m_textContent += showAllAITrain(m_hTrainProcess);
		m_textContent += showAllTaskLimit(m_hTrainProcess);
		//m_textContent += showContentIn80A038(m_hTrainProcess);
		void *InputMem = NULL;
		swscanf(m_strDBListHead, L"%x", &InputMem);

		if (InputMem != NULL)
			m_textContent += IteratorList(m_hTrainProcess, (LPVOID)InputMem, DefaultHandle);
	}
	catch (int)
	{
		m_textContent = L"��ȡ����ʧ��";
	}

	UpdateData(FALSE);
}

void CMSTSMemoryViewerDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}
