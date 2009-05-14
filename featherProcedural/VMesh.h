/*
 *  VMesh.h
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VMESH_H
#define _VMESH_H
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>

class FBend;

class VMesh
{
public:
	VMesh();
	~VMesh();
	
	void setGrow(const MObject& mesh);
	void setScale(float val) {m_scale = val;}
	void setWind(float val) {m_wind = val;}
	void setWidth(float val) {m_width = val;}
	char hasMesh() const {return has_mesh;}
	
	void getBBox(MPoint& corner1, MPoint& corner2);
	//const MDagPath& getMesh() const {return pgrow;}
	
	void draw();
	void drawVelocity();
	MObject createFeather();
	
	void init();
	void update();
	
	MVector getVelocity(int i) const { if(i<m_v_buf.length()) return m_v_buf[i]; else return MVector(0,0,0);}
	
	void save(const char* filename, int isStart);
private:
	char has_mesh;
	MObject pgrow;
	float m_scale, m_wind, m_width;
	MPointArray m_p_buf;
	MVectorArray m_v_buf;
	FBend* fb;
};
#endif