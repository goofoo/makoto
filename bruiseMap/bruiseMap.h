/*
 *  bruiseMap.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _bruiseMap_H
#define _bruiseMap_H
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatArray.h>
#include "../shared/DiceTriangle.h"

class bruiseMap
{
public:
	bruiseMap();
	~bruiseMap();
	
	void setBase(const MObject& mesh);
	void setGuide(const MObject& mesh);
	char hasBase() const {return has_base;}
	char hasGuide() const {return has_guide;}
	
	int dice(float bias);
	//void trace(float bias);
	void draw();
	void init();
	void save(float bias);
private:
	char has_base, has_guide;
	MObject pbase, pguide;
	DiceParam* ddice;
	float* ddist;
	int n_samp, n_vert;
	MFloatArray hitArray;
	float* map_dist;
};
#endif