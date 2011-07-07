//日志功能头文件
#pragma once
#include <cstdarg>
#include <string>
using std::string;
//////////////////////////////////////////////////////////////////////////
//
// CLogger -- 用来做日志的类
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
	// printf类型接口的日志函数
	static void Log(char *format, ...);
public:
	// 日志文件
	static string s_logFile;
};
