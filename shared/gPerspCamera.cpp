#include "gPerspCamera.h"
#include "math.h"
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
#endif

#include <iostream>
using namespace std;

gPerspCamera::gPerspCamera()
{
	m_eye = XYZ(0.0, 0.0, 16.0);
	I = XYZ(0.0, 0.0, -16.0);
	In = I.normal();
	m_view = m_eye + I;
	up = XYZ(0.0, 1.0, 0.0);
	side = XYZ(1.0, 0.0, 0.0);
	
	updateSpace();
	
	m_fov = 35.0f;
	m_aspect = 1.0;
	
	m_clipnear = 0.1;
	m_clipfar = 10000.0;
}

gPerspCamera::gPerspCamera(float fov, float _near, float _far, XYZ& ori)
{
	m_eye_ori = m_eye = ori;
	I = XYZ(-ori.x, -ori.y, -ori.z);
	In = I.normal();
	
	m_view = XYZ(0,0,0);
	ori.normalize();
	if(fabs(ori.y)<0.9f)
		up = XYZ(0.0, 1.0, 0.0);
	else
		up = XYZ(1.0, 0.0, 0.0);
	side = up^I; side.normalize();
	up = I^side; up.normalize();
	
	updateSpace();
	
	m_fov = fov;
	m_aspect = 1.0;
	
	m_clipnear = _near;
	m_clipfar = _far;
}

gPerspCamera::gPerspCamera(float fov, int width, int height, float _near, float _far)
{	
	m_eye = XYZ(0.0, 0.0, 16.0);
	I = XYZ(0.0, 0.0, -16.0);
	In = I.normal();
	m_view = m_eye + I;
	up = XYZ(0.0, 1.0, 0.0);
	side = XYZ(1.0, 0.0, 0.0);
	
	updateSpace();
	
	m_fov = fov;
	m_aspect = (double)m_width/(double)m_height;
	
	m_clipnear = _near;
	m_clipfar = _far;

}

void gPerspCamera::reset() {
	m_eye = m_eye_ori;
	I = XYZ(-m_eye.x, -m_eye.y, -m_eye.z);
	In = I.normal();
	m_view = XYZ(0,0,0);
	up   = XYZ(0.0, 1.0, 0.0);
	side = XYZ(1.0, 0.0, 0.0);
	
	updateSpace();
}

void gPerspCamera::setViewPort( int w, int h ) 
{
	m_width = w;
	if(h==0) h=1;
	m_height = h;
	
	m_aspect = (float)m_width/(float)m_height;
	
	double e = 1.0/tan(m_fov/360*PI);
	double a = 1.0/m_aspect;
	double beta = 2.0*atan(a/e);
	GLdouble left,right,bottom,top;
		
		top = tan(beta/2) * m_clipnear;
		bottom = -top;
		left = m_aspect * bottom;
		right = -left;
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    //gluPerspective(beta/PI*180, m_aspect, m_clipnear, m_clipfar);
	glFrustum(left, right, bottom, top, m_clipnear, m_clipfar);
	
//calculate view frustum
// left
	m_plane[0].a = -e/sqrt(e*e+1); m_plane[0].b =0; m_plane[0].c = 1/sqrt(e*e+1); m_plane[0].d = 0;
// right
	m_plane[1].a = e/sqrt(e*e+1); m_plane[1].b =0; m_plane[1].c = 1/sqrt(e*e+1); m_plane[1].d = 0;
// bottom
	m_plane[2].a = 0; m_plane[2].b =e/sqrt(e*e+a*a); m_plane[2].c = a/sqrt(e*e+a*a); m_plane[2].d = 0;
// top
	m_plane[3].a = 0; m_plane[3].b =-e/sqrt(e*e+a*a); m_plane[3].c = a/sqrt(e*e+a*a); m_plane[3].d = 0;
	
	glMatrixMode(GL_MODELVIEW);
}

