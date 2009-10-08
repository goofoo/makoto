#include <string.h>
#include <stdio.h>
#include "gExtension.h"

PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

#ifdef WIN32

//PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
#endif

#if defined(WIN32)
#  include <windows.h>
#  define GET_EXT_POINTER(name, type) \
      name = (type)wglGetProcAddress(#name)

#elif defined(LINUX)
#  include <GL/glx.h>
#  define GET_EXT_POINTER(name, type) \
      name = (type)glXGetProcAddressARB((const GLubyte*) #name)
#endif

char gExtensionInit()
{
	GET_EXT_POINTER(glIsRenderbufferEXT, PFNGLISRENDERBUFFEREXTPROC);
	GET_EXT_POINTER(glBindRenderbufferEXT, PFNGLBINDRENDERBUFFEREXTPROC);
	GET_EXT_POINTER(glDeleteRenderbuffersEXT, PFNGLDELETERENDERBUFFERSEXTPROC);
	GET_EXT_POINTER(glGenRenderbuffersEXT, PFNGLGENRENDERBUFFERSEXTPROC);
	GET_EXT_POINTER(glRenderbufferStorageEXT, PFNGLRENDERBUFFERSTORAGEEXTPROC);
	GET_EXT_POINTER(glGetRenderbufferParameterivEXT, PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC);
	GET_EXT_POINTER(glIsFramebufferEXT, PFNGLISFRAMEBUFFEREXTPROC);
	GET_EXT_POINTER(glBindFramebufferEXT, PFNGLBINDFRAMEBUFFEREXTPROC);
	GET_EXT_POINTER(glDeleteFramebuffersEXT, PFNGLDELETEFRAMEBUFFERSEXTPROC);
	GET_EXT_POINTER(glGenFramebuffersEXT, PFNGLGENFRAMEBUFFERSEXTPROC);
	GET_EXT_POINTER(glCheckFramebufferStatusEXT, PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC);
	GET_EXT_POINTER(glFramebufferTexture1DEXT, PFNGLFRAMEBUFFERTEXTURE1DEXTPROC);
	GET_EXT_POINTER(glFramebufferTexture2DEXT, PFNGLFRAMEBUFFERTEXTURE2DEXTPROC);
	GET_EXT_POINTER(glFramebufferTexture3DEXT, PFNGLFRAMEBUFFERTEXTURE3DEXTPROC);
	GET_EXT_POINTER(glFramebufferRenderbufferEXT, PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC);
	GET_EXT_POINTER(glGetFramebufferAttachmentParameterivEXT, PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC);
	GET_EXT_POINTER(glGenerateMipmapEXT, PFNGLGENERATEMIPMAPEXTPROC);
	
	if( !glIsRenderbufferEXT || !glBindRenderbufferEXT || !glDeleteRenderbuffersEXT || 
	!glGenRenderbuffersEXT || !glRenderbufferStorageEXT || !glGetRenderbufferParameterivEXT || 
	!glIsFramebufferEXT || !glBindFramebufferEXT || !glDeleteFramebuffersEXT || 
	!glGenFramebuffersEXT || !glCheckFramebufferStatusEXT || !glFramebufferTexture1DEXT || 
	!glFramebufferTexture2DEXT || !glFramebufferTexture3DEXT || !glFramebufferRenderbufferEXT||  
	!glGetFramebufferAttachmentParameterivEXT || !glGenerateMipmapEXT ) {
		printf( "ERROR: One or more EXT_framebuffer_object functions were not found");
		return 0;
	}
#ifdef WIN32
	
	//GET_EXT_POINTER(glMultiTexCoord4fARB, PFNGLMULTITEXCOORD4FARBPROC);
#endif
	return 1;
}

const char* getExtension()
{
	return (const char*)glGetString( GL_EXTENSIONS );
}
//~:
