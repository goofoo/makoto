/*
 *  FXMLTriangleMap.h
 *  hairBase
 *
 *  Created by zhang on 08-12-17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FXML_TRIANGLEMAP_H
#define _FXML_TRIANGLEMAP_H


#include "../shared/zData.h"
#include <string>
#include "../shared/zXMLDoc.h"
#include <fstream>
#include <vector>
using namespace std;
//#include "FUVSet.h"
//typedef vector<FUVSet*>UVList;
//#include "FColorSet.h"
//typedef vector<FColorSet*>ColorList;
#include "../sh_lighting/FTriangle.h"
#include "DFeather.h"
class FXMLTriangleMap
{
public:
	FXMLTriangleMap();
	FXMLTriangleMap(const char* filename);
	~FXMLTriangleMap();
// write	
	void beginMap(const char* name);
	void endMap(const char* path);
	
	void staticBegin(const char* path, int n_tri);
	void staticEnd();
	
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
	
	void addP(int num, const XYZ* data);
	void addN(int num, const XYZ* data);
	void addTangent(int num, const XYZ* data);
	void addBBox(float x0, float y0, float z0, float x1, float y1, float z1);
	void addDynamicVectorArray(const char* name, int num, const XYZ* data);
// read
	void free();
	int load(const char* filename);
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
	void dice(float rate, DFeather* res) const;
	void diceNoRT(float rate, DFeather* res) const;
// show
	//void draw() const;
	//void drawTangent() const;
/*	
	void addFaceCount(int val, const int* data);
	
	void uvSetBegin(const char* name);
	void uvSetEnd();
	void addFloatParam(const char* name, int num, const float* data);
	void addS(const char* name, int num, const float* data);
	void addT(const char* name, int num, const float* data);
	
	
	
	void addStaticVectorEntry(const char* name, int num, const XYZ* data);
	void addDynamicVectorEntry(const char* name, int num, const XYZ* data);
	void addIntAttrib(const char* name, int val);
	void addSpace(float space[4][4]);
	
	void free();
	int load(const char* filename);
	
	const char* getXMLName() const { return m_xml_name.c_str(); }
	const char* getMeshName() const { return m_meshName.c_str(); }	
	
	
	void drawNoColor() const;
	void drawNormal() const;
	
	void drawUV(int id) const;
	void drawTangentSpace(const float size) const;


	
	int nfaces() const {return m_numFace;}
	const int* nverts() const {return m_faceCount;}
	const int* verts() const {return m_vertices;}
	const XYZ* points() const {return m_cvs;}
	const XYZ* pointsOpen() const {return m_pOpen;}
	const XYZ* pointsClose() const {return m_pClose;}
	
	int getNumUVSet() const { return m_uvSet.size(); }
	const char* getSNameById(int i) const { return m_uvSet[i]->sname.c_str();}
	const char* getTNameById(int i) const { return m_uvSet[i]->tname.c_str();}
	const float* getSById(int i) const { return m_uvSet[i]->s;}
	const float* getTById(int i) const { return m_uvSet[i]->t;}
	
	void updateFrom(const char* filename);
	void setMotion(float open, float close);
	
	void getNormal(XYZ& res, unsigned int idx ) const { res = m_normals[idx]; }
	void getTangent(XYZ& res, unsigned int idx) const { res = m_tangents[idx]; }
	void getVertex(XYZ& res, unsigned int idx) const { res = m_subd_cvs[idx]; }
	void getMovedVertex(XYZ& res, float bias, unsigned int idx) const { res = m_subd_cvs[idx]; res += m_normals[idx]* bias; }
	
	char toSkipInterreflection() const { return m_i_skip_second; }
	char toSkipScattering() const { return m_i_skip_third; }
	
	void setDrawColor(const XYZ& input, unsigned int idx) { m_draw_color[idx] = input; }
	void getCenterAndSize(XYZ& center, float& size) const;
	
	void resetColor();
	
	void appendColorSet(const char* paramname, const char* filename);
	int getNumColorSet() const { return m_colorSet.size(); }
	const char* getColorSetNameById(int idx) const { return m_colorSet[idx]->name.c_str(); }
	const XYZ* getColorSetById(int idx) const { return m_colorSet[idx]->data; }
	void getSpaceX(XYZ& vec) const {vec.x = m_space[0][0]; vec.y = m_space[0][1]; vec.z = m_space[0][2]; vec.normalize();}
	void getSpaceY(XYZ& vec) const {vec.x = m_space[1][0]; vec.y = m_space[1][1]; vec.z = m_space[1][2]; vec.normalize();}
	void getSpaceZ(XYZ& vec) const {vec.x = m_space[2][0]; vec.y = m_space[2][1]; vec.z = m_space[2][2]; vec.normalize();}
*/	
private:
	ZXMLDoc doc;
	int pos_s, pos_d;
	ofstream static_file, dynamic_file;
	
	string m_xml_name, m_meshName, m_staticName, m_dynamicName;
	int m_numFace, m_numFaceVertex, m_numVertex;
	int* m_vertices;
	float* m_rootS;
	float* m_rootT;
	XYZ* m_cvs;
	XYZ* m_normals;
	XYZ* m_binormals;
	XYZ* m_directions;
	XYZ* m_pref;
	XYZ* m_ups;
	XYZ* m_winds;
	float* m_curl;
	float* m_scales;
	float* m_density;
	XYZ m_bbox0, m_bbox1;
	float m_area;
	char m_isNull;
/*	int* m_faceCount;
	
	XYZ* m_subd_cvs;
	
	XYZ* m_draw_color;
	XYZ* m_color;
	
	
	
	
	char m_has_color;
	
	UVList m_uvSet;
	ColorList m_colorSet;
	
	XYZ* m_pOpen;
	XYZ* m_pClose;
	
	char m_i_skip_second, m_i_skip_third;
	float m_space[4][4];*/
};
#endif
//:~