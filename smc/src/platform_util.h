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
