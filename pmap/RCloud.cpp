#include "RCloud.h"
#include "../shared/zData.h"
#include "../shared/zMath.h"

void RCloud::sphereRand(XYZ& q, int seed)
{	
	int g_seed = seed*11 +idx*13;
	float noi = noise.randfint( g_seed );
// 0 <= theta <= 2PI
	float theta = noi * (2*PI);

// -1 <= u <= 1
	g_seed = seed*7 +idx*29;
	noi = noise.randfint( g_seed );
	float u = noi * 2 -1.0;
	 
	q.x = cos(theta) * sqrt(1.0-(u*u));
	q.y = sin(theta) * sqrt(1.0-(u*u));
	q.z = u;
}

RCloud::RCloud():coe0(0), coe1(0)
{
}

void RCloud::emit()
{
	int numDot = 9+detail;

	if(numDot > 500) numDot = 500;

	XYZ* pp = new XYZ[numDot];
	XYZ* po = new XYZ[numDot];
	float* pw = new float[numDot];
	double theta, phi;
	float rr, noi, adens = density/(numDot-7);
	
	for(int i=0; i<numDot; i++) {
		sphereRand(pp[i], i);
		
		xyz2sph(pp[i], theta, phi);
		
		noi = noise.randfint(i);
		noi = 0.05 + noi*0.95;
		rr = sqrt(sqrt(noi));
		
		rr *= radius*2;
		
		pp[i] *= rr;
		//sph2xyz(rr, theta, phi, pp[i].x, pp[i].y, pp[i].z);
		
		rr = radius*34/(numDot+11+dusty*37);
		rr += rr*(1-noi)*(1-dusty);
		
		//seed++;
		//noi = noise.randfint(seed);
		//rr += (noi - 0.5f)*rr*0.3f;
		pw[i] = rr;
		
		rr = adens;
		rr += (rr*sqrt(1 - noi) - rr)*(1-dusty);
		//seed++;
		//noi = noise.randfint(seed);
		//rr += (noi - 0.5f)*rr*0.1f;
		po[i] = XYZ(rr);
	}
	RtToken keys[32];
	keys[0] = "P"; keys[1] = "width"; keys[2] = "Os";
	
	RtPointer vals[32];
	vals[0] = (RtPointer)pp; vals[1] = (RtPointer)pw; vals[2] = (RtPointer)po;
	
	int nparam = 3;
	if(coe0) {
		keys[nparam] = "constant color keysh0";
		RtColor keysh0 = {coe0[0].x, coe0[0].y, coe0[0].z};
		vals[nparam] = (RtPointer)&keysh0;
		nparam++;
		keys[nparam] = "constant color keysh1";
		RtColor keysh1 = {coe0[1].x, coe0[1].y, coe0[1].z};
		vals[nparam] = (RtPointer)&keysh1;
		nparam++;
		keys[nparam] = "constant color keysh2";
		RtColor keysh2 = {coe0[2].x, coe0[2].y, coe0[2].z};
		vals[nparam] = (RtPointer)&keysh2;
		nparam++;
		keys[nparam] = "constant color keysh3";
		RtColor keysh3 = {coe0[3].x, coe0[3].y, coe0[3].z};
		vals[nparam] = (RtPointer)&keysh3;
		nparam++;
		keys[nparam] = "constant color keysh4";
		RtColor keysh4 = {coe0[4].x, coe0[4].y, coe0[4].z};
		vals[nparam] = (RtPointer)&keysh4;
		nparam++;
		keys[nparam] = "constant color keysh5";
		RtColor keysh5 = {coe0[5].x, coe0[5].y, coe0[5].z};
		vals[nparam] = (RtPointer)&keysh5;
		nparam++;
		keys[nparam] = "constant color keysh1";
		RtColor keysh6 = {coe0[6].x, coe0[6].y, coe0[6].z};
		vals[nparam] = (RtPointer)&keysh6;
		nparam++;
		keys[nparam] = "constant color keysh1";
		RtColor keysh7 = {coe0[7].x, coe0[7].y, coe0[7].z};
		vals[nparam] = (RtPointer)&keysh7;
		nparam++;
		keys[nparam] = "constant color keysh1";
		RtColor keysh8 = {coe0[8].x, coe0[8].y, coe0[8].z};
		vals[nparam] = (RtPointer)&keysh8;
		nparam++;
	}
	if(coe1) {
		keys[nparam] = "constant color backsh0";
		RtColor backsh0 = {coe1[0].x, coe1[0].y, coe1[0].z};
		vals[nparam] = (RtPointer)&backsh0;
		nparam++;
		keys[nparam] = "constant color backsh1";
		RtColor backsh1 = {coe1[1].x, coe1[1].y, coe1[1].z};
		vals[nparam] = (RtPointer)&backsh1;
		nparam++;
		keys[nparam] = "constant color backsh2";
		RtColor backsh2 = {coe1[2].x, coe1[2].y, coe1[2].z};
		vals[nparam] = (RtPointer)&backsh2;
		nparam++;
		keys[nparam] = "constant color backsh3";
		RtColor backsh3 = {coe1[3].x, coe1[3].y, coe1[3].z};
		vals[nparam] = (RtPointer)&backsh3;
		nparam++;
		keys[nparam] = "constant color backsh4";
		RtColor backsh4 = {coe1[4].x, coe1[4].y, coe1[4].z};
		vals[nparam] = (RtPointer)&backsh4;
		nparam++;
		keys[nparam] = "constant color backsh5";
		RtColor backsh5 = {coe1[5].x, coe1[5].y, coe1[5].z};
		vals[nparam] = (RtPointer)&backsh5;
		nparam++;
		keys[nparam] = "constant color backsh1";
		RtColor backsh6 = {coe1[6].x, coe1[6].y, coe1[6].z};
		vals[nparam] = (RtPointer)&backsh6;
		nparam++;
		keys[nparam] = "constant color backsh1";
		RtColor backsh7 = {coe1[7].x, coe1[7].y, coe1[7].z};
		vals[nparam] = (RtPointer)&backsh7;
		nparam++;
		keys[nparam] = "constant color backsh1";
		RtColor backsh8 = {coe1[8].x, coe1[8].y, coe1[8].z};
		vals[nparam] = (RtPointer)&backsh8;
		nparam++;
	}

	RiPointsV(numDot,nparam,keys,vals);
	
	delete[] pp;
	delete[] pw;
	delete[] po;
}
