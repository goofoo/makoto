#include "zMath.h"
#include "zFMatrix44f.h"

zFMatrix44f::zFMatrix44f() {}
zFMatrix44f::~zFMatrix44f() {}

void zFMatrix44f::reset(MATRIX44F& mat)
{
	mat.m11 = 1.0f;
	mat.m12 = 0.0f;
	mat.m13 = 0.0f;
	mat.m14 = 0.0f;
	
	mat.m21 = 0.0f;
	mat.m22 = 1.0f;
	mat.m23 = 0.0f;
	mat.m24 = 0.0f;
	
	mat.m31 = 0.0f;
	mat.m32 = 0.0f;
	mat.m33 = 1.0f;
	mat.m34 = 0.0f;
	
	mat.m41 = 0.0f;
	mat.m42 = 0.0f;
	mat.m43 = 0.0f;
	mat.m44 = 1.0f;
}

void zFMatrix44f::rotateY(MATRIX44F& mat, float angle)
{
	MATRIX44F tmp;
	reset(tmp);
	tmp.m11 = cos(angle);
	tmp.m13 = -sin(angle);
	tmp.m31 = sin(angle);
	tmp.m33 = cos(angle);
	
	multiply(mat, tmp);
}

void zFMatrix44f::multiply(MATRIX44F& mat, const MATRIX44F& other)
{
	MATRIX44F tmp = mat;
	mat.m11 = tmp.m11* other.m11 + tmp.m12* other.m21 + tmp.m13*other.m31 + tmp.m14*other.m41;
	mat.m12 = tmp.m11* other.m12 + tmp.m12* other.m22 + tmp.m13*other.m32 + tmp.m14*other.m42;
	mat.m13 = tmp.m11* other.m13 + tmp.m12* other.m23 + tmp.m13*other.m33 + tmp.m14*other.m43;
	mat.m14 = tmp.m11* other.m14 + tmp.m12* other.m24 + tmp.m13*other.m34 + tmp.m14*other.m44;
	
	mat.m21 = tmp.m21* other.m11 + tmp.m22* other.m21 + tmp.m23*other.m31 + tmp.m24*other.m41;
	mat.m22 = tmp.m21* other.m12 + tmp.m22* other.m22 + tmp.m23*other.m32 + tmp.m24*other.m42;
	mat.m23 = tmp.m21* other.m13 + tmp.m22* other.m23 + tmp.m23*other.m33 + tmp.m24*other.m43;
	mat.m24 = tmp.m21* other.m14 + tmp.m22* other.m24 + tmp.m23*other.m34 + tmp.m24*other.m44;
	
	mat.m31 = tmp.m31* other.m11 + tmp.m32* other.m21 + tmp.m33*other.m31 + tmp.m34*other.m41;
	mat.m32 = tmp.m31* other.m12 + tmp.m32* other.m22 + tmp.m33*other.m32 + tmp.m34*other.m42;
	mat.m33 = tmp.m31* other.m13 + tmp.m32* other.m23 + tmp.m33*other.m33 + tmp.m34*other.m43;
	mat.m34 = tmp.m31* other.m14 + tmp.m32* other.m24 + tmp.m33*other.m34 + tmp.m34*other.m44;
	
	mat.m41 = tmp.m41* other.m11 + tmp.m42* other.m21 + tmp.m43*other.m31 + tmp.m44*other.m41;
	mat.m42 = tmp.m41* other.m12 + tmp.m42* other.m22 + tmp.m43*other.m32 + tmp.m44*other.m42;
	mat.m43 = tmp.m41* other.m13 + tmp.m42* other.m23 + tmp.m43*other.m33 + tmp.m44*other.m43;
	mat.m44 = tmp.m41* other.m14 + tmp.m42* other.m24 + tmp.m43*other.m34 + tmp.m44*other.m44;
}

void zFMatrix44f::transform(XYZ& p, const MATRIX44F& mat)
{
	float x = p.x;
	float y = p.y;
	float z = p.z;
	p.x = x*mat.m11 + y*mat.m21 + z*mat.m31 + mat.m41;
	p.y = x*mat.m12 + y*mat.m22 + z*mat.m32 + mat.m42;
	p.z = x*mat.m13 + y*mat.m23 + z*mat.m33 + mat.m43;
}

void zFMatrix44f::ntransform(XYZ& n, const MATRIX44F& mat)
{
	float x = n.x;
	float y = n.y;
	float z = n.z;
	n.x = x*mat.m11 + y*mat.m21 + z*mat.m31;
	n.y = x*mat.m12 + y*mat.m22 + z*mat.m32;
	n.z = x*mat.m13 + y*mat.m23 + z*mat.m33;
}

void zFMatrix44f::translate(MATRIX44F& mat, float x, float y, float z)
{
	mat.m41 += x;
	mat.m42 += y;
	mat.m43 += z;
}

void zFMatrix44f::vnormalize(XYZ& v)
{
	float leng = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x /= leng;
	v.y /= leng;
	v.z /= leng;
}

float zFMatrix44f::vlength(const XYZ& v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

XYZ zFMatrix44f::vcross(const XYZ& vec, const XYZ& other) 
{
	XYZ out;	
	// Calculate the cross product with the non communitive equation
	out.x = vec.y * other.z - vec.z * other.y;
	out.y = vec.z * other.x - vec.x * other.z;
	out.z = vec.x * other.y - vec.y * other.x;
	return out;
}

void zFMatrix44f::setOrientation(MATRIX44F& mat, const XYZ& right, const XYZ& up, const XYZ& front)
{
	mat.m11 = right.x;
	mat.m12 = right.y;
	mat.m13 = right.z;

	mat.m21 = up.x;
	mat.m22 = up.y;
	mat.m23 = up.z;

	mat.m31 = front.x;
	mat.m32 = front.y;
	mat.m33 = front.z;
}

void zFMatrix44f::setOrientationByVector(MATRIX44F& mat, XYZ& v)
{
	if(vlength(v)<0.01)
	{
		v.x = 0.0f;
		v.y = 0.0f;
		v.z = 1.0f;
	}
	vnormalize(v);
	
	XYZ up;
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	
	/*if(abs(v.y)>0.999)
	{
		up.x = 0.0f;
		up.y = 0.0f;
		up.z = -1.0f;
	}*/
	
	XYZ side = vcross(v, up);
	
	vnormalize(side);
	up = vcross(side, v);
	vnormalize(up);
	
	setOrientation(mat, side, up, v);
}

void zFMatrix44f::setOrientationByXYZ(MATRIX44F& mat, float x, float y, float z)
{
	XYZ v;
	v.x = x;
	v.y = y;
	v.z = z;
	setOrientationByVector(mat, v);
}

void zFMatrix44f::scale(MATRIX44F& mat, float scale)
{
	MATRIX44F tmp;
	reset(tmp);
	tmp.m11 = scale;
	tmp.m22 = scale;
	tmp.m33 = scale;
	
	multiply(mat, tmp);
}

//:~