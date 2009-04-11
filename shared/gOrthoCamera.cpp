#include "gOrthoCamera.h"

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

#include "zMath.h"


gOrthoCamera::gOrthoCamera()
{

	depth = 8.0f;
	_depth = 8.0f;
	
	eye = XYZ(0.5, 0.5, depth);
	I = XYZ(0.0, 0.0, -depth);
	view = eye + I;
	up = XYZ(0.0, 1.0, 0.0);
	side = XYZ(1.0, 0.0, 0.0);
	
	fov = 1.0f;
	_fov = 1.0f;
	
	clip_near = 0.1;
	clip_far = 1000.0;
}

void gOrthoCamera::reset()
{
	
	eye = XYZ(0.5, 0.5, _depth);
	I = XYZ(0.0, 0.0, -_depth);
	view = eye + I;
	up   = XYZ(0.0, 1.0, 0.0);
	side = XYZ(1.0, 0.0, 0.0);
	
	fov = _fov;
	

	right = fov/2.0;
	top = right/aspect;
	left = -right;
	bottom = -top;
		
		

}

void gOrthoCamera::setViewPort( int w, int h )
{

	width = w;
	if ( h==0) h=1;
	height = h;
	
	glViewport(0, 0, width, height);
	
	aspect = (float)width/(float)height;
	
	right = fov/2.0;
	
	top = right/aspect;
	left = -right;
	bottom = -top;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(left,right,bottom,top,clip_near,clip_far);
	
	glMatrixMode(GL_MODELVIEW);
}



void gOrthoCamera::update()
{
	glLoadIdentity();
	
	gluLookAt(eye.x, eye.y, eye.z,
			  view.x, view.y, view.z,
			  up.x, up.y, up.z);
}

void gOrthoCamera::track( float x, float y )
{
	XYZ up_n = up.normal();
	XYZ side_n = (up^I).normal();
	side = side_n;
	
	double fdist = fov/512.0f;
	
	XYZ dv = up_n*y*fdist + side_n*x*fdist;

	eye = eye + dv;
	view = view + dv;
	
}

void gOrthoCamera::tumble( float x, float y )
{
	XYZ up_n = up.normal();
	XYZ side_n = up.cross(I).normal();
	side = side_n;
	
	double fdist = I.length();
	
	XYZ dv = up_n*y*fdist/128.0 + side_n*x*fdist/128.0;
	XYZ dI = dv - I;

	eye = view + dI.normal()*fdist;
	I = view - eye;
	up = I.cross(side_n);
	up.normalize();
}

void gOrthoCamera::dolly( float z )
{

	fov -= z/64.0;
	if(fov<0.01) fov = 0.01;
	right = fov/2.0;
	top = right/aspect;
	left = -right;
	bottom = -top;

}
//:~
