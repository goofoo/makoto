#include ".\fvisibility.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

void clamp(float& val, float min, float max)
{
	if(val < min) val= min;
	else if(val > max) val= max;
}

FVisibility::FVisibility(int w, int h, double fov, float light_size):m_clipnear(1.)
{
	m_light_size = light_size;
	m_light_size_1 = m_light_size*0.99999f;
	m_light_area = m_light_size*m_light_size*4;
	m_v_grid = m_light_size/VBUFWIDTHHALF;
	m_width = w;
	m_height = h;
	m_aspect = (float)m_width/(float)m_height;

	m_fov = fov;
	m_factor = tan(m_fov/360.0f*PI);
	double e = 1.0/m_factor;
	double a = 1.0/m_aspect;
	double beta = 2.0*atan(a/e);
	double left,right,bottom,top;
		
		top = tan(beta/2) * m_clipnear;
		bottom = -top;
		left = m_aspect * bottom;
		right = -left;
	
// left
	m_plane[0].a = -e/sqrt(e*e+1); m_plane[0].b =0; m_plane[0].c = 1/sqrt(e*e+1); m_plane[0].d = 0;
// right
	m_plane[1].a = e/sqrt(e*e+1); m_plane[1].b =0; m_plane[1].c = 1/sqrt(e*e+1); m_plane[1].d = 0;
// bottom
	m_plane[2].a = 0; m_plane[2].b =e/sqrt(e*e+a*a); m_plane[2].c = a/sqrt(e*e+a*a); m_plane[2].d = 0;
// top
	m_plane[3].a = 0; m_plane[3].b =-e/sqrt(e*e+a*a); m_plane[3].c = a/sqrt(e*e+a*a); m_plane[3].d = 0;
}

FVisibility::~FVisibility(void)
{
}

void FVisibility::setSpace(const XYZ& eye, const XYZ& view, const XYZ& up, const XYZ& side)
{
	m_eye = eye;
	m_space.v[3][3]=1; m_space.v[0][3]=m_space.v[1][3]=m_space.v[2][3]=0;
	m_space.v[0][0] = side.x; m_space.v[0][1] = side.y; m_space.v[0][2] = side.z; 
	m_space.v[1][0] = up.x; m_space.v[1][1] = up.y; m_space.v[1][2] = up.z; 
	m_space.v[2][0] = view.x; m_space.v[2][1] = view.y; m_space.v[2][2] = view.z; 
	m_space.v[3][0] = eye.x; m_space.v[3][1] = eye.y; m_space.v[3][2] = eye.z;
	
	m_spaceinv = m_space; m_spaceinv.inverse();
}

int FVisibility::dice(const qnode& data, float threshold, XYZ& objectP)
{
	float rr = sqrt(data.area)*2.f;
	
	XYZ node2eye = m_eye -data.pos;
	node2eye.normalize();
	// backside: skip
	//if( data.nor.dot(node2eye) < data.cone-1.01 ) return -1;
	
	XYZ q(data.pos);
	toObject(q);
	
	objectP = q;
	
	if(q.z < m_clipnear-rr) return -1;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
	// outside: skip
	if(dist[0] > rr || dist[1] > rr || dist[2] > rr || dist[3] > rr ) return -1;
	
	// too big or too much normal scattering: need split
	if( rr > threshold*q.z*m_factor/(float)m_width ) return 0;

	return 1;
}
/*
int FVisibility::isBackVisible(const qnode& data, float threshold, XYZ& objectP)
{
	float rr = sqrt(data.area)*2.f;
	
	XYZ node2eye = data.pos - m_eye;
	node2eye.normalize();
	// backside: skip
	if( data.nor.dot(node2eye) < data.cone-1.17 ) return -1;
	
	XYZ q(data.pos);
	toObject(q);
	
	objectP = q;
	
	if(q.z < m_clipnear-rr) return -1;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
	// outside: skip
	if(dist[0] > rr || dist[1] > rr || dist[2] > rr || dist[3] > rr ) return -1;
	
	// too big or too much normal scattering: need split
	if( rr > threshold*q.z*m_factor/(float)m_width*2 ) return 0;

	return 1;
}
*/
int FVisibility::isVisible(XYZ& p)
{
	XYZ q(p);
	toObject(q);
	
	if(q.z < m_clipnear) return 0;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
	// outside: skip
	if(dist[0] > 0 || dist[1] > 0 || dist[2] > 0 || dist[3] > 0 ) return 0;

	return 1;
}

