// Target and Format Options
// when __APPLE__ is defined
// GL_TEXTURE_RECTANGLE_ARB GL_RGBA_FLOAT16_APPLE
// or
// GL_TEXTURE_RECTANGLE_NV GL_FLOAT_RGBA16_NV
// when float-point frame buffer object doesn't support GL blending
// GL_TEXTURE_2D GL_RGBA

#ifndef Z_DIBITMAP_H
#define Z_DIBITMAP_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../shared/glext.h"
#endif


class zFrameBuffer
{
public:
	zFrameBuffer(int cx, int cy);
	~zFrameBuffer(void);
	
	void begin(GLuint i_image, GLenum target);
	void readRGBA(float* data);
	void readRGBA(float* data, int wnd_w, int wnd_h);
	void readRGBA(float* data, int wnd_w, int wnd_h, int crop_x,  int crop_y, int crop_width, int crop_height);
	void readR(float* data);
	void readAlpha(float* data);
	void end();
	
	//void genTexture(GLuint& tex, GLenum target, GLint internalformat, GLenum format);
	void genTexture(GLuint& tex, GLenum target, GLint internalformat, GLenum format, float* data);
	void genSwapTexture(GLuint& tex, GLenum target, GLint internalformat, GLenum format);
	void setOrtho();
	
protected:
	int m_frameWidth, m_frameHeight;

	// frame buffer id
	GLuint m_frameBuffer;
	GLuint m_depthRenderBuffer;
};
#endif
