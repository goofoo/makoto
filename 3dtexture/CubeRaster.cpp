/*
 *  CubeRaster.cpp
 *  pmap
 *
 *  Created by jian zhang on 9/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#define CUBERASTER_BUFFER_H 8
#define CUBERASTER_BUFFER_N 16
#define CUBERASTER_BUFFER_SIZE 1536
#define CUBERASTER_BUFFER_POSX 0
#define CUBERASTER_BUFFER_NEGX 256
#define CUBERASTER_BUFFER_POSY 512
#define CUBERASTER_BUFFER_NEGY 768
#define CUBERASTER_BUFFER_POSZ 1024
#define CUBERASTER_BUFFER_NEGZ 1280

#define CUBERASTER_BUFFER8_H 4
#define CUBERASTER_BUFFER8_N 8
#define CUBERASTER_BUFFER8_SIZE 384
#define CUBERASTER_BUFFER8_POSX 0
#define CUBERASTER_BUFFER8_NEGX 64
#define CUBERASTER_BUFFER8_POSY 128
#define CUBERASTER_BUFFER8_NEGY 192
#define CUBERASTER_BUFFER8_POSZ 256
#define CUBERASTER_BUFFER8_NEGZ 320

#define CUBERASTER_BUFFER4_H 2
#define CUBERASTER_BUFFER4_N 4
#define CUBERASTER_BUFFER4_SIZE 96
#define CUBERASTER_BUFFER4_POSX 0
#define CUBERASTER_BUFFER4_NEGX 16
#define CUBERASTER_BUFFER4_POSY 32
#define CUBERASTER_BUFFER4_NEGY 48
#define CUBERASTER_BUFFER4_POSZ 64
#define CUBERASTER_BUFFER4_NEGZ 80

#define CUBERASTER_BUFFER2_H 1
#define CUBERASTER_BUFFER2_N 2
#define CUBERASTER_BUFFER2_SIZE 24
#define CUBERASTER_BUFFER2_POSX 0
#define CUBERASTER_BUFFER2_NEGX 4
#define CUBERASTER_BUFFER2_POSY 8
#define CUBERASTER_BUFFER2_NEGY 12
#define CUBERASTER_BUFFER2_POSZ 16
#define CUBERASTER_BUFFER2_NEGZ 20

#include "CubeRaster.h"
#include "../shared/gBase.h"

void addon(float&a, const float& b) {float fac = b; if(fac > 0.9) fac = 0.9; a += (1.f - a)*fac;}

CubeRaster::CubeRaster()
{
	m_pOpacity = new float[CUBERASTER_BUFFER_SIZE];
	fOpacity2 = new float[CUBERASTER_BUFFER2_SIZE];
	fOpacity4 = new float[CUBERASTER_BUFFER4_SIZE];
	fOpacity8 = new float[CUBERASTER_BUFFER8_SIZE];
}

CubeRaster::~CubeRaster()
{
	delete[] m_pOpacity;
	delete[] fOpacity2;
	delete[] fOpacity4;
	delete[] fOpacity8;
}

void CubeRaster::clear()
{
	for(unsigned i=0; i<CUBERASTER_BUFFER_SIZE; i++) m_pOpacity[i] = 0;
	for(unsigned i=0; i<CUBERASTER_BUFFER2_SIZE; i++) fOpacity2[i] = 0;
	for(unsigned i=0; i<CUBERASTER_BUFFER4_SIZE; i++) fOpacity4[i] = 0;
	for(unsigned i=0; i<CUBERASTER_BUFFER8_SIZE; i++) fOpacity8[i] = 0;
}

void CubeRaster::compose()
{
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {

			m_pOpacity[CUBERASTER_BUFFER_POSX + j*CUBERASTER_BUFFER_N + i] += fOpacity8[CUBERASTER_BUFFER8_POSX + j/2*CUBERASTER_BUFFER8_N + i/2];
			m_pOpacity[CUBERASTER_BUFFER_POSX + j*CUBERASTER_BUFFER_N + i] += fOpacity4[CUBERASTER_BUFFER4_POSX + j/4*CUBERASTER_BUFFER4_N + i/4];
			m_pOpacity[CUBERASTER_BUFFER_POSX + j*CUBERASTER_BUFFER_N + i] += fOpacity2[CUBERASTER_BUFFER2_POSX + j/8*CUBERASTER_BUFFER2_N + i/8];
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSX + j*CUBERASTER_BUFFER_N + i], fOpacity8[CUBERASTER_BUFFER8_POSX + j/2*CUBERASTER_BUFFER8_N + i/2]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSX + j*CUBERASTER_BUFFER_N + i], fOpacity4[CUBERASTER_BUFFER4_POSX + j/4*CUBERASTER_BUFFER4_N + i/4]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSX + j*CUBERASTER_BUFFER_N + i], fOpacity2[CUBERASTER_BUFFER2_POSX + j/8*CUBERASTER_BUFFER2_N + i/8]);
		}
	}
	
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {

			m_pOpacity[CUBERASTER_BUFFER_NEGX + j*CUBERASTER_BUFFER_N + i] += fOpacity8[CUBERASTER_BUFFER8_NEGX + j/2*CUBERASTER_BUFFER8_N + i/2];
			m_pOpacity[CUBERASTER_BUFFER_NEGX + j*CUBERASTER_BUFFER_N + i] += fOpacity4[CUBERASTER_BUFFER4_NEGX + j/4*CUBERASTER_BUFFER4_N + i/4];
			m_pOpacity[CUBERASTER_BUFFER_NEGX + j*CUBERASTER_BUFFER_N + i] += fOpacity2[CUBERASTER_BUFFER2_NEGX + j/8*CUBERASTER_BUFFER2_N + i/8];
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGX + j*CUBERASTER_BUFFER_N + i], fOpacity8[CUBERASTER_BUFFER8_NEGX + j/2*CUBERASTER_BUFFER8_N + i/2]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGX + j*CUBERASTER_BUFFER_N + i], fOpacity4[CUBERASTER_BUFFER4_NEGX + j/4*CUBERASTER_BUFFER4_N + i/4]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGX + j*CUBERASTER_BUFFER_N + i], fOpacity2[CUBERASTER_BUFFER2_NEGX + j/8*CUBERASTER_BUFFER2_N + i/8]);
		}
	}
	
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {

			m_pOpacity[CUBERASTER_BUFFER_POSY + j*CUBERASTER_BUFFER_N + i] += fOpacity8[CUBERASTER_BUFFER8_POSY + j/2*CUBERASTER_BUFFER8_N + i/2];
			m_pOpacity[CUBERASTER_BUFFER_POSY + j*CUBERASTER_BUFFER_N + i] += fOpacity4[CUBERASTER_BUFFER4_POSY + j/4*CUBERASTER_BUFFER4_N + i/4];
			m_pOpacity[CUBERASTER_BUFFER_POSY + j*CUBERASTER_BUFFER_N + i] += fOpacity2[CUBERASTER_BUFFER2_POSY + j/8*CUBERASTER_BUFFER2_N + i/8];
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSY + j*CUBERASTER_BUFFER_N + i], fOpacity8[CUBERASTER_BUFFER8_POSY + j/2*CUBERASTER_BUFFER8_N + i/2]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSY + j*CUBERASTER_BUFFER_N + i], fOpacity4[CUBERASTER_BUFFER4_POSY + j/4*CUBERASTER_BUFFER4_N + i/4]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSY + j*CUBERASTER_BUFFER_N + i], fOpacity2[CUBERASTER_BUFFER2_POSY + j/8*CUBERASTER_BUFFER2_N + i/8]);
		}
	}
	
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {

			m_pOpacity[CUBERASTER_BUFFER_NEGY + j*CUBERASTER_BUFFER_N + i] += fOpacity8[CUBERASTER_BUFFER8_NEGY + j/2*CUBERASTER_BUFFER8_N + i/2];
			m_pOpacity[CUBERASTER_BUFFER_NEGY + j*CUBERASTER_BUFFER_N + i] += fOpacity4[CUBERASTER_BUFFER4_NEGY + j/4*CUBERASTER_BUFFER4_N + i/4];
			m_pOpacity[CUBERASTER_BUFFER_NEGY + j*CUBERASTER_BUFFER_N + i] += fOpacity2[CUBERASTER_BUFFER2_NEGY + j/8*CUBERASTER_BUFFER2_N + i/8];
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGY + j*CUBERASTER_BUFFER_N + i], fOpacity8[CUBERASTER_BUFFER8_NEGY + j/2*CUBERASTER_BUFFER8_N + i/2]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGY + j*CUBERASTER_BUFFER_N + i], fOpacity4[CUBERASTER_BUFFER4_NEGY + j/4*CUBERASTER_BUFFER4_N + i/4]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGY + j*CUBERASTER_BUFFER_N + i], fOpacity2[CUBERASTER_BUFFER2_NEGY + j/8*CUBERASTER_BUFFER2_N + i/8]);
		}
	}
	
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {

			m_pOpacity[CUBERASTER_BUFFER_POSZ + j*CUBERASTER_BUFFER_N + i] += fOpacity8[CUBERASTER_BUFFER8_POSZ + j/2*CUBERASTER_BUFFER8_N + i/2];
			m_pOpacity[CUBERASTER_BUFFER_POSZ + j*CUBERASTER_BUFFER_N + i] += fOpacity4[CUBERASTER_BUFFER4_POSZ + j/4*CUBERASTER_BUFFER4_N + i/4];
			m_pOpacity[CUBERASTER_BUFFER_POSZ + j*CUBERASTER_BUFFER_N + i] += fOpacity2[CUBERASTER_BUFFER2_POSZ + j/8*CUBERASTER_BUFFER2_N + i/8];
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSZ + j*CUBERASTER_BUFFER_N + i], fOpacity8[CUBERASTER_BUFFER8_POSZ + j/2*CUBERASTER_BUFFER8_N + i/2]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSZ + j*CUBERASTER_BUFFER_N + i], fOpacity4[CUBERASTER_BUFFER4_POSZ + j/4*CUBERASTER_BUFFER4_N + i/4]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_POSZ + j*CUBERASTER_BUFFER_N + i], fOpacity2[CUBERASTER_BUFFER2_POSZ + j/8*CUBERASTER_BUFFER2_N + i/8]);
		}
	}
	
	for(int j=0; j<CUBERASTER_BUFFER_N; j++) {
		for(int i=0; i<CUBERASTER_BUFFER_N; i++) {

			m_pOpacity[CUBERASTER_BUFFER_NEGZ + j*CUBERASTER_BUFFER_N + i] += fOpacity8[CUBERASTER_BUFFER8_NEGZ + j/2*CUBERASTER_BUFFER8_N + i/2];
			m_pOpacity[CUBERASTER_BUFFER_NEGZ + j*CUBERASTER_BUFFER_N + i] += fOpacity4[CUBERASTER_BUFFER4_NEGZ + j/4*CUBERASTER_BUFFER4_N + i/4];
			m_pOpacity[CUBERASTER_BUFFER_NEGZ + j*CUBERASTER_BUFFER_N + i] += fOpacity2[CUBERASTER_BUFFER2_NEGZ + j/8*CUBERASTER_BUFFER2_N + i/8];
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGZ + j*CUBERASTER_BUFFER_N + i], fOpacity8[CUBERASTER_BUFFER8_NEGZ + j/2*CUBERASTER_BUFFER8_N + i/2]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGZ + j*CUBERASTER_BUFFER_N + i], fOpacity4[CUBERASTER_BUFFER4_NEGZ + j/4*CUBERASTER_BUFFER4_N + i/4]);
			//addon(m_pOpacity[CUBERASTER_BUFFER_NEGZ + j*CUBERASTER_BUFFER_N + i], fOpacity2[CUBERASTER_BUFFER2_NEGZ + j/8*CUBERASTER_BUFFER2_N + i/8]);
		}
	}
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

void CubeRaster::writemip(XYZ& ray, float val, int detail)
{
	if(detail < 1) write(ray, val);
	else if(detail < 2) write8(ray, val);
	else if(detail < 4) write4(ray, val);
	else write2(ray, val);
}

void CubeRaster::write2(XYZ& ray, float val)
{
	if(ray.lengthSquare() < 0.001) return;
	ray.normalize();
	float x = ray.x; if(x<0) x *= -1;
	float y = ray.y; if(y<0) y *= -1;
	float z = ray.z; if(z<0) z *= -1;
	unsigned offset;
	float u, v;
	if(x >y && x>z) {
		if(ray.x > 0) offset = CUBERASTER_BUFFER2_POSX;
		else offset = CUBERASTER_BUFFER2_NEGX;
		
		u = (ray.z/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER2_N;
		v = (ray.y/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER2_N;

	}
	else if(y>x && y>z) {
		if(ray.y > 0) offset = CUBERASTER_BUFFER2_POSY;
		else offset = CUBERASTER_BUFFER2_NEGY;
		
		u = (ray.x/y + 1 - 0.0000001)*0.5*CUBERASTER_BUFFER2_N;
		v = (ray.z/y + 1- 0.0000001)*0.5*CUBERASTER_BUFFER2_N;
	}
	else {
		if(ray.z > 0) offset = CUBERASTER_BUFFER2_POSZ;
		else offset = CUBERASTER_BUFFER2_NEGZ;
		
		u = (ray.x/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER2_N;
		v = (ray.y/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER2_N;
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
	
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER2_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER2_N) /*addon(fOpacity2[offset + nv0*CUBERASTER_BUFFER2_N + nu0], val);*/fOpacity2[offset + nv0*CUBERASTER_BUFFER2_N + nu0] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER2_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER2_N) /*addon(fOpacity2[offset + nv0*CUBERASTER_BUFFER2_N + nu1], val);*/fOpacity2[offset + nv0*CUBERASTER_BUFFER2_N + nu1] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER2_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER2_N) /*addon(fOpacity2[offset + nv1*CUBERASTER_BUFFER2_N + nu1], val);*/fOpacity2[offset + nv1*CUBERASTER_BUFFER2_N + nu1] += val;		
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER2_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER2_N) /*addon(fOpacity2[offset + nv1*CUBERASTER_BUFFER2_N + nu0], val);*/fOpacity2[offset + nv1*CUBERASTER_BUFFER2_N + nu0] += val;
}

