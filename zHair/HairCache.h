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
#include <string>

struct Dguide
{
	short num_seg;
	XYZ dsp_col;
	XYZ* P;
	XYZ* N;
	XYZ* T;
	XYZ* dispv;
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
	
	void setDiceNumber(const float val) {ndice = val;} 
	void setTwist(const float val) {twist = val;}
	void setClumping(const float val) {clumping = val;}
	void setFuzz(const float val) {fuzz = val;}
	void setKink(const float val) {kink = val;}
	void setWidth(const float v0, const float v1) {rootwidth = v0; tipwidth = v1;}
	
	const char* getCacheName() const {return m_cachename.c_str();}
	float getTwist() const {return twist;}
	float getClumping() const {return clumping;}
	float getFuzz() const {return fuzz;}
	float getKink() const {return kink;}
	
	int getNumCurves() const {return n_samp;}
	const int* getNumVertices() const {return nvertices;}
	const XYZ* points() const {return vertices;}
	const float* getWidth() const {return widths;}
	const float* getS() const {return coord_s;}
	const float* getT() const {return coord_t;}
	
private:
	float ndice, rootwidth, tipwidth;
	DiceParam* ddice;
	int n_samp;
	float sum_area;
	int* pconnection;
	float* uarray;
	float* varray;
	XYZ* parray;

	Dguide* guide_data;
	unsigned num_guide, n_vert, n_tri;

	unsigned* bind_data;
	float twist, clumping, fuzz, kink;
	MATRIX44F* guide_spaceinv;
	std::string m_cachename;
	int* nvertices;
	XYZ* vertices;
	float* widths;
	float *coord_s, *coord_t;
};
#endif
