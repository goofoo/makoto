/*
 *  HairCache.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "HairCache.h"
#include "../shared/FNoise.h"
#include "../shared/QuickSort.h"
#include "../shared/zFnEXR.h"
#include "../shared/zGlobal.h"
#include <iostream>
#include <fstream>
using namespace std;

HairCache::HairCache():ddice(0),n_samp(0),guide_data(0),bind_data(0),guide_spaceinv(0),
parray(0),pconnection(0),uarray(0),varray(0),pBind(0),
sum_area(0.f),ndice(24),
nvertices(0),vertices(0),widths(0),coord_s(0),coord_t(0),
rootColorArray(0), tipColorArray(0), pNSeg(0),kbald(0.f),pDensmap(0),factorwidth(1.f)
{
	m_densityname = "nil";
}
HairCache::~HairCache() 
{
	if(ddice) delete[] ddice;
	if(guide_data) {
		for(unsigned i=0; i<num_guide; i++) guide_data[i].release();
		delete[] guide_data;
	}
	if(bind_data) delete[] bind_data;
	if(guide_spaceinv) delete[] guide_spaceinv;
	if(parray) delete[] parray;
	if(pconnection) delete[] pconnection;
	if(uarray) delete[] uarray;
	if(varray) delete[] varray;
	if(nvertices) delete[] nvertices;
	if(vertices) delete[] vertices;
	if(widths) delete[] widths;
	if(coord_s) delete[] coord_s;
	if(coord_t) delete[] coord_t;
	if(rootColorArray) delete[] rootColorArray;
	if(tipColorArray) delete[] tipColorArray;
	if(pBind) delete[] pBind;
	if(pNSeg) delete[] pNSeg;
	if(pDensmap) delete[] pDensmap;
}

float HairCache::getEpsilon() const
{
	return sqrt(sum_area/n_tri/2/(ndice+2));
}

int HairCache::dice(float detail)
{
	if(!pconnection || !pBind) return 0;
		
	float epsilon = sqrt(sum_area/n_tri/2/(ndice+2));

	int estimate_ncell = n_tri*(ndice+2)*2;
	
	factorwidth = 1.f;
	/*if(detail < estimate_ncell) {
		int old_ndice = ndice;
		ndice = detail/n_tri/2;
		if(ndice < 1) ndice = 1;
		epsilon = sqrt(sum_area/n_tri/2/ndice);
		//cout<<"real ndice: "<<ndice<<endl;
		estimate_ncell = n_tri*ndice*2;
		
		factorwidth =(float)old_ndice/ndice;
	}*/
	estimate_ncell += estimate_ncell/9;
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[estimate_ncell];
	n_samp = 0;

	DiceTriangle ftri;
	int a, b, c;
	
	int seed = 12;
	for(unsigned i=0; i<n_tri; i++) 
	{
		a = pconnection[i*3];
		b = pconnection[i*3+1];
		c = pconnection[i*3+2];
		
		ftri.create(pBind[a], pBind[b], pBind[c]);
		ftri.setId(a, b, c);
		ftri.setS(uarray[i*3], uarray[i*3+1], uarray[i*3+2]);
		ftri.setT(varray[i*3], varray[i*3+1], varray[i*3+2]);
		ftri.rasterize(epsilon, ddice, n_samp, seed);seed++;
	}
	
	return n_samp;	
}

int HairCache::pushFaceVertice()
{
	if(!pconnection) return 0;
	
	factorwidth = 1.f;
	
	n_samp = n_tri*3;
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[n_samp];

	int a, b, c, acc=0;
	
	for(unsigned i=0; i<n_tri; i++) {
		a = pconnection[i*3];
		b = pconnection[i*3+1];
		c = pconnection[i*3+2];
		
		ddice[acc].alpha = 1.f;
		ddice[acc].beta = ddice[acc].gamma = 0.f;
		ddice[acc].id0 = a;
		ddice[acc].id1 = b;
		ddice[acc].id2 = c;
		ddice[acc].coords = uarray[acc];
		ddice[acc].coordt = varray[acc];
		
		acc++;
		
		ddice[acc].alpha = 1.f;
		ddice[acc].beta = ddice[acc].gamma = 0.f;
		ddice[acc].id0 = b;
		ddice[acc].id1 = c;
		ddice[acc].id2 = a;
		ddice[acc].coords = uarray[acc];
		ddice[acc].coordt = varray[acc];
		
		acc++;
		
		ddice[acc].alpha = 1.f;
		ddice[acc].beta = ddice[acc].gamma = 0.f;
		ddice[acc].id0 = c;
		ddice[acc].id1 = a;
		ddice[acc].id2 = b;
		ddice[acc].coords = uarray[acc];
		ddice[acc].coordt = varray[acc];
		
		acc++;
	}
	
	return n_samp;	
}

void HairCache::bind()
{
	if(!guide_data || n_samp < 1) return;

	if(bind_data) delete[] bind_data;
	bind_data = new triangle_bind_info[n_samp];
	
	if(pNSeg) delete[] pNSeg;
	pNSeg = new unsigned[n_samp];
	
	for(unsigned i=0; i<n_samp; i++)
	{
// find the nearest guide
		XY from(ddice[i].coords, ddice[i].coordt);
		ValueAndId* idx = new ValueAndId[num_guide];
		for(unsigned j=0; j<num_guide; j++)
		{
			idx[j].idx = j;
			
			XY to(guide_data[j].u, guide_data[j].v);
			idx[j].val = from.distantTo(to);
		}
		
		QuickSort::sort(idx, 0, num_guide-1);
// find closest guider valid in 3D
		unsigned validid = 0;
		XYZ pto = pBind[ddice[i].id0]*ddice[i].alpha + pBind[ddice[i].id1]*ddice[i].beta + pBind[ddice[i].id2]*ddice[i].gamma;
		XYZ dto = pto - guide_data[idx[validid].idx].P[0];
		float dist2valid = dto.length();
		
		while(dist2valid > guide_data[idx[validid].idx].radius*2 && validid<num_guide-1) {
			validid++;
			dto = pto - guide_data[idx[validid].idx].P[0];
			dist2valid = dto.length();
		}
		
		bind_data[i].wei[0] = 1.f;
		bind_data[i].wei[1] = bind_data[i].wei[2] = 0.f;

		if(isInterpolate==1 && validid < num_guide-6) {
			XY corner[3];XYZ pw[3]; float dist[3];
			
			for(unsigned hdl=0; hdl<3; hdl++) {
			
				bind_data[i].idx[0] = idx[validid].idx;
				bind_data[i].idx[1] = idx[validid+1+hdl].idx;
				bind_data[i].idx[2] = idx[validid+2+hdl].idx;
		
				corner[0].x = guide_data[idx[validid].idx].u;
				corner[1].x = guide_data[idx[validid+1+hdl].idx].u;
				corner[2].x = guide_data[idx[validid+2+hdl].idx].u;
				corner[0].y = guide_data[idx[validid].idx].v;
				corner[1].y = guide_data[idx[validid+1+hdl].idx].v;
				corner[2].y = guide_data[idx[validid+2+hdl].idx].v;
				
				pw[0] = guide_data[idx[validid].idx].P[0];
				pw[1] = guide_data[idx[validid+1+hdl].idx].P[0];
				pw[2] = guide_data[idx[validid+2+hdl].idx].P[0];
				
				dist[0] = guide_data[idx[validid].idx].radius;
				dist[1] = guide_data[idx[validid+1+hdl].idx].radius;
				dist[2] = guide_data[idx[validid+2+hdl].idx].radius;
				
				if( BindTriangle::set(corner, from, pw, pto, dist, bind_data[i]) ) hdl = 4;
				else bind_data[i].idx[0] = bind_data[i].idx[1] = bind_data[i].idx[2] = idx[validid].idx;
			}
		}
		else {
			bind_data[i].idx[0] = idx[validid].idx;
			bind_data[i].idx[1] = idx[validid].idx;
			bind_data[i].idx[2] = idx[validid].idx;
		}
		
		pNSeg[i] = guide_data[bind_data[i].idx[0]].num_seg;// * bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].num_seg * bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].num_seg * bind_data[i].wei[2];
		
		delete[] idx;
	}
}

int HairCache::load(const char* filename)
{
	ifstream infile;
	infile.open(filename, ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		cout<<"Cannot open file: "<<filename<<endl;
		return 0;
	}
	m_cachename = filename;
	infile.read((char*)&num_guide,sizeof(unsigned));
	
	if(guide_data) {
		for(unsigned i=0; i<num_guide; i++) guide_data[i].release();
		delete[] guide_data;
	}
    guide_data = new Dguide[num_guide];
	for(unsigned i = 0;i<num_guide;i++)
	{
		infile.read((char*)&guide_data[i].num_seg, sizeof(guide_data[i].num_seg));

		guide_data[i].P = new XYZ[guide_data[i].num_seg];
		guide_data[i].N = new XYZ[guide_data[i].num_seg];
		guide_data[i].T = new XYZ[guide_data[i].num_seg];
		guide_data[i].dispv = new XYZ[guide_data[i].num_seg];
		guide_data[i].space = new MATRIX44F[guide_data[i].num_seg];

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)&guide_data[i].u, sizeof(float));
		infile.read((char*)&guide_data[i].v, sizeof(float));
		infile.read((char*)&guide_data[i].radius, sizeof(float));
	}
	infile.read((char*)&sum_area,sizeof(float));
	//infile.read((char*)&n_tri,sizeof(unsigned));
	
	//if(pconnection) delete[] pconnection;
	//pconnection = new int[n_tri*3];
	//infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	if(parray) delete[] parray;
	parray = new XYZ[n_vert];
	infile.read((char*)parray, sizeof(XYZ)*n_vert);
	
	infile.close();	
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		for(unsigned j=0; j<guide_data[i].num_seg; j++)
		{
			guide_data[i].space[j].setIdentity();
			XYZ binor = guide_data[i].N[j].cross(guide_data[i].T[j]);
			guide_data[i].space[j].setOrientations(guide_data[i].T[j], binor, guide_data[i].N[j]);
			guide_data[i].space[j].setTranslation(guide_data[i].P[j]);
		}
		guide_spaceinv[i] = guide_data[i].space[0];
		guide_spaceinv[i].inverse();
	}
	
	return 1;
}

int HairCache::loadStart(const char* filename)
{
	ifstream infile;
	infile.open(filename, ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		cout<<"Cannot open file: "<<filename<<endl;
		return 0;
	}
	m_cachename = filename;
	infile.read((char*)&num_guide,sizeof(unsigned));
	
	if(guide_data) {
		for(unsigned i=0; i<num_guide; i++) guide_data[i].release();
		delete[] guide_data;
	}
    guide_data = new Dguide[num_guide];
	for(unsigned i = 0;i<num_guide;i++)
	{
		infile.read((char*)&guide_data[i].num_seg, sizeof(guide_data[i].num_seg));

		guide_data[i].P = new XYZ[guide_data[i].num_seg];
		guide_data[i].N = new XYZ[guide_data[i].num_seg];
		guide_data[i].T = new XYZ[guide_data[i].num_seg];
		guide_data[i].dispv = new XYZ[guide_data[i].num_seg];
		guide_data[i].space = new MATRIX44F[guide_data[i].num_seg];

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)&guide_data[i].u, sizeof(float));
		infile.read((char*)&guide_data[i].v, sizeof(float));
		infile.read((char*)&guide_data[i].radius, sizeof(float));
	}
	infile.read((char*)&sum_area,sizeof(float));
	infile.read((char*)&n_tri,sizeof(unsigned));
	
	if(pconnection) delete[] pconnection;
	pconnection = new int[n_tri*3];
	infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	if(pBind) delete[] pBind;
	pBind = new XYZ[n_vert];
	infile.read((char*)pBind, sizeof(XYZ)*n_vert);
	
	if(uarray) delete[] uarray;
	uarray = new float[n_tri*3];
	infile.read((char*)uarray, sizeof(float)*n_tri*3);
	
	if(varray) delete[] varray;
	varray = new float[n_tri*3];
	infile.read((char*)varray, sizeof(float)*n_tri*3);
	
	infile.close();	
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		for(unsigned j=0; j<guide_data[i].num_seg; j++)
		{
			guide_data[i].space[j].setIdentity();
			XYZ binor = guide_data[i].N[j].cross(guide_data[i].T[j]);
			guide_data[i].space[j].setOrientations(guide_data[i].T[j], binor, guide_data[i].N[j]);
			guide_data[i].space[j].setTranslation(guide_data[i].P[j]);
		}
		guide_spaceinv[i] = guide_data[i].space[0];
		guide_spaceinv[i].inverse();
	}
	return 1;
}

void HairCache::create()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray || !pNSeg) return;
	
	int g_seed = 13;
	FNoise fnoi;
	
	float noi;
	realncurve = 0;
	char* isoverbald = new char[n_samp];
	for(unsigned i=0; i<n_samp; i++) {
		noi = fnoi.randfint( g_seed ); g_seed++;
		if(pDensmap) muliplyDensityMap(noi, ddice[i].coords, ddice[i].coordt);
		if(noi > kbald) {
			isoverbald[i] = 1;
			realncurve++;
		}
		else isoverbald[i] = 0;
	}
	
// set nvertices
	npoints = 0;
	int nwidths = 0;
	if(nvertices) delete[] nvertices;
	nvertices = new int[realncurve];
	unsigned acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		if(isoverbald[i]) {
			nvertices[acc] = pNSeg[i] + 5 + pNSeg[i];
			npoints += nvertices[acc];
			nwidths += nvertices[acc]-2;
			acc++;
		}
	}
	
