#include "BoundHair.h"
#include "../shared/DiceTriangle.h"
#include "../shared/FNoise.h"
#include "BindTriangle.h"

BoundHair::BoundHair():cvs_a(0), cvs_b(0), cvs_c(0),velocities(0), shutters(0),densities(0)
{
	points = new XYZ[3]; 
	bindPoints = new XYZ[3];
	ucoord = new float[3];
	vcoord = new float[3];
	attrib = new float[5];
}

void BoundHair::calculateBBox(float *box) const
{
	box[0] = box[2] = box[4] = 10e6;
	box[1] = box[3] = box[5] = -10e6;
	XYZ v;
	float lv, mlv = 0;
	for(unsigned i=0; i<=nsegs[0]; i++) {
		if(cvs_a[i].x < box[0]) box[0] = cvs_a[i].x;
		if(cvs_a[i].x > box[1]) box[1] = cvs_a[i].x;
		if(cvs_a[i].y < box[2]) box[2] = cvs_a[i].y;
		if(cvs_a[i].y > box[3]) box[3] = cvs_a[i].y;
		if(cvs_a[i].z < box[4]) box[4] = cvs_a[i].z;
		if(cvs_a[i].z > box[5]) box[5] = cvs_a[i].z;
		if(i>0) {
			v = cvs_a[i] - cvs_a[i-1];
			lv = v.length();
			if(lv > mlv) mlv = lv;
		}
	}
	
	for(unsigned i=0; i<=nsegs[1]; i++) {
		if(cvs_b[i].x < box[0]) box[0] = cvs_b[i].x;
		if(cvs_b[i].x > box[1]) box[1] = cvs_b[i].x;
		if(cvs_b[i].y < box[2]) box[2] = cvs_b[i].y;
		if(cvs_b[i].y > box[3]) box[3] = cvs_b[i].y;
		if(cvs_b[i].z < box[4]) box[4] = cvs_b[i].z;
		if(cvs_b[i].z > box[5]) box[5] = cvs_b[i].z;
		if(i>0) {
			v = cvs_b[i] - cvs_b[i-1];
			lv = v.length();
			if(lv > mlv) mlv = lv;
		}
	}
	
	for(unsigned i=0; i<=nsegs[2]; i++) {
		if(cvs_c[i].x < box[0]) box[0] = cvs_c[i].x;
		if(cvs_c[i].x > box[1]) box[1] = cvs_c[i].x;
		if(cvs_c[i].y < box[2]) box[2] = cvs_c[i].y;
		if(cvs_c[i].y > box[3]) box[3] = cvs_c[i].y;
		if(cvs_c[i].z < box[4]) box[4] = cvs_c[i].z;
		if(cvs_c[i].z > box[5]) box[5] = cvs_c[i].z;
		if(i>0) {
			v = cvs_c[i] - cvs_c[i-1];
			lv = v.length();
			if(lv > mlv) mlv = lv;
		}
	}

	mlv /= 4;
	box[0] -= mlv;
	box[1] += mlv;
	box[2] -= mlv;
	box[3] += mlv;
	box[4] -= mlv;
	box[5] += mlv;
	
	if(velocities) {
		v.x = box[0] + velocities[0].x;
		if(v.x < box[0]) box[0] = v.x;
		v.x = box[1] + velocities[0].x;
		if(v.x > box[1]) box[1] = v.x;
		
		v.y = box[2] + velocities[0].y;
		if(v.y < box[2]) box[2] = v.y;
		v.y = box[3] + velocities[0].y;
		if(v.y > box[3]) box[3] = v.y;

		v.z = box[4] + velocities[0].z;
		if(v.z < box[4]) box[4] = v.z;
		v.z = box[5] + velocities[0].z;
		if(v.z > box[5]) box[5] = v.z;

		v.x = box[0] + velocities[1].x;
		if(v.x < box[0]) box[0] = v.x;
		v.x = box[1] + velocities[1].x;
		if(v.x > box[1]) box[1] = v.x;
		
		v.y = box[2] + velocities[1].y;
		if(v.y < box[2]) box[2] = v.y;
		v.y = box[3] + velocities[1].y;
		if(v.y > box[3]) box[3] = v.y;

		v.z = box[4] + velocities[1].z;
		if(v.z < box[4]) box[4] = v.z;
		v.z = box[5] + velocities[1].z;
		if(v.z > box[5]) box[5] = v.z;

		v.x = box[0] + velocities[2].x;
		if(v.x < box[0]) box[0] = v.x;
		v.x = box[1] + velocities[2].x;
		if(v.x > box[1]) box[1] = v.x;
		
		v.y = box[2] + velocities[2].y;
		if(v.y < box[2]) box[2] = v.y;
		v.y = box[3] + velocities[2].y;
		if(v.y > box[3]) box[3] = v.y;

		v.z = box[4] + velocities[2].z;
		if(v.z < box[4]) box[4] = v.z;
		v.z = box[5] + velocities[2].z;
		if(v.z > box[5]) box[5] = v.z;
	}
}