int FVisibility::isInFrustum(XYZ& p, XYZ& objP)
{
	XYZ q(p);
	toObject(q);
	objP= q;
	if(q.z < m_clipnear) return 0;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
	// outside: skip
	if(dist[0] > 0 || dist[1] > 0 || dist[2] > 0 || dist[3] > 0 ) return 0;

	return 1;
}

void FVisibility::recursive_lookup(const XYZ& q, const MINMAX* map, int id, short level, float& res)
{
// not in shadow
	if(q.z < map[id].min) return;
		XYZ corner0, corner1, corner2, corner3;
		croppedProject(q, map[id], corner0, corner1, corner2, corner3);
	float val = (corner1.x-corner0.x)*(corner3.y-corner0.y);
	if(val==0) return;
// in shadow
	if(q.z - 0.5 > map[id].max && map[id].child[0]==0) 
	{
		
		//cout<<" "<<level;
		res += val/m_light_area;
		return;
	}
// go deeper in the tree
	short new_level = level+1;
	if(map[id].child[0]>0) recursive_lookup(q, map, map[id].child[0], new_level, res);
	if(map[id].child[1]>0) recursive_lookup(q, map, map[id].child[1], new_level, res);
	if(map[id].child[2]>0) recursive_lookup(q, map, map[id].child[2], new_level, res);
	if(map[id].child[3]>0) recursive_lookup(q, map, map[id].child[3], new_level, res);
}

float FVisibility::occlusion(XYZ& p,  const MINMAX* map)
{
	XYZ q(p);
	toObject(q);
	
	if(q.z < m_clipnear) return 0;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
	// outside: skip
	if(dist[0] > 0 || dist[1] > 0 || dist[2] > 0 || dist[3] > 0 ) return 0;
	/*	
	double hhalf = q.z*m_factor;
	double xcoord = -q.x/hhalf;
	xcoord = (xcoord+1)/2;
	
	double ycoord = q.y/hhalf;
	ycoord = (ycoord+1)/2;
	
	int u = (m_width-1)*xcoord;
	int v = (m_height-1)*ycoord;
	*/
	//if(q.z - 0.5 < depthBuffer[v*m_width + u]) return 0;
	float res = 0;
	recursive_lookup(q, map, 0, 0, res);

	/*
	clampedProject(q, depthBuffer[v*m_width + u], u, v, corner0, corner1, corner2, corner3);
	
	float res = (corner1.x-corner0.x)*(corner3.y-corner0.y)/m_light_size/m_light_size/4;
	//
*/
	
	return res;
}

