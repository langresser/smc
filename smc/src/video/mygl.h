#pragma once

#include "SDL.h"

#ifdef WIN32
#include <EGL/egl.h>
#endif

#include "SDL_opengl.h"
//#include "SDL_opengles.h"

#ifdef WIN32
#include "eglport.h"
//#define USE_EGL
void getScreenSize(int* width, int* height);
void showJoystick();
void closeAds();

void hideJoystick();
void showAds();
#endif

#ifdef WIN32
#ifndef USE_EGL
#define USE_OPENGL_CEGUI
#define glOrthof glOrtho
#define glClearDepthf glClearDepth
#endif
#endif