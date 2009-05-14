/*
 *  VWing.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VWing_H
#define _VWing_H
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
class VWing
{
public:
	VWing();
	~VWing();
	
	char hasNurbs() const {return has_nurbs;}
	void setNurbs(const MObject& obj);
	void draw() const;
	void getBBox(MPoint& corner1, MPoint& corner2);
	void save(const char* filename);
	void setShaftWidth(float w0, float w1) {m_shaft_width0 = w0; m_shaft_width1 = w1;}
	void setFeatherWidth(float w0, float w1) {m_feather_width0 = w0; m_feather_width1 = w1;}
	void setFeatherEdge(float w0, float w1) {m_feather_edge0 = w0; m_feather_edge1 = w1;}
	void setFeatherTwist(float w0, float w1) {m_feather_twist0 = w0; m_feather_twist1 = w1;}
	void setReverse(int v) {m_is_reversed = v;}
	void setStep(int v) {m_n_step = v;}
	MObject createShaft() const;
	MObject createFeather();
	void getTexcoordOffset(MDoubleArray& texcoord) const;
	void setNoise(float size, float frequency, int seed)
	{
		m_noise_period = 24.f/frequency;
		m_noise_k = size;
		m_seed_offset = seed;
	}
	void setTime(double v) {m_time = v/m_noise_period;}
	float getSeed() {return vnoi0;}
	float getSeed1() {return vnoi1;}
	float getFract() {return vnoi;}
		/*void setScale(float val) {m_scale = val;}
	void setWind(float val) {m_wind = val;}
	
	
	
	const MDagPath& getMesh() const {return pgrow;}
	
	
	void drawVelocity() const;
	
	void init();
	void update();
	
	MVector getVelocity(int i) const { if(i<m_v_buf.length()) return m_v_buf[i]; else return MVector(0,0,0);}
	
	*/
private:
	char has_nurbs;
	MObject pnurbs;
	float m_shaft_width0, m_shaft_width1, m_feather_width0, m_feather_width1, m_feather_twist0, m_feather_twist1, m_feather_edge0, m_feather_edge1;
	int m_is_reversed, m_n_step;
	float m_time;
	float vnoi, fnoi, vnoi0, vnoi1;
	int g_seed, g_seed1;
	int m_seed_offset;
	float m_noise_k, m_noise_period;
	/*float m_scale, m_wind;
	MPointArray m_p_buf;
	MVectorArray m_v_buf;*/
};
#endif