void CubeRaster::write4(XYZ& ray, float val)
{
	if(ray.lengthSquare() < 0.001) return;
	ray.normalize();
	float x = ray.x; if(x<0) x *= -1;
	float y = ray.y; if(y<0) y *= -1;
	float z = ray.z; if(z<0) z *= -1;
	unsigned offset;
	float u, v;
	if(x >y && x>z) {
		if(ray.x > 0) offset = CUBERASTER_BUFFER4_POSX;
		else offset = CUBERASTER_BUFFER4_NEGX;
		
		u = (ray.z/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER4_N;
		v = (ray.y/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER4_N;

	}
	else if(y>x && y>z) {
		if(ray.y > 0) offset = CUBERASTER_BUFFER4_POSY;
		else offset = CUBERASTER_BUFFER4_NEGY;
		
		u = (ray.x/y + 1 - 0.0000001)*0.5*CUBERASTER_BUFFER4_N;
		v = (ray.z/y + 1- 0.0000001)*0.5*CUBERASTER_BUFFER4_N;
	}
	else {
		if(ray.z > 0) offset = CUBERASTER_BUFFER4_POSZ;
		else offset = CUBERASTER_BUFFER4_NEGZ;
		
		u = (ray.x/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER4_N;
		v = (ray.y/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER4_N;
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
	
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER4_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER4_N) /*addon(fOpacity4[offset + nv0*CUBERASTER_BUFFER4_N + nu0], val);*/fOpacity4[offset + nv0*CUBERASTER_BUFFER4_N + nu0] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER4_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER4_N) /*addon(fOpacity4[offset + nv0*CUBERASTER_BUFFER4_N + nu1], val);*/fOpacity4[offset + nv0*CUBERASTER_BUFFER4_N + nu1] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER4_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER4_N) /*addon(fOpacity4[offset + nv1*CUBERASTER_BUFFER4_N + nu1], val);*/fOpacity4[offset + nv1*CUBERASTER_BUFFER4_N + nu1] += val;		
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER4_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER4_N) /*addon(fOpacity4[offset + nv1*CUBERASTER_BUFFER4_N + nu0], val);*/fOpacity4[offset + nv1*CUBERASTER_BUFFER4_N + nu0] += val;
}

void CubeRaster::write8(XYZ& ray, float val)
{
	if(ray.lengthSquare() < 0.001) return;
	ray.normalize();
	float x = ray.x; if(x<0) x *= -1;
	float y = ray.y; if(y<0) y *= -1;
	float z = ray.z; if(z<0) z *= -1;
	unsigned offset;
	float u, v;
	if(x >y && x>z) {
		if(ray.x > 0) offset = CUBERASTER_BUFFER8_POSX;
		else offset = CUBERASTER_BUFFER8_NEGX;
		
		u = (ray.z/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER8_N;
		v = (ray.y/x + 1- 0.0000001)*0.5*CUBERASTER_BUFFER8_N;

	}
	else if(y>x && y>z) {
		if(ray.y > 0) offset = CUBERASTER_BUFFER8_POSY;
		else offset = CUBERASTER_BUFFER8_NEGY;
		
		u = (ray.x/y + 1 - 0.0000001)*0.5*CUBERASTER_BUFFER8_N;
		v = (ray.z/y + 1- 0.0000001)*0.5*CUBERASTER_BUFFER8_N;
	}
	else {
		if(ray.z > 0) offset = CUBERASTER_BUFFER8_POSZ;
		else offset = CUBERASTER_BUFFER8_NEGZ;
		
		u = (ray.x/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER8_N;
		v = (ray.y/z + 1- 0.0000001)*0.5*CUBERASTER_BUFFER8_N;
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
	
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER8_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER8_N) /*addon(fOpacity8[offset + nv0*CUBERASTER_BUFFER8_N + nu0], val);*/fOpacity8[offset + nv0*CUBERASTER_BUFFER8_N + nu0] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER8_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER8_N) /*addon(fOpacity8[offset + nv0*CUBERASTER_BUFFER8_N + nu1], val);*/fOpacity8[offset + nv0*CUBERASTER_BUFFER8_N + nu1] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER8_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER8_N) /*addon(fOpacity8[offset + nv1*CUBERASTER_BUFFER8_N + nu1], val);*/fOpacity8[offset + nv1*CUBERASTER_BUFFER8_N + nu1] += val;		
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER8_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER8_N) /*addon(fOpacity8[offset + nv1*CUBERASTER_BUFFER8_N + nu0], val);*/fOpacity8[offset + nv1*CUBERASTER_BUFFER8_N + nu0] += val;
}

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
	
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER_N) /*addon(m_pOpacity[offset + nv0*CUBERASTER_BUFFER_N + nu0], val); */m_pOpacity[offset + nv0*CUBERASTER_BUFFER_N + nu0] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER_N && nv0 > -1 && nv0 < CUBERASTER_BUFFER_N) /*addon(m_pOpacity[offset + nv0*CUBERASTER_BUFFER_N + nu1], val);*/m_pOpacity[offset + nv0*CUBERASTER_BUFFER_N + nu1] += val;
	if(nu1 > -1 && nu1 < CUBERASTER_BUFFER_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER_N) /*addon(m_pOpacity[offset + nv1*CUBERASTER_BUFFER_N + nu1], val);*/m_pOpacity[offset + nv1*CUBERASTER_BUFFER_N + nu1] += val;		
	if(nu0 > -1 && nu0 < CUBERASTER_BUFFER_N && nv1 > -1 && nv1 < CUBERASTER_BUFFER_N) /*addon(m_pOpacity[offset + nv1*CUBERASTER_BUFFER_N + nu0], val);*/m_pOpacity[offset + nv1*CUBERASTER_BUFFER_N + nu0] += val;
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
	//lighting = 1.f - opacity;
	//if(lighting < 0) lighting = 0;
	lighting = 1.f/ (1.f+opacity*4);
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
	getLight(m_pOpacity[offset + v*CUBERASTER_BUFFER_N + u], lighting);
}
