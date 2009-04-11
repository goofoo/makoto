#include "FBSpline.h"

#define M11	 0.0	
#define M12	 1.0
#define M13	 0.0
#define M14	 0.0
#define M21	-0.5
#define M22	 0.0
#define M23	 0.5
#define M24	 0.0
#define M31	 1.0
#define M32	-2.5
#define M33	 2.0
#define M34	-0.5
#define M41	-0.5
#define M42	 1.5
#define M43	-1.5
#define M44	 0.5

float catmullRomSpline1(float x, float v0,float v1,
				float v2,float v3) 
{

	float c1,c2,c3,c4;

	c1 =  	      M12*v1;
	c2 = M21*v0          + M23*v2;
	c3 = M31*v0 + M32*v1 + M33*v2 + M34*v3;
	c4 = M41*v0 + M42*v1 + M43*v2 + M44*v3;

	return(((c4*x + c3)*x +c2)*x + c1);
}

FBSpline::FBSpline(void):pcv(0)
{
}

FBSpline::~FBSpline(void)
{
	clear();
}

void FBSpline::create(int num, float* cv)
{
	clear();
	
	n_cv = num;
	grid = 1.0f/float(num-1);
	pcv = new float[num];
	
	for(int i=0; i<num; i++) pcv[i] = cv[i];
}

void FBSpline::clear()
{
	if(pcv) delete[] pcv;
}


void FBSpline::setEnd(float v0, float v1)
{
	end0 = v0;
	end1 = v1;
}

float FBSpline::getValue(float v) const
{
	int t1 = v/grid;
	int t2 = t1+1;
	int t0 = t1-1;
	if(t0<0) t0=0;
	int t3 = t2+1;
	if(t3>n_cv-1) t3 = n_cv-1;
	
	float x = v/grid-t1;
	
	if(t1 == n_cv-2) return catmullRomSpline1(x, pcv[t0], pcv[t1], pcv[t2], end1);
	else if(t1==0)  return catmullRomSpline1(x, end0, pcv[t1], pcv[t2], pcv[t3]);
	
	return catmullRomSpline1(x, pcv[t0], pcv[t1], pcv[t2], pcv[t3]);
}

FBSpline3D::FBSpline3D(void):pcv(0)
{
}

FBSpline3D::~FBSpline3D(void)
{
	clear();
}

void FBSpline3D::create(int num, XYZ* cv)
{
	clear();
	
	n_cv = num;
	grid = 1.0f/float(num-1);
	pcv = new XYZ[num];
	
	for(int i=0; i<num; i++) pcv[i] = cv[i];
}

void FBSpline3D::clear()
{
	if(pcv) delete[] pcv;
}

XYZ FBSpline3D::getValue(float v) const
{
	int t1 = v/grid;
	int t2 = t1+1;
	int t0 = t1-1;
	if(t0<0) t0=0;
	int t3 = t2+1;
	if(t3>n_cv-1) t3 = n_cv-1;
	
	float x = v/grid-t1;
	
	XYZ res;
	res.x = catmullRomSpline1(x, pcv[t0].x, pcv[t1].x, pcv[t2].x, pcv[t3].x);
	res.y = catmullRomSpline1(x, pcv[t0].y, pcv[t1].y, pcv[t2].y, pcv[t3].y);
	res.z = catmullRomSpline1(x, pcv[t0].z, pcv[t1].z, pcv[t2].z, pcv[t3].z);
	
	return res;
}
//~: