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

RCloud::RCloud()
{
}

void RCloud::emit()
{
	int numDot = 9+detail*detail*0.25;

	if(numDot > 1000) numDot = 1000;

	XYZ* pp = new XYZ[numDot];
	XYZ* po = new XYZ[numDot];
	XYZ* pc = new XYZ[numDot];
	float* pw = new float[numDot];
	double theta, phi;
	float rr, noi, adens = density*5/(numDot-8);
	if(adens < 0.008) adens = 0.008;
	
	for(int i=0; i<numDot; i++) {
		sphereRand(pp[i], i);
		
		xyz2sph(pp[i], theta, phi);
		
		noi = noise.randfint(i);
		noi = 0.03 + noi*0.969;
		rr = sqrt(noi);
		
		rr *= radius*2;
		
		pp[i] *= rr;
		
		rr = radius*29/(detail+17+dusty*37);
		if(noi<0.4) rr += rr*(0.95-noi)*(1-dusty);
		else rr -= rr*1.3*(noi-0.3)*(1-dusty);
		
		pw[i] = rr;
		
		rr = adens;
		rr += (rr*(1 - noi) - rr)*(1-dusty);
		po[i].x = po[i].y = po[i].z = rr;
		
		pc[i] = col;
	}
	RtToken nams[32];
	nams[0] = "P"; nams[1] = "width"; nams[2] = "Os";nams[3] = "Cs";
	
	RtPointer vals[32];
	vals[0] = (RtPointer)pp; vals[1] = (RtPointer)pw; vals[2] = (RtPointer)po;vals[3] = (RtPointer)pc;
	
	int nparam = 4;

	nams[nparam] = "constant color keysh0";
	RtFloat keysh0[3] = {coe0[0].x, coe0[0].y, coe0[0].z};
	vals[nparam] = (RtPointer)keysh0;
	nparam++;
	nams[nparam] = "constant color keysh1";
	RtFloat keysh1[3] = {coe0[1].x, coe0[1].y, coe0[1].z};
	vals[nparam] = (RtPointer)keysh1;
	nparam++;
	nams[nparam] = "constant color keysh2";
	RtFloat keysh2[3] = {coe0[2].x, coe0[2].y, coe0[2].z};
	vals[nparam] = (RtPointer)keysh2;
	nparam++;
	nams[nparam] = "constant color keysh3";
	RtFloat keysh3[3] = {coe0[3].x, coe0[3].y, coe0[3].z};
	vals[nparam] = (RtPointer)keysh3;
	nparam++;
	nams[nparam] = "constant color keysh4";
	RtFloat keysh4[3] = {coe0[4].x, coe0[4].y, coe0[4].z};
	vals[nparam] = (RtPointer)keysh4;
	nparam++;
	nams[nparam] = "constant color keysh5";
	RtFloat keysh5[3] = {coe0[5].x, coe0[5].y, coe0[5].z};
	vals[nparam] = (RtPointer)keysh5;
	nparam++;
	nams[nparam] = "constant color keysh6";
	RtFloat keysh6[3] = {coe0[6].x, coe0[6].y, coe0[6].z};
	vals[nparam] = (RtPointer)keysh6;
	nparam++;
	nams[nparam] = "constant color keysh7";
	RtFloat keysh7[3] = {coe0[7].x, coe0[7].y, coe0[7].z};
	vals[nparam] = (RtPointer)keysh7;
	nparam++;
	nams[nparam] = "constant color keysh8";
	RtFloat keysh8[3] = {coe0[8].x, coe0[8].y, coe0[8].z};
	vals[nparam] = (RtPointer)keysh8;
	nparam++;
	
	
	nams[nparam] = "constant color backsh0";
	RtFloat backsh0[3] = {coe1[0].x, coe1[0].y, coe1[0].z};
	vals[nparam] = (RtPointer)backsh0;
	nparam++;
	nams[nparam] = "constant color backsh1";
	RtFloat backsh1[3] = {coe1[1].x, coe1[1].y, coe1[1].z};
	vals[nparam] = (RtPointer)backsh1;
	nparam++;
	nams[nparam] = "constant color backsh2";
	RtFloat backsh2[3] = {coe1[2].x, coe1[2].y, coe1[2].z};
	vals[nparam] = (RtPointer)backsh2;
	nparam++;
	nams[nparam] = "constant color backsh3";
	RtFloat backsh3[3] = {coe1[3].x, coe1[3].y, coe1[3].z};
	vals[nparam] = (RtPointer)backsh3;
	nparam++;
	nams[nparam] = "constant color backsh4";
	RtFloat backsh4[3] = {coe1[4].x, coe1[4].y, coe1[4].z};
	vals[nparam] = (RtPointer)backsh4;
	nparam++;
	nams[nparam] = "constant color backsh5";
	RtFloat backsh5[3] = {coe1[5].x, coe1[5].y, coe1[5].z};
	vals[nparam] = (RtPointer)backsh5;
	nparam++;
	nams[nparam] = "constant color backsh6";
	RtFloat backsh6[3] = {coe1[6].x, coe1[6].y, coe1[6].z};
	vals[nparam] = (RtPointer)backsh6;
	nparam++;
	nams[nparam] = "constant color backsh7";
	RtFloat backsh7[3] = {coe1[7].x, coe1[7].y, coe1[7].z};
	vals[nparam] = (RtPointer)backsh7;
	nparam++;
	nams[nparam] = "constant color backsh8";
	RtFloat backsh8[3] = {coe1[8].x, coe1[8].y, coe1[8].z};
	vals[nparam] = (RtPointer)backsh8;
	nparam++;

	RiPointsV(numDot,nparam,nams,vals);
	
	delete[] pp;
	delete[] pw;
	delete[] po;
	delete[] pc;
}

void RCloud::setCoe0(const XYZ* data)
{
	coe0 = data;
}

void RCloud::setCoe1(const XYZ* data)
{
	coe1 = data;
}
