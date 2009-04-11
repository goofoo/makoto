/*
 *  gBase.h
 *  pcfViewer
 *
 *  Created by zhang on 07-9-29.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef OPENGL_BASE_H
#define OPENGL_BASE_H

#ifdef WIN32
# include <windows.h>
#endif
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#include "./glext.h"
#endif

#include "zData.h"

class gBase
{
public :
	static int initExt();
	static const char* checkEXT(const char* name);
	static void drawLocator(const XYZ& origin);
	static void drawCoordinate();
	static void drawTex2DCoordinate();
	static void drawBoundingBox(XYZ* bbox);
	static void drawSplat24(qnode& data);
	static void drawSplat12(qnode& data, float scale);
	static void drawSplat121(qnode& data, float scale);
	static void drawSplat12(qnode& data, XYZ& eye, float scale);
	static void drawSplat12(float px, float py, float pz, float nx, float ny, float nz, float radius);
	static void drawSplatAt(XYZ& pos, XYZ& facing, float radius);
	static void drawQuadAt(XYZ& pos, XYZ& facing, float size);
	static void drawSplatST(float s, float t, float radius, float r, float g, float b);
	static void drawQuad(float x, float y, float z, float w, float t);
	static void texCoord4f(GLenum target, float x, float y, float z, float w);
	static void genTexture(GLuint& tex, GLenum target, int width, int height, GLint internalformat, GLenum format, GLenum type);
	static void genTexture(GLuint& tex, GLenum target, int width, int height, GLint internalformat, GLenum format, GLenum type, const float* data);
	static void checkOcclusionQuerySupported();
	static void genQuery(GLuint n, GLuint* ids);
	static void deleteQuery(GLuint n, GLuint* ids);
	static void drawTextureQuad(GLuint width, GLint height, const float* data, float exposure);
	static void drawTextureSphere(GLuint width, GLint height, const float* data, float exposure, XYZ* coord);
	static void getSphCoord(GLuint width, GLint height, XYZ* data);
	static void sph2vec(float u, float v, float& x, float& y, float& z);
	static void drawQuadArray(int width, int height, float exposure, float* data);
	static void drawPointArray(int length, float exposure, float* data);
	static void drawSprite(XYZ& center, float width, MATRIX44F& space);
	static void drawBox(const float xmin, const float xmax, const float ymin, const float ymax, const float zmin, const float zmax);
	
};
#endif
//~: