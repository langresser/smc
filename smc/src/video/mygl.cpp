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