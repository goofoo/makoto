#include "RCloud.h"
#include "../shared/zData.h"
#include "../shared/zMath.h"

void sphereRand(XYZ& q)
{
// 0 <= theta <= 2PI
	float theta = float(random()%271)/271.f * (2*PI);

// -1 <= u <= 1
	float u = (float(random()%219)/219.f) * 2 -1.0;
	 
	q.x = cos(theta) * sqrt(1.0-(u*u));
	q.y = sin(theta) * sqrt(1.0-(u*u));
	q.z = u;
}

RCloud::RCloud()
{
}

void RCloud::emit()
{
	int numDot = 900;
	XYZ* pp = new XYZ[numDot];
	XYZ* po = new XYZ[numDot];
	float* pw = new float[numDot];
	double theta, phi;
	float rr;
	
	for(int i=0; i<numDot; i++) {
		sphereRand(pp[i]);
		xyz2sph(pp[i], theta, phi);
		
		rr = sqrt(float(random()%409)/409.f);
		rr *= radius*2;
		sph2xyz(rr, theta, phi, pp[i].x, pp[i].y, pp[i].z);
		
		po[i] = XYZ(0.01f);
		pw[i] = rr/7;
	}
	
	RiPoints(numDot, "P", (RtPointer)pp, "width", (RtPointer)pw, "Os", (RtPointer)po, RI_NULL);
	
	delete[] pp;
	delete[] po;
}
