#ifndef OPENGL_EXTENSION_H
#define OPENGL_EXTENSION_H

#ifdef WIN32
# include <windows.h>
#endif

#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
#else
# include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifdef LINUX
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glext.h>
#endif

#ifdef LINUX
extern "C" {
extern void glActiveTexture(GLenum);
extern void glClientActiveTexture(GLenum);
extern void glMultiTexCoord2f( GLenum, GLfloat, GLfloat );	
extern void glMultiTexCoord3f( GLenum, GLfloat, GLfloat, GLfloat );
extern void glMultiTexCoord4f( GLenum, GLfloat, GLfloat, GLfloat, GLfloat );
}
#endif

#ifdef WIN32
extern PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB;
#endif

char init();
const char* getExtension();

#endif
//~:
