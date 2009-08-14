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
parray(0),pframe1(0),pconnection(0),uarray(0),varray(0),pBind(0),
sum_area(0.f),
pNSeg(0),pDensmap(0)
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
	if(pBind) delete[] pBind;
	if(pNSeg) delete[] pNSeg;
	if(pDensmap) delete[] pDensmap;
	if(pframe1) delete[] pframe1;
}

float HairCache::getEpsilon(float& ndice) const
{
	return sqrt(sum_area/n_tri/2/(ndice+2));
}

int HairCache::pushFaceVertice()
{
	if(!pconnection) return 0;
	
	n_samp = n_tri*3;
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[n_samp];

	int a, b, c;
	
	for(unsigned i=0; i<n_tri; i++) {
		a = pconnection[i*3];
		b = pconnection[i*3+1];
		c = pconnection[i*3+2];
		
		ddice[i*3].alpha = 1.f;
		ddice[i*3].beta = ddice[i*3].gamma = 0.f;
		ddice[i*3].id0 = a;
		ddice[i*3].id1 = b;
		ddice[i*3].id2 = c;
		ddice[i*3].coords = uarray[i*3];
		ddice[i*3].coordt = varray[i*3];
		
		ddice[i*3+1].alpha = 1.f;
		ddice[i*3+1].beta = ddice[i*3+1].gamma = 0.f;
		ddice[i*3+1].id0 = b;
		ddice[i*3+1].id1 = c;
		ddice[i*3+1].id2 = a;
		ddice[i*3+1].coords = uarray[i*3+1];
		ddice[i*3+1].coordt = varray[i*3+1];
		
		ddice[i*3+2].alpha = 1.f;
		ddice[i*3+2].beta = ddice[i*3+2].gamma = 0.f;
		ddice[i*3+2].id0 = c;
		ddice[i*3+2].id1 = a;
		ddice[i*3+2].id2 = b;
		ddice[i*3+2].coords = uarray[i*3+2];
		ddice[i*3+2].coordt = varray[i*3+2];
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
// restrict st between 0 - 1
		if(ddice[i].coords < 0) ddice[i].coords = 0;
		if(ddice[i].coords > 1) ddice[i].coords = 1;
		if(ddice[i].coordt < 0) ddice[i].coordt = 0;
		if(ddice[i].coordt > 1) ddice[i].coordt = 1;
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
		
		bind_data[i].idx[0] = bind_data[i].idx[1] = bind_data[i].idx[2] = idx[validid].idx;

		if(validid < num_guide-6) {
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
	//infile.read((char*)&sum_area,sizeof(float));
	//infile.read((char*)&n_tri,sizeof(unsigned));
	
	//if(pconnection) delete[] pconnection;
	//pconnection = new int[n_tri*3];
	//infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	if(parray) delete[] parray;
	parray = new XYZ[n_vert];
	infile.read((char*)parray, sizeof(XYZ)*n_vert);
	
	infile.close();
	
	if(pframe1) delete[] pframe1;
	pframe1 = new XYZ[n_vert];
	
	for(unsigned i=0; i<n_vert; i++) pframe1[i] = parray[i];
	
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

int HairCache::loadNext()
{
	std::string filename = m_cachename;
	int frm = zGlobal::getFrameNumber(filename);
	frm++;
	zGlobal::setFrameNumberAndExtension(filename, frm, "hair");
	
	ifstream infile;
	infile.open(filename.c_str(), ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		cout<<"Cannot open file: "<<filename<<endl;
		return 0;
	}
	
	int offs = sizeof(unsigned);
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		offs += sizeof(short);
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(float)*3;
	}
	
	offs += sizeof(unsigned);
	
	infile.seekg(offs, ios::beg);
	if(pframe1) delete[] pframe1;
	pframe1 = new XYZ[n_vert];
	infile.read((char*)pframe1, sizeof(XYZ)*n_vert);
	
	infile.close();
	
	return 1;
}

char HairCache::setDensityMap(const char* filename)
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
		return 1;
	}
	return 0;
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
	
	//outfile.write((char*)&ndice,sizeof(float));
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
	//if(todice != ndice) {
	//	infile.close();
	//	return 0;
	//}
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

void HairCache::lookupTriangle(unsigned& idx, XYZ* points) const
{
	points[0] = parray[pconnection[idx*3]];
	points[1] = parray[pconnection[idx*3+1]];
	points[2] = parray[pconnection[idx*3+2]];
}

void HairCache::lookupTriangleBind(unsigned& idx, XYZ* points) const
{
	points[0] = pBind[pconnection[idx*3]];
	points[1] = pBind[pconnection[idx*3+1]];
	points[2] = pBind[pconnection[idx*3+2]];
}

void HairCache::lookupTriangleUV(unsigned& idx, float* u, float* v) const
{
	u[0] = uarray[pconnection[idx*3]];
	u[1] = uarray[pconnection[idx*3+1]];
	u[2] = uarray[pconnection[idx*3+2]];
	v[0] = varray[pconnection[idx*3]];
	v[1] = varray[pconnection[idx*3+1]];
	v[2] = varray[pconnection[idx*3+2]];
}

void HairCache::lookupGuiderNSeg(unsigned& idx, unsigned* nsegs) const
{
	nsegs[0] = pNSeg[idx*3];
	nsegs[1] = pNSeg[idx*3+1];
	nsegs[2] = pNSeg[idx*3+2];
}

void HairCache::lookupGuiderCVs(unsigned& idx, float& k, XYZ* cvs_a, XYZ* cvs_b, XYZ* cvs_c) const
{
	int g_seed = idx*91;
	FNoise fnoi;
	float noi, keepx;
	
	for(unsigned i=0; i<3; i++) {
		unsigned isamp = idx*3 + i;
		
		if(i==0) cvs_a[0] = parray[ddice[isamp].id0];
		else if(i==1) cvs_b[0] = parray[ddice[isamp].id0];
		else cvs_c[0] = parray[ddice[isamp].id0];
		
		XYZ pt[3];
		MATRIX44F tspace[3];
		XYZ ppre, pcur, dv[3], pw[3], dmean, ddv;
		if(i==0) ppre = pt[0] = pt[1] = pt[2] = cvs_a[0];
		else if(i==1) ppre = pt[0] = pt[1] = pt[2] = cvs_b[0];
		else ppre = pt[0] = pt[1] = pt[2] = cvs_c[0];
		
		guide_spaceinv[bind_data[isamp].idx[0]].transform(pt[0]);
		guide_spaceinv[bind_data[isamp].idx[1]].transform(pt[1]);
		guide_spaceinv[bind_data[isamp].idx[2]].transform(pt[2]);
		
		int num_seg = pNSeg[isamp];
		float dparam = 1.f/(float)num_seg;
		float param;
		if(bind_data[isamp].wei[0] > .91f) {
			for(int j = 0; j< num_seg; j++) {
				param = dparam*j;
				
				guide_data[bind_data[isamp].idx[0]].getDvAtParam(dv[0], param, num_seg);
				guide_data[bind_data[isamp].idx[0]].getSpaceAtParam(tspace[0], param);
				
				pw[0] = pt[0];
				
				keepx = pw[0].x;
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*k; g_seed++;
				pw[0] *= noi;
				pw[0].x = keepx;
				tspace[0].transform(pw[0]);
				
				pcur = pw[0];
				
				pcur += dv[0];
				
				ddv = pcur - ppre;
				ddv.normalize();
				ddv *= dv[0].length();
				
				ppre += ddv;
				
				if(i==0) cvs_a[j+1] = ppre;
				else if(i==1) cvs_b[j+1] = ppre;
				else cvs_c[j+1] = ppre;
			}
		}
		else {
			for(int j = 0; j< num_seg; j++) {
				param = dparam*j;
				
				guide_data[bind_data[isamp].idx[0]].getDvAtParam(dv[0], param, num_seg);
				guide_data[bind_data[isamp].idx[1]].getDvAtParam(dv[1], param, num_seg);
				guide_data[bind_data[isamp].idx[2]].getDvAtParam(dv[2], param, num_seg);
					
				guide_data[bind_data[isamp].idx[0]].getSpaceAtParam(tspace[0], param);
				guide_data[bind_data[isamp].idx[1]].getSpaceAtParam(tspace[1], param);
				guide_data[bind_data[isamp].idx[2]].getSpaceAtParam(tspace[2], param);
				
				dmean = dv[0] * bind_data[i].wei[0] + dv[1] * bind_data[i].wei[1] + dv[2] * bind_data[i].wei[2];
				dmean.setLength(dv[0].length());
				
				pw[0] = pt[0];
				
				keepx = pw[0].x;
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*k; g_seed++;
				pw[0] *= noi;
				pw[0].x = keepx;
				tspace[0].transform(pw[0]);
				
				pw[1] = pt[1];
				
				keepx = pw[1].x;
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*k; g_seed++;
				pw[1] *= noi;
				pw[1].x = keepx;
				tspace[1].transform(pw[1]);
				
				pw[2] = pt[2];
				
				keepx = pw[2].x;
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*k; g_seed++;
				pw[2] *= noi;
				pw[2].x = keepx;
				tspace[2].transform(pw[2]);
				
				pcur = pw[0]*bind_data[isamp].wei[0] + pw[1]*bind_data[isamp].wei[1] + pw[2]*bind_data[isamp].wei[2];
				
				pcur += dmean;
				
				ddv = pcur - ppre;
				ddv.normalize();
				ddv *= dmean.length();

				ppre += ddv;
				
				if(i==0) cvs_a[j+1] = ppre;
				else if(i==1) cvs_b[j+1] = ppre;
				else cvs_c[j+1] = ppre;
			}
		}
	}
}

void HairCache::lookupVelocity(unsigned& idx, float& fract, XYZ* velocities) const
{
	velocities[0] = pframe1[pconnection[idx*3]] - parray[pconnection[idx*3]];
	velocities[1] = pframe1[pconnection[idx*3+1]] - parray[pconnection[idx*3+1]];
	velocities[2] = pframe1[pconnection[idx*3+2]] - parray[pconnection[idx*3+2]];
	velocities[0] *= fract;
	velocities[1] *= fract;
	velocities[2] *= fract;
}

void HairCache::lookupDensity(unsigned& idx, float* densities) const
{
	if(!pDensmap) {
		densities[0] = densities[1] = densities[2] = 1.f;
		return;
	}
	
	float u = uarray[pconnection[idx*3]];
	float v = varray[pconnection[idx*3]];
	
	int it = (densmap_h-1)*(1.f-v);
	int is = (densmap_w-1)*u;
	densities[0] = pDensmap[it*densmap_w + is];
	
	u = uarray[pconnection[idx*3+1]];
	v = varray[pconnection[idx*3+1]];
	
	it = (densmap_h-1)*(1.f-v);
	is = (densmap_w-1)*u;
	densities[1] = pDensmap[it*densmap_w + is];
	
	u = uarray[pconnection[idx*3+2]];
	v = varray[pconnection[idx*3+2]];
	
	it = (densmap_h-1)*(1.f-v);
	is = (densmap_w-1)*u;
	densities[2] = pDensmap[it*densmap_w + is];
}
//~: