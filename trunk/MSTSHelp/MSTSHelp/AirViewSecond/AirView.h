// AirView.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAirViewApp:
// �йش����ʵ�֣������ AirView.cpp
//

class CAirViewApp : public CWinApp
{
public:
	CAirViewApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAirViewApp theApp;