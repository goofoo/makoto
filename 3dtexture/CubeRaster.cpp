/*
 *  CubeRaster.cpp
 *  pmap
 *
 *  Created by jian zhang on 9/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#define CUBERASTER_BUFFER_H 6
#define CUBERASTER_BUFFER_N 12
#define CUBERASTER_BUFFER_SIZE 864
#define CUBERASTER_BUFFER_POSX 0
#define CUBERASTER_BUFFER_NEGX 144
#define CUBERASTER_BUFFER_POSY 288
#define CUBERASTER_BUFFER_NEGY 432
#define CUBERASTER_BUFFER_POSZ 576
#define CUBERASTER_BUFFER_NEGZ 720

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
/*
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

void CubeRaster::write(XYZ& ray, float val, float angle)
{
	if(ray.lengthSquare() < 0.001) return;
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
	float radius = 1+angle*CUBERASTER_MAXANGLEINV;
	int nsamp = radius;
	int nu, nv;
	float noi;
	for(int j=-nsamp; j<=nsamp; j++) {
		
		for(int i=-nsamp; i<=nsamp; i++) {
			//noi = float(random()%209)/209.f - 0.5f;
			nv = v + j;
			if(nv < 0 || nv >= CUBERASTER_BUFFER_N) continue;
			//
			//noi = float(random()%137)/137.f - 0.5f;
			nu = u + i;
			if(nu < 0 || nu >= CUBERASTER_BUFFER_N) continue;
			//
			
			m_pOpacity[offset + nv*CUBERASTER_BUFFER_N + nu] += val;
			
			nv = v + j -0.5;
			if(nv < 0 || nv >= CUBERASTER_BUFFER_N) continue;
			nu = u + i;
			if(nu < 0 || nu >= CUBERASTER_BUFFER_N) continue;
			
			m_pOpacity[offset + nv*CUBERASTER_BUFFER_N + nu] += val;
			
			nv = v + j;
			if(nv < 0 || nv >= CUBERASTER_BUFFER_N) continue;
			nu = u + i;
			if(nu < 0 || nu >= CUBERASTER_BUFFER_N) continue;
			
			m_pOpacity[offset + nv*CUBERASTER_BUFFER_N + nu] += val;
			
			nv = v + j;
			if(nv < 0 || nv >= CUBERASTER_BUFFER_N) continue;
			nu = u + i-0.5;
			if(nu < 0 || nu >= CUBERASTER_BUFFER_N) continue;
			
			m_pOpacity[offset + nv*CUBERASTER_BUFFER_N + nu] += val;
			
		}
	}
}
*/
void CubeRaster::write(XYZ& ray, float val)
{
	if(ray.lengthSquare() < 0.001) return;
	ray.normalize();
	float x = ray.x; if(x<0) x *= -1;
	float y = ray.y; if(y<0) y *= -1;
	float z = ray.z; if(z<0) z *= -1;
	unsigned offset;
	float u, v;
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
	int nu0, nv0, nu1, nv1;
	
	nu0 = u; nv0 = v;
	nu1 = u+1; nv1 = v+1;
	if(u-nu0 < 0.5) {
		nu0 = u-1;
		nu1 = u;
	}
	if(v-nv0 < 0.5) {
		nv0 = v-1;
		nv1 = v;
	}
	
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER_N) m_pOpacity[offset + nv0*CUBERASTER_BUFFER_N + nu0] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER_N) m_pOpacity[offset + nv0*CUBERASTER_BUFFER_N + nu1] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER_N) m_pOpacity[offset + nv1*CUBERASTER_BUFFER_N + nu1] += val;		
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER_N) m_pOpacity[offset + nv1*CUBERASTER_BUFFER_N + nu0] += val;
}

void CubeRaster::draw() const
{
	float x, y, z, a, l;
	unsigned offset;
	x = CUBERASTER_BUFFER_H;
	offset = CUBERASTER_BUFFER_POSX;
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {
			z = -CUBERASTER_BUFFER_H + i;
			y = -CUBERASTER_BUFFER_H + j;
			
			a = m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i];
			
			if(a > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			getLight(a, l);
			
			glColor3f(l, 1-l, 0);
			
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
			
			a = m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i];
			if(a > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			getLight(a, l);
			
			glColor3f(l, 1-l, 0);
			
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
			
			a = m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i];
			if(a > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			getLight(a, l);
			
			glColor3f(l, 1-l, 0);
			
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
			
			a = m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i];
			if(a > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			getLight(a, l);
			
			glColor3f(l, 1-l, 0);
			
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
			
			a = m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i];
			if(a > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			getLight(a, l);
			
			glColor3f(l, 1-l, 0);
			
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
			
			a = m_pOpacity[offset + j*CUBERASTER_BUFFER_N + i];
			if(a > 0) glBegin(GL_QUADS);
			else glBegin(GL_LINE_LOOP);
			
			getLight(a, l);
			
			glColor3f(l, 1-l, 0);
			
			glVertex3f(x, y, z);
			glVertex3f(x+1, y, z);
			glVertex3f(x+1, y+1, z);
			glVertex3f(x, y+1, z);
			glEnd();
		}
	}
}

void CubeRaster::getLight(const float& opacity, float& lighting) const
{
	lighting = 1.f - opacity;
	if(lighting < 0) lighting = 0;
}

void CubeRaster::readLight(XYZ& ray, float& lighting) const
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
	/*
	int nu0, nv0, nu1, nv1;
	
	nu0 = u; nv0 = v;
	nu1 = u+1; nv1 = v+1;
	if(u-nu0 < 0.5) {
		nu0 = u-1;
		nu1 = u;
	}
	if(v-nv0 < 0.5) {
		nv0 = v-1;
		nv1 = v;
	}
	*/
	getLight(m_pOpacity[offset + v*CUBERASTER_BUFFER_N + u], lighting);
}
