//��־����ͷ�ļ�
#pragma once
#include <cstdarg>
#include <string>
using std::string;
//////////////////////////////////////////////////////////////////////////
//
// CLogger -- ��������־����
//
//////////////////////////////////////////////////////////////////////////

class CLogger
{
public:
	// Constructor.
	CLogger(void);
public:
	// Deconstructor.
	~CLogger(void);
public:
	// printf���ͽӿڵ���־����
	static void Log(char *format, ...);
public:
	// ��־�ļ�
	static string s_logFile;
};
