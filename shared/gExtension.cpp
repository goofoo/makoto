#include <string.h>
#include <stdio.h>
#include "gExtension.h"

#ifdef WIN32
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
#endif

#if defined(WIN32)
#  include <windows.h>
#  define GET_EXT_POINTER(name, type) \
      name = (type)wglGetProcAddress(#name)

#elif defined(LINUX)
#  include <GL/glx.h>
#  define GET_EXT_POINTER(name, type) \
      name = (type)glXGetProcAddressARB((const GLubyte*)#name)
#endif

char init()
{
#ifdef WIN32
	GET_EXT_POINTER(glMultiTexCoord4fARB, PFNGLMULTITEXCOORD4FARBPROC);
#endif
	return 1;
}

const char* getExtension()
{
	return (const char*)glGetString( GL_EXTENSIONS );
}
//~:
