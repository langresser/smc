#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>

#ifndef WIN32
#include "iosUtil.h"
#else
#define snprintf _snprintf
#endif

void initDir();
std::string getFullPath(const char* fileName);
FILE* open_file(const char* file, const char* mode);

unsigned int TimeGet();

class TimeLogger
{
public:
	TimeLogger(const char* name)
	{
		m_name = name;
		m_startTime = TimeGet();
	}
	~TimeLogger()
	{
		unsigned int costTime = TimeGet() - m_startTime;
		printf("%s  cost   %dms\n", m_name.c_str(), costTime);
	}
private:
	unsigned int m_startTime;
	std::string m_name;
};



#define ENABLE_PROFILE

#ifdef ENABLE_PROFILE
#define LOGTIME(r) TimeLogger timeLogger(r)
#define LOGTIME2(r, x) TimeLogger timeLogger#x(r)
#else
#define LOGTIME(r)
#define LOGTIME2(r, x)
#endif