void FVisibility::getLookAt(float& eyex, float& eyey, float& eyez, float& viewx, float& viewy, float& viewz, float& upx, float& upy, float& upz)
{
	eyex = m_space.v[3][0];
	eyey = m_space.v[3][1];
	eyez = m_space.v[3][2];
	viewx = m_space.v[3][0] + m_space.v[2][0];
	viewy = m_space.v[3][1] + m_space.v[2][1];
	viewz = m_space.v[3][2] + m_space.v[2][2];
	upx = m_space.v[1][0]; 
	upy = m_space.v[1][1]; 
	upz = m_space.v[1][2]; 
}
/*
void FVisibility::getPixel(float depth, int u, int v, short level, XYZ& pp, float& pixelsize)
{
	pixelsize = depth*m_factor/(m_width/2);
	pp.x = depth*m_factor - pixelsize*(u + 0.5);
	pp.y = -depth*m_factor + pixelsize*(v + 0.5);
	pp.z = depth;
	
	pixelsize = depth*m_factor/(pow(2,level)/2);
}
*/
void project(const XYZ& objectP, const XYZ& center, float pixelsize, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3)
{
	XYZ ray; float t;
	//float d2s, t, xfill, filf; 
	
	corner0.x = center.x - pixelsize;
	corner0.y = center.y - pixelsize;
	corner0.z = center.z;
	
	ray = corner0 - objectP;
	//d2s = ray.length();
	//filf = d2s/(pixel.diff+d2s);
	ray.normalize();
	
	//xfill= pixel.diff*ray.x/ray.z*filf;
	
	//if(xfill<0) corner0.x += xfill;
	
		
	//ray = corner0 - objectP;
	//ray.normalize();
	
	t = -(objectP.z-1)/ray.z;
	corner0.x = objectP.x+ray.x*t;
	corner0.y = objectP.y+ray.y*t;
	corner0.z = 1;
	
	corner1.x = center.x + pixelsize;
	corner1.y = center.y - pixelsize;
	corner1.z = center.z;
	
	ray = corner1 - objectP;
	//d2s = ray.length();
	//filf = d2s/(pixel.diff+d2s);
	ray.normalize();
	
	//xfill= pixel.diff*ray.x/ray.z*filf;
	
	//if(xfill>0) corner1.x += xfill;
		
	//ray = corner1 - objectP;
	//ray.normalize();
	
	t = -(objectP.z-1)/ray.z;
	corner1.x = objectP.x+ ray.x*t;
	corner1.y = objectP.y+ ray.y*t;
	corner1.z = 1;
	
	corner2.x = center.x + pixelsize;
	corner2.y = center.y + pixelsize;
	corner2.z = center.z;
	
	ray = corner2 - objectP;
	//d2s = ray.length();
	//filf = d2s/(pixel.diff+d2s);
	ray.normalize();
	
	//xfill= pixel.diff*ray.x/ray.z*filf;
	
	//if(xfill>0) corner2.x += xfill;
		
	//ray = corner2 - objectP;
	//ray.normalize();
	
	t = -(objectP.z-1)/ray.z;
	corner2.x = objectP.x+ ray.x*t;
	corner2.y = objectP.y+ray.y*t;
	corner2.z = 1;
	
	corner3.x = center.x - pixelsize;
	corner3.y = center.y + pixelsize;
	corner3.z = center.z;
	
	ray = corner3 - objectP;
	//d2s = -ray.z;
	ray.normalize();
	
	//xfill= pixel.diff*ray.x/ray.z*d2s/(pixel.diff+d2s);
	
	//if(xfill<0) corner3.x += xfill;
		
	//ray = corner3 - objectP;
	//ray.normalize();
	
	t = -(objectP.z-1)/ray.z;
	corner3.x= objectP.x+ray.x*t;
	corner3.y= objectP.y+ray.y*t;
	corner3.z = 1;
}

void FVisibility::croppedProject(const XYZ& objectP, const MINMAX& pixel, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3)
{
	project( objectP, pixel.pos, pixel.size, corner0, corner1, corner2, corner3);
	
	clamp(corner0.x, -m_light_size, m_light_size);
	clamp(corner0.y, -m_light_size, m_light_size);
	
	clamp(corner1.x, -m_light_size, m_light_size);
	clamp(corner1.y, -m_light_size, m_light_size);
	
	clamp(corner2.x, -m_light_size, m_light_size);
	clamp(corner2.y, -m_light_size, m_light_size);
	
	clamp(corner3.x, -m_light_size, m_light_size);
	clamp(corner3.y, -m_light_size, m_light_size);
}

void FVisibility::croppedProject(const XYZ& objectP, const XYZ& center, float pixelsize, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3)
{
	project( objectP, center, pixelsize, corner0, corner1, corner2, corner3);
	
	clamp(corner0.x, -m_light_size, m_light_size);
	clamp(corner0.y, -m_light_size, m_light_size);
	
	clamp(corner1.x, -m_light_size, m_light_size);
	clamp(corner1.y, -m_light_size, m_light_size);
	
	clamp(corner2.x, -m_light_size, m_light_size);
	clamp(corner2.y, -m_light_size, m_light_size);
	
	clamp(corner3.x, -m_light_size, m_light_size);
	clamp(corner3.y, -m_light_size, m_light_size);
}

