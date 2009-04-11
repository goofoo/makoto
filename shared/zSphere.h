/*
 *  zSphere.h
 *  runtime_ogl_vertex_fragment
 *
 *  Created by zhang on 07-3-11.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _Z_SPHERE_H
#define _Z_SPHERE_H

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "zData.h"

struct sphereSample
{
	XYZ pos, col;
	XY texcoord;
};

class zSphere
{
public:
	zSphere::zSphere(float radius, int gridX, int gridY);
	zSphere::~zSphere();
	void draw();
	void drawUV();
	void draw(float x, float y, float z);
	void setColorAt(int& x, int& y, float& r, float& g, float& b);
	void getUVAt(int& x, int& y, float& u, float& v);
private:
	//float* m_pVertex;
	//float* m_pTexcoord;
	int m_grid_x, m_grid_y, m_count;
	float m_radius;
	sphereSample* m_samples;
	
	void param(float u, float v, float& x, float& y, float& z);
};
#endif