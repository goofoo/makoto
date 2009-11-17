#include <stdio.h>
#include "zFrameBuffer.h"

#define CHECK_FRAMEBUFFER_STATUS()                            \
      {                                                           \
        GLenum status;                                            \
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
        switch(status) {                                          \
          case GL_FRAMEBUFFER_COMPLETE_EXT:                       \
            break;                                                \
          case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                    \
			printf("ERROR: Cannot create Framebuffer!");\
            break;                                                \
          default:                                                \
			printf("ERROR: Programming error!");\
            break;                                            \
        }\
      }\

zFrameBuffer::zFrameBuffer(int cx, int cy)
{	
	m_frameWidth = cx;
	m_frameHeight = cy;

// create the frame-buffer(color) and render-buffer(depth) object
	glGenFramebuffersEXT( 1, &m_frameBuffer );
	glGenRenderbuffersEXT( 1, &m_depthRenderBuffer );
	
// initialize the render-buffer
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_frameWidth, m_frameHeight );
	
	CHECK_FRAMEBUFFER_STATUS()
}

zFrameBuffer::~zFrameBuffer(void)
{
	glDeleteFramebuffersEXT( 1, &m_frameBuffer );
	glDeleteRenderbuffersEXT( 1, &m_depthRenderBuffer );
}

void zFrameBuffer::begin(GLuint i_image, GLenum target)
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_frameBuffer );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, i_image, 0 );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthRenderBuffer );
	CHECK_FRAMEBUFFER_STATUS()
}

void zFrameBuffer::end()
{	
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

void zFrameBuffer::readRGBA(float* data)
{
	glReadPixels( 0, 0, m_frameWidth, m_frameHeight, GL_RGBA, GL_FLOAT, data );
}

void zFrameBuffer::readRGBA(float* data, int wnd_w, int wnd_h)
{
	glReadPixels( 0, 0, wnd_w, wnd_h, GL_RGBA, GL_FLOAT, data );
}

void zFrameBuffer::readRGBA(float* data, int wnd_w, int wnd_h, int crop_x,  int crop_y, int crop_width, int crop_height)
{
	if(wnd_w <= m_frameWidth && wnd_h <= m_frameHeight)
		glReadPixels( 0, 0, wnd_w, wnd_h, GL_RGBA, GL_FLOAT, data );
	else
	{
		float* tmp = new float[crop_width*crop_height*4];
		glReadPixels( 0, 0, crop_width, crop_height, GL_RGBA, GL_FLOAT, tmp);
		for(int j=0; j<crop_height; j++)
		{
			for(int i=0; i<crop_width; i++)
			{
				data[((crop_y+j)*wnd_w+(crop_x+i))*4]   = tmp[(j*crop_width+i)*4];
				data[((crop_y+j)*wnd_w+(crop_x+i))*4+1] = tmp[(j*crop_width+i)*4+1];
				data[((crop_y+j)*wnd_w+(crop_x+i))*4+2] = tmp[(j*crop_width+i)*4+2];
				data[((crop_y+j)*wnd_w+(crop_x+i))*4+3] = tmp[(j*crop_width+i)*4+3];
			}
		}
		delete[] tmp;
	}
}

void zFrameBuffer::readR(float* data)
{
	glReadPixels( 0, 0, m_frameWidth, m_frameHeight, GL_RED, GL_FLOAT, data );
}

void zFrameBuffer::readAlpha(float* data)
{
	glReadPixels( 0, 0, m_frameWidth, m_frameHeight, GL_ALPHA, GL_FLOAT, data );
}
/*
void zFrameBuffer::genTexture(GLuint& tex, GLenum target, GLint internalformat, GLenum format)
{
	if(tex>0) glDeleteTextures(1, &tex);
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexImage2D(target, 0, internalformat, m_frameWidth, m_frameHeight, 0, format, GL_FLOAT, 0);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
*/
void zFrameBuffer::genTexture(GLuint& tex, GLenum target, GLint internalformat, GLenum format, float* data)
{
	if(tex>0) glDeleteTextures(1, &tex);
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexImage2D(target, 0, internalformat, m_frameWidth, m_frameHeight, 0, format, GL_FLOAT, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void zFrameBuffer::genSwapTexture(GLuint& tex, GLenum target, GLint internalformat, GLenum format)
{
// delete existing texture first
	if(tex>0) glDeleteTextures(1, &tex); 
	
	float* tmp = new float[m_frameWidth*m_frameHeight*4];
	glReadPixels( 0, 0, m_frameWidth, m_frameHeight, format, GL_FLOAT, tmp);
	glGenTextures(1, &tex);
	
	glBindTexture(target, tex);
	glTexImage2D(target, 0, internalformat, m_frameWidth, m_frameHeight, 0, format, GL_FLOAT, tmp);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	delete[] tmp;
}

void zFrameBuffer::setOrtho()
{
	glViewport(0, 0, m_frameWidth, m_frameHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_frameWidth/2, m_frameWidth/2, -m_frameHeight/2, m_frameHeight/2, .1, 10.);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(m_frameWidth/2, m_frameHeight/2, 1,
			  m_frameWidth/2, m_frameHeight/2, -1,
			  0, 1, 0);
}
//:~