void BoundHair::emit() const
{
	float widthScale = 1.0, threshold = 1.0, realthreshold;
	threshold = distantSimp;
	widthScale = 1.0/threshold;
	
	int g_seed;
	FNoise fnoi;
	float noi;
	
	DiceTriangle ftri;
	ftri.create(bindPoints[0], bindPoints[1], bindPoints[2]);
	
	float delta = ftri.getArea();
	
	if(delta < epsilon*epsilon) delta *= 4.f;
	
	int estimate_ncell = delta/epsilon/epsilon*1.3f;
	if(estimate_ncell < 15) estimate_ncell = 15;
	
	DiceParam* ddice = new DiceParam[estimate_ncell];
	
	int n_samp = 0;
	
	ftri.setId(0, 1, 2);
	ftri.setS(ucoord[0], ucoord[1], ucoord[2]);
	ftri.setT(vcoord[0], vcoord[1], vcoord[2]);
	ftri.rasterize(epsilon, ddice, n_samp, seed+31);
	
	//printf(" %d:%d ", estimate_ncell, n_samp);
	if(n_samp > 0) {
// prune by threashold
		char* isurvive = new char[n_samp];
		int n_survive = 0;
		for(unsigned i=0; i<n_samp; i++) {
			g_seed  = seed*29 + i*17;
			noi = fnoi.randfint( g_seed );
			
			realthreshold = threshold;
			if(densities) realthreshold *= densities[ddice[i].id0]*ddice[i].alpha +  densities[ddice[i].id1]*ddice[i].beta + densities[ddice[i].id2]*ddice[i].gamma;
			
			if(noi < realthreshold) {
				isurvive[i] = 1;
				n_survive++;
			}
			else isurvive[i] = 0;
		}
		
		if(n_survive > 0) {
			
			XYZ* pbuf = new XYZ[n_samp];
			unsigned* nsegbuf = new unsigned[n_samp];
			for(unsigned i=0; i<n_samp; i++) {
				pbuf[i] = points[ddice[i].id0]*ddice[i].alpha + points[ddice[i].id1]*ddice[i].beta + points[ddice[i].id2]*ddice[i].gamma;
				nsegbuf[i] = nsegs[ddice[i].id0];
			}
			
			int ncurves = n_survive;
			int* nvertices = new int[ncurves];
			
			int npoints = 0;
			int nwidths = 0;
			int acc = 0;
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					nvertices[acc] = nsegbuf[i] + 5;
					if(threshold > 0.5) nvertices[acc] += nsegbuf[i];
					npoints += nvertices[acc];
					nwidths += nvertices[acc]-2;
					acc++;
				}
			}
			
			float* widths = new float[nwidths];
			acc = 0;
			float rootWidth = attrib[3];
			float tipWidth = attrib[4];
			
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					widths[acc] = rootWidth * widthScale;
					acc++;
					float dwidth = (tipWidth - rootWidth)/nsegbuf[i] ;
					for(unsigned j = 0; j<= nsegbuf[i]; j++) {
						widths[acc] = (rootWidth + dwidth*j)*widthScale;
						acc++;
						if(threshold > 0.5) {
							if(j != nsegbuf[i]) {
								widths[acc] = widths[acc-1];
								acc++;
							}
						}
					}
					widths[acc] = tipWidth * widthScale;
					acc++;
				}
			}
			
			float* coord_s = new float[ncurves];
			float* coord_t = new float[ncurves];
			float* mutant = new float[ncurves];
			acc = 0;
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					coord_s[acc] = ddice[i].coords;
					coord_t[acc] = ddice[i].coordt;
					
					g_seed  = seed*53 + i*5;
					mutant[acc] = fnoi.randfint( g_seed );
					
					acc++;
				}
			}
			
			XYZ* vertices = new XYZ[npoints];
			
			XYZ ppre, pcur, pt[3], q, dv[3], dmean, ddv, pcen, sidewind;
			int num_seg;
			acc = 0;
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					g_seed  = seed*19 + i*67;
					ppre = pbuf[i];
					
					vertices[acc] = ppre;
					acc++;
					vertices[acc] = ppre;
					acc++;
					
					num_seg = nsegbuf[i];
					float dparam = 1.f/(float)num_seg;
					float param;
					for(int j = 0; j< num_seg; j++) {
						param = dparam*j;
						getPatParam(pt[0], param, nsegs[0], cvs_a);
						getPatParam(pt[1], param, nsegs[1], cvs_b);
						getPatParam(pt[2], param, nsegs[2], cvs_c);
						param += dparam;
						getPatParam(q, param, nsegs[0], cvs_a);
						dv[0] = q - pt[0];
						pcen = q;
						getPatParam(q, param, nsegs[1], cvs_b);
						dv[1] = q - pt[1];
						pcen += q;
						getPatParam(q, param, nsegs[2], cvs_c);
						dv[2] = q - pt[2];
						pcen += q;
						pcen /= 3.f;
						
						vertices[acc] = ppre;
						acc++;
						
						pcur = pt[0] * ddice[i].alpha + pt[1] * ddice[i].beta + pt[2] * ddice[i].gamma;
						
						dmean = dv[0] * ddice[i].alpha + dv[1] * ddice[i].beta + dv[2] * ddice[i].gamma;
						dmean.setLength(dv[0].length());
						
						
						noi = 1.f + (fnoi.randfint( g_seed )-0.5)*attrib[0]; g_seed++;
						dmean *= noi;
						pcur += dmean;
						
						pcur += (pcen - pcur)*attrib[2]*j/num_seg;
						
						noi = fnoi.randfint( g_seed ); g_seed++;
						if(noi<0.33) sidewind = pt[0] - pt[1];
						else if(noi<0.66) sidewind = pt[1] - pt[2];
						else sidewind = pt[2] - pt[0];
						sidewind.setLength(dv[0].length());
						
						noi = (fnoi.randfint( g_seed )-0.5)*2*attrib[1]; g_seed++;

						pcur += sidewind*noi*(1 - attrib[2]*j/num_seg);
						
						ddv = pcur - ppre;
						ddv.normalize();
						ddv *= dmean.length();
						
						if(threshold > 0.5) {
							vertices[acc] = ppre + ddv*0.5f;
							acc++;
						}

						ppre += ddv;
					}
					vertices[acc] = ppre;
					acc++;
					vertices[acc] = ppre;
					acc++;
					vertices[acc] = ppre;
					acc++;
				}
			}
			
			delete[] pbuf;
			
			//if(shutters) RiMotionBegin(2, (RtFloat)shutters[0], (RtFloat)shutters[1]);
			
			RiCurves("cubic", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)vertices, "width", (RtPointer)widths, 
			"uniform float s", (RtFloat*)coord_s, "uniform float t", (RtFloat*)coord_t, "uniform float mutant", (RtFloat*)mutant,
			RI_NULL); 
			/*
			if(shutters) {
				acc = 0;
				for(unsigned i=0; i<n_samp; i++) {
					if(isurvive[i]) {
						ddv = velocities[0] * ddice[i].alpha + velocities[1] * ddice[i].beta + velocities[2] * ddice[i].gamma;
						
						vertices[acc] += ddv;
						acc++;
						vertices[acc] += ddv;
						acc++;
						num_seg = nsegbuf[i];
						for(int j = 0; j< num_seg; j++) {
							vertices[acc] += ddv;
							acc++;
							vertices[acc] += ddv;
							acc++;
						}
						vertices[acc] += ddv;
						acc++;
						vertices[acc] += ddv;
						acc++;
						vertices[acc] += ddv;
						acc++;
					}
				}
				
				RiCurves("cubic", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)vertices, "width", (RtPointer)widths, 
				"uniform float s", (RtFloat*)coord_s, "uniform float t", (RtFloat*)coord_t, "uniform float mutant", (RtFloat*)mutant,
				RI_NULL);
				
				RiMotionEnd();
			}
			*/
			delete[] nsegbuf;
			delete[] nvertices;
			delete[] vertices;
			delete[] widths;
			delete[] coord_s;
			delete[] coord_t;
			delete[] mutant;

		}
		delete[] isurvive;
	}
	
	delete[] ddice;
}

void BoundHair::emit(float detail) const
{
	float widthScale = 1.0, threshold = 1.0, realthreshold;
	if(detail < 9000) {
		if(detail <1000) {
			threshold = 0.2;
			widthScale = 5;
		}
		else {
			threshold = 1.0 - 0.8*(9000 - detail)/8000;
			widthScale = 1.0/threshold;
		}
	}
	
	int g_seed;
	FNoise fnoi;
	float noi;
	
	DiceTriangle ftri;
	ftri.create(bindPoints[0], bindPoints[1], bindPoints[2]);
	
	float delta = ftri.getArea();
	
	if(delta < epsilon*epsilon) delta *= 4.f;
	
	int estimate_ncell = delta/epsilon/epsilon*1.3f;
	if(estimate_ncell < 15) estimate_ncell = 15;
	
	DiceParam* ddice = new DiceParam[estimate_ncell];
	
	int n_samp = 0;
	
	ftri.setId(0, 1, 2);
	ftri.setS(ucoord[0], ucoord[1], ucoord[2]);
	ftri.setT(vcoord[0], vcoord[1], vcoord[2]);
	ftri.rasterize(epsilon, ddice, n_samp, seed+31);
	
	//printf(" %d:%d ", estimate_ncell, n_samp);
	if(n_samp > 0) {
// prune by threashold
		char* isurvive = new char[n_samp];
		int n_survive = 0;
		for(unsigned i=0; i<n_samp; i++) {
			g_seed  = seed*29 + i*17;
			noi = fnoi.randfint( g_seed );
			
			realthreshold = threshold;
			if(densities) realthreshold *= densities[ddice[i].id0]*ddice[i].alpha +  densities[ddice[i].id1]*ddice[i].beta + densities[ddice[i].id2]*ddice[i].gamma;
			
			if(noi < realthreshold) {
				isurvive[i] = 1;
				n_survive++;
			}
			else isurvive[i] = 0;
		}
		
		if(n_survive > 0) {
			
			XYZ* pbuf = new XYZ[n_samp];
			unsigned* nsegbuf = new unsigned[n_samp];
			for(unsigned i=0; i<n_samp; i++) {
				pbuf[i] = points[ddice[i].id0]*ddice[i].alpha + points[ddice[i].id1]*ddice[i].beta + points[ddice[i].id2]*ddice[i].gamma;
				nsegbuf[i] = nsegs[ddice[i].id0];
			}
			
			int ncurves = n_survive;
			int* nvertices = new int[ncurves];
			
			int npoints = 0;
			int nwidths = 0;
			int acc = 0;
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					nvertices[acc] = nsegbuf[i] + 5;
					if(threshold > 0.5) nvertices[acc] += nsegbuf[i];
					npoints += nvertices[acc];
					nwidths += nvertices[acc]-2;
					acc++;
				}
			}
			
			float* widths = new float[nwidths];
			acc = 0;
			float rootWidth = attrib[3];
			float tipWidth = attrib[4];
			
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					widths[acc] = rootWidth * widthScale;
					acc++;
					float dwidth = (tipWidth - rootWidth)/nsegbuf[i] ;
					for(unsigned j = 0; j<= nsegbuf[i]; j++) {
						widths[acc] = (rootWidth + dwidth*j)*widthScale;
						acc++;
						
						if(threshold > 0.5) {
							if(j != nsegbuf[i]) {
								widths[acc] = widths[acc-1];
								acc++;
							}
						}
					}
					widths[acc] = tipWidth * widthScale;
					acc++;
				}
			}
			
			float* coord_s = new float[ncurves];
			float* coord_t = new float[ncurves];
			float* mutant = new float[ncurves];
			acc = 0;
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					coord_s[acc] = ddice[i].coords;
					coord_t[acc] = ddice[i].coordt;
					
					g_seed  = seed*53 + i*5;
					mutant[acc] = fnoi.randfint( g_seed );
					
					acc++;
				}
			}
			
			XYZ* vertices = new XYZ[npoints];
			
			XYZ ppre, pcur, pt[3], q, dv[3], dmean, ddv, pcen, sidewind;
			int num_seg;
			acc = 0;
			for(unsigned i=0; i<n_samp; i++) {
				if(isurvive[i]) {
					g_seed  = seed*19 + i*67;
					ppre = pbuf[i];
					
					vertices[acc] = ppre;
					acc++;
					vertices[acc] = ppre;
					acc++;
					
					num_seg = nsegbuf[i];
					float dparam = 1.f/(float)num_seg;
					float param;
					for(int j = 0; j< num_seg; j++) {
						param = dparam*j;
						getPatParam(pt[0], param, nsegs[0], cvs_a);
						getPatParam(pt[1], param, nsegs[1], cvs_b);
						getPatParam(pt[2], param, nsegs[2], cvs_c);
						param += dparam;
						getPatParam(q, param, nsegs[0], cvs_a);
						dv[0] = q - pt[0];
						pcen = q;
						getPatParam(q, param, nsegs[1], cvs_b);
						dv[1] = q - pt[1];
						pcen += q;
						getPatParam(q, param, nsegs[2], cvs_c);
						dv[2] = q - pt[2];
						pcen += q;
						pcen /= 3.f;
						
						vertices[acc] = ppre;
						acc++;
						
						pcur = pt[0] * ddice[i].alpha + pt[1] * ddice[i].beta + pt[2] * ddice[i].gamma;
						
						dmean = dv[0] * ddice[i].alpha + dv[1] * ddice[i].beta + dv[2] * ddice[i].gamma;
						dmean.setLength(dv[0].length());
						
						
						noi = 1.f + (fnoi.randfint( g_seed )-0.5)*attrib[0]; g_seed++;
						dmean *= noi;
						pcur += dmean;
						
						pcur += (pcen - pcur)*attrib[2]*j/num_seg;
						
						noi = fnoi.randfint( g_seed ); g_seed++;
						if(noi<0.33) sidewind = pt[0] - pt[1];
						else if(noi<0.66) sidewind = pt[1] - pt[2];
						else sidewind = pt[2] - pt[0];
						sidewind.setLength(dv[0].length());
						
						noi = (fnoi.randfint( g_seed )-0.5)*2*attrib[1]; g_seed++;

						pcur += sidewind*noi*(1 - attrib[2]*j/num_seg);
						
						ddv = pcur - ppre;
						ddv.normalize();
						ddv *= dmean.length();
						
						if(threshold > 0.5) {
							vertices[acc] = ppre + ddv*0.5f;
							acc++;
						}

						ppre += ddv;
					}
					vertices[acc] = ppre;
					acc++;
					vertices[acc] = ppre;
					acc++;
					vertices[acc] = ppre;
					acc++;
				}
			}
			
			delete[] pbuf;
			
			//if(shutters) RiMotionBegin(2, (RtFloat)shutters[0], (RtFloat)shutters[1]);
			
			RiCurves("cubic", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)vertices, "width", (RtPointer)widths, 
			"uniform float s", (RtFloat*)coord_s, "uniform float t", (RtFloat*)coord_t, "uniform float mutant", (RtFloat*)mutant,
			RI_NULL); 
			/*
			if(shutters) {
				acc = 0;
				for(unsigned i=0; i<n_samp; i++) {
					if(isurvive[i]) {
						ddv = velocities[0] * ddice[i].alpha + velocities[1] * ddice[i].beta + velocities[2] * ddice[i].gamma;
						
						vertices[acc] += ddv;
						acc++;
						vertices[acc] += ddv;
						acc++;
						num_seg = nsegbuf[i];
						for(int j = 0; j< num_seg; j++) {
							vertices[acc] += ddv;
							acc++;
							vertices[acc] += ddv;
							acc++;
						}
						vertices[acc] += ddv;
						acc++;
						vertices[acc] += ddv;
						acc++;
						vertices[acc] += ddv;
						acc++;
					}
				}
				
				RiCurves("cubic", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)vertices, "width", (RtPointer)widths, 
				"uniform float s", (RtFloat*)coord_s, "uniform float t", (RtFloat*)coord_t, "uniform float mutant", (RtFloat*)mutant,
				RI_NULL);
				
				RiMotionEnd();
			}
			*/
			delete[] nsegbuf;
			delete[] nvertices;
			delete[] vertices;
			delete[] widths;
			delete[] coord_s;
			delete[] coord_t;
			delete[] mutant;

		}
		delete[] isurvive;
	}
	
	delete[] ddice;
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
	
	float width = 0.05;
	RiCurves("cubic", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)pcv, "constantwidth", (RtPointer)&width, RI_NULL); 
	
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
	float width = 0.05;
	RiCurves("linear", (RtInt)ncurves, (RtInt*)nvertices, "nonperiodic", "P", (RtPoint*)bpoints, "constantwidth", (RtPointer)&width, RI_NULL); 
	delete[] bpoints;
}

void BoundHair::release()
{
	delete[] cvs_a;
	delete[] cvs_b;
	delete[] cvs_c;
	delete[] points;
	delete[] bindPoints;
	delete[] ucoord;
	delete[] vcoord;
	delete[] attrib;
	if(velocities) delete[] velocities;
	if(shutters) delete[] shutters;
	if(densities) delete[] densities;
}

void BoundHair::getPatParam(XYZ& p, const float& param, const unsigned& nseg, const XYZ* cvs) const
{
	float fparam = param * nseg;
	int iparam = fparam - 0.000001;
	p = cvs[iparam] + (cvs[iparam+1] - cvs[iparam]) * (fparam-iparam);
}
//:~