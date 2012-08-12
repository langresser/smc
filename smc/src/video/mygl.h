#pragma once

#include "SDL.h"

#ifdef WIN32
#include <EGL/egl.h>
#endif

//#include "SDL_opengl.h"
#include "SDL_opengles.h"

#ifdef WIN32
#include "eglport.h"

void getScreenSize(int* width, int* height);
#endif

//#define USE_EGL
//#define USE_OPENGL_CEGUI
#ifdef WIN32
#ifndef USE_EGL
#define glOrthof glOrtho
#define glClearDepthf glClearDepth
#endif
#endif