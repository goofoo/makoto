/*
 *  CubeRaster.cpp
 *  pmap
 *
 *  Created by jian zhang on 9/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#define CUBERASTER_BUFFER_H 5
#define CUBERASTER_BUFFER_N 10
#define CUBERASTER_BUFFER_SIZE 600
#define CUBERASTER_BUFFER_POSX 0
#define CUBERASTER_BUFFER_NEGX 100
#define CUBERASTER_BUFFER_POSY 200
#define CUBERASTER_BUFFER_NEGY 300
#define CUBERASTER_BUFFER_POSZ 400
#define CUBERASTER_BUFFER_NEGZ 500

#include "CubeRaster.h"
#include "../shared/gBase.h"

CubeRaster::CubeRaster()
{
	m_pOpacity = new float[CUBERASTER_BUFFER_SIZE];
}

CubeRaster::~CubeRaster()
{
	delete[] m_pOpacity;
}

void CubeRaster::clear()
{
	for(unsigned i=0; i<CUBERASTER_BUFFER_SIZE; i++) m_pOpacity[i] = 0;
}

void CubeRaster::write(XYZ& ray)
{
	ray.normalize();
	float x = ray.x; if(x<0) x *= -1;
	float y = ray.y; if(y<0) y *= -1;
	float z = ray.z; if(z<0) z *= -1;
	unsigned offset, u, v;
	if(x >y && x>z) {
		if(ray.x > 0) offset = CUBERASTER_BUFFER_POSX;
		else offset = CUBERASTER_BUFFER_NEGX;
		
		u = (ray.z/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER_N;
		v = (ray.y/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER_N;

	}
	else if(y>x && y>z) {
		if(ray.y > 0) offset = CUBERASTER_BUFFER_POSY;
		else offset = CUBERASTER_BUFFER_NEGY;
		
		u = (ray.x/y + 1 - 0.0000001)*0.5*CUBERASTER_BUFFER_N;
		v = (ray.z/y + 1- 0.0000001)*0.5*CUBERASTER_BUFFER_N;
	}
	else {
		if(ray.z > 0) offset = CUBERASTER_BUFFER_POSZ;
		else offset = CUBERASTER_BUFFER_NEGZ;
		
		u = (ray.x/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER_N;
		v = (ray.y/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER_N;
	}
	m_pOpacity[offset + v*CUBERASTER_BUFFER_N + u] = 1;
}

void CubeRaster::draw() const
{
	float x, y, z;
	unsigned offset;
	x = CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_POSX;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			z = -CUBERASTER_BUFFER_H + i;
			y = -CUBERASTER_BUFFER_H + j;
			
			if(m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i] > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			glVertex3f(x, y, z);
			glVertex3f(x, y, z+1);
			glVertex3f(x, y+1, z+1);
			glVertex3f(x, y+1, z);
			glEnd();
		}
	}
	x = -CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_NEGX;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			z = -CUBERASTER_BUFFER_H + i;
			y = -CUBERASTER_BUFFER_H + j;
			
			if(m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i] > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			glVertex3f(x, y, z);
			glVertex3f(x, y, z+1);
			glVertex3f(x, y+1, z+1);
			glVertex3f(x, y+1, z);
			glEnd();
		}
	}
	y = CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_POSY;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			x = -CUBERASTER_BUFFER_H + i;
			z = -CUBERASTER_BUFFER_H + j;
			
			if(m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i] > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			glVertex3f(x, y, z);
			glVertex3f(x+1, y, z);
			glVertex3f(x+1, y, z+1);
			glVertex3f(x, y, z+1);
			glEnd();
		}
	}
	y = -CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_NEGY;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			x = -CUBERASTER_BUFFER_H + i;
			z = -CUBERASTER_BUFFER_H + j;
			
			if(m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i] > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			glVertex3f(x, y, z);
			glVertex3f(x+1, y, z);
			glVertex3f(x+1, y, z+1);
			glVertex3f(x, y, z+1);
			glEnd();
		}
	}
	z = CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_POSZ;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			x = -CUBERASTER_BUFFER_H + i;
			y = -CUBERASTER_BUFFER_H + j;
			
			if(m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i] > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			glVertex3f(x, y, z);
			glVertex3f(x+1, y, z);
			glVertex3f(x+1, y+1, z);
			glVertex3f(x, y+1, z);
			glEnd();
		}
	}
	z = -CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_NEGZ;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			x = -CUBERASTER_BUFFER_H + i;
			y = -CUBERASTER_BUFFER_H + j;
			
			if(m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i] > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			glVertex3f(x, y, z);
			glVertex3f(x+1, y, z);
			glVertex3f(x+1, y+1, z);
			glVertex3f(x, y+1, z);
			glEnd();
		}
	}
}