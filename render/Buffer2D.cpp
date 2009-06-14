/*
 *  Buffer2D.cpp
 *  
 *
 *  Created by jian zhang on 6/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
using namespace std;
#include "Buffer2D.h"

Buffer2D::Buffer2D():rPixels(0), gPixels(0), bPixels(0)
{
}

Buffer2D::~Buffer2D()
{
	if(rPixels) delete[] rPixels;
	if(gPixels) delete[] gPixels;
	if(bPixels) delete[] bPixels;
}

void Buffer2D::create(int _width, int _height)
{
	m_width = _width;
	m_height = _height;
	int size = m_width*m_height;
	rPixels = new half[size];
	gPixels = new half[size];
	bPixels = new half[size];
	
	for(int i=0; i<size; i++) rPixels[i] = gPixels[i] = bPixels[i] = half(.5f);
	/*for(int j=0; j<_height; j++)
		for(int i=0; i<_width; i++)
		{
			rPixels[j*_width+i] = (float)i/_width;
			gPixels[j*_width+i] = (float)j/_height;
			bPixels[j*_width+i] = 1.f;
		}*/
}

void Buffer2D::rasterize(const TRIANGLE2D& triangle)
{
	float f120 = barycentric_coord(triangle.s[1], triangle.t[1], triangle.s[2], triangle.t[2], triangle.s[0], triangle.t[0]);
	float f201 = barycentric_coord(triangle.s[2], triangle.t[2], triangle.s[0], triangle.t[0], triangle.s[1], triangle.t[1]);
	float f012 = barycentric_coord(triangle.s[0], triangle.t[0], triangle.s[1], triangle.t[1], triangle.s[2], triangle.t[2]);
	float s_max = 0;
	float t_max = 0;
	float s_min = 1;
	float t_min = 1;
	for(int i=0; i<3; i++)
	{
		if(triangle.s[i] > s_max) s_max = triangle.s[i];
		if(triangle.t[i] > t_max) t_max = triangle.t[i];
		if(triangle.s[i] < s_min) s_min = triangle.s[i];
		if(triangle.t[i] < t_min) t_min = triangle.t[i];
	}
	
	float delta = 1.f/m_width;
	s_min = int(s_min*m_width)*delta;
	t_min = int(t_min*m_width)*delta;
	
	int grid_x = (s_max-s_min)/delta + 1;
	int grid_y = (t_max-t_min)/delta + 1;
	
	float alpha, beta, gamma, x, y;
	int s, t;
	
	for(int j=0; j<=grid_y; j++)
	{
		for(int i=0; i<=grid_x; i++)
		{
			x = s_min + delta*(i+0.5);
			y = t_min + delta*(j+0.5);
			
			alpha = barycentric_coord(triangle.s[1], triangle.t[1], triangle.s[2], triangle.t[2], x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(triangle.s[2], triangle.t[2], triangle.s[0], triangle.t[0], x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(triangle.s[0], triangle.t[0], triangle.s[1], triangle.t[1], x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			s = s_min / delta + i;
			t = t_min / delta + j;
			
			bPixels[t*m_width+s] = half(1.f);
		}
	}
}

float Buffer2D::barycentric_coord(float ax, float ay, float bx, float by, float x, float y)
{
	return (ay - by)*x + (bx - ax)*y +ax*by - bx*ay;
}