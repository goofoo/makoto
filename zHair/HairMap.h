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
#include "BindTriangle.h"
#include <string>

//#define USE_MAYA2007

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
	int faceId;

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

class hairMap
{
public:
	hairMap();
	~hairMap();
	
	char isNil() const {return is_nil;}
	
	void setBase(const MObject& mesh);
	void setGuide(const MObjectArray& meshes);
	char hasBase() const {return has_base;}
	char hasGuide() const {return has_guide;}
	
	void updateBase();
	
	int pushFaceVertice();
	void bind();
	
	void draw();
	void drawUV();
	void initGuide();
	void updateGuide();
	void drawGuide();
	void drawGuideUV();
	void drawBind();
	void drawBBox();

	int save(const char* filename);
	int saveStart(const char* filename);
	int load(const char* filename);
	int loadStart(const char* filename);
	int loadNext();
	
	void setClumping(const float val) {clumping = val;}
	void setFuzz(const float val) {fuzz = val;}
	void setKink(const float val) {kink = val;}
	void setDrawAccuracy(const int val) {if(val>0) draw_step = val;}
	void setRootColor(const float r, const float g, const float b) {root_color.x = r; root_color.y = g; root_color.z = b;}
	void setTipColor(const float r, const float g, const float b) {tip_color.x = r; tip_color.y = g; tip_color.z = b;}
	void setMutantColor(const float r, const float g, const float b) {mutant_color.x = r; mutant_color.y = g; mutant_color.z = b;}
	void setMutantColorScale(const float v) {mutant_scale = v;}
	void setOffset(const float val) {m_offset = val;}
	void setBald(const float val) {m_bald = val;}
	void setSnowSize(const float val) {m_snow_size = val;}
	void setSnowRate(const float val) {m_snow_rate = val;}
	
	float getBBox0X() const {return bbox_low.x;}
	float getBBox0Y() const {return bbox_low.y;}
	float getBBox0Z() const {return bbox_low.z;}
	float getBBox1X() const {return bbox_high.x;}
	float getBBox1Y() const {return bbox_high.y;}
	float getBBox1Z() const {return bbox_high.z;}
	const char* getCacheName() const {return m_cachename.c_str();}
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
	float getBald() const {return m_bald;}
	const char* getDensityName() const {return m_densityname.c_str();}
	
	void createSnow(double& time, MObject& meshData) const;
	
	void setDensityMap(const char* filename);
	void muliplyDensityMap(float& val, float& s, float& t) const;
	
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
	XYZ* pframe1;

	Dguide* guide_data;
	unsigned* pNSeg;
	unsigned num_guide, num_guideobj, n_vert, n_tri;

	triangle_bind_info* bind_data;
	float clumping, fuzz, kink, mutant_scale;
	unsigned draw_step;
	MATRIX44F* guide_spaceinv;
	XYZ bbox_low, bbox_high;
	std::string m_cachename, m_densityname;
	XYZ tip_color, root_color, mutant_color;
	int* nsegbuf;
	float m_offset, m_bald, m_snow_size, m_snow_rate;
	int densmap_w, densmap_h;
	float* pDensmap;
	char is_nil;
};
#endif
