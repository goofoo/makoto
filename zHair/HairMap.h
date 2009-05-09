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

struct Dguide
{
	short num_seg;
	XYZ dsp_col;
	XYZ ori_p, ori_side, ori_up;
	XYZ* disp_v;
};

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
	void bind();
	
	void draw();
	void initGuide();
	void drawGuide();

	//int saveDguide();
	//void loadDguide();
	
private:
	char has_base, has_guide;
	MObject obase, oguide;
	DiceParam* ddice;
	int n_samp, n_vert;
	Dguide* guide_data;
	unsigned num_guide;
	unsigned* bind_data;
};
#endif