// set widths
	if(widths) delete[] widths;
	widths = new float[nwidths];
	acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		if(isoverbald[i]) {
			widths[acc] = rootwidth * factorwidth;
			acc++;
			float dwidth = (tipwidth - rootwidth)/pNSeg[i] ;
			for(short j = 0; j<= pNSeg[i]; j++) 
			{
				//widths[acc] = rootwidth + dwidth*j;
				widths[acc] = (rootwidth + dwidth*j)*factorwidth;
				acc++;
				
				if(j != pNSeg[i]) {
					widths[acc] = widths[acc-1];
					acc++;
				}
			}
			widths[acc] = tipwidth * factorwidth;
			acc++;
		}
	}
/*	
// set opacities
	if(opacities) delete[] opacities;
	opacities = new XYZ[nwidths];
	acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		if(isoverbald[i]) {
			opacities[acc] = XYZ(1.f);
			acc++;
			float dos = 1.f/pNSeg[i];
			for(short j = 0; j<= pNSeg[i]; j++) 
			{
				opacities[acc] = XYZ(1.f - dos*j);
				acc++;
				
				if(j != pNSeg[i]) {
					opacities[acc] = opacities[acc-1];
					acc++;
				}
			}
			opacities[acc] = XYZ(0.f);
			acc++;
		}
	}
*/	
// set texcoord
	if(coord_s) delete[] coord_s;
	if(coord_t) delete[] coord_t;
	coord_s = new float[realncurve];
	coord_t = new float[realncurve];
	for(unsigned i=0; i<realncurve; i++) 
	{
		coord_s[i] = ddice[i].coords;
		coord_t[i] = ddice[i].coordt;
	}

// set colors
	if(rootColorArray) delete[] rootColorArray;
	if(tipColorArray) delete[] tipColorArray;
	rootColorArray = new XYZ[realncurve];
	tipColorArray = new XYZ[realncurve];
	
	float keepx;
	
	for(unsigned i=0; i<realncurve; i++) 
	{
		noi  = fnoi.randfint( g_seed )*mutant_scale; g_seed++;
		rootColorArray[i] = root_color + (mutant_color - root_color)*noi;
		tipColorArray[i] = tip_color + (mutant_color - tip_color)*noi;
	}
	
// set vertices
	if(vertices) delete[] vertices;
	vertices = new XYZ[npoints];
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++) pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
	
	MATRIX44F tspace, tspace1, tspace2;
	XYZ ppre, pcur, dv, ddv, pt[3], pw[3], dv0, dv1, dv2;
	acc=0;
	for(unsigned i=0; i<n_samp; i++)
	{
		if(isoverbald[i]) {
			ppre = pbuf[i];
			
			//pobj = ppre;
			pt[0] = pt[1] = pt[2] = ppre;
			//guide_spaceinv[bind_data[i].idx[0]].transform(pobj);
			
			guide_spaceinv[bind_data[i].idx[0]].transform(pt[0]);
			guide_spaceinv[bind_data[i].idx[1]].transform(pt[1]);
			guide_spaceinv[bind_data[i].idx[2]].transform(pt[2]);
			

			vertices[acc] = ppre;
			acc++;
			vertices[acc] = ppre;
			acc++;

			int num_seg = pNSeg[i];
			float dparam = 1.f/(float)num_seg;
			float param;
			if(bind_data[i].wei[0] > .8f) {
				for(int j = 0; j< num_seg; j++) {
					param = dparam*j;
					
					guide_data[bind_data[i].idx[0]].getDvAtParam(dv, param, num_seg);
					guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
					
					vertices[acc] = ppre;
					acc++;
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					
					keepx = pt[0].x;
					pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
					pw[0] *= noi;
					pw[0].x = keepx;
					tspace.transform(pw[0]);
					
					pcur = pw[0];
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
					dv *= noi;
					pcur += dv;
					
					ddv = pcur - ppre;
					ddv.normalize();
					ddv *= dv.length();
					
					vertices[acc] = ppre + ddv*0.5f;
					acc++;

					ppre += ddv;
				}
			}
			else {
				for(int j = 0; j< num_seg; j++) {
					param = dparam*j;
					
					guide_data[bind_data[i].idx[0]].getDvAtParam(dv0, param, num_seg);
					guide_data[bind_data[i].idx[1]].getDvAtParam(dv1, param, num_seg);
					guide_data[bind_data[i].idx[2]].getDvAtParam(dv2, param, num_seg);
						
					guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
					guide_data[bind_data[i].idx[1]].getSpaceAtParam(tspace1, param);
					guide_data[bind_data[i].idx[2]].getSpaceAtParam(tspace2, param);
					
					vertices[acc] = ppre;
					acc++;
					
					//dv = guide_data[bind_data[i].idx[0]].dispv[j]*bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].dispv[j]*bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].dispv[j]*bind_data[i].wei[2];
					dv = dv0 * bind_data[i].wei[0] + dv1 * bind_data[i].wei[1] + dv2 * bind_data[i].wei[2];
					dv.setLength(dv0.length());
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;

					keepx = pt[0].x;
					pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
					pw[0] *= noi;
					pw[0].x = keepx;
					//guide_data[bind_data[i].idx[0]].space[j].transform(pw[0]);
					tspace.transform(pw[0]);
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					keepx = pt[1].x;
					pw[1] = pt[1]*(1 - clumping*(j+1)/num_seg);
					pw[1] *= noi;
					pw[1].x = keepx;
					//guide_data[bind_data[i].idx[1]].space[j].transform(pw[1]);
					tspace1.transform(pw[1]);
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					keepx = pt[2].x;
					pw[2] = pt[2]*(1 - clumping*(j+1)/num_seg);
					pw[2] *= noi;
					pw[2].x = keepx;
					//guide_data[bind_data[i].idx[2]].space[j].transform(pw[2]);
					tspace2.transform(pw[2]);
					
					pcur = pw[0]*bind_data[i].wei[0] + pw[1]*bind_data[i].wei[1] + pw[2]*bind_data[i].wei[2];
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
					dv *= noi;
					pcur += dv;
					
					ddv = pcur - ppre;
					ddv.normalize();
					ddv *= dv.length();
					
					vertices[acc] = ppre + ddv*0.5f;
					acc++;

					ppre += ddv;
				}
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
	delete[] isoverbald;
}

void HairCache::createSimple()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray || !pNSeg) return;
	
	int g_seed = 13;
	FNoise fnoi;
	
	float noi;
	realncurve = 0;
	char* isoverbald = new char[n_samp];
	for(unsigned i=0; i<n_samp; i++) {
		noi = fnoi.randfint( g_seed ); g_seed++;
		if(pDensmap) muliplyDensityMap(noi, ddice[i].coords, ddice[i].coordt);
		if(noi > kbald) {
			isoverbald[i] = 1;
			realncurve++;
		}
		else isoverbald[i] = 0;
	}
	
// set nvertices
	npoints = 0;
	int nwidths = 0;
	if(nvertices) delete[] nvertices;
	nvertices = new int[realncurve];
	unsigned acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		if(isoverbald[i]) {
			nvertices[acc] = pNSeg[i] + 5;
			npoints += nvertices[acc];
			nwidths += nvertices[acc]-2;
			acc++;
		}
	}
	
// set widths
	if(widths) delete[] widths;
	widths = new float[nwidths];
	acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		if(isoverbald[i]) {
			widths[acc] = rootwidth * factorwidth; 
			acc++;
			float dwidth = (tipwidth - rootwidth)/pNSeg[i] ;
			for(short j = 0; j<= pNSeg[i]; j++) 
			{
				widths[acc] = rootwidth + dwidth*j;
				widths[acc] *= factorwidth;
				acc++;
			}
			widths[acc] = tipwidth * factorwidth;
			acc++;
		}
	}
/*	
// set opacities
	if(opacities) delete[] opacities;
	opacities = new XYZ[nwidths];
	acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		if(isoverbald[i]) {
			opacities[acc] = XYZ(1.f);
			acc++;
			float dos = 1.f/pNSeg[i];
			for(short j = 0; j<= pNSeg[i]; j++) 
			{
				opacities[acc] = XYZ(1.f - dos*j);
				acc++;
			}
			opacities[acc] = XYZ(0.f);
			acc++;
		}
	}
	
// set texcoord
	if(coord_s) delete[] coord_s;
	if(coord_t) delete[] coord_t;
	coord_s = new float[realncurve];
	coord_t = new float[realncurve];
	for(unsigned i=0; i<realncurve; i++) 
	{
		coord_s[i] = ddice[i].coords;
		coord_t[i] = ddice[i].coordt;
	}

// set colors
	if(rootColorArray) delete[] rootColorArray;
	if(tipColorArray) delete[] tipColorArray;
	rootColorArray = new XYZ[realncurve];
	tipColorArray = new XYZ[realncurve];
*/	
	float keepx;
	
	for(unsigned i=0; i<realncurve; i++) 
	{
		noi  = fnoi.randfint( g_seed )*mutant_scale; g_seed++;
		//rootColorArray[i] = root_color + (mutant_color - root_color)*noi;
		//tipColorArray[i] = tip_color + (mutant_color - tip_color)*noi;
	}
	
