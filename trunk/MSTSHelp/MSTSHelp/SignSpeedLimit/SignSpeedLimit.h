// SignSpeedLimit.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSignSpeedLimitApp:
// �йش����ʵ�֣������ SignSpeedLimit.cpp
//

class CSignSpeedLimitApp : public CWinApp
{
public:
	CSignSpeedLimitApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSignSpeedLimitApp theApp;