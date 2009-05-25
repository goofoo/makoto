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
#include <iostream>
#include <fstream>
using namespace std;

HairCache::HairCache():ddice(0),n_samp(0),guide_data(0),bind_data(0),guide_spaceinv(0),
parray(0),pconnection(0),uarray(0),varray(0),
sum_area(0.f),ndice(24),
nvertices(0),vertices(0)
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
}

int HairCache::dice()
{
	if(!pconnection || !parray) return 0;
		
	float epsilon = sqrt(sum_area/n_tri/2/ndice);

	int estimate_ncell = n_tri*ndice*2;
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
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++)
	{
		pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
	}

	if(bind_data) delete[] bind_data;
	bind_data = new unsigned[n_samp];
	XYZ togd;
	float dist;
	for(unsigned i=0; i<n_samp; i++)
	{
// find the nearest guide
		float min_dist = 10e6;
		for(unsigned j=0; j<num_guide; j++)
		{
			togd = pbuf[i] - guide_data[j].P[0];
			dist = togd.length();
			
			if(dist < min_dist)
			{
				bind_data[i] = j;
				min_dist = dist;
			}
		}
	}
	
	delete[] pbuf;
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

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
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
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		guide_spaceinv[i].setIdentity();
		XYZ binor = guide_data[i].N[0].cross(guide_data[i].T[0]);
		guide_spaceinv[i].setOrientations(guide_data[i].T[0], binor, guide_data[i].N[0]);
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

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
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
		guide_spaceinv[i].setIdentity();
		XYZ binor = guide_data[i].N[0].cross(guide_data[i].T[0]);
		guide_spaceinv[i].setOrientations(guide_data[i].T[0], binor, guide_data[i].N[0]);
		guide_spaceinv[i].inverse();
	}
	return 1;
}

void HairCache::create()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray) return;
	
	int npoints = 0;
	if(nvertices) delete[] nvertices;
	nvertices = new int[n_samp];
	for(unsigned i=0; i<n_samp; i++) 
	{
		nvertices[i] = guide_data[bind_data[i]].num_seg;
		npoints += nvertices[i];
	}
	
	if(vertices) delete[] vertices;
	vertices = new XYZ[npoints];
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++) pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;

	int g_seed = 13;
	FNoise fnoi;
	
	float noi;
	
	XYZ ppre, dv, axisobj, axisworld, guiderotaxis;
	unsigned acc=0;
	for(unsigned i=0; i<n_samp; i++)
	{
		ppre = pbuf[i]; 
		axisworld = axisobj = pbuf[i] -  guide_data[bind_data[i]].P[0];
		guide_spaceinv[bind_data[i]].transformAsNormal(axisobj);
		axisobj.x = 0;
		for(short j = 0; j< guide_data[bind_data[i]].num_seg; j++) 
		{
			vertices[acc] = ppre;
			acc++;
			
			dv = guide_data[bind_data[i]].dispv[j];
			MATRIX44F mat;
			
			XYZ binor = guide_data[bind_data[i]].N[j].cross(guide_data[bind_data[i]].T[j]);
			mat.setOrientations(guide_data[bind_data[i]].T[j], binor, guide_data[bind_data[i]].N[j]);
			
			XYZ rt = axisobj;
			rt.rotateAroundAxis(XYZ(1,0,0), twist*j*dv.length()/axisobj.length());
			mat.transformAsNormal(rt);

			axisworld = rt;
			
			axisworld.normalize();

			XYZ rot2p = ppre + dv -  guide_data[bind_data[i]].P[j];
			noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
			dv.rotateAlong(rot2p, -clumping*noi);

			noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
			dv.rotateAroundAxis(axisworld, -twist*noi);
			
			noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
			ppre += dv*noi;
		}
	}
	
	delete[] pbuf;
}
//~: