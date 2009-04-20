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
#include "../shared/DiceTriangle.h"

class bruiseMap
{
public:
	bruiseMap();
	~bruiseMap();
	
	void setBase(const MObject& mesh);
	void setGuide(const MObject& mesh);
	char hasMesh() const {return has_mesh;}
	
	int dice();
	
	void draw();
private:
	char has_mesh;
	MObject pbase, pguide;
	DiceParam* ddice;
	int n_samp, n_vert;
};
#endif