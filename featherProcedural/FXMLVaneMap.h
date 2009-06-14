/*
 *  FXMLVaneMap.h
 *  hairBase
 *
 *  Created by zhang on 08-12-17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FXML_VANEMAP_H
#define _FXML_VANEMAP_H


#include "../shared/zData.h"
#include <string>
#include "../shared/zXMLDoc.h"
#include <fstream>

using namespace std;

class FXMLVaneMap
{
public:
	FXMLVaneMap();
	~FXMLVaneMap();
// write	

	void beginMap(const char* name);
	void endMap(const char* path);
	void dataBegin(const char* path);
	void dataEnd();
	void vaneBegin(int n);
	void vaneEnd();
	void addP(int num, const XYZ* data);
	void addVectorArray(const char* name, int num, const XYZ* data);
	void addBBox(float x0, float y0, float z0, float x1, float y1, float z1);
	
// read
	FXMLVaneMap(const char* filename);
	void free();
	int load(const char* filename);
	void finish();
	char getVane(int& num);
	void getData(XYZ* p, XYZ* up, XYZ* down, XYZ* normal);
	void next();

private:
	ZXMLDoc doc;
	ofstream data_file;
	ifstream ffin;
	int pos_d;
	char m_isNull;
/*	
	
	
	
	void staticBeginNoWrite(const char* path, int n_tri);
	void staticEndNoWrite();
	
	void addVertexId(int val, const int* data);
	void addVertexId(int val);
	void addPref(int num, const XYZ* data);
	void addPref(int num);
	void addFaceArea(int num, const float* data);
	void addFaceArea(int num);
	void addStaticFloatArray(const char* name, int num, const float* data);
	void addStaticFloatArray(const char* name, int num);
	
	void dynamicBegin(const char* path);
	void dynamicEnd();
	
	
	void addN(int num, const XYZ* data);
	void addTangent(int num, const XYZ* data);
	
	
// read
	
// query
	char isNull() const {return m_isNull;}
	int getNumFace() const { return m_numFace; }
	int getNumFaceVertex() const { return m_numFaceVertex; }
	int getNumVertex() const { return m_numVertex; }
	float getArea() const {return m_area;}
	void getCenterAndSize(XYZ& center, float& size) const
	{
		center = m_bbox0 + m_bbox1;
		center /= 2.0f;
		size = m_bbox1.x - m_bbox0.x + m_bbox1.y - m_bbox0.y + m_bbox1.z - m_bbox0.z;
	}
	
	float getBBox0X() const {return m_bbox0.x;}
	float getBBox1X() const {return m_bbox1.x;}
	float getBBox0Y() const {return m_bbox0.y;}
	float getBBox1Y() const {return m_bbox1.y;}
	float getBBox0Z() const {return m_bbox0.z;}
	float getBBox1Z() const {return m_bbox1.z;}
// dicing
	void dice(int n_cell, DFeather* res) const;
// show
	void draw() const;
	void drawTangent() const;


	
	ofstream static_file, dynamic_file;
	
	string m_xml_name, m_meshName, m_staticName, m_dynamicName;
	int m_numFace, m_numFaceVertex, m_numVertex;
	int* m_vertices;
	XYZ* m_cvs;
	XYZ* m_normals;
	XYZ* m_binormals;
	XYZ* m_directions;
	XYZ* m_pref;
	XYZ* m_ups;
	XYZ* m_winds;
	float* m_scales;
	XYZ m_bbox0, m_bbox1;
	float m_area;
	
*/
};
#endif
//:~