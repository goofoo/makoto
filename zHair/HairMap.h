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
#include "../shared/DiceTriangle.h"

class hairMap
{
public:
	hairMap();
	~hairMap();
	
	void setBase(const MObject& mesh);
	void setGuide(const MObject& mesh);
	char hasBase() const {return has_base;}
	char hasGuide() const {return has_guide;}
	
	int dice();
	
	void draw();
	void drawGuide();
private:
	char has_base, has_guide;
	MObject obase, oguide;
	DiceParam* ddice;
	int n_samp, n_vert;
};
#endif