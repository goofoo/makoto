/*
//
//  0  4  8 12
//  1  5  9 13
//  2  6 10 14
//  3  7 11 15
//
// 1 0 0 x
// 0 1 0 y
// 0 0 1 z
// 0 0 0 1
//
*/


//#include "stdafx.h"
#include "gMatrix44d.h"

gMatrix44d::gMatrix44d()
{

	
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	
	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;
	
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;
	
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
	
}


gMatrix44d::~gMatrix44d()
{
}

double * gMatrix44d::matrix()
{
	return m;
}

double * gMatrix44d::inverseMatrix()
{
	
	double det = m[0] * determinant33( m[5], m[9], m[13], m[6], m[10], m[14], m[7], m[11], m[15] )
		- m[4] * determinant33( m[1], m[9], m[13], m[2], m[10], m[14], m[3], m[11], m[15] )
		+ m[8] * determinant33( m[1], m[5], m[13], m[2], m[6], m[14], m[3], m[7], m[15] )
		- m[12] * determinant33( m[1], m[5], m[9], m[2], m[6], m[10], m[3], m[7], m[11] );
	
	mi[0] = determinant33( m[5], m[9], m[13], m[6], m[10], m[14], m[7], m[11], m[15] ) / det;
	mi[4] = - determinant33( m[4], m[8], m[12], m[6], m[10], m[14], m[7], m[11], m[15] ) / det;
	mi[8] = determinant33( m[4], m[8], m[12], m[5], m[9], m[13], m[7], m[11], m[15] ) / det;
	mi[12] = - determinant33( m[4], m[8], m[12], m[5], m[9], m[13], m[6], m[10], m[14] ) / det;
	
	mi[1] = - determinant33( m[1], m[9], m[13], m[2], m[10], m[14], m[3], m[11], m[15] ) / det;
	mi[5] = determinant33( m[0], m[8], m[12], m[2], m[10], m[14], m[3], m[11], m[15] ) / det;
	mi[9] = - determinant33( m[0], m[8], m[12], m[1], m[9], m[13], m[3], m[11], m[15] ) / det;
	mi[13] = determinant33( m[0], m[8], m[12], m[1], m[9], m[13], m[2], m[10], m[14] ) / det;
	
	mi[2] = determinant33( m[1], m[5], m[13], m[2], m[6], m[14], m[3], m[7], m[15] ) / det;
	mi[6] = - determinant33( m[0], m[4], m[12], m[2], m[6], m[14], m[3], m[7], m[15] ) / det;
	mi[10] = determinant33( m[0], m[4], m[12], m[1], m[5], m[13], m[3], m[7], m[15] ) / det;
	mi[14] = - determinant33( m[0], m[4], m[12], m[1], m[5], m[13], m[2], m[6], m[14] ) / det;
	
	mi[3] = - determinant33( m[1], m[5], m[9], m[2], m[6], m[10], m[3], m[7], m[11] ) / det;
	mi[7] = determinant33( m[0], m[4], m[8], m[2], m[6], m[10], m[3], m[7], m[11] ) / det;
	mi[11] = - determinant33( m[0], m[4], m[8], m[1], m[5], m[9], m[3], m[7], m[11] ) / det;
	mi[15] = determinant33( m[0], m[4], m[8], m[1], m[5], m[9], m[2], m[6], m[10] ) / det;
	
	return mi;
}

void gMatrix44d::setOrientation(gVector3d right, gVector3d up, gVector3d front)
{
	m[0] = right.normal().x;
	m[1] = right.normal().y;
	m[2] = right.normal().z;

	m[4] = up.normal().x;
	m[5] = up.normal().y;
	m[6] = up.normal().z;

	m[8] = front.normal().x;
	m[9] = front.normal().y;
	m[10] = front.normal().z;
}

void gMatrix44d::setTranslation( gVector3d position )
{
	m[12] = position.x;
	m[13] = position.y;
	m[14] = position.z;
}

double gMatrix44d::determinant33( double a, double b, double c, double d, double e, double f, double g, double h, double i )
{
	return double( a*( e*i - h*f ) - b*( d*i - g*f ) + c*( d*h - g*e ) );
}
