//#include "stdafx.h"
#include "gVector3f.h"
#include "math.h"


gVector3f::gVector3f()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;	
}
		
gVector3f::gVector3f(float xx, float yy, float zz) 
{
	x = xx; y = yy; z = zz;
}

gVector3f::gVector3f(XYZ& xyz)
{
	x = xyz.x; y = xyz.y; z = xyz.z;
}

gVector3f::gVector3f(XYZ& end, XYZ& start)
{
	x= end.x - start.x;
	y= end.y - start.y;
	z= end.z - start.z;
}

float gVector3f::dot(gVector3f other) 
{
	return (float)(x * other.x + y * other.y + z * other.z);
}
		
gVector3f gVector3f::cross(gVector3f other) 
{
	gVector3f normal;	
	// Calculate the cross product with the non communitive equation
	normal.x = y * other.z - z * other.y;
	normal.y = z * other.x - x * other.z;
	normal.z = x * other.y - y * other.x;
		
	return normal;
}
		
float gVector3f::length() 
{
	return (float)sqrt( x * x + y * y + z * z );
}

gVector3f gVector3f::normal() 
{
	float mag = (float)sqrt( x * x + y * y + z * z );
	return gVector3f(x / mag, y / mag, z / mag);
}

void gVector3f::normalize()
{
	float mag = (float)sqrt( x * x + y * y + z * z );
	x /= mag;
	y /= mag;
	z /= mag;
}

gVector3f gVector3f::transformAsNormal(float* mat)
{
	float bx = x*mat[0] + y*mat[4] + z*mat[8];
	float by = x*mat[1] + y*mat[5] + z*mat[9];
	float bz = x*mat[2] + y*mat[6] + z*mat[10];
	
	float mag = (float)sqrt( bx * bx + by * by + bz * bz );
					
	return gVector3f(bx/mag, by/mag, bz/mag);
}


gVector3f gVector3f::reverse()
{
	return gVector3f( -x, -y, -z );
}

gVector3f gVector3f::refract(gVector3f& N, float eta)
{
	float IdotN = x * N.x + y * N.y + z * N.z;
	float k = 1 - eta*eta*(1 - IdotN*IdotN);

	return k < 0 ? gVector3f(0.,0.,0.) : gVector3f(x*eta, y*eta, z*eta) - N*(eta*IdotN + sqrt(k));
}


		

		



