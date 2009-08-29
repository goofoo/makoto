/*
 *  HairCache.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HairCache_H
#define _HairCache_H
#include "../shared/DiceTriangle.h"
#include "BindTriangle.h"
#include <string>

struct ddensmap;

struct Dguide
{
	short num_seg;
	XYZ dsp_col;
	XYZ* P;
	XYZ* N;
	XYZ* T;
	XYZ* dispv;
	MATRIX44F* space;
	float u, v, radius;
	
	void getDvAtParam(XYZ& res, float param, int mapmax)
	{
		float fparam = param*num_seg;
		int iparam = (int)fparam;
		int iparam1 = iparam+1;
		if(iparam1 > num_seg - 1) iparam1 = num_seg - 1;
		res = dispv[iparam] + (dispv[iparam1] - dispv[iparam]) * (fparam-iparam);
		res *= float(num_seg)/mapmax;
	}
	
	void getSpaceAtParam(MATRIX44F& res, float param)
	{
		float fparam = param*num_seg;
		int iparam = (int)fparam;
		int iparam1 = iparam+1;
		if(iparam1 > num_seg - 1) iparam1 = num_seg - 1;
		res = space[iparam];
		XYZ v0, v1;
		space[iparam].getTranslation(v0);
		space[iparam1].getTranslation(v1);
		v0 = v0 + (v1 - v0) * (fparam-iparam);
		res.setTranslation(v0);
	}
	
	void release() 
	{
		delete[] P;
		delete[] N;
		delete[] T;
		delete[] dispv;
		delete[] space;
	}
};

class HairCache
{
public:
	HairCache();
	~HairCache();
	
	int pushFaceVertice();
	void bind();
	
	int load(const char* filename);
	int loadStart(const char* filename);
	int loadNext();
	
	const char* getCacheName() const {return m_cachename.c_str();}
	
	char setDensityMap(const char* filename);
	void muliplyDensityMap(float& val, float& s, float& t) const;
	
	void dump() const;
	char lazi();
	
	unsigned getNumTriangle() const {return n_tri;}
	void lookupTriangle(unsigned& idx, XYZ* points) const;
	void lookupTriangleBind(unsigned& idx, XYZ* points) const;
	void lookupTriangleUV(unsigned& idx, float* u, float* v) const;
	void lookupGuiderNSeg(unsigned& idx, unsigned* nsegs) const;
	void lookupGuiderCVs(unsigned& idx, float& k, XYZ* cvs_a, XYZ* cvs_b, XYZ* cvs_c) const;
	void lookupVelocity(unsigned& idx, float& fract, XYZ* velocities) const;
	void lookupDensity(unsigned& idx, float* densities) const;
	float getEpsilon(float& ndice) const;
	void lookupMeanDisplace(unsigned& idx, float& fract, XYZ& disp) const;
	void lookupMeanPos(unsigned& idx, float& fract, XYZ& p0, XYZ& p1) const;
	void setDensity(ddensmap& map) const;
	
private:
	DiceParam* ddice;
	int n_samp;
	float sum_area;
	int* pconnection;
	float* uarray;
	float* varray;
	XYZ* parray;
	XYZ* pBind;
	XYZ* pframe1;

	Dguide* guide_data;
	unsigned* pNSeg;
	unsigned num_guide, n_vert, n_tri;

	triangle_bind_info* bind_data;
	MATRIX44F* guide_spaceinv;
	std::string m_cachename, m_densityname;
	int densmap_w, densmap_h;
	float* pDensmap;
};
#endif
