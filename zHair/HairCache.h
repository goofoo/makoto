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
	
	int dice();
	void bind();
	
	int load(const char* filename);
	int loadStart(const char* filename);
	
	void create();
	
	void setInterpolate(const int val) {isInterpolate = val;}
	void setDiceNumber(const float val) {ndice = val;} 
	void setClumping(const float val) {clumping = val;}
	void setFuzz(const float val) {fuzz = val;}
	void setKink(const float val) {kink = val;}
	void setWidth(const float v0, const float v1) {rootwidth = v0; tipwidth = v1;}
	void setRootColor(const float r, const float g, const float b) {root_color.x = r; root_color.y = g; root_color.z = b;}
	void setTipColor(const float r, const float g, const float b) {tip_color.x = r; tip_color.y = g; tip_color.z = b;}
	void setMutantColor(const float r, const float g, const float b) {mutant_color.x = r; mutant_color.y = g; mutant_color.z = b;}
	void setMutantColorScale(const float v) {mutant_scale = v;}
	void setBald(const float v) {kbald = v;}
	
	const char* getCacheName() const {return m_cachename.c_str();}
	float getClumping() const {return clumping;}
	float getFuzz() const {return fuzz;}
	float getKink() const {return kink;}
	
	int getNumCurves() const {return realncurve;}
	const int* getNumVertices() const {return nvertices;}
	const XYZ* points() const {return vertices;}
	const float* getWidth() const {return widths;}
	const float* getS() const {return coord_s;}
	const float* getT() const {return coord_t;}
	const XYZ* getRootColors() const {return rootColorArray;}
	const XYZ* getTipColors() const {return tipColorArray;}
	const XYZ* getOs() const {return opacities;}
	int getNumPoints() const {return npoints;}
	
	void setDensityMap(const char* filename);
	void muliplyDensityMap(float& val, float& s, float& t) const;
	
private:
	float ndice, rootwidth, tipwidth;
	DiceParam* ddice;
	int n_samp, npoints;
	float sum_area;
	int* pconnection;
	float* uarray;
	float* varray;
	XYZ* parray;

	Dguide* guide_data;
	unsigned* pNSeg;
	unsigned num_guide, n_vert, n_tri;

	triangle_bind_info* bind_data;
	float clumping, fuzz, kink, mutant_scale, kbald;
	MATRIX44F* guide_spaceinv;
	std::string m_cachename, m_densityname;
	int* nvertices;
	XYZ* vertices;
	float* widths;
	float *coord_s, *coord_t;
	XYZ tip_color, root_color, mutant_color;
	XYZ *rootColorArray, *tipColorArray, *opacities;
	int isInterpolate;
	int realncurve;
	int densmap_w, densmap_h;
	float* pDensmap;
};
#endif
