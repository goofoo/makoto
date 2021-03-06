/*
 *  FeatherMap.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FeatherMap_H
#define _FeatherMap_H
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MObjectArray.h>
#include "../shared/DiceTriangle.h"
#include <string>

struct AGuider
{
	short num_seg;
	XYZ dsp_col;
	XYZ* P;
	XYZ* N;
	XYZ* T;
	XYZ* dispv;
};

class FeatherMap
{
public:
	FeatherMap();
	~FeatherMap();
	
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

	int save(const char* filename);
	int saveStart(const char* filename);
	int load(const char* filename);
	int loadStart(const char* filename);
	
	void setTwist(const float val) {twist = val;}
	void setClumping(const float val) {clumping = val;}
	void setFuzz(const float val) {fuzz = val;}
	void setKink(const float val) {kink = val;}
	void setDrawAccuracy(const int val) {if(val>0) draw_step = val;}
	void setRootColor(const float r, const float g, const float b) {root_color.x = r; root_color.y = g; root_color.z = b;}
	void setTipColor(const float r, const float g, const float b) {tip_color.x = r; tip_color.y = g; tip_color.z = b;}
	void setMutantColor(const float r, const float g, const float b) {mutant_color.x = r; mutant_color.y = g; mutant_color.z = b;}
	void setMutantColorScale(const float v) {mutant_scale = v;}
	
	float getBBox0X() const {return bbox_low.x;}
	float getBBox0Y() const {return bbox_low.y;}
	float getBBox0Z() const {return bbox_low.z;}
	float getBBox1X() const {return bbox_high.x;}
	float getBBox1Y() const {return bbox_high.y;}
	float getBBox1Z() const {return bbox_high.z;}
	const char* getCacheName() const {return m_cachename.c_str();}
	float getTwist() const {return twist;}
	float getClumping() const {return clumping;}
	float getFuzz() const {return fuzz;}
	float getKink() const {return kink;}
	float getRootColorR() const {return root_color.x;}
	float getRootColorG() const {return root_color.y;}
	float getRootColorB() const {return root_color.z;}
	float getTipColorR() const {return tip_color.x;}
	float getTipColorG() const {return tip_color.y;}
	float getTipColorB() const {return tip_color.z;}
	float getMutantColorR() const {return mutant_color.x;}
	float getMutantColorG() const {return mutant_color.y;}
	float getMutantColorB() const {return mutant_color.z;}
	float getMutantColorScale() const {return mutant_scale;}
	
private:
	char has_base, has_guide;
	MObject obase;
	MObjectArray oguide;
	DiceParam* ddice;
	int n_samp;
	float sum_area;
	int* pconnection;
	float* uarray;
	float* varray;
	XYZ* parray;

	AGuider* guide_data;
	unsigned num_guide, num_guideobj, n_vert, n_tri;

	unsigned* bind_data;
	float twist, clumping, fuzz, kink, mutant_scale;
	unsigned draw_step;
	MATRIX44F* guide_spaceinv;
	XYZ bbox_low, bbox_high;
	std::string m_cachename;
	XYZ tip_color, root_color, mutant_color;
};
#endif
