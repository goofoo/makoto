#ifndef ZMATH_H
#define ZMATH_H
#include "zData.h"
#include <math.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define EPSILON 10e-8
#define PI 3.14159265358979323846
#define PIF 3.1415926535f
#define GRAVITATION 9.81f
#define INV_SQRT_TWO 0.707107f
#define ONEDIVETA 0.7500188f

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

#define rsqrt(x) 1.0/sqrt(x)

inline float saturate(const float& x)
{
	if(x>1.0f) return 1.0f;
	else if(x<0.0f) return 0.0f;
	else return x;
}

inline float absf(float a)
{
	return (a>=0.0f)? a:-a;
}

inline float fmax(float a, float b)
{
	if(b>a) return b;
	else return a;
}

inline float fmin(float a, float b)
{
	if(b<a) return b;
	else return a;
}

inline float fmin(float a, float b, float c, float d)
{
	if(a<=b && a<=c && a<=d) return a;
	else if(b<=a && b<=c && b<=d) return b;
	else if(c<=a && c<=b && c<=d) return c;
	else return d;
}

inline float distance_between(const XYZ& from, const XYZ& to)
{
	return sqrt((to.x-from.x)*(to.x-from.x) + (to.y-from.y)*(to.y-from.y) + (to.z-from.z)*(to.z-from.z));
}

inline float distance_between_squared(const XYZ& from, const XYZ& to)
{
	return ((to.x-from.x)*(to.x-from.x) + (to.y-from.y)*(to.y-from.y) + (to.z-from.z)*(to.z-from.z));
}

inline float dot(const XYZ& a, const XYZ& b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

inline float length(const XYZ& a) 
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

inline XYZ cross(const XYZ front, const XYZ up)
{
	XYZ o;	
	// Calculate the cross product with the non communitive equation
	o.x = front.y * up.z - front.z * up.y;
	o.y = front.z * up.x - front.x * up.z;
	o.z = front.x * up.y - front.y * up.x;
	return o;
}

inline void normalize(XYZ& v)
{
	float len = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	v.x /= len;
	v.y /= len;
	v.z /= len;
}

inline int isPowerof2(int n)
{
	int m=2;
	do {
		if(m==n) return 1;
		else	m *=2;
	} while (m<=n);

	return 0;
}

inline float rad2deg(float in)
{
	return in/PIF*180.0f;
}

inline float deg2rad(float in)
{
	float out = in/180.0f*PIF;
	return out;
}

inline int Powerof2(int n,int *m,int *twopm)
{
	if (n <= 1) {
		*m = 0;
		*twopm = 1;
		return(FALSE);
	}

   *m = 1;
   *twopm = 2;
   do {
      (*m)++;
      (*twopm) *= 2;
   } while (2*(*twopm) <= n);

   if (*twopm != n) 
		return(FALSE);
	else
		return(TRUE);
}

inline double neg1Pow(int k)
{
	return pow(-1.0,k);
}
/*
inline double ranf()
{
	return (double)rand()/(double)RAND_MAX;
}

inline void gauss(double work[2])
{
	// Algorithm by Dr. Everett (Skip) Carter, Jr.

	double x1, x2, w;

	do {
		x1 = 2.0 * ranf() - 1.0;
		x2 = 2.0 * ranf() - 1.0;
		w = x1 * x1 + x2 * x2; 
	} while ( w >= 1.0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	work[0] = x1 * w;	// first gauss random
	work[1] = x2 * w;	// second gauss random
}
*/
inline double clamp(double x) 
{
	if(x<0.) x = 0.;
	else if(x>1.) x = 1.;
	return x;
}

inline double fclamp( double in, double min, double max )
{
	double out = in;
	if( in < min )
		out = min;
	else if( in > max )
		out = max;
	return out;
}


/*
inline int ray_plane_hit(XYZ& p_hit, XYZ p0, XYZ ray, double A, double B, double C, double D)
{
	double t=-(A*p0.x + B*p0.y + C*p0.z + D)/(A*ray.x + B*ray.y + C*ray.z);
	if(t<0) return FALSE;
	p_hit.x=p0.x + ray.x*t;
	p_hit.y=p0.y + ray.y*t;
	p_hit.z=p0.z + ray.z*t;
	return TRUE;
}

inline int ray_plane_hit(XYZ& p_hit, XYZ p0, XYZ ray, double A, double B, double C, double D, double& t)
{
	t=-(A*p0.x + B*p0.y + C*p0.z + D)/(A*ray.x + B*ray.y + C*ray.z);
	if(t<0) return FALSE;
	p_hit.x=p0.x + ray.x*t;
	p_hit.y=p0.y + ray.y*t;
	p_hit.z=p0.z + ray.z*t;
	return TRUE;
}

inline int line_plane_hit(XYZ& p_hit, XYZ p0, XYZ p1, double A, double B, double C, double D)
{
	XYZ ray;
	ray.x = p1.x-p0.x;
	ray.y = p1.y-p0.y;
	ray.z = p1.z-p0.z;
	
	double leng = sqrt(ray.x*ray.x+ray.y*ray.y+ray.z*ray.z);
	
	ray.x /= leng;
	ray.y /= leng;
	ray.z /= leng;
	
	double t=-(A*p0.x + B*p0.y + C*p0.z + D)/(A*ray.x + B*ray.y + C*ray.z);
	if(t<0 || t>leng) return FALSE;
	p_hit.x=p0.x + ray.x*t;
	p_hit.y=p0.y + ray.y*t;
	p_hit.z=p0.z + ray.z*t;
	return TRUE;
}

inline int plane_plane_intersection(const XYZ& p0, const XYZ& n0, const XYZ& p1, const XYZ& n1, XYZ& origin, XYZ& dir)
{
	dir = cross(n0, n1);
	if(length(dir)==0) return 0;
	
	normalize(dir);
	float d0=dot(p0,n0), d1=dot(p1,n1);
	float det = dot(n0, n0) * dot(n1,n1) - dot(n0,n1)*dot(n0,n1);

	float c0 = (d0*dot(n1,n1)-d1*dot(n0,n1))/det;
	float c1 = (d1*dot(n0,n0)-d0*dot(n0,n1))/det;

	origin.x = c0*n0.x+c1*n1.x;
	origin.y = c0*n0.y +c1*n1.y;
	origin.z = c0*n0.z +c1*n1.z;
	
//printf("pi:%f %f %f\n", origin.x, origin.y, origin.z);
//printf("vi:%f %f %f\n", dir.x, dir.y, dir.z);
	return 1;
}

inline int ray_box_intersection(const XYZ& origin, const XYZ& ray, const XYZ& boxmin, const XYZ& boxmax)
{
	float T1, T2, Ttmp, Tnear = -10e8, Tfar = 10e8;
	// for x
	if(ray.x==0)// paralell to YZ plane
	{
		if(origin.x < boxmin.x || origin.x > boxmax.x) return 0;
	}
	else
	{
		T1 = (boxmin.x - origin.x)/ray.x;
		T2 = (boxmax.x - origin.x)/ray.x;
		
		if(T1>T2)
		{
			Ttmp=T1;
			T1=T2;
			T2=Ttmp;
		}
		
		if(T1>Tnear) Tnear = T1;
		if(T2<Tfar) Tfar = T2;
		
		if(Tfar<0 || Tnear>Tfar) return 0;
		
	}
	// for y
	if(ray.y==0)// paralell to XZ plane
	{
		if(origin.y < boxmin.y || origin.y > boxmax.y) return 0;
	}
	else
	{
		T1 = (boxmin.y - origin.y)/ray.y;
		T2 = (boxmax.y - origin.y)/ray.y;
		
		if(T1>T2)
		{
			Ttmp=T1;
			T1=T2;
			T2=Ttmp;
		}
		
		if(T1>Tnear) Tnear = T1;
		if(T2<Tfar) Tfar = T2;
		
		if(Tfar<0 || Tnear>Tfar) return 0;
		
	}
	// for z
	if(ray.z==0)// paralell to XY plane
	{
		if(origin.z < boxmin.z || origin.z > boxmax.z) return 0;
	}
	else
	{
		T1 = (boxmin.z - origin.z)/ray.z;
		T2 = (boxmax.z - origin.z)/ray.z;
		
		if(T1>T2)
		{
			Ttmp=T1;
			T1=T2;
			T2=Ttmp;
		}
		
		if(T1>Tnear) Tnear = T1;
		if(T2<Tfar) Tfar = T2;
		
		if(Tfar<0 || Tnear>Tfar) return 0;
		
	}
	
	return 1;
}

inline int plane_box_intersection(const XYZ& origin, const XYZ& normal, const XYZ& boxmin, const XYZ& boxmax)
{
	XYZ corner;
	int hit=0;
	
	corner.x = boxmin.x - origin.x;
	corner.y = boxmin.y - origin.y;
	corner.z = boxmin.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmax.x - origin.x;
	corner.y = boxmin.y - origin.y;
	corner.z = boxmin.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmin.x - origin.x;
	corner.y = boxmax.y - origin.y;
	corner.z = boxmin.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmax.x - origin.x;
	corner.y = boxmax.y - origin.y;
	corner.z = boxmin.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmin.x - origin.x;
	corner.y = boxmin.y - origin.y;
	corner.z = boxmax.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmax.x - origin.x;
	corner.y = boxmin.y - origin.y;
	corner.z = boxmax.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmin.x - origin.x;
	corner.y = boxmax.y - origin.y;
	corner.z = boxmax.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	corner.x = boxmax.x - origin.x;
	corner.y = boxmax.y - origin.y;
	corner.z = boxmax.z - origin.z;
	normalize(corner);
	if(dot(corner, normal)>0.1) return 1;
	
	return hit;
}

inline int point_in_box(const XYZ& p, const XYZ& bbmin, const XYZ& bbmax)
{
	if(p.x>=bbmin.x && p.x<=bbmax.x && p.y>=bbmin.y && p.y<=bbmax.y && p.z>=bbmin.z && p.z<=bbmax.z) return 1;
	return 0;
}

inline int factorial(int v)
{
		if (v == 0)
			return (1);

		int result = v;
		while (--v > 0)
			result *= v;
		
		return (result);
}

inline void quick_sort(rpcRecord* a, int lo, int hi, short axis)
{
//  lo is the lower index, hi is the upper index
//  of the region of array a that is to be sorted
    int i=lo, j=hi;
    rpcRecord h;
    XYZ x=a[(lo+hi)/2].pos;
	
	if(axis==0)
	{
	    //  partition
	    do
	    {    
	        while (a[i].pos.x<x.x) i++; 
	        while (a[j].pos.x>x.x) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else if(axis==1)
	{
		do
	    {    
	        while (a[i].pos.y<x.y) i++; 
	        while (a[j].pos.y>x.y) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else
	{
		do
	    {    
	        while (a[i].pos.z<x.z) i++; 
	        while (a[j].pos.z>x.z) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}

    //  recursion
    if (lo<j) quick_sort(a, lo, j, axis);
    if (i<hi) quick_sort(a, i, hi, axis);
}

inline void median_split(rpcRecord* a, int lo, int hi, short axis)
{
	quick_sort(a, lo, hi, axis);
	int mid = (lo+hi)/2;
	if(lo<mid)
	{
		short new_axis = (axis+1)%3;
		median_split(a, lo, mid-1, new_axis);
		median_split(a, mid+1, hi, new_axis);
	}
}

inline void quick_sort(rpcSample* a, int lo, int hi, short axis)
{
//  lo is the lower index, hi is the upper index
//  of the region of array a that is to be sorted
    int i=lo, j=hi;
    rpcSample h;
    XYZ x=a[(lo+hi)/2].pos;
	
	if(axis==0)
	{
	    //  partition
	    do
	    {    
	        while (a[i].pos.x<x.x) i++; 
	        while (a[j].pos.x>x.x) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else if(axis==1)
	{
		do
	    {    
	        while (a[i].pos.y<x.y) i++; 
	        while (a[j].pos.y>x.y) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else
	{
		do
	    {    
	        while (a[i].pos.z<x.z) i++; 
	        while (a[j].pos.z>x.z) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}

    //  recursion
    if (lo<j) quick_sort(a, lo, j, axis);
    if (i<hi) quick_sort(a, i, hi, axis);
}

inline void median_split(rpcSample* a, int lo, int hi, short axis)
{
	quick_sort(a, lo, hi, axis);
	int mid = (lo+hi)/2;
	if(lo<mid)
	{
		short new_axis = (axis+1)%3;
		median_split(a, lo, mid-1, new_axis);
		median_split(a, mid+1, hi, new_axis);
	}
}

inline void quick_sort(XYZ* a, int lo, int hi, short axis)
{
//  lo is the lower index, hi is the upper index
//  of the region of array a that is to be sorted
    int i=lo, j=hi;
    XYZ h;
    XYZ x=a[(lo+hi)/2];
	
	if(axis==0)
	{
	    //  partition
	    do
	    {    
	        while (a[i].x<x.x) i++; 
	        while (a[j].x>x.x) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else if(axis==1)
	{
		do
	    {    
	        while (a[i].y<x.y) i++; 
	        while (a[j].y>x.y) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else
	{
		do
	    {    
	        while (a[i].z<x.z) i++; 
	        while (a[j].z>x.z) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}

    //  recursion
    if (lo<j) quick_sort(a, lo, j, axis);
    if (i<hi) quick_sort(a, i, hi, axis);
}
*/
inline void xyz2sph(const XYZ& normal, double& theta, double& phi)
{
	theta = acos(normal.z);
	if(normal.x==0 ) {
		if( normal.y >0) phi = 0;
		else phi = PI*1.5;
	}
	else if(normal.x>0 && normal.y >0) phi = atan(normal.y/normal.x);
	else if(normal.x>0 && normal.y <0) phi = 2*PI + atan(normal.y/normal.x);
	else phi = PI + atan(normal.y/normal.x);
}

inline void sph2vec(const float theta, const float phi, float& x, float& y, float& z)
{
    x = sin(theta) * cos(phi);
    y = sin(theta) * sin(phi);
    z = cos(theta);
}

inline void sph2xyz(const float r, const double theta, const double phi, float& x, float& y, float& z)
{
    x = r*sin(theta) * cos(phi);
    y = r*sin(theta) * sin(phi);
    z = r*cos(theta);
}


#endif /* ZMATH_H */
