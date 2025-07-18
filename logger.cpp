﻿#include <stdarg.h>
#include <stdlib.h>
#include "predef.h"
#include "logger.h"
//#include "network.h"
#include "utils.h"

void Logger::log(const char* format, ...)
{
	bool consolelog_enable = true;
	//bool filelog_enable = (FILE_LOG & logtype) == 0 ? false : true;		// define으로 제어
	bool filelog_enable = false;

	char buf[2048] = { 0, };
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);

	std::string ds = Utils::getCurrentDateTime();
	ds = ds + buf + '\n';

	if (filelog_enable)
	{
		FILE* fp = fopen("log.txt", "at");
		if (fp == NULL) return;
		fputs(ds.c_str(), fp);
		fclose(fp);
	}

	if (consolelog_enable)
		printf(ds.c_str());
}

void Logger::log(int camnum, const char* format, ...)
{
	bool consolelog_enable = true;
	//bool filelog_enable = (FILE_LOG & logtype) == 0 ? false : true;		// define으로 제어
	bool filelog_enable = false;

	char buf[2048] = { 0, };
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);

	std::string ds = Utils::getCurrentDateTime();

	if(camnum > -1)
		ds = Utils::format_string("Camara[%d] ", camnum) + ds + buf + '\n';
	else
		ds = ds + buf + '\n';

	if (filelog_enable)
	{
		FILE * fp = fopen("log.txt", "at");
		if (fp == NULL) return;
		fputs(ds.c_str(), fp);
		fclose(fp);
	}

	if(consolelog_enable)
		printf(ds.c_str());

#ifdef TESTBUILD
	if (Network::getinstance()->getenable())
	{
#define CLIENT_LOG_INFO		0x0a
#define CLIENT_LOG_WARN		0x0b
#define CLIENT_LOG_ERR		0x0c

		char packet = CLIENT_LOG_INFO;
		if(logtype == LOG_INFO)
			packet = CLIENT_LOG_INFO;
		else if (logtype == LOG_WARN)
			packet = CLIENT_LOG_WARN;
		else if (logtype == LOG_ERR)
			packet = CLIENT_LOG_ERR;

		Network::getinstance()->write(packet, (char*)ds.c_str(), ds.size());
	}
#endif
}