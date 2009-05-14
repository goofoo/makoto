#include "fvane.h"

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

float catmullRomSpline(float x, float v0,float v1,
				float v2,float v3) 
{

	float c1,c2,c3,c4;

	c1 =  	      M12*v1;
	c2 = M21*v0          + M23*v2;
	c3 = M31*v0 + M32*v1 + M33*v2 + M34*v3;
	c4 = M41*v0 + M42*v1 + M43*v2 + M44*v3;

	return(((c4*x + c3)*x +c2)*x + c1);
}

FVane::FVane(void):pp(0),
pup(0),
pdown(0),
pnormal(0)
{
}

FVane::~FVane(void)
{
	clear();
}

void FVane::create(int num, XYZ* p, XYZ* up, XYZ* down, XYZ* normal)
{
	clear();
	
	n_cv = num;
	grid = 1.0f/float(num-1);
	pp = new XYZ[num];
	pup = new XYZ[num];
	pdown = new XYZ[num];
	pnormal = new XYZ[num];
	
	for(int i=0; i<num; i++)
	{
		pp[i] = p[i];
		pup[i] = up[i];
		pdown[i] = down[i];
		pnormal[i] = normal[i];
	}
}

void FVane::clear()
{
	if(pp) delete[] pp;
	if(pup) delete[] pup;
	if(pdown) delete[] pdown;
	if(pnormal) delete[] pnormal;
}

int FVane::getNStep(float delta) const
{
	return getLength() / delta;
}

float FVane::getLength() const
{
	float sum_len = 0;
	XYZ dv;
	for(int i = 0; i<n_cv-1; i++)
	{
		dv = pp[i+1] - pp[i];
		sum_len += dv.length();
	}
	return sum_len;
}

XYZ FVane::getPointAt(float& S, float& T, XYZ& normal) const
{
	int t1 = T/grid;
	int t2 = t1+1;
	int t0 = t1-1;
	int t3 = t2+1;
	
	if(t0<0) t0=0;
	if(t1<0) t1=0;
	if(t2>n_cv-1) t2 = n_cv-1;
	if(t3>n_cv-1) t3 = n_cv-1;
	
	XYZ res;
	float x = T/grid-t1;
	res.x = catmullRomSpline(x, pp[t0].x, pp[t1].x, pp[t2].x, pp[t3].x);
	res.y = catmullRomSpline(x, pp[t0].y, pp[t1].y, pp[t2].y, pp[t3].y);
	res.z = catmullRomSpline(x, pp[t0].z, pp[t1].z, pp[t2].z, pp[t3].z);
	
	normal.x = catmullRomSpline(x, pnormal[t0].x, pnormal[t1].x, pnormal[t2].x, pnormal[t3].x);
	normal.y = catmullRomSpline(x, pnormal[t0].y, pnormal[t1].y, pnormal[t2].y, pnormal[t3].y);
	normal.z = catmullRomSpline(x, pnormal[t0].z, pnormal[t1].z, pnormal[t2].z, pnormal[t3].z);
	normal.normalize();
	
	XYZ ds;
	float size;
	if(S<0.5)
	{
		size = (0.5-S)/0.5;
		
		ds.x = catmullRomSpline(x, pup[t0].x, pup[t1].x, pup[t2].x, pup[t3].x);
		ds.y = catmullRomSpline(x, pup[t0].y, pup[t1].y, pup[t2].y, pup[t3].y);
		ds.z = catmullRomSpline(x, pup[t0].z, pup[t1].z, pup[t2].z, pup[t3].z);
		
		res = res+ds*size;
	}
	else if(S>0.5)
	{
		size = (S-0.5)/0.5;
		
		ds.x = catmullRomSpline(x, pdown[t0].x, pdown[t1].x, pdown[t2].x, pdown[t3].x);
		ds.y = catmullRomSpline(x, pdown[t0].y, pdown[t1].y, pdown[t2].y, pdown[t3].y);
		ds.z = catmullRomSpline(x, pdown[t0].z, pdown[t1].z, pdown[t2].z, pdown[t3].z);
		
		res = res+ds*size;
	}
	return res;
}
//~: