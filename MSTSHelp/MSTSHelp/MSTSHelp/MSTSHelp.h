// MSTSHelp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMSTSHelpApp:
// �йش����ʵ�֣������ MSTSHelp.cpp
//

class CMSTSHelpApp : public CWinApp
{
public:
	CMSTSHelpApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMSTSHelpApp theApp;