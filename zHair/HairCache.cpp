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
#include <iostream>
#include <fstream>
using namespace std;

HairCache::HairCache():ddice(0),n_samp(0),guide_data(0),bind_data(0),guide_spaceinv(0),
parray(0),pconnection(0),uarray(0),varray(0),
sum_area(0.f),ndice(24),
nvertices(0),vertices(0),widths(0),coord_s(0),coord_t(0),
rootColorArray(0), tipColorArray(0), opacities(0),pNSeg(0)
{
}
HairCache::~HairCache() 
{
	if(ddice) delete[] ddice;
	if(guide_data) delete[] guide_data;
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
	if(opacities) delete[] opacities;
	if(pNSeg) delete[] pNSeg;
}

int HairCache::dice()
{
	if(!pconnection || !parray) return 0;
		
	float epsilon = sqrt(sum_area/n_tri/2/(ndice+2));

	int estimate_ncell = n_tri*(ndice+2)*2;
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
		
		ftri.create(parray[a], parray[b], parray[c]);
		ftri.setId(a, b, c);
		ftri.setS(uarray[i*3], uarray[i*3+1], uarray[i*3+2]);
		ftri.setT(varray[i*3], varray[i*3+1], varray[i*3+2]);
		ftri.rasterize(epsilon, ddice, n_samp, seed);seed++;
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
		XYZ pto = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
		XYZ dto = pto - guide_data[idx[validid].idx].root;
		float dist2valid = dto.length();
		
		while(dist2valid > guide_data[idx[validid].idx].radius*2 && validid<num_guide-1) {
			validid++;
			dto = pto - guide_data[idx[validid].idx].root;
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
				
				pw[0] = guide_data[idx[validid].idx].root;
				pw[1] = guide_data[idx[validid+1+hdl].idx].root;
				pw[2] = guide_data[idx[validid+2+hdl].idx].root;
				
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
		
		pNSeg[i] = guide_data[bind_data[i].idx[0]].num_seg * bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].num_seg * bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].num_seg * bind_data[i].wei[2];
		
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
	
	if(guide_data) delete[] guide_data;
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
		infile.read((char*)&guide_data[i].root, sizeof(XYZ));
	}
	infile.read((char*)&sum_area,sizeof(float));
	infile.read((char*)&n_tri,sizeof(unsigned));
	
	if(pconnection) delete[] pconnection;
	pconnection = new int[n_tri*3];
	infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
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
	infile.read((char*)&num_guide,sizeof(unsigned));
	
	if(guide_data) delete[] guide_data;
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
		infile.read((char*)&guide_data[i].root, sizeof(XYZ));
	}
	infile.read((char*)&sum_area,sizeof(float));
	infile.read((char*)&n_tri,sizeof(unsigned));
	
	if(pconnection) delete[] pconnection;
	pconnection = new int[n_tri*3];
	infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	if(parray) delete[] parray;
	parray = new XYZ[n_vert];
	infile.read((char*)parray, sizeof(XYZ)*n_vert);
	
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
// set nvertices
	npoints = 0;
	int nwidths = 0;
	if(nvertices) delete[] nvertices;
	nvertices = new int[n_samp];
	for(unsigned i=0; i<n_samp; i++) 
	{
		nvertices[i] = pNSeg[i] + 5;
		npoints += nvertices[i];
		nwidths += nvertices[i]-2;
	}
// set widths
	if(widths) delete[] widths;
	widths = new float[nwidths];
	unsigned acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
		widths[acc] = rootwidth;
		acc++;
		float dwidth = (tipwidth - rootwidth)/pNSeg[i] ;
		for(short j = 0; j<= pNSeg[i]; j++) 
		{
			widths[acc] = rootwidth + dwidth*j;
			acc++;
		}
		widths[acc] = tipwidth;
		acc++;
	}
// set opacities
	if(opacities) delete[] opacities;
	opacities = new XYZ[nwidths];
	acc=0;
	for(unsigned i=0; i<n_samp; i++) 
	{
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
// set texcoord
	if(coord_s) delete[] coord_s;
	if(coord_t) delete[] coord_t;
	coord_s = new float[n_samp];
	coord_t = new float[n_samp];
	for(unsigned i=0; i<n_samp; i++) 
	{
		coord_s[i] = ddice[i].coords;
		coord_t[i] = ddice[i].coordt;
	}
// set colors
	if(rootColorArray) delete[] rootColorArray;
	if(tipColorArray) delete[] tipColorArray;
	rootColorArray = new XYZ[n_samp];
	tipColorArray = new XYZ[n_samp];
	
	int g_seed = 13;
	FNoise fnoi;
	
	float noi, keepx;
	
	for(unsigned i=0; i<n_samp; i++) 
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
	XYZ ppre, pcur, dv, ddv, pobj, pt[3], pw[3], dv0, dv1, dv2;
	acc=0;
	for(unsigned i=0; i<n_samp; i++)
	{
		ppre = pbuf[i];
		
		pobj = ppre;
		pt[0] = pt[1] = pt[2] = ppre;
		guide_spaceinv[bind_data[i].idx[0]].transform(pobj);
		
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
			//dv.setLength(dv0.length());
			
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
		vertices[acc] = ppre;
		acc++;
		vertices[acc] = ppre;
		acc++;
		vertices[acc] = ppre;
		acc++;
	}
	
	delete[] pbuf;
}
//~: