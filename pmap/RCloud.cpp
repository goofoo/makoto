#include "RCloud.h"
#include "../shared/zData.h"
#include "../shared/zMath.h"

void RCloud::sphereRand(XYZ& q, int seed)
{	
	int g_seed = seed*11 + seed%19;
	float noi = noise.randfint( g_seed );
// 0 <= theta <= 2PI
	float theta = noi * (2*PI);

// -1 <= u <= 1
	g_seed = seed*3 + seed%13;
	noi = noise.randfint( g_seed );
	float u = noi * 2 -1.0;
	 
	q.x = cos(theta) * sqrt(1.0-(u*u));
	q.y = sin(theta) * sqrt(1.0-(u*u));
	q.z = u;
}

RCloud::RCloud()
{
}

void RCloud::emit(float detail)
{
	int numDot = 5+7*sqrt(detail);
	if(numDot > 1200) numDot = 1200;
	XYZ* pp = new XYZ[numDot];
	XYZ* po = new XYZ[numDot];
	float* pw = new float[numDot];
	double theta, phi;
	float rr, noi, adens = density/numDot;
	int seed;
	
	for(int i=0; i<numDot; i++) {
		sphereRand(pp[i], i);
		xyz2sph(pp[i], theta, phi);
		
		seed = i*13 + i%29*7;
		noi = noise.randfint(seed);
		rr = sqrt(noi);
		rr *= radius*2;
		sph2xyz(rr, theta, phi, pp[i].x, pp[i].y, pp[i].z);
		
		rr /= 3.1f;
		
		seed++;
		noi = noise.randfint(seed);
		rr += (noi - 0.5f)*rr*0.4f;
		pw[i] = rr;
		
		rr = adens;
		
		seed++;
		noi = noise.randfint(seed);
		rr += (noi - 0.5f)*rr*0.4f;
		po[i] = XYZ(rr);
	}
	
	RiPoints(numDot, "P", (RtPointer)pp, "width", (RtPointer)pw, "Os", (RtPointer)po, RI_NULL);
	
	delete[] pp;
	delete[] po;
}
