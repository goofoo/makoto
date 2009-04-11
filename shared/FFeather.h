/*
 *  FFeather.h
 *  featherTest
 *
 *  Created by zhang on 08-12-9.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FFEATHER_H
#define _FFEATHER_H
#include "../shared/zData.h"
#include <vector>
using namespace std;

class DPatch
{
public:
	DPatch() {}
	DPatch(int n) {nseg = n;}
	~DPatch() {}
	
	int nseg;
	XYZ cvs[8];
	XYZ Up;
	float length;
private:
	
};

class FFeather
{
public:
	FFeather();
	~FFeather();
	
	FFeather(const char* filename);
	
	char load(const char* filename);
	
	float getBBox0X() const {return m_bbox_min.x;}
	float getBBox1X() const {return m_bbox_max.x;}
	float getBBox0Y() const {return m_bbox_min.y;}
	float getBBox1Y() const {return m_bbox_max.y;}
	float getBBox0Z() const {return m_bbox_min.z;}
	float getBBox1Z() const {return m_bbox_max.z;}
	
	char isNull() const {return is_null;}
	
	void draw(float ratio);
	
private:
	void release();
	void parse(const char* content);
	void countCurves(const char*content);
	void parseNCurves(const char*content);
	void parseP(const char*content);
	void parseN(const char*content);
	
	DPatch* m_patch_list;
	
	XYZ m_bbox_min, m_bbox_max;
	int m_n_curve;
	
	char is_null;
};

#endif
//:~