void gPerspCamera::setViewPort(double fov, int w, int h ) 
{
	m_fov = fov;
	m_width = w;
	if(h==0) h=1;
	m_height = h;
	
	m_aspect = (float)m_width/(float)m_height;
	
	double e = 1.0/tan(m_fov/360*PI);
	double a = 1.0/m_aspect;
	double beta = 2.0*atan(a/e);
	GLdouble left,right,bottom,top;
		
		top = tan(beta/2) * m_clipnear;
		bottom = -top;
		left = m_aspect * bottom;
		right = -left;
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    //gluPerspective(beta/PI*180, m_aspect, m_clipnear, m_clipfar);
	glFrustum(left, right, bottom, top, m_clipnear, m_clipfar);
	
//calculate view frustum
// left
	m_plane[0].a = -e/sqrt(e*e+1); m_plane[0].b =0; m_plane[0].c = 1/sqrt(e*e+1); m_plane[0].d = 0;
// right
	m_plane[1].a = e/sqrt(e*e+1); m_plane[1].b =0; m_plane[1].c = 1/sqrt(e*e+1); m_plane[1].d = 0;
// bottom
	m_plane[2].a = 0; m_plane[2].b =e/sqrt(e*e+a*a); m_plane[2].c = a/sqrt(e*e+a*a); m_plane[2].d = 0;
// top
	m_plane[3].a = 0; m_plane[3].b =-e/sqrt(e*e+a*a); m_plane[3].c = a/sqrt(e*e+a*a); m_plane[3].d = 0;
	
	glMatrixMode(GL_MODELVIEW);
}

void gPerspCamera::setSoftViewPort(double fov, int w, int h )
{
	m_width = w;
	if(h==0) h=1;
	m_height = h;
	
	m_aspect = (float)m_width/(float)m_height;
	
	m_fov = fov;
	
	double e = 1.0/tan(m_fov/360*PI);
	double a = 1.0/m_aspect;
	double beta = 2.0*atan(a/e);
	GLdouble left,right,bottom,top;
		
		top = tan(beta/2) * m_clipnear;
		bottom = -top;
		left = m_aspect * bottom;
		right = -left;
	
//calculate view frustum
// left
	m_plane[0].a = -e/sqrt(e*e+1); m_plane[0].b =0; m_plane[0].c = 1/sqrt(e*e+1); m_plane[0].d = 0;
// right
	m_plane[1].a = e/sqrt(e*e+1); m_plane[1].b =0; m_plane[1].c = 1/sqrt(e*e+1); m_plane[1].d = 0;
// bottom
	m_plane[2].a = 0; m_plane[2].b =e/sqrt(e*e+a*a); m_plane[2].c = a/sqrt(e*e+a*a); m_plane[2].d = 0;
// top
	m_plane[3].a = 0; m_plane[3].b =-e/sqrt(e*e+a*a); m_plane[3].c = a/sqrt(e*e+a*a); m_plane[3].d = 0;
}

void gPerspCamera::setCropViewPort( int wnd_w, int wnd_h, int crop_x, int crop_y)
{
	m_width = wnd_w;
	m_height = wnd_h;
	if(wnd_h==0) m_height=1;
	
	m_aspect = (float)m_width/(float)m_height;
	
	double e = 1.0/tan(m_fov/360*PI);
	double a = 1.0/m_aspect;
	double beta = 2.0*atan(a/e);
	GLdouble left,right,bottom,top;
		
		top = tan(beta/2) * m_clipnear;
		bottom = -top;
		left = m_aspect * bottom;
		right = -left;
	
	glViewport(0, 0, wnd_w, wnd_h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	if(wnd_w <= 1024 && wnd_h <= 1024) 
	{
		
		//gluPerspective(beta/PI*180, m_aspect, m_clipnear, m_clipfar);
		glFrustum(left, right, bottom, top, m_clipnear, m_auto_far);
	}
	else 
	{
		//glViewport(0, 0, wnd_w, wnd_h);
		/*GLdouble _left,_right,_bottom,_top, offx, offy;
		
		_top = tan(beta/2) * m_clipnear;
		_bottom = -_top;
		_left = m_aspect * _bottom;
		_right = -_left;*/
		
		GLdouble offx = (double)crop_x/(double)wnd_w*(right-left);
		GLdouble offy = (double)crop_y/(double)wnd_h*(top-bottom);

		glFrustum(left+offx, right+offx, bottom+offy, top+offy, m_clipnear, m_auto_far);
    }
	
//calculate view frustum
// left
	m_plane[0].a = -e/sqrt(e*e+1); m_plane[0].b =0; m_plane[0].c = 1/sqrt(e*e+1); m_plane[0].d = 0;
// right
	m_plane[1].a = e/sqrt(e*e+1); m_plane[1].b =0; m_plane[1].c = 1/sqrt(e*e+1); m_plane[1].d = 0;
// bottom
	m_plane[2].a = 0; m_plane[2].b =e/sqrt(e*e+a*a); m_plane[2].c = a/sqrt(e*e+a*a); m_plane[2].d = 0;
// top
	m_plane[3].a = 0; m_plane[3].b =-e/sqrt(e*e+a*a); m_plane[3].c = a/sqrt(e*e+a*a); m_plane[3].d = 0;
	
	glMatrixMode(GL_MODELVIEW);
}

void gPerspCamera::update(XYZ& e, XYZ& s, XYZ& u, XYZ& f)
{
	glLoadIdentity();
    //
		
		gluLookAt(e.x, e.y,e.z,
		  e.x+f.x, e.y+f.y, e.z+f.z,
		  u.x, u.y, u.z);
}

void gPerspCamera::update()
{

    glLoadIdentity();
    //
		
		gluLookAt(m_eye.x, m_eye.y, m_eye.z,
		  m_view.x, m_view.y, m_view.z,
		  up.x, up.y, up.z);
}

void gPerspCamera::track( float x, float y )
{
	XYZ up_n = up.normal();
	XYZ side_n = (up^I).normal();
	side = side_n;
	
	float fdist = I.length()/256.0f;

	
	XYZ dv = up_n*y*fdist + side_n*x*fdist;

	m_eye = m_eye + dv;
	m_view = m_view + dv;
	
	updateSpace();
}

void gPerspCamera::tumble( float x, float y )
{
	up.normalize();
	side = up^I;
	side.normalize();
	
	float fdist = I.length();
	
	XYZ dv = up*(y*fdist/128.0f) + side*(x*fdist/128.0f);
	XYZ dI = dv - I; dI.normalize();

	m_eye = m_view + dI*fdist;
	I = m_view - m_eye;
	In = I.normal();
	
	side = up^I;
	side.normalize();
	up = I^side;
	up.normalize();
	
	updateSpace();
}

void gPerspCamera::dolly( float z )
{
	
	float fdist = I.length();
	float dz = z*fdist/256.0f;

	if( dz > fdist ) dz = fdist - 0.1f;
	m_eye = m_eye + I.normal() * dz ;
	I = m_view - m_eye;
	In = I.normal();
	
	updateSpace();
	
}

void gPerspCamera::lookAt(float m_eyeX, float m_eyeY, float m_eyeZ,
			float m_viewX,     float m_viewY,     float m_viewZ,
			float upX, float upY, float upZ)
{
	m_eye = XYZ(m_eyeX, m_eyeY, m_eyeZ);
	m_view = XYZ(m_viewX, m_viewY, m_viewZ);
	up   = XYZ(upX, upY, upZ);
	I = m_view - m_eye;
	In = I.normal();
}

void gPerspCamera::viewAll( XYZ center, float width )
{
	m_view = center;
	
	float dist = width;
	
	dist = (dist+dist/2.0f)*0.5f/tan(m_fov*0.5f/180.0f*3.1415927f);
	
	I = XYZ(0.0f, 0.0f, -dist);
	In = I.normal();
	m_eye = m_view - I;
	up = XYZ(0.0, 1.0, 0.0);
	side.x = 1.0f;
	side.y = side.z = 0.0f;
	updateSpace();
}

void gPerspCamera::setMatrix(XYZ& e, XYZ& s, XYZ& u, XYZ& f)
{
	m_eye = e;
	m_view = e + f;
	up = u;
	f.normalize();
	u.normalize();
	s.normalize();
	
	m_space.v[3][3]=1; m_space.v[0][3]=m_space.v[1][3]=m_space.v[2][3]=0;
	m_space.v[0][0] = s.x; m_space.v[0][1] = s.y; m_space.v[0][2] = s.z; 
	m_space.v[1][0] = u.x; m_space.v[1][1] = u.y; m_space.v[1][2] = u.z; 
	m_space.v[2][0] = f.x; m_space.v[2][1] = f.y; m_space.v[2][2] = f.z; 
	m_space.v[3][0] = e.x; m_space.v[3][1] = e.y; m_space.v[3][2] = e.z; 
	
	m_spaceinv = m_space; m_spaceinv.inverse();
	
	V = m_eye - m_view; V.normalize();
}

void gPerspCamera::updateSpace()
{
	XYZ f = I; f.normalize();
	XYZ u = up; u.normalize();
	XYZ s = side; s.normalize();
	
	m_space.v[3][3]=1; m_space.v[0][3]=m_space.v[1][3]=m_space.v[2][3]=0;
	m_space.v[0][0] = s.x; m_space.v[0][1] = s.y; m_space.v[0][2] = s.z; 
	m_space.v[1][0] = u.x; m_space.v[1][1] = u.y; m_space.v[1][2] = u.z; 
	m_space.v[2][0] = f.x; m_space.v[2][1] = f.y; m_space.v[2][2] = f.z; 
	m_space.v[3][0] = m_eye.x; m_space.v[3][1] = m_eye.y; m_space.v[3][2] = m_eye.z; 
	
	m_spaceinv = m_space; m_spaceinv.inverse();
}

void gPerspCamera::getGLMatrixFull(float* mat)
{
	mat[0]  = m_space.v[0][0];  mat[1] =  m_space.v[0][1]; mat[2]  = m_space.v[0][2]; mat[3]  = m_space.v[0][3];
	mat[4]  = m_space.v[1][0];  mat[5] =  m_space.v[1][1]; mat[6]  = m_space.v[1][2]; mat[7]  = m_space.v[1][3];
	mat[8]  = m_space.v[2][0];  mat[9] =  m_space.v[2][1]; mat[10] = m_space.v[2][2]; mat[11] = m_space.v[2][3];
	mat[12] = m_space.v[3][0];  mat[13] = m_space.v[3][1]; mat[14] = m_space.v[3][2]; mat[15] = m_space.v[3][3];
}

void gPerspCamera::getGLMatrixRotation(float* mat)
{
	mat[0]  = m_space.v[0][0];  mat[1] =  m_space.v[0][1]; mat[2]  = m_space.v[0][2];
	mat[4]  = m_space.v[1][0];  mat[5] =  m_space.v[1][1]; mat[6]  = m_space.v[1][2];
	mat[8]  = m_space.v[2][0];  mat[9] =  m_space.v[2][1]; mat[10] = m_space.v[2][2]; 
	mat[3]  = mat[7] = mat[11] = 0; mat[15] = 1.0f;
}

void gPerspCamera::getHUDPosUpLeft(XYZ& pw, float height)
{
	float depth = (float)m_clipfar-0.01f;
	float hhalf = depth*tan(m_fov/2/180.0f*PI);
	float vhalf = hhalf/m_aspect;
	
	pw.x = hhalf*0.95f; pw.y = vhalf*0.95f*height; pw.z = m_clipfar-0.01f;
	
	m_space.transform(pw);
}

void gPerspCamera::getRasterPosition(XYZ& pw)
{
	float depth = (float)m_clipfar*0.99;
	float hhalf = depth*tan(m_fov/2/180.0f*PI);
	float vhalf = hhalf/m_aspect;
	
	pw.x = hhalf*.99999; pw.y = -vhalf*.99999; pw.z = depth;
	
	m_space.transform(pw);

}

int gPerspCamera::isInViewFrustum(const qnode& data, float threshold)
{
	float rr = sqrt(data.area)*2;
	
	XYZ node2eye = m_eye - data.pos;
	node2eye.normalize();
	// backside: skip
	if( data.nor.dot(node2eye) < data.cone-1.2 ) return -1;
	
	XYZ q(data.pos);
	m_spaceinv.transform(q);
	
	if(q.z > m_clipfar+ rr) return -1;
	if(q.z < m_clipnear-rr) return -1;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
	// outside: skip
	if(dist[0] > rr || dist[1] > rr || dist[2] > rr || dist[3] > rr ) return -1;
	
	// too big or too much normal scattering: need split
	if( rr/2*(float)m_width > threshold*q.z*tan(m_fov/2/180.0f*PI) ) return 0;

	return 1;
}

int gPerspCamera::isInViewFrustum(const qnode& data, float threshold, float left, float right, float bottom, float top)
{
	float rr = sqrt(data.area)*2;
	
	XYZ node2eye = m_eye - data.pos;
	node2eye.normalize();
	// backside: skip
	if( data.nor.dot(node2eye) < data.cone-1.2 ) return -1;
	
	XYZ q(data.pos);
	m_spaceinv.transform(q);
	
	if(q.z > m_clipfar+ rr) return -1;
	if(q.z < m_clipnear -rr) return -1;
		
	float dist[4];
	for(int i=0; i<4; i++)
	{
		dist[i] = -(m_plane[i].a * q.x + m_plane[i].b * q.y + m_plane[i].c * q.z + m_plane[i].d)/(m_plane[i].a * m_plane[i].a + m_plane[i].b * m_plane[i].b + m_plane[i].c * m_plane[i].c);
	}
	
// outside frustum: skip
	if(dist[0] > rr || dist[1] > rr || dist[2] > rr || dist[3] > rr ) return -1;

// outside crop: skip
	float hwide = q.z*tan(m_fov/2/180.0f*PI);
	float xcoord = (-q.x/hwide + 1)/2.0;
	float ycoord = (q.y/(hwide/m_aspect) + 1)/2.0;
	float dw = rr/hwide;
	
	if(xcoord<left-dw) return -1;
	if(xcoord>right+dw) return -1;
	if(ycoord<bottom-dw) return -1;
	if(ycoord>top+dw) return -1;
		
	// too big or too much normal scattering: need split
	if( rr/2*(float)m_width > threshold*q.z*tan(m_fov/2/180.0f*PI) ) return 0;

	return 1;
}

void gPerspCamera::getCameraP(const qnode& data, float& dist, float& facingforward)
{
	XYZ q = data.pos;
	m_spaceinv.transform(q);
	dist = q.length();
	
	facingforward = dist/m_auto_far;
/*	
	XYZ n(data.nor);
	m_spaceinv.transformAsNormal(n);
	n.normalize();
	
	XYZ node2eye = m_eye - data.pos;
	node2eye.normalize();

	facingforward =  node2eye.dot(n);
	
	double hhalf = q.z*tan(m_fov/2/180.0f*PI);
	double xcoord = -q.x/hhalf;
	if(xcoord>1) xcoord=1;
	else if(xcoord<-1) xcoord=-1;
	xcoord = (xcoord+1)/2;
	
	double ycoord = q.y/(hhalf/m_aspect);
	if(ycoord>1) ycoord=1;
	else if(ycoord<-1) ycoord=-1;
	ycoord = (ycoord+1)/2;
*/	
//	int icoord = (m_width-1)*xcoord;
//	int jcoord = (m_height-1)*(1-ycoord);
	
	//MGlobal::displayInfo(MString(" ")+dist+ " "+icoord + " " +jcoord);
}

unsigned int gPerspCamera::depthTest(int id, qnode* data, int* buffer, float& occ, float& ratio)
{
	XYZ q = data[id].pos;
	m_spaceinv.transform(q);
	float depth = q.length();
	
	ratio = depth/m_auto_far;
	
	double hhalf = q.z*tan(m_fov/2/180.0f*PI);
	double xcoord = -q.x/hhalf;
	if(xcoord>1) xcoord=1;
	else if(xcoord<-1) xcoord=-1;
	xcoord = (xcoord+1)/2;
	
	double ycoord = q.y/(hhalf/m_aspect);
	if(ycoord>1) ycoord=1;
	else if(ycoord<-1) ycoord=-1;
	ycoord = (ycoord+1)/2;
	
	int icoord = (m_width-1)*xcoord;
	int jcoord = (m_height-1)*ycoord;
/*	
	XYZ node2eye = m_eye - data.pos;
	node2eye.normalize();
	float edn = node2eye.dot(data.nor);
	if(edn<0.1) edn=0.1;
*/
//MGlobal::displayInfo(MString(" ")+buffer[(m_width*jcoord+icoord)*2]+ " "+icoord + " " +jcoord);

// empty to write
	if(buffer[m_width*jcoord+icoord] < 0) 
	{
		buffer[m_width*jcoord+icoord] = id;
		return 1;
	}
	
	XYZ bp = data[buffer[m_width*jcoord+icoord]].pos;
	m_spaceinv.transform(bp);
	XYZ bn = data[buffer[m_width*jcoord+icoord]].nor;
	m_spaceinv.transformAsNormal(bn);
	
// discard 
	if( /*depth - bp.length() > 2*sqrt(data[id].area) && */bn.z < -0.5f) 
	{
//		occ = data[buffer[m_width*jcoord+icoord]].col.x;
		return 0;
	}
	buffer[m_width*jcoord+icoord] = id;
	return 1;
}

unsigned int gPerspCamera::depthTest(const qnode& data, float* buffer)
{
	XYZ q = data.pos;
	m_spaceinv.transform(q);
	float depth = q.length();
	
	double hhalf = q.z*tan(m_fov/2/180.0f*PI);
	double xcoord = -q.x/hhalf;
	if(xcoord>1) xcoord=1;
	else if(xcoord<-1) xcoord=-1;
	xcoord = (xcoord+1)/2;
	
	double ycoord = q.y/(hhalf/m_aspect);
	if(ycoord>1) ycoord=1;
	else if(ycoord<-1) ycoord=-1;
	ycoord = (ycoord+1)/2;
	
	int icoord = (m_width-1)*xcoord;
	int jcoord = (m_height-1)*ycoord;
/*	
	XYZ node2eye = m_eye - data.pos;
	node2eye.normalize();
	float edn = node2eye.dot(data.nor);
	if(edn<0.1) edn=0.1;
*/
//MGlobal::displayInfo(MString(" ")+buffer[(m_width*jcoord+icoord)*2]+ " "+icoord + " " +jcoord);

// empty to write
	if(buffer[m_width*jcoord+icoord]<0.1) 
	{
		buffer[m_width*jcoord+icoord] = depth;
		return 1;
	}
	
// discard 
	if( depth - buffer[m_width*jcoord+icoord] > 2*sqrt(data.area) ) 
	{
		return 0;
	}

	return 1;
}

void gPerspCamera::writeToBuffer(const qnode& data, float* buffer, float value)
{
	XYZ q(data.pos);
	m_spaceinv.transform(q);
	
	double hhalf = q.z*tan(m_fov/2/180.0f*PI);
	double xcoord = -q.x/hhalf;
	if(xcoord>1) xcoord=1;
	else if(xcoord<-1) xcoord=-1;
	xcoord = (xcoord+1)/2;
	
	double ycoord = q.y/(hhalf/m_aspect);
	if(ycoord>1) ycoord=1;
	else if(ycoord<-1) ycoord=-1;
	ycoord = (ycoord+1)/2;
	
	int icoord = (m_width-1)*xcoord;
	int jcoord = (m_height-1)*ycoord;

	buffer[m_width*jcoord+icoord] = value;
}

void gPerspCamera::readFromBuffer(const qnode& data, float* buffer, float& value)
{
	XYZ q(data.pos);
	m_spaceinv.transform(q);
	
	double hhalf = q.z*tan(m_fov/2/180.0f*PI);
	double xcoord = -q.x/hhalf;
	if(xcoord>1) xcoord=1;
	else if(xcoord<-1) xcoord=-1;
	xcoord = (xcoord+1)/2;
	
	double ycoord = q.y/(hhalf/m_aspect);
	if(ycoord>1) ycoord=1;
	else if(ycoord<-1) ycoord=-1;
	ycoord = (ycoord+1)/2;
	
	int icoord = (m_width-1)*xcoord;
	int jcoord = (m_height-1)*ycoord;

	value = buffer[m_width*jcoord+icoord];
}

float gPerspCamera::getDepth(const XYZ& pw)
{
	XYZ q(pw);
	m_spaceinv.transform(q);
	return q.z;
}

void gPerspCamera::calculateClips(XYZ* bbox)
{
	m_auto_far = 10.0f;
	for(int i=0; i<8; i++)
	{
		XYZ q = bbox[i];
		m_spaceinv.transform(q);
		if(q.z > m_auto_far) m_auto_far = q.z;
	}
}

void gPerspCamera::calculateClips(const qnode& data)
{
	float rr = sqrt(data.area)*2;
	XYZ q(data.pos);
	m_spaceinv.transform(q);
	if(q.z+rr > m_auto_far) m_auto_far = q.z+rr;
}
//:~