void FVisibility::getLightPlane(XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3)
{
	corner0.z = corner1.z = corner2.z = corner3.z = 0;
	corner0.x = -m_light_size;
	corner0.y = -m_light_size;
	corner1.x =  m_light_size;
	corner1.y = -m_light_size;
	corner2.x =  m_light_size;
	corner2.y =  m_light_size;
	corner3.x = -m_light_size;
	corner3.y = m_light_size;
	toWorld(corner0);
	toWorld(corner1);
	toWorld(corner2);
	toWorld(corner3);
}

void FVisibility::toWorld(XYZ& p)
{
	m_space.transform(p);
}

void FVisibility::toObject(XYZ& p)
{
	m_spaceinv.transform(p);
}

void FVisibility::toObject(qnode& n)
{
	m_spaceinv.transform(n.pos);
	m_spaceinv.transformAsNormal(n.nor);
}

void FVisibility::getKernel(XYZ& q, short& umin, short& umax, short& vmin, short& vmax)
{	
	float pixelsize = m_clipnear*m_factor/512;
	
	short wk = m_light_size * (q.z - m_clipnear)/q.z/pixelsize+2;
	
	pixelsize = q.z*m_factor/512;
	
	float cu = (q.z*m_factor - q.x)/pixelsize+0.5;
	float cv = (q.z*m_factor + q.y)/pixelsize+0.5;
	
	//int icu = cu; if(cu-icu<0.5) cu = icu; else cu = icu+1;
	//	int icv = cv; if(cv-icv<0.5) cv = icv; else cv = icv+1;
	
	umin = cu - wk; if(umin<0) umin=0;
	umax = cu + wk; if(umax>1023) umax=1023;
		
	vmin = cv - wk; if(vmin<0) vmin=0;
	vmax = cv + wk; if(vmax>1023) vmax=1023;
}

void FVisibility::uvToObject(float& u, float& v, float depth)
{
	float pixelsize = depth*m_factor/512;
	u = pixelsize*512 - pixelsize*(u + 0.5) + pixelsize*0.5;
	v = -pixelsize*512 + pixelsize*(v + 0.5) + pixelsize*0.5;
}

void FVisibility::uvToObject(int& u, int& v, float depth)
{
	float pixelsize = depth*m_factor/512;
	u = pixelsize*512 - pixelsize*(u + 0.5) + pixelsize*0.5;
	v = -pixelsize*512 + pixelsize*(v + 0.5) + pixelsize*0.5;
}

void FVisibility::objectToUV(float& u, float& v, const XYZ& q)
{
	float pixelsize = q.z*m_factor/512;
	u = 512 - q.x/pixelsize;
	v = 512 + q.y/pixelsize;
}

void FVisibility::objectToUV(int& u, int& v, const XYZ& q)
{
	float pixelsize = q.z*m_factor/512;
	u = 512 - q.x/pixelsize;
	v = 512 + q.y/pixelsize;
}

float FVisibility::backProject(const XYZ& objectP, const MINMAX& pixel)
{
	float projx = objectP.x + objectP.z/(objectP.z - pixel.min)*(pixel.pos.x - objectP.x);
	float projy = objectP.y + objectP.z/(objectP.z - pixel.min)*(pixel.pos.y - objectP.y);
	float projs = objectP.z/(objectP.z - pixel.min)*pixel.size;
	
	float xmin = projx - projs;
	float xmax = projx + projs;
	float ymin = projy - projs;
	float ymax = projy + projs;
// outside	
	if(xmax < -m_light_size || xmin > m_light_size || ymax < -m_light_size || ymin > m_light_size)
		return -1;
	
	if(xmin < -m_light_size) xmin = -m_light_size;
	if(xmax > m_light_size) xmax = m_light_size;
	if(ymin < -m_light_size) ymin = -m_light_size;
	if(ymax > m_light_size) ymax = m_light_size;
		
	return (xmax-xmin)*(ymax-ymin);
}