// set vertices
	if(vertices) delete[] vertices;
	vertices = new XYZ[npoints];
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++) pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
	
	MATRIX44F tspace, tspace1, tspace2;
	XYZ ppre, pcur, dv, ddv, pt[3], pw[3], dv0, dv1, dv2;
	acc=0;
	for(unsigned i=0; i<n_samp; i++)
	{
		if(isoverbald[i]) {
			ppre = pbuf[i];
			
			//pobj = ppre;
			pt[0] = pt[1] = pt[2] = ppre;
			//guide_spaceinv[bind_data[i].idx[0]].transform(pobj);
			
			guide_spaceinv[bind_data[i].idx[0]].transform(pt[0]);
			guide_spaceinv[bind_data[i].idx[1]].transform(pt[1]);
			guide_spaceinv[bind_data[i].idx[2]].transform(pt[2]);
			

			vertices[acc] = ppre;
			acc++;
			vertices[acc] = ppre;
			acc++;

			int num_seg = pNSeg[i];
			float dparam = 1.f/(float)num_seg;
			float param;
			if(bind_data[i].wei[0] > .8f) {
				for(int j = 0; j< num_seg; j++) {
					param = dparam*j;
					
					guide_data[bind_data[i].idx[0]].getDvAtParam(dv, param, num_seg);
					guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
					
					vertices[acc] = ppre;
					acc++;
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					
					keepx = pt[0].x;
					pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
					pw[0] *= noi;
					pw[0].x = keepx;
					tspace.transform(pw[0]);
					
					pcur = pw[0];
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
					dv *= noi;
					pcur += dv;
					
					ddv = pcur - ppre;
					ddv.normalize();
					ddv *= dv.length();

					ppre += ddv;
				}
			}
			else {
				for(int j = 0; j< num_seg; j++) {
					param = dparam*j;
					
					guide_data[bind_data[i].idx[0]].getDvAtParam(dv0, param, num_seg);
					guide_data[bind_data[i].idx[1]].getDvAtParam(dv1, param, num_seg);
					guide_data[bind_data[i].idx[2]].getDvAtParam(dv2, param, num_seg);
						
					guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
					guide_data[bind_data[i].idx[1]].getSpaceAtParam(tspace1, param);
					guide_data[bind_data[i].idx[2]].getSpaceAtParam(tspace2, param);
					
					vertices[acc] = ppre;
					acc++;
					
					//dv = guide_data[bind_data[i].idx[0]].dispv[j]*bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].dispv[j]*bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].dispv[j]*bind_data[i].wei[2];
					dv = dv0 * bind_data[i].wei[0] + dv1 * bind_data[i].wei[1] + dv2 * bind_data[i].wei[2];
					dv.setLength(dv0.length());
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;

					keepx = pt[0].x;
					pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
					pw[0] *= noi;
					pw[0].x = keepx;
					//guide_data[bind_data[i].idx[0]].space[j].transform(pw[0]);
					tspace.transform(pw[0]);
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					keepx = pt[1].x;
					pw[1] = pt[1]*(1 - clumping*(j+1)/num_seg);
					pw[1] *= noi;
					pw[1].x = keepx;
					//guide_data[bind_data[i].idx[1]].space[j].transform(pw[1]);
					tspace1.transform(pw[1]);
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					keepx = pt[2].x;
					pw[2] = pt[2]*(1 - clumping*(j+1)/num_seg);
					pw[2] *= noi;
					pw[2].x = keepx;
					//guide_data[bind_data[i].idx[2]].space[j].transform(pw[2]);
					tspace2.transform(pw[2]);
					
					pcur = pw[0]*bind_data[i].wei[0] + pw[1]*bind_data[i].wei[1] + pw[2]*bind_data[i].wei[2];
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
					dv *= noi;
					pcur += dv;
					
					ddv = pcur - ppre;
					ddv.normalize();
					ddv *= dv.length();

					ppre += ddv;
				}
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
	delete[] isoverbald;
}

void HairCache::setDensityMap(const char* filename)
{
	densmap_w =-1, densmap_h = -1;
	if(pDensmap) {
		delete[] pDensmap;
		pDensmap = 0;
		m_densityname = "nil";
	}
	try 
	{
		ZFnEXR::checkFile(filename, &densmap_w, &densmap_h);
	}
	catch (const std::exception &exc) 
    { 
		cout<<exc.what()<<endl; 
	}
	if(densmap_w > 16 && densmap_h > 16) {
		pDensmap = new float[densmap_w*densmap_h];
		ZFnEXR::readR(filename, densmap_w, densmap_h, pDensmap);
		m_densityname = filename;
	}
}

void HairCache::muliplyDensityMap(float& val, float& s, float& t) const
{
	int it = (densmap_h-1)*(1.f-t);
	int is = (densmap_w-1)*s;
	val *= pDensmap[it*densmap_w + is];
}

void HairCache::dump() const
{
	std::string dumpname = m_cachename;
	zGlobal::cutByFirstDot(dumpname);
	dumpname += ".hairdump";
	ofstream outfile;
	outfile.open(dumpname.c_str(), ios_base::out | ios_base::binary);
	if(!outfile.is_open()) 
	{
		cout<<"Cannot open file: "<<dumpname<<endl;
		return;
	}
	
	outfile.write((char*)&ndice,sizeof(float));
	outfile.write((char*)&n_samp,sizeof(int));
	outfile.write((char*)bind_data,sizeof(triangle_bind_info)*n_samp);
	outfile.write((char*)pNSeg,sizeof(unsigned)*n_samp);
	outfile.write((char*)ddice,sizeof(DiceParam)*n_samp);
	
	outfile.close();
	return;
}

char HairCache::lazi()
{
	std::string dumpname = m_cachename;
	zGlobal::cutByFirstDot(dumpname);
	dumpname += ".hairdump";
	ifstream infile;
	infile.open(dumpname.c_str(), ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		cout<<"Cannot open file: "<<dumpname<<endl;
		return 0;
	}
	float todice;
	infile.read((char*)&todice,sizeof(float));
	if(todice != ndice) {
		infile.close();
		return 0;
	}
	infile.read((char*)&n_samp,sizeof(int));
	
	if(bind_data) delete[] bind_data;
	bind_data = new triangle_bind_info[n_samp];
	
	if(pNSeg) delete[] pNSeg;
	pNSeg = new unsigned[n_samp];
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[n_samp];
	
	infile.read((char*)bind_data,sizeof(triangle_bind_info)*n_samp);
	infile.read((char*)pNSeg,sizeof(unsigned)*n_samp);
	infile.read((char*)ddice,sizeof(DiceParam)*n_samp);
	
	infile.close();
	return 1;
}

void HairCache::lookupTriangle(unsigned idx, XYZ* points) const
{
	points[0] = parray[pconnection[idx*3]];
	points[1] = parray[pconnection[idx*3+1]];
	points[2] = parray[pconnection[idx*3+2]];
}

void HairCache::lookupTriangleBind(unsigned idx, XYZ* points) const
{
	points[0] = pBind[pconnection[idx*3]];
	points[1] = pBind[pconnection[idx*3+1]];
	points[2] = pBind[pconnection[idx*3+2]];
}

void HairCache::lookupTriangleUV(unsigned idx, float* u, float* v) const
{
	u[0] = uarray[pconnection[idx*3]];
	u[1] = uarray[pconnection[idx*3+1]];
	u[2] = uarray[pconnection[idx*3+2]];
	v[0] = varray[pconnection[idx*3]];
	v[1] = varray[pconnection[idx*3+1]];
	v[2] = varray[pconnection[idx*3+2]];
}

void HairCache::lookupGuiderNSeg(unsigned idx, unsigned* nsegs) const
{
	nsegs[0] = pNSeg[idx*3];
	nsegs[1] = pNSeg[idx*3+1];
	nsegs[2] = pNSeg[idx*3+2];
}

void HairCache::lookupGuiderCVs(unsigned idx, XYZ* cvs_a, XYZ* cvs_b, XYZ* cvs_c) const
{
	for(unsigned i=0; i<3; i++) {
		unsigned isamp = idx*3 + i;
		
		if(i==0) cvs_a[0] = parray[pconnection[isamp]];
		else if(i==1) cvs_b[0] = parray[pconnection[isamp]];
		else cvs_c[0] = parray[pconnection[isamp]];
		
		XYZ pt[3];
		MATRIX44F tspace[3];
		XYZ pcur, dv[3], pw[3];
		if(i==0) pt[0] = pt[1] = pt[2] = cvs_a[0];
		else if(i==1) pt[0] = pt[1] = pt[2] = cvs_b[0];
		else pt[0] = pt[1] = pt[2] = cvs_c[0];
		
		guide_spaceinv[bind_data[isamp].idx[0]].transform(pt[0]);
		guide_spaceinv[bind_data[isamp].idx[1]].transform(pt[1]);
		guide_spaceinv[bind_data[isamp].idx[2]].transform(pt[2]);
		
		int num_seg = pNSeg[isamp];
		float dparam = 1.f/(float)num_seg;
		float param;
		//if(bind_data[isamp].wei[0] > .91f) {
		for(int j = 0; j< num_seg; j++) {
			param = dparam*j;
			
			guide_data[bind_data[isamp].idx[0]].getDvAtParam(dv[0], param, num_seg);
			guide_data[bind_data[isamp].idx[0]].getSpaceAtParam(tspace[0], param);
			
			pw[0] = pt[0];
			//noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
			
			//keepx = pt[0].x;
			//pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
			//pw[0] *= noi;
			//pw[0].x = keepx;
			tspace[0].transform(pw[0]);
			
			pcur = pw[0];
			
			//noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
			//dv *= noi;
			pcur += dv[0];
			
			//ddv = pcur - ppre;
			//ddv.normalize();
			//ddv *= dv.length();
			
			//vertices[acc] = ppre + ddv*0.5f;
			//acc++;
			if(i==0) cvs_a[j+1] = pcur;
			else if(i==1) cvs_b[j+1] = pcur;
			else cvs_c[j+1] = pcur;
		}
	}
//}
	
	/*ppre = pbuf[i];
			
			//pobj = ppre;
			pt[0] = pt[1] = pt[2] = ppre;
			//guide_spaceinv[bind_data[i].idx[0]].transform(pobj);
			
			guide_spaceinv[bind_data[i].idx[0]].transform(pt[0]);
			guide_spaceinv[bind_data[i].idx[1]].transform(pt[1]);
			guide_spaceinv[bind_data[i].idx[2]].transform(pt[2]);
			

			vertices[acc] = ppre;
			acc++;
			vertices[acc] = ppre;
			acc++;

			int num_seg = pNSeg[i];
			float dparam = 1.f/(float)num_seg;
			float param;
			if(bind_data[i].wei[0] > .8f) {
				for(int j = 0; j< num_seg; j++) {
					param = dparam*j;
					
					guide_data[bind_data[i].idx[0]].getDvAtParam(dv, param, num_seg);
					guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
					
					vertices[acc] = ppre;
					acc++;
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					
					keepx = pt[0].x;
					pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
					pw[0] *= noi;
					pw[0].x = keepx;
					tspace.transform(pw[0]);
					
					pcur = pw[0];
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
					dv *= noi;
					pcur += dv;
					
					ddv = pcur - ppre;
					ddv.normalize();
					ddv *= dv.length();
					
					vertices[acc] = ppre + ddv*0.5f;
					acc++;

					ppre += ddv;
				}
			}
			else {
				for(int j = 0; j< num_seg; j++) {
					param = dparam*j;
					
					guide_data[bind_data[i].idx[0]].getDvAtParam(dv0, param, num_seg);
					guide_data[bind_data[i].idx[1]].getDvAtParam(dv1, param, num_seg);
					guide_data[bind_data[i].idx[2]].getDvAtParam(dv2, param, num_seg);
						
					guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
					guide_data[bind_data[i].idx[1]].getSpaceAtParam(tspace1, param);
					guide_data[bind_data[i].idx[2]].getSpaceAtParam(tspace2, param);
					
					vertices[acc] = ppre;
					acc++;
					
					//dv = guide_data[bind_data[i].idx[0]].dispv[j]*bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].dispv[j]*bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].dispv[j]*bind_data[i].wei[2];
					dv = dv0 * bind_data[i].wei[0] + dv1 * bind_data[i].wei[1] + dv2 * bind_data[i].wei[2];
					dv.setLength(dv0.length());
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;

					keepx = pt[0].x;
					pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
					pw[0] *= noi;
					pw[0].x = keepx;
					//guide_data[bind_data[i].idx[0]].space[j].transform(pw[0]);
					tspace.transform(pw[0]);
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					keepx = pt[1].x;
					pw[1] = pt[1]*(1 - clumping*(j+1)/num_seg);
					pw[1] *= noi;
					pw[1].x = keepx;
					//guide_data[bind_data[i].idx[1]].space[j].transform(pw[1]);
					tspace1.transform(pw[1]);
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
					keepx = pt[2].x;
					pw[2] = pt[2]*(1 - clumping*(j+1)/num_seg);
					pw[2] *= noi;
					pw[2].x = keepx;
					//guide_data[bind_data[i].idx[2]].space[j].transform(pw[2]);
					tspace2.transform(pw[2]);
					
					pcur = pw[0]*bind_data[i].wei[0] + pw[1]*bind_data[i].wei[1] + pw[2]*bind_data[i].wei[2];
					
					noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
					dv *= noi;
					pcur += dv;
					
					ddv = pcur - ppre;
					ddv.normalize();
					ddv *= dv.length();
					
					vertices[acc] = ppre + ddv*0.5f;
					acc++;

					ppre += ddv;
				}
			}
			vertices[acc] = ppre;
			acc++;
			vertices[acc] = ppre;
			acc++;
			vertices[acc] = ppre;
			acc++;
	*/
}
//~: