#ifndef _ZGLEXTENSION_H
#define _ZGLEXTENSION_H

#ifdef _WIN32
#include<windows.h>
#endif
#pragma once
#include<stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"

#ifdef _WIN32

extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
extern PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
extern PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
extern PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
extern PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;

extern PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT = NULL;
extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
extern PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
extern PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
extern PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
extern PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

//extern PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB= NULL;

int initGLExt()
{
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
	glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) wglGetProcAddress("glGetUniformLocationARB");
	glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) wglGetProcAddress("glGetInfoLogARB");
	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) wglGetProcAddress("glCompileShaderARB");
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) wglGetProcAddress("glCreateProgramObjectARB");
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) wglGetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) wglGetProcAddress("glUseProgramObjectARB");
	glUniform1iARB = (PFNGLUNIFORM1IARBPROC) wglGetProcAddress("glUniform1iARB");
	glUniform1fARB = (PFNGLUNIFORM1FARBPROC) wglGetProcAddress("glUniform1fARB");
	glUniform3fARB = (PFNGLUNIFORM3FARBPROC) wglGetProcAddress("glUniform3fARB");

		
	glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");
	
	//glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)wglGetProcAddress("glMultiTexCoord2iARB");
	
	if( !glActiveTextureARB || !glTexImage3D || !glGetUniformLocationARB || !glGetInfoLogARB || !glCreateShaderObjectARB ||
		!glShaderSourceARB || !glCompileShaderARB || !glCreateProgramObjectARB || !glAttachObjectARB || !glLinkProgramARB ||
		!glUseProgramObjectARB || !glUniform1iARB || !glUniform1fARB || !glUniform3fARB || 
		!glIsRenderbufferEXT || !glBindRenderbufferEXT || !glDeleteRenderbuffersEXT || 
			!glGenRenderbuffersEXT || !glRenderbufferStorageEXT || !glGetRenderbufferParameterivEXT || 
			!glIsFramebufferEXT || !glBindFramebufferEXT || !glDeleteFramebuffersEXT || 
			!glGenFramebuffersEXT || !glCheckFramebufferStatusEXT || !glFramebufferTexture1DEXT || 
			!glFramebufferTexture2DEXT || !glFramebufferTexture3DEXT || !glFramebufferRenderbufferEXT||  
			!glGetFramebufferAttachmentParameterivEXT || !glGenerateMipmapEXT)
			//!glMultiTexCoord2iARB )
	{
		MessageBox(NULL,"One or more EXT_framebuffer_object functions were not found",
			"ERROR",MB_OK|MB_ICONEXCLAMATION);
		exit(-1);
	}

	return 1;	
}



#endif

#endif