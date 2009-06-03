#ifndef ZDATA_H
#define ZDATA_H
#include <iostream>
#include <math.h>
#define PI 3.14159265358979323846

struct XY
{
	XY() {}
	XY(float& _x, float& _y):x(_x), y(_y) {}
	float x,y;
};

struct XYZ
{
	XYZ() {}
	XYZ(const float v) {x= y= z=v;}
	XYZ(const int v) {x= y= z=(float)v;}
	XYZ(const float* p) {x= p[0]; y=p[1]; z=p[2];}
	XYZ(const float& _x, const float& _y, const float& _z):x(_x), y(_y), z(_z) {}
	XYZ(const XYZ& from, const XYZ& to) 
	{
		x = to.x - from.x; 
		y = to.y - from.y; 
		z = to.z - from.z; 
	}
	XYZ(float* p) 
	{
		x = p[0];
		y = p[1];
		z = p[2];
	}
	
	void operator+=( const XYZ& other )
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}
	
	void operator-=( const XYZ& other )
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}
	
	void operator/=( int& other )
	{
		x /= other;
		y /= other;
		z /= other;
	}
	
	void operator/=( float& other )
	{
		x /= other;
		y /= other;
		z /= other;
	}
	
	void operator/=( const float other )
	{
		x /= other;
		y /= other;
		z /= other;
	}
	
	void operator*=( float& other )
	{
		x *= other;
		y *= other;
		z *= other;
	}
	
	void operator*=( double& other )
	{
		x *= (float)other;
		y *= (float)other;
		z *= (float)other;
	}
	
	void operator*=( const double other )
	{
		x *= (float)other;
		y *= (float)other;
		z *= (float)other;
	}
	
	XYZ operator+( const XYZ& other ) const
	{
		return XYZ(x+other.x, y+other.y, z+other.z);
	}
	
	XYZ operator-( const XYZ& other ) const
	{
		return XYZ(x-other.x, y-other.y, z-other.z);
	}
	
	XYZ operator*( const XYZ& other ) const
	{
		return XYZ(x*other.x, y*other.y, z*other.z);
	}
	
	XYZ operator*( float scale ) const
	{
		return XYZ(x*scale,y*scale,z*scale);
	}
	
	XYZ operator/( float scale ) const
	{
		return XYZ(x/scale,y/scale,z/scale);
	}
	
	double length() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	
	float lengthSquare() const
	{
		return (x*x + y*y + z*z);
	}
	
	double maxvalue() const
	{
		if(x>y && x>z) return x;
		else if(y>x && y>z) return y;
		else return z;
	}
	
	float dot(const XYZ& other) const
	{
		return ( x*other.x + y*other.y + z*other.z);
	}
	
	XYZ cross(const XYZ& other) const
	{
		return XYZ(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}
	
	void normalize()
	{
		double len = length();
		if(len > 10e-8)
		{
		x /= (float)len;
		y /= (float)len;
		z /= (float)len;
		}
		else
		{
			x = y = z = 0.577350f;
		}
	}
	
	XYZ normal() const
	{
		double mag = sqrt( x * x + y * y + z * z ) + 10e-8;
		return XYZ(x /(float)mag, y /(float)mag, z /(float)mag);
	}
	
	XYZ operator^(const XYZ& other) const
	{
		return XYZ(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}
	
	void zero() { x=y=z=0.0f;}
	
	float distanceSquaredTo(const XYZ& other) const
	{
		return ((other.x-x)*(other.x-x) + (other.y-y)*(other.y-y) + (other.z-z)*(other.z-z));
	}
	
	float distanceTo(const XYZ& other) const
	{
		return sqrt((other.x-x)*(other.x-x) + (other.y-y)*(other.y-y) + (other.z-z)*(other.z-z));
	}
	
	float operator[](int i) const
	{
		if(i==0) return x;
		else if(i==1) return y;
		else return z;
	}
	
	void perpendicular(XYZ& other) const
	{
		//normalize();
		XYZ up(0,1,0);
		if( y >0.9 || y <-0.9)
		{
			up.x = 1.0f; up.y = 0.0f;
		}
		other = cross(up);
		other.normalize();
	}
	
	void reverse()
	{
		x *= -1;
		y *= -1;
		z *= -1;
	}
	
	XYZ reversed() const
	{
		return XYZ(-x, -y, -z);
	}
	
	void findUpAndSide(XYZ& up, XYZ& side)
	{
		XYZ ref(0,1,0);
		if( y >0.9 || y <-0.9)
		{
			ref.x = 1.0f; ref.y = 0.0f;
		}
		side = cross(ref);
		side.reverse();
		side.normalize();
		up = cross(side);
		up.normalize();
	}
	
	void setcomp(int c, float v)
	{
		if(c==0) x = v;
		else if(c==1) y = v;
		else z = v;
	}
	
	void set(float vx, float vy, float vz)
	{
		x = vx; y = vy; z = vz;
	}
	
	void set(float v)
	{
		x = y = z = v;
	}
	
	void rotateXY(float ang)
	{
		float a = x;
		x = a*cos(ang);
		y = a*sin(ang);
	}
	
	void rotateXZ(float ang)
	{
		float a = x;
		x = a*cos(ang);
		z = a*sin(ang);
	}
	
	void rotateZY(float ang)
	{
		float a = z;
		z = a*cos(ang);
		y = a*sin(ang);
	}
	
	void rotateAroundAxis(const XYZ& axis, float theta)
	{
		if(theta==0) return;
		XYZ ori(x,y,z);
		float l = ori.length();
		ori.normalize();
		
		XYZ up = axis.cross(ori);
		up.normalize();
		
		XYZ side = ori - axis*(axis.dot(ori));
		
		up *=side.length();
		
		ori += side*(cos(theta) - 1);
		ori += up*sin(theta);
		
		ori.normalize();
		x = ori.x*l;
		y = ori.y*l;
		z = ori.z*l;
	}
	
	void rotateAlong(const XYZ& v, float eta)
	{
		XYZ ori(x,y,z);
		float l = ori.length();
		ori += v*eta;
		ori.normalize();
		x = ori.x*l;
		y = ori.y*l;
		z = ori.z*l;
	}
		
	float angleToVec(const XYZ& a, XYZ& axis) const
	{
		XYZ buf(x,y,z); buf.normalize();
		XYZ abuf = a; abuf.normalize();
		axis = buf.cross(abuf);
		axis.normalize();
		return acos(abuf.dot(buf));
	}
	
	void randRotate(const float size)
	{
		float leng = length();
		normalize();
		x += size*(rand( )%31/31.f - 0.5f);
		y += size*(rand( )%41/41.f - 0.5f);
		z += size*(rand( )%73/73.f - 0.5f);
		normalize();
		x *= leng;
		y *= leng;
		z *= leng;
	}
	
	float x,y,z;
};

struct XYZW{
	float x,y,z,w;
	XYZW() {}
	XYZW(float _x, float _y, float _z, float _w) {x=_x; y=_y; z=_z; w=_w;}
	XYZ xyz() {return XYZ(x,y,z);}
};

typedef struct {
	float r,g,b;
} RGB;

typedef struct {
	float r,g,b,a;
} RGBA;

typedef struct {
   float real,imag;
} COMPLEX;

struct rpcRecord
{
	rpcRecord() {}
	XYZ pos, nor;
};

struct rpcSample
{
	rpcSample() {}
	rpcSample(float x, float y, float z, float nx, float ny, float nz, float cx, float cy, float cz) 
	{
		pos=XYZ(x,y,z);
		nor=XYZ(nx,ny,nz);
		col=XYZ(cx,cy,cz);
	}
	XYZ pos, nor, col;
};

struct sssSample
{
	XY sph;
	XYZ col;
	float length;
};

struct pcdSample
{
	pcdSample() { area = 0.0f;}
	pcdSample(float x, float y, float z, float nx, float ny, float nz, float cx, float cy, float cz, float _area) 
	{
		pos=XYZ(x,y,z);
		nor=XYZ(nx,ny,nz);
		col=XYZ(cx,cy,cz);
		area = _area;
	}
	XYZ pos, nor, col;
	float area;
};

struct COESample
{
	XYZ pos, nor;
	float area, k;
	XYZ coeff[16];
};

struct qnode
{
	qnode() {child[0]=child[1]=child[2]=child[3]=0;}
	void parse(const pcdSample& sample) { pos = sample.pos; nor = sample.nor; area = sample.area; /*col= sample.col;*/}
	XYZ pos, nor;//, col;//, tang, binor;
	float area, cone;
	int child[4];
};

struct MINMAX
{
	MINMAX() {child[0]=child[1]=child[2]=child[3]=0;}
	float min, max;
	XYZ pos;
	float size;
	int child[4];
	short umin,vmin, umax, vmax, level;
};

struct MATRIX33F
{
	float v[3][3];
	float& operator() (int i, int j)
	{
		return v[i][j];
	}
	const float& operator() (int i, int j) const
	{
		return v[i][j];
	}
	void initialize()
	{
		v[0][0] = v[0][1] = v[0][2] = v[1][0] = v[1][1] = v[1][2] = v[2][0] = v[2][1] = v[2][2] = 0.0f;
	}
	void setIdentity()
	{
		v[0][0] = v[1][1] = v[2][2] = 1.0f;
		v[0][1] = v[0][2] = v[1][0] = v[1][2] = v[2][0] = v[2][1] = 0.0f;
	}
	
	void transpose()
	{
		float a;
		a = v[0][1]; v[0][1] = v[1][0]; v[1][0] = a;
		a = v[0][2]; v[0][2] = v[2][0]; v[2][0] = a;
		a = v[1][2]; v[1][2] = v[2][1]; v[2][1] = a;
	}
};

struct MATRIX44F
{
	MATRIX44F() {setIdentity();}
	float v[4][4];
	
	float& operator() (int i, int j)
	{
		return v[i][j];
	}
	const float& operator() (int i, int j) const
	{
		return v[i][j];
	}
	
	void setIdentity()
	{
		v[0][0] = v[1][1] = v[2][2] = v[3][3] = 1.0f;
		v[0][1] = v[0][2] = v[0][3] = v[1][0] = v[1][2] = v[1][3] = v[2][0] = v[2][1] = v[2][3] = v[3][0] = v[3][1] = v[3][2] = 0.0f;
	}
	
	void setTranslation(const XYZ& t)
	{
		v[3][0] = t.x;
		v[3][1] = t.y;
		v[3][2] = t.z;
	}
	
	void translate(const XYZ& t)
	{
		v[3][0] += t.x;
		v[3][1] += t.y;
		v[3][2] += t.z;
	}
	
	void translate(const float x, const float y, const float z)
	{
		v[3][0] += x;
		v[3][1] += y;
		v[3][2] += z;
	}
	
	void setTranslation(float x, float y, float z)
	{
		v[3][0] = x;
		v[3][1] = y;
		v[3][2] = z;
	}
	
	void setRotation(const MATRIX33F& r)
	{
		v[0][0] = r(0,0);
		v[0][1] = r(0,1);
		v[0][2] = r(0,2);
		v[1][0] = r(1,0);
		v[1][1] = r(1,1);
		v[1][2] = r(1,2);
		v[2][0] = r(2,0);
		v[2][1] = r(2,1);
		v[2][2] = r(2,2);
	}
	
	void setOrientations(const XYZ& side, const XYZ& up, const XYZ& front)
	{
		v[0][0] = side.x;
		v[0][1] = side.y;
		v[0][2] = side.z;
		v[1][0] = up.x;
		v[1][1] = up.y;
		v[1][2] = up.z;
		v[2][0] = front.x;
		v[2][1] = front.y;
		v[2][2] = front.z;
	}
	
	void scale(float x, float y, float z)
	{
		v[0][0] *= x;
		v[0][1] *= x;
		v[0][2] *= x;
		v[1][0] *= y;
		v[1][1] *= y;
		v[1][2] *= y;
		v[2][0] *= z;
		v[2][1] *= z;
		v[2][2] *= z;
	}
	
	float determinant33( float a, float b, float c, float d, float e, float f, float g, float h, float i ) const
	{
		return float( a*( e*i - h*f ) - b*( d*i - g*f ) + c*( d*h - g*e ) );
	}
	
	void inverse()
	{
		float det =   v[0][0] * determinant33( v[1][1], v[2][1], v[3][1], v[1][2], v[2][2], v[3][2], v[1][3], v[2][3], v[3][3] )
					- v[1][0] * determinant33( v[0][1], v[2][1], v[3][1], v[0][2], v[2][2], v[3][2], v[0][3], v[2][3], v[3][3] )
					+ v[2][0] * determinant33( v[0][1], v[1][1], v[3][1], v[0][2], v[1][2], v[3][2], v[0][3], v[1][3], v[3][3] )
					- v[3][0] * determinant33( v[0][1], v[1][1], v[2][1], v[0][2], v[1][2], v[2][2], v[0][3], v[1][3], v[2][3] );
	
	
	float m00 =   determinant33( v[1][1], v[2][1], v[3][1], v[1][2], v[2][2], v[3][2], v[1][3], v[2][3], v[3][3] ) / det;
	float m10 = - determinant33( v[1][0], v[2][0], v[3][0], v[1][2], v[2][2], v[3][2], v[1][3], v[2][3], v[3][3] ) / det;
	float m20 =   determinant33( v[1][0], v[2][0], v[3][0], v[1][1], v[2][1], v[3][1], v[1][3], v[2][3], v[3][3] ) / det;
	float m30 = - determinant33( v[1][0], v[2][0], v[3][0], v[1][1], v[2][1], v[3][1], v[1][2], v[2][2], v[3][2] ) / det;
	
	float m01 = - determinant33( v[0][1], v[2][1], v[3][1], v[0][2], v[2][2], v[3][2], v[0][3], v[2][3], v[3][3] ) / det;
	float m11 =   determinant33( v[0][0], v[2][0], v[3][0], v[0][2], v[2][2], v[3][2], v[0][3], v[2][3], v[3][3] ) / det;
	float m21 = - determinant33( v[0][0], v[2][0], v[3][0], v[0][1], v[2][1], v[3][1], v[0][3], v[2][3], v[3][3] ) / det;
	float m31 =   determinant33( v[0][0], v[2][0], v[3][0], v[0][1], v[2][1], v[3][1], v[0][2], v[2][2], v[3][2] ) / det;
	
	float m02 =   determinant33( v[0][1], v[1][1], v[3][1], v[0][2], v[1][2], v[3][2], v[0][3], v[1][3], v[3][3] ) / det;
	float m12 = - determinant33( v[0][0], v[1][0], v[3][0], v[0][2], v[1][2], v[3][2], v[0][3], v[1][3], v[3][3] ) / det;
	float m22 =   determinant33( v[0][0], v[1][0], v[3][0], v[0][1], v[1][1], v[3][1], v[0][3], v[1][3], v[3][3] ) / det;
	float m32 = - determinant33( v[0][0], v[1][0], v[3][0], v[0][1], v[1][1], v[3][1], v[0][2], v[1][2], v[3][2] ) / det;
	
	float m03 = - determinant33( v[0][1], v[1][1], v[2][1], v[0][2], v[1][2], v[2][2], v[0][3], v[1][3], v[2][3] ) / det;
	float m13 =   determinant33( v[0][0], v[1][0], v[2][0], v[0][2], v[1][2], v[2][2], v[0][3], v[1][3], v[2][3] ) / det;
	float m23 = - determinant33( v[0][0], v[1][0], v[2][0], v[0][1], v[1][1], v[2][1], v[0][3], v[1][3], v[2][3] ) / det;
	float m33 =   determinant33( v[0][0], v[1][0], v[2][0], v[0][1], v[1][1], v[2][1], v[0][2], v[1][2], v[2][2] ) / det;
	
	v[0][0] = m00;
	v[0][1] = m01;
	v[0][2] = m02;
	v[0][3] = m03;
	
	v[1][0] = m10;
	v[1][1] = m11;
	v[1][2] = m12;
	v[1][3] = m13;
	
	v[2][0] = m20;
	v[2][1] = m21;
	v[2][2] = m22;
	v[2][3] = m23;
	
	v[3][0] = m30;
	v[3][1] = m31;
	v[3][2] = m32;
	v[3][3] = m33;

	}
	
	void transformAsNormal(XYZ& i) const
	{
		float tx = i.x*v[0][0] + i.y*v[1][0] + i.z*v[2][0];
		float ty = i.x*v[0][1] + i.y*v[1][1] + i.z*v[2][1];
		float tz = i.x*v[0][2] + i.y*v[1][2] + i.z*v[2][2];
		
		i.x = tx;
		i.y = ty;
		i.z = tz;
	}
	
	void transform(XYZ& i) const
	{
		float tx = i.x*v[0][0] + i.y*v[1][0] + i.z*v[2][0] + v[3][0];
		float ty = i.x*v[0][1] + i.y*v[1][1] + i.z*v[2][1] + v[3][1];
		float tz = i.x*v[0][2] + i.y*v[1][2] + i.z*v[2][2] + v[3][2];
		
		i.x = tx;
		i.y = ty;
		i.z = tz;
	}
	
	void operator*=( const MATRIX44F& other )
	{
		float m00 = v[0][0]* other(0,0) + v[0][1]* other(1,0)  + v[0][2]* other(2,0) + v[0][3]* other(3,0);
		float m01 = v[0][0]* other(0,1) + v[0][1]* other(1,1)  + v[0][2]* other(2,1) + v[0][3]* other(3,1);
		float m02 = v[0][0]* other(0,2) + v[0][1]* other(1,2)  + v[0][2]* other(2,2) + v[0][3]* other(3,2);
		float m03 = v[0][0]* other(0,3) + v[0][1]* other(1,3)  + v[0][2]* other(2,3) + v[0][3]* other(3,3);
		
		float m10 = v[1][0]* other(0,0) + v[1][1]* other(1,0)  + v[1][2]* other(2,0) + v[1][3]* other(3,0);
		float m11 = v[1][0]* other(0,1) + v[1][1]* other(1,1)  + v[1][2]* other(2,1) + v[1][3]* other(3,1);
		float m12 = v[1][0]* other(0,2) + v[1][1]* other(1,2)  + v[1][2]* other(2,2) + v[1][3]* other(3,2);
		float m13 = v[1][0]* other(0,3) + v[1][1]* other(1,3)  + v[1][2]* other(2,3) + v[1][3]* other(3,3);
		
		float m20 = v[2][0]* other(0,0) + v[2][1]* other(1,0)  + v[2][2]* other(2,0) + v[2][3]* other(3,0);
		float m21 = v[2][0]* other(0,1) + v[2][1]* other(1,1)  + v[2][2]* other(2,1) + v[2][3]* other(3,1);
		float m22 = v[2][0]* other(0,2) + v[2][1]* other(1,2)  + v[2][2]* other(2,2) + v[2][3]* other(3,2);
		float m23 = v[2][0]* other(0,3) + v[2][1]* other(1,3)  + v[2][2]* other(2,3) + v[2][3]* other(3,3);
		
		float m30 = v[3][0]* other(0,0) + v[3][1]* other(1,0)  + v[3][2]* other(2,0) + v[3][3]* other(3,0);
		float m31 = v[3][0]* other(0,1) + v[3][1]* other(1,1)  + v[3][2]* other(2,1) + v[3][3]* other(3,1);
		float m32 = v[3][0]* other(0,2) + v[3][1]* other(1,2)  + v[3][2]* other(2,2) + v[3][3]* other(3,2);
		float m33 = v[3][0]* other(0,3) + v[3][1]* other(1,3)  + v[3][2]* other(2,3) + v[3][3]* other(3,3);
		
		v[0][0] = m00; v[0][1] = m01; v[0][2] = m02; v[0][3] = m03;
		v[1][0] = m10; v[1][1] = m11; v[1][2] = m12; v[1][3] = m13;
		v[2][0] = m20; v[2][1] = m21; v[2][2] = m22; v[2][3] = m23;
		v[3][0] = m30; v[3][1] = m31; v[3][2] = m32; v[3][3] = m33;
	}
	
	void multiply( const MATRIX44F& other )
	{
		float m00 = v[0][0]* other(0,0) + v[0][1]* other(1,0)  + v[0][2]* other(2,0) + v[0][3]* other(3,0);
		float m01 = v[0][0]* other(0,1) + v[0][1]* other(1,1)  + v[0][2]* other(2,1) + v[0][3]* other(3,1);
		float m02 = v[0][0]* other(0,2) + v[0][1]* other(1,2)  + v[0][2]* other(2,2) + v[0][3]* other(3,2);
		float m03 = v[0][0]* other(0,3) + v[0][1]* other(1,3)  + v[0][2]* other(2,3) + v[0][3]* other(3,3);
		
		float m10 = v[1][0]* other(0,0) + v[1][1]* other(1,0)  + v[1][2]* other(2,0) + v[1][3]* other(3,0);
		float m11 = v[1][0]* other(0,1) + v[1][1]* other(1,1)  + v[1][2]* other(2,1) + v[1][3]* other(3,1);
		float m12 = v[1][0]* other(0,2) + v[1][1]* other(1,2)  + v[1][2]* other(2,2) + v[1][3]* other(3,2);
		float m13 = v[1][0]* other(0,3) + v[1][1]* other(1,3)  + v[1][2]* other(2,3) + v[1][3]* other(3,3);
		
		float m20 = v[2][0]* other(0,0) + v[2][1]* other(1,0)  + v[2][2]* other(2,0) + v[2][3]* other(3,0);
		float m21 = v[2][0]* other(0,1) + v[2][1]* other(1,1)  + v[2][2]* other(2,1) + v[2][3]* other(3,1);
		float m22 = v[2][0]* other(0,2) + v[2][1]* other(1,2)  + v[2][2]* other(2,2) + v[2][3]* other(3,2);
		float m23 = v[2][0]* other(0,3) + v[2][1]* other(1,3)  + v[2][2]* other(2,3) + v[2][3]* other(3,3);
		
		float m30 = v[3][0]* other(0,0) + v[3][1]* other(1,0)  + v[3][2]* other(2,0) + v[3][3]* other(3,0);
		float m31 = v[3][0]* other(0,1) + v[3][1]* other(1,1)  + v[3][2]* other(2,1) + v[3][3]* other(3,1);
		float m32 = v[3][0]* other(0,2) + v[3][1]* other(1,2)  + v[3][2]* other(2,2) + v[3][3]* other(3,2);
		float m33 = v[3][0]* other(0,3) + v[3][1]* other(1,3)  + v[3][2]* other(2,3) + v[3][3]* other(3,3);
		
		v[0][0] = m00; v[0][1] = m01; v[0][2] = m02; v[0][3] = m03;
		v[1][0] = m10; v[1][1] = m11; v[1][2] = m12; v[1][3] = m13;
		v[2][0] = m20; v[2][1] = m21; v[2][2] = m22; v[2][3] = m23;
		v[3][0] = m30; v[3][1] = m31; v[3][2] = m32; v[3][3] = m33;
	}
	
	void rotateY(const float angle)
	{
		MATRIX44F tmp;
		tmp.setIdentity();
		tmp.v[0][0] = cos(angle);
		tmp.v[0][2] = -sin(angle);
		tmp.v[2][0] = sin(angle);
		tmp.v[2][2] = cos(angle);
		
		multiply(tmp);
	}
	
	void scale(const float val)
	{
		MATRIX44F tmp;
		tmp.setIdentity();
		tmp.v[0][0] = tmp.v[1][1] = tmp.v[2][2] = val;
		
		multiply(tmp);
	}
};

struct ABCD{
	float a,b,c,d;
	ABCD():a(0), b(0), c(0), d(0) {}
	ABCD(float _a, float _b, float _c, float _d):a(_a), b(_b), c(_c), d(_d) {}
};

struct medianSample
{
	float u, v, r, g, b, x , y, z;
};

struct edgeRule
{
	short n;
	float x0, y0, x1, y1, x2, y2, x3, y3;
	short u0, v0, u1, v1;
	edgeRule() {n=0;}
	edgeRule(short _n, float _x0, float _y0, float _x1, float _y1, float _x2, float _y2, float _x3, float _y3, short _u0, short _v0, short _u1, short _v1) 
	{n= _n; x0= _x0; y0= _y0; x1= _x1; y1= _y1; x2= _x2; y2= _y2; x3= _x3; y3= _y3; u0 = _u0; v0 = _v0; u1 = _u1; v1 = _v1;}
};

struct Edge
{
	float x0, y0, z0, x1, y1, z1;
	Edge() {z1=0;}
	Edge(float _x0, float _y0, float _z0, float _x1, float _y1, float _z1) 
		{x0= _x0; y0= _y0; z0= _z0; x1= _x1; y1= _y1; z1= _z1; }
};

struct CoeRec
{
	float data[16];
};

struct CoeSample
{
	XYZ pos, nor, col;
	float area;
	float coe[16];
};
#endif /* ZDATA_H */
