#pragma once
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
#include "iosUtil.h"
#else
#define snprintf _snprintf
#endif

void initDir();
FILE* open_file(const char* file, const char* mode);
