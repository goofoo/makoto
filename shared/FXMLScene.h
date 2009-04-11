/*
 *  FXMLScene.h
 *  vertexCache
 *
 *  Created by zhang on 08-11-23.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FXMLSCENE_H
#define _FXMLSCENE_H
#include <string>
#include "zXMLDoc.h"
#include <fstream>
#include <vector>
using namespace std;

class FQSPLAT;
class FXMLMesh;
typedef vector<FXMLMesh*>MeshList;
typedef vector<FQSPLAT*>TreeList;

class FXMLScene
{
public:
	FXMLScene();
	~FXMLScene();
/*writing*/	
	void begin(const char* path, int frame);
	void end(const char* path);
	void meshBegin(const char* name);
	void triangleInfo(int n_tri, float area);
	void addAttribute(const char* name, int val);
	void meshEnd();
	void staticBegin();
	void staticEnd();
	void addFaceCount(int val, const int* data);
	void addFaceConnection(int val, const int* data);
	void addTriangleConnection(int val, const int* data);
	void uvSetBegin(const char* name);
	void uvSetEnd();
	void addS(const char* name, int num, const float* data);
	void addT(const char* name, int num, const float* data);
	void addVertexColor(const char* name, int num, const XYZ* data);
	void dynamicBegin();
	void dynamicEnd();
	void addP(int num, const XYZ* data);
	void addN(int num, const XYZ* data);
	void addTangent(int num, const XYZ* data);
	void addBBox(float x0, float y0, float z0, float x1, float y1, float z1);
	void addTransform(const char* name, float space[4][4]);
	void addCamera(const char* name, float space[4][4]);
/*reading*/
	int load(const char* filename);
/*query*/
	float getSceneArea() {return m_scene_area;}
	int getSceneNumTriangle() {return m_scene_numtri;}
	int getNumMesh() const {return m_mesh.size();}
	const MeshList& getMesh() const {return m_mesh;}
	const FXMLMesh* getMesh(int id) const {return m_mesh[id];}
	const TreeList& getShadowTree() const {return m_shadow_tree;}
	const FQSPLAT* getRTTree() const {return m_rt_tree;}
	int getFrame() const {return m_frame;}
	void getSpaceX(XYZ& vec) const {vec.x = m_backscat_space[0][0]; vec.y = m_backscat_space[0][1]; vec.z = m_backscat_space[0][2]; vec.normalize();}
	void getSpaceY(XYZ& vec) const {vec.x = m_backscat_space[1][0]; vec.y = m_backscat_space[1][1]; vec.z = m_backscat_space[1][2]; vec.normalize();}
	void getSpaceZ(XYZ& vec) const {vec.x = m_backscat_space[2][0]; vec.y = m_backscat_space[2][1]; vec.z = m_backscat_space[2][2]; vec.normalize();}
	char hasLight() const {return m_light_numtri>0;}
	const char* getXMLName() const {return m_xml_name.c_str();}
/*dicing*/
	void dice(int n_cell);
	void diceWithRTandColor(int n_cell);
	void dice(int n_cell, FQSPLAT* cloud) const;
	void diceLight(int n_cell, FQSPLAT* cloud) const;
	void diceWithRT(int n_cell, FQSPLAT* cloud) const;
	void removeTree();
	void removeRTTree();
private:
	ZXMLDoc doc;
	string m_xml_name;
	MeshList m_mesh, m_light;
	TreeList m_shadow_tree;
	FQSPLAT* m_rt_tree;

	float m_scene_area, m_light_area;
	int m_scene_numtri, m_light_numtri;
	
	ofstream static_file, dynamic_file;
	int pos_s, pos_d;
	int m_numFace, m_numFaceVertex, m_numVertex, m_numTriangle, m_frame;
	float m_backscat_space[4][4];
};
#endif
//:~
