/*
 *  VVane.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VVane_H
#define _VVane_H
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
class VVane
{
public:
	VVane();
	~VVane();
	
	char hasNurbs() const {return has_nurbs;}
	void setNurbs(const MObjectArray& obj);
	void draw() const;
	void getBBox(MPoint& corner1, MPoint& corner2);
	void save(const char* filename);
	MObject createShaft();
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
	MObjectArray pnurbs;
	/*float m_scale, m_wind;
	MPointArray m_p_buf;
	MVectorArray m_v_buf;*/
};
#endif