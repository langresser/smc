#include "mygl.h"

#ifdef WIN32
void getScreenSize(int* width, int* height)
{
	if (width) {
		*width = 960;
	}

	if (height) {
		*height = 640;
	}
}
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