int FVisibility::backProject(const XYZ& objectP, const qnode& node, float& xmin, float& xmax, float& ymin, float& ymax)
{
	float projx = objectP.x + objectP.z/(objectP.z - node.pos.z + node.area)*(node.pos.x - objectP.x);
	float projy = objectP.y + objectP.z/(objectP.z - node.pos.z + node.area)*(node.pos.y - objectP.y);
	float projs = objectP.z/(objectP.z - node.pos.z + node.area)*node.area;
	
	xmin = projx - projs;
	xmax = projx + projs;
	ymin = projy - projs;
	ymax = projy + projs;
// outside	
	if(xmax < -m_light_size || xmin > m_light_size || ymax < -m_light_size || ymin > m_light_size)
		return 0;
	
	return 1;
}

int FVisibility::backProject(const XYZ& objectP, const MINMAX& pixel, float& xmin, float& xmax, float& ymin, float& ymax)
{
	float projx = objectP.x + objectP.z/(objectP.z - pixel.min)*(pixel.pos.x - objectP.x);
	float projy = objectP.y + objectP.z/(objectP.z - pixel.min)*(pixel.pos.y - objectP.y);
	float projs = objectP.z/(objectP.z - pixel.min)*pixel.size;
	
	xmin = projx - projs;
	xmax = projx + projs;
	ymin = projy - projs;
	ymax = projy + projs;
// outside	
	if(xmax < -m_light_size || xmin > m_light_size || ymax < -m_light_size || ymin > m_light_size)
		return 0;
	
	return 1;
}

void FVisibility::writeBuffer(float* buf, float& xmin, float& xmax, float& ymin, float& ymax)
{
	if(xmin < -m_light_size) xmin = -m_light_size;
	if(xmax > m_light_size_1) xmax = m_light_size_1;
	if(ymin < -m_light_size) ymin = -m_light_size;
	if(ymax > m_light_size_1) ymax = m_light_size_1;
		
	xmin = (xmin+m_light_size)/m_v_grid;
	xmax = (xmax +m_light_size)/m_v_grid;
	ymin = (ymin+m_light_size)/m_v_grid;
	ymax = (ymax+m_light_size)/m_v_grid;
	
	
	short vmin = ymin;
	short vmax = ymax;
	
	short umin = xmin;
	short umax = xmax;
	
	float posx = xmax-(float)umax;
	float negx = (float)umax-xmin;
	float posy = ymax-(float)vmax;
	float negy = (float)vmax-ymin;
	
	if(umin==umax && vmin==vmax)
	{
		buf[vmin*VBUFWIDTH+umin] += (xmax-xmin)*(ymax-ymin);
	}
	else if(vmin==vmax && umin==umax-1)
	{
		buf[vmin*VBUFWIDTH+umin] += negx*(ymax-ymin);
		buf[vmin*VBUFWIDTH+umax] += posx*(ymax-ymin);
	}
	else if(umin==umax && vmin==vmax-1)
	{
		buf[vmin*VBUFWIDTH+umin] += (xmax-xmin)*negy;
		buf[vmax*VBUFWIDTH+umin] += (xmax-xmin)*posy;
	}
	else if(vmin==vmax-1 && umin==umax-1)
	{
		buf[vmin*VBUFWIDTH+umin] += negx*negy;
		buf[vmin*VBUFWIDTH+umax] += posx*negy;
		buf[vmax*VBUFWIDTH+umin] += negx*posy;
		buf[vmax*VBUFWIDTH+umax] += posx*posy;
	}
	else
	{
		for(unsigned short j=vmin+1; j<vmax; j++)
		for(unsigned short i=umin+1; i<umax; i++)
			buf[j*VBUFWIDTH+i] = 1.f;
		
		for(unsigned short i=umin+1; i<umax; i++)
		{
			buf[vmin*VBUFWIDTH+i] += negy;
			buf[vmax*VBUFWIDTH+i] += posy;
		}
		
		for(unsigned short j=vmin+1; j<vmax; j++)
		{
			buf[j*VBUFWIDTH+umin] += negx;
			buf[j*VBUFWIDTH+umax] += posx;
		}
		
		buf[vmin*VBUFWIDTH+umin] += negx*negy;
		buf[vmin*VBUFWIDTH+umax] += posx*negy;
		buf[vmax*VBUFWIDTH+umin] += negx*posy;
		buf[vmax*VBUFWIDTH+umax] += posx*posy;
	}
	
	//for(unsigned short j=vmin; j<=vmax; j++)
		//for(unsigned short i=umin; i<=umax; i++)
			//buf[j*VBUFWIDTH+i] = 1.f;
	//
	//cout<<" "<<umin<<":"<<umax<<":"<<vmin<<":"<<vmax;
}

