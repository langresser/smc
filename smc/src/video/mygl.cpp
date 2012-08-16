#include "mygl.h"

#ifdef WIN32
void initDir()
{
}

void getScreenSize(int* width, int* height)
{
	if (width) {
		*width = 1024;
	}

	if (height) {
		*height = 768;
	}
}

void showJoystick()
{}

void closeAds()
{}

void hideJoystick()
{}

void showAds()
{}
#endif

#ifdef WIN32
#ifndef USE_EGL
#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else
#pragma comment(lib, "libGLESv1_CM.lib");
#pragma comment(lib, "libEGL.lib");
#endif
#endif
