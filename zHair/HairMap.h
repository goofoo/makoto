/*
 *  hairMap.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _hairMap_H
#define _hairMap_H
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MObjectArray.h>
#include "../shared/DiceTriangle.h"

struct Dguide
{
	short num_seg;
	XYZ dsp_col;
	XYZ* P;
	XYZ* N;
	XYZ* T;
	XYZ* dispv;
};

class hairMap
{
public:
	hairMap();
	~hairMap();
	
	void setBase(const MObject& mesh);
	void setGuide(const MObjectArray& meshes);
	char hasBase() const {return has_base;}
	char hasGuide() const {return has_guide;}
	
	void updateBase();
	
	int dice();
	void bind();
	
	void draw();
	void initGuide();
	void updateGuide();
	void drawGuide();

	int saveDguide(const char* filename);
	int loadDguide(const char* filename);
	
	void setTwist(const float val) {twist = val;}
	void setClumping(const float val) {clumping = val;}
	void setNoise(const float val) {knoise = val;}
	
private:
	char has_base, has_guide;
	MObject obase;
	MObjectArray oguide;
	DiceParam* ddice;
	int n_samp;
	float sum_area;
	int* pconnection;
	XYZ* parray;

	Dguide* guide_data;
	unsigned num_guide, num_guideobj, n_vert, n_tri;

	unsigned* bind_data;
	float twist, clumping, knoise;
	MATRIX44F* guide_spaceinv;
};
#endif
