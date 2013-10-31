//日志文件实现文件
#include "StdAfx.h"
#include "Logger.h"

CLogger::CLogger(void)
{
}

CLogger::~CLogger(void)
{
}

string CLogger::s_logFile;

void CLogger::Log(char *format, ...)
{
#ifdef PRINT_LOG
	FILE *fp;

	if (s_logFile.empty())
	{
		s_logFile = "logfile.txt";
		errno_t err = fopen_s(&fp, s_logFile.c_str(), "w");
	}
	else
	{
		errno_t err = fopen_s(&fp, s_logFile.c_str(), "a");
	}

	va_list ap;
	va_start(ap, format);
	vfprintf(fp, format, ap);
	va_end(ap);
	time_t t2;
	time(&t2);
	char buffer[100];
	ctime_s(buffer, 100, &t2);
	fprintf(fp, " %s", buffer);
	fclose(fp);
#endif
}
