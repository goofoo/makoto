#include "BoundHair.h"
#include "../shared/DiceTriangle.h"
#include "BindTriangle.h"

BoundHair::BoundHair():cvs_a(0), cvs_b(0), cvs_c(0) 
{
	points = new XYZ[3]; 
	bindPoints = new XYZ[3];
	ucoord = new float[3];
	vcoord = new float[3];
}
    

void BoundHair::calculateBBox(float *box) const
{
	box[0] = box[2] = box[4] = 10e6;
	box[1] = box[3] = box[5] = -10e6;
	
	for(unsigned i=0; i<=nsegs[0]; i++) {
		if(cvs_a[i].x < box[0]) box[0] = cvs_a[i].x;
		if(cvs_a[i].x > box[1]) box[1] = cvs_a[i].x;
		if(cvs_a[i].y < box[2]) box[2] = cvs_a[i].y;
		if(cvs_a[i].y > box[3]) box[3] = cvs_a[i].y;
		if(cvs_a[i].z < box[4]) box[4] = cvs_a[i].z;
		if(cvs_a[i].z > box[5]) box[5] = cvs_a[i].z;
	}
	
	for(unsigned i=0; i<=nsegs[1]; i++) {
		if(cvs_b[i].x < box[0]) box[0] = cvs_b[i].x;
		if(cvs_b[i].x > box[1]) box[1] = cvs_b[i].x;
		if(cvs_b[i].y < box[2]) box[2] = cvs_b[i].y;
		if(cvs_b[i].y > box[3]) box[3] = cvs_b[i].y;
		if(cvs_b[i].z < box[4]) box[4] = cvs_b[i].z;
		if(cvs_b[i].z > box[5]) box[5] = cvs_b[i].z;
	}
	
	for(unsigned i=0; i<=nsegs[2]; i++) {
		if(cvs_c[i].x < box[0]) box[0] = cvs_c[i].x;
		if(cvs_c[i].x > box[1]) box[1] = cvs_c[i].x;
		if(cvs_c[i].y < box[2]) box[2] = cvs_c[i].y;
		if(cvs_c[i].y > box[3]) box[3] = cvs_c[i].y;
		if(cvs_c[i].z < box[4]) box[4] = cvs_c[i].z;
		if(cvs_c[i].z > box[5]) box[5] = cvs_c[i].z;
	}
}

void BoundHair::emit(float detail) const
{
	DiceTriangle ftri;
	ftri.create(bindPoints[0], bindPoints[1], bindPoints[2]);
	
	float delta = ftri.getArea();
	
	if(delta < epsilon*epsilon) delta *= 4.f;
	
	int estimate_ncell = delta/epsilon/epsilon*1.3f;
	if(estimate_ncell < 16) estimate_ncell = 16;
	
	DiceParam* ddice = new DiceParam[estimate_ncell];
	
	int n_samp = 0;
	
	ftri.setId(0, 1, 2);
	ftri.setS(ucoord[0], ucoord[1], ucoord[2]);
	ftri.setT(vcoord[0], vcoord[1], vcoord[2]);
	ftri.rasterize(epsilon, ddice, n_samp, seed+31);
	
	printf(" %d:%d ", estimate_ncell, n_samp);
	
	if(n_samp > 0) {
	
	
	}
	
	delete[] ddice;
	//int npolys = 1;
	//int nvertices[3]; nvertices[0] = 3;
	//int vertices[3] = {0, 1, 2};
	//XYZ pp[3];
	//pp[0] = cvs_a[0];
	//pp[1] = cvs_b[0];
	//pp[2] = cvs_c[0];
	
	//printf("%d %d %d  ", nsegs[0], nsegs[1], nsegs[2]);
	
	//RiPointsPolygons( (RtInt)npolys, (RtInt*)nvertices, (RtInt*)vertices,
	//"P", (RtPoint*)pp, RI_NULL);
}

void BoundHair::emitGuider() const
{
	int ncurves = 3;

	int nvertices[3];
	nvertices[0] = nsegs[0] + 5;
	nvertices[1] = nsegs[1] + 5;
	nvertices[2] = nsegs[2] + 5;
	
	XYZ* pcv = new XYZ[nvertices[0] + nvertices[1] + nvertices[2]];
	int acc = 0;
	
	pcv[acc] = cvs_a[0];
	acc++;
	pcv[acc] = cvs_a[0];
	acc++;
	for(unsigned i=0; i<=nsegs[0]; i++) {
		pcv[acc] = cvs_a[i];
		acc++;
	}
	pcv[acc] = cvs_a[nsegs[0]];
	acc++;
	pcv[acc] = cvs_a[nsegs[0]];
	acc++;
	
	pcv[acc] = cvs_b[0];
	acc++;
	pcv[acc] = cvs_b[0];
	acc++;
	for(unsigned i=0; i<=nsegs[1]; i++) {
		pcv[acc] = cvs_b[i];
		acc++;
	}
	pcv[acc] = cvs_b[nsegs[1]];
	acc++;
	pcv[acc] = cvs_b[nsegs[1]];
	acc++;
	
	pcv[acc] = cvs_c[0];
	acc++;
	pcv[acc] = cvs_c[0];
	acc++;
	for(unsigned i=0; i<=nsegs[2]; i++) {
		pcv[acc] = cvs_c[i];
		acc++;
	}
	pcv[acc] = cvs_c[nsegs[2]];
	acc++;
	pcv[acc] = cvs_c[nsegs[2]];
	acc++;
	
	RiCurves("cubic", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)pcv, RI_NULL); 
	
	delete[] pcv;
}

void BoundHair::emitBBox() const
{
	float box[6];
	calculateBBox(box);
	
	XYZ* bpoints = new XYZ[24];
	bpoints[0] = XYZ(box[0], box[2], box[4]);
	bpoints[1] = XYZ(box[1], box[2], box[4]);
	bpoints[2] = XYZ(box[0], box[3], box[4]);
	bpoints[3] = XYZ(box[1], box[3], box[4]);
	bpoints[4] = XYZ(box[0], box[2], box[5]);
	bpoints[5] = XYZ(box[1], box[2], box[5]);
	bpoints[6] = XYZ(box[0], box[3], box[5]);
	bpoints[7] = XYZ(box[1], box[3], box[5]);
	
	bpoints[8] = XYZ(box[0], box[2], box[4]);
	bpoints[9] = XYZ(box[0], box[3], box[4]);
	bpoints[10] = XYZ(box[1], box[2], box[4]);
	bpoints[11] = XYZ(box[1], box[3], box[4]);
	bpoints[12] = XYZ(box[0], box[2], box[5]);
	bpoints[13] = XYZ(box[0], box[3], box[5]);
	bpoints[14] = XYZ(box[1], box[2], box[5]);
	bpoints[15] = XYZ(box[1], box[3], box[5]);
	
	bpoints[16] = XYZ(box[0], box[2], box[4]);
	bpoints[17] = XYZ(box[0], box[2], box[5]);
	bpoints[18] = XYZ(box[1], box[2], box[4]);
	bpoints[19] = XYZ(box[1], box[2], box[5]);
	bpoints[20] = XYZ(box[0], box[3], box[4]);
	bpoints[21] = XYZ(box[0], box[3], box[5]);
	bpoints[22] = XYZ(box[1], box[3], box[4]);
	bpoints[23] = XYZ(box[1], box[3], box[5]);
	
	int ncurves = 12;
	int nvertices[12]; for(unsigned i=0; i<12; i++) nvertices[i] = 2;
	
	RiCurves("linear", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)bpoints, RI_NULL); 
	delete[] bpoints;
}

void BoundHair::release()
{
	if(cvs_a) delete[] cvs_a;
	if(cvs_b) delete[] cvs_b;
	if(cvs_c) delete[] cvs_c;
	if(points) delete[] points;
	if(bindPoints) delete[] bindPoints;
	delete[] ucoord;
	delete[] vcoord;
}
//:~