void FVisibility::backProject(const XYZ& objectP, const MINMAX& pixel, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3)
{
	float projx = objectP.x + objectP.z/(objectP.z - pixel.min)*(pixel.pos.x - objectP.x);
	float projy = objectP.y + objectP.z/(objectP.z - pixel.min)*(pixel.pos.y - objectP.y);
	float projs = objectP.z/(objectP.z - pixel.min)*pixel.size;
	
	float xmin = projx - projs;
	float xmax = projx + projs;
	float ymin = projy - projs;
	float ymax = projy + projs;
	
	//if(xmin < -m_light_size) xmin = -m_light_size;
	//if(xmax > m_light_size) xmax = m_light_size;
	//if(ymin < -m_light_size) ymin = -m_light_size;
	//if(ymax > m_light_size) ymax = m_light_size;
	
	corner0.x = xmin;
	corner0.y = ymin;
	corner0.z = 0;
	
	corner1.x = xmax;
	corner1.y = ymin;
	corner1.z = 0;
	
	corner2.x = xmax;
	corner2.y = ymax;
	corner2.z = 0;
	
	corner3.x = xmin;
	corner3.y = ymax;
	corner3.z = 0;
}

void FVisibility::onScreen(const MINMAX& pixel, float clip, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3)
{
	float projx = pixel.pos.x*clip/pixel.min;
	float projy =  pixel.pos.y*clip/pixel.min;
	float projs = pixel.size*clip/pixel.min;
	
	float xmin = projx - projs;
	float xmax = projx + projs;
	float ymin = projy - projs;
	float ymax = projy + projs;
	
	corner0.x = xmin;
	corner0.y = ymin;
	corner0.z = clip;
	
	corner1.x = xmax;
	corner1.y = ymin;
	corner1.z = clip;
	
	corner2.x = xmax;
	corner2.y = ymax;
	corner2.z = clip;
	
	corner3.x = xmin;
	corner3.y = ymax;
	corner3.z = clip;
}

void FVisibility::getVPixel(int i, int j, XYZ& a, XYZ& b, XYZ& c, XYZ& d)
{
	a.x = m_v_grid*i + m_v_grid*0.5 - m_light_size - m_v_grid*0.5;
	a.y = m_v_grid*j + m_v_grid*0.5 - m_light_size - m_v_grid*0.5;
	a.z = 0;
	
	toWorld(a);
	
	b.x = m_v_grid*i + m_v_grid*0.5 - m_light_size + m_v_grid*0.5;
	b.y = m_v_grid*j + m_v_grid*0.5 - m_light_size - m_v_grid*0.5;
	b.z = 0;
	
	toWorld(b);
	
	c.x = m_v_grid*i + m_v_grid*0.5 - m_light_size + m_v_grid*0.5;
	c.y = m_v_grid*j + m_v_grid*0.5 - m_light_size + m_v_grid*0.5;
	c.z = 0;
	
	toWorld(c);
	
	d.x = m_v_grid*i + m_v_grid*0.5 - m_light_size - m_v_grid*0.5;
	d.y = m_v_grid*j + m_v_grid*0.5 - m_light_size + m_v_grid*0.5;
	d.z = 0;
	
	toWorld(d);
}
//:~