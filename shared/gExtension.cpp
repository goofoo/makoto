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

PFNGLACTIVETEXTUREARBPROC		glActiveTexture = NULL;
PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1i = NULL;
PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2i = NULL;
PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3i = NULL;
PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4i = NULL;
PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1f = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4f = NULL;

PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;

PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;

PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC glUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC glUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC glUniform4iARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB = NULL;

PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB  = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;

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

	GET_EXT_POINTER(glActiveTexture, PFNGLACTIVETEXTUREPROC);
	GET_EXT_POINTER(glMultiTexCoord1i, PFNGLMULTITEXCOORD1IPROC);
	GET_EXT_POINTER(glMultiTexCoord2i, PFNGLMULTITEXCOORD2IPROC);
	GET_EXT_POINTER(glMultiTexCoord3i, PFNGLMULTITEXCOORD3IPROC);
	GET_EXT_POINTER(glMultiTexCoord4i, PFNGLMULTITEXCOORD4IPROC);
	GET_EXT_POINTER(glMultiTexCoord1f, PFNGLMULTITEXCOORD1FPROC);
	GET_EXT_POINTER(glMultiTexCoord2f, PFNGLMULTITEXCOORD2FPROC);
	GET_EXT_POINTER(glMultiTexCoord3f, PFNGLMULTITEXCOORD3FPROC);
	GET_EXT_POINTER(glMultiTexCoord4f, PFNGLMULTITEXCOORD4FPROC);
	
	if(!glActiveTexture || !glMultiTexCoord1i || !glMultiTexCoord2i ||
		!glMultiTexCoord3i || !glMultiTexCoord4i || !glMultiTexCoord1f ||
		!glMultiTexCoord2f || !glMultiTexCoord3f || !glMultiTexCoord4f) {
		printf( "ERROR: One or more texture functions were not found");
		return 0;
	} 
	
	GET_EXT_POINTER(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
	if(!glBlendFuncSeparate) {
		printf( "ERROR: blend separate functions were not found");
		return 0;
	} 
	
	GET_EXT_POINTER(glTexImage3D, PFNGLTEXIMAGE3DPROC);
	if(!glTexImage3D) {
		printf( "ERROR: texture 3d functions were not found");
		return 0;
	}
	
	GET_EXT_POINTER(glUniform1iARB, PFNGLUNIFORM1IARBPROC);
	GET_EXT_POINTER(glUniform2iARB, PFNGLUNIFORM2IARBPROC);
	GET_EXT_POINTER(glUniform3iARB, PFNGLUNIFORM3IARBPROC);
	GET_EXT_POINTER(glUniform4iARB, PFNGLUNIFORM4IARBPROC);
	GET_EXT_POINTER(glUniform1fARB, PFNGLUNIFORM1FARBPROC);
	GET_EXT_POINTER(glUniform2fARB, PFNGLUNIFORM2FARBPROC);
	GET_EXT_POINTER(glUniform3fARB, PFNGLUNIFORM3FARBPROC);
	GET_EXT_POINTER(glUniform4fARB, PFNGLUNIFORM4FARBPROC);
	GET_EXT_POINTER(glUniformMatrix4fvARB, PFNGLUNIFORMMATRIX4FVARBPROC);
	
	if(!glUniform1iARB || !glUniform2iARB || !glUniform3iARB || !glUniform4iARB ||
		!glUniform1fARB || !glUniform2fARB || !glUniform3fARB || !glUniform4fARB ||
		!glUniformMatrix4fvARB) {
		printf( "ERROR: shader param functions were not found");
		return 0;
	}
	
	GET_EXT_POINTER(glDeleteObjectARB, PFNGLDELETEOBJECTARBPROC);
	GET_EXT_POINTER(glUseProgramObjectARB, PFNGLUSEPROGRAMOBJECTARBPROC);
	GET_EXT_POINTER(glCreateShaderObjectARB, PFNGLCREATESHADEROBJECTARBPROC);
	GET_EXT_POINTER(glShaderSourceARB, PFNGLSHADERSOURCEARBPROC);
	GET_EXT_POINTER(glLinkProgramARB, PFNGLLINKPROGRAMARBPROC);
	GET_EXT_POINTER(glCreateProgramObjectARB, PFNGLCREATEPROGRAMOBJECTARBPROC);
	GET_EXT_POINTER(glAttachObjectARB, PFNGLATTACHOBJECTARBPROC);
	GET_EXT_POINTER(glCompileShaderARB, PFNGLCOMPILESHADERARBPROC);
	GET_EXT_POINTER(glGetObjectParameterivARB, PFNGLGETOBJECTPARAMETERIVARBPROC);
	GET_EXT_POINTER(glGetUniformLocationARB, PFNGLGETUNIFORMLOCATIONARBPROC);
	
	if(!glDeleteObjectARB || !glUseProgramObjectARB || !glCreateShaderObjectARB ||
		!glShaderSourceARB || !glLinkProgramARB || !glCreateProgramObjectARB ||
		!glAttachObjectARB || !glCompileShaderARB || !glGetObjectParameterivARB ||
		!glGetUniformLocationARB) {
		printf( "ERROR: shader functions were not found");
		return 0;
	}
	
	return 1;
}

const char* getExtension()
{
	return (const char*)glGetString( GL_EXTENSIONS );
}

char gCheckExtension(char* extName)
{
	char *p = (char *) glGetString(GL_EXTENSIONS);
    char *end;
    int extNameLen;

    extNameLen = strlen(extName);
    end = p + strlen(p);
    
    while (p < end) {
         int n = strcspn(p, " ");
         if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
                return 1;
            }
            p += (n + 1);
        }
    return 0;
}
//~:
