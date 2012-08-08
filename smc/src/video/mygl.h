#pragma once

#include "SDL.h"

#ifdef WIN32
#include <EGL/egl.h>
#endif

#include "SDL_opengles.h"

//#define USE_GL

#ifdef WIN32
#include "eglport.h"

void getScreenSize(int* width, int* height);
#endif