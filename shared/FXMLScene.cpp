/*
 *  FXMLScene.cpp
 *  vertexCache
 *
 *  Created by zhang on 08-11-23.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "../sh_lighting/FQSplat.h"
#include "FXMLMesh.h"
#include "FXMLScene.h"
#include "XMLUtil.h"
#include "zGlobal.h"

FXMLScene::FXMLScene():
pos_s(0), 
pos_d(0),
m_rt_tree(0)
{}
FXMLScene::~FXMLScene() {}

void FXMLScene::begin(const char* filename, int frame)
{
	string static_path(filename);
	zGlobal::cutByFirstDot(static_path);
	static_path.append(".sta");
	
	static_file.open(static_path.c_str(), ios::out | ios::binary);
	
	string dynamic_path(filename);
	zGlobal::cutByLastDot(dynamic_path);
	dynamic_path.append(".dyn");
	
	dynamic_file.open(dynamic_path.c_str(), ios::out | ios::binary);
	
	doc.begin();
	doc.elementBegin("scene");
	doc.addAttribute("frame", frame);
}

void FXMLScene::end(const char* path)
{
	doc.elementEnd();
	doc.end(path);
	static_file.close();
	dynamic_file.close();
}

void FXMLScene::meshBegin(const char* name)
{
	doc.elementBegin("mesh");
	doc.addAttribute("name", name);
}

void FXMLScene::triangleInfo(int num_triangles, float area)
{
	doc.addAttribute("num_triangle", num_triangles);
	doc.addAttribute("area", area);
}

void FXMLScene::addAttribute(const char* name, int val)
{
	doc.elementBegin("attribute");
	doc.addAttribute("name", name);
	doc.addAttribute("value", val);
	doc.elementEnd();
}

void FXMLScene::addAttribute(const char* name, float val)
{
	doc.elementBegin("attribute");
	doc.addAttribute("name", name);
	doc.addAttribute("value", val);
	doc.elementEnd();
}

void FXMLScene::meshEnd()
{
	doc.elementEnd();
}

void FXMLScene::staticBegin()
{
	doc.elementBegin("static");
}

void FXMLScene::staticEnd()
{
	doc.elementEnd();
	
}

void FXMLScene::addFaceCount(int val, const int* data)
{
	int offset = (int)sizeof(int)*val;
	doc.elementBegin("faceCount");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", val);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, val*sizeof(int));
}

void FXMLScene::addFaceConnection(int val, const int* data)
{
	int offset = (int)sizeof(int)*val;
	doc.elementBegin("faceConnection");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", val);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, val*sizeof(int));
}

void FXMLScene::addTriangleConnection(int val, const int* data)
{
	int offset = (int)sizeof(int)*val;
	doc.elementBegin("triangleConnection");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", val);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, val*sizeof(int));
}

void FXMLScene::uvSetBegin(const char* name)
{
	doc.elementBegin("uvSet");
	doc.addAttribute("name", name);
}

void FXMLScene::uvSetEnd()
{
	doc.elementEnd();
}

void FXMLScene::addS(const char* name, int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("s");
	doc.addAttribute("name", name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, num*sizeof(float));
}

void FXMLScene::addT(const char* name, int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("t");
	doc.addAttribute("name", name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, num*sizeof(float));
}

void FXMLScene::addVertexColor(const char* name, int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("vertex_color");
	doc.addAttribute("name", name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLScene::dynamicBegin()
{
	doc.elementBegin("dynamic");
}

void FXMLScene::dynamicEnd()
{
	doc.elementEnd();
}

void FXMLScene::addP(int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("P");
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	dynamic_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLScene::addN(int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("N");
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	dynamic_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLScene::addTangent(int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("Tangent");
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	dynamic_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLScene::addGridSize(int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("GridSize");
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	dynamic_file.write((char*)data, sizeof(float)*num);
}

void FXMLScene::addBBox(float x0, float y0, float z0, float x1, float y1, float z1)
{
	doc.elementBegin("bbox");
	doc.addAttribute("low", x0, y0, z0);
	doc.addAttribute("high", x1, y1, z1);
	doc.elementEnd();
}

void FXMLScene::transformBegin(const char* name, float space[4][4])
{
	doc.elementBegin("transform");
	doc.addAttribute("name", name);
	doc.addAttribute("X", space[0][0], space[0][1], space[0][2]);
	doc.addAttribute("Y", space[1][0], space[1][1], space[1][2]);
	doc.addAttribute("Z", space[2][0], space[2][1], space[2][2]);
	doc.addAttribute("W", space[3][0], space[3][1], space[3][2]);
	
}

void FXMLScene::transformEnd()
{
	doc.elementEnd();
}

void FXMLScene::addScale(float x, float y, float z)
{
	doc.addAttribute("scale", x, y, z);
}

void FXMLScene::nurbssurfaceBegin(const char* name, int degreeU, int degreeV, int formU, int formV)
{
	doc.elementBegin("nurbs_surface");
	doc.addAttribute("name", name);
	doc.addAttribute("degreeU", degreeU);
	doc.addAttribute("degreeV", degreeV);
	doc.addAttribute("formU", formU);
	doc.addAttribute("formV", formV);
}

void FXMLScene::nurbssurfaceEnd()
{
	doc.elementEnd();
}

void FXMLScene::addStaticVec(const char* name, int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin(name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLScene::addStaticFloat(const char* name, int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin(name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, sizeof(float)*num);
}

void FXMLScene::cameraBegin(const char* name, float space[4][4])
{
	doc.elementBegin("camera");
	doc.addAttribute("name", name);
	doc.addAttribute("X", space[0][0], space[0][1], space[0][2]);
	doc.addAttribute("Y", space[1][0], space[1][1], space[1][2]);
	doc.addAttribute("Z", space[2][0], space[2][1], space[2][2]);
	doc.addAttribute("W", space[3][0], space[3][1], space[3][2]);
	
}

void FXMLScene::cameraEnd()
{
	doc.elementEnd();
}

//////////////////////////
//
//  loading
//
//////////////////////////

int FXMLScene::load(const char* filename)
{
	m_backscat_space[0][0]=1; m_backscat_space[0][1]=0; m_backscat_space[0][2]=0;
	m_backscat_space[1][0]=0; m_backscat_space[1][1]=1; m_backscat_space[1][2]=0;
	m_backscat_space[2][0]=0; m_backscat_space[2][1]=0; m_backscat_space[2][2]=1;
	m_backscat_space[3][0]=0; m_backscat_space[3][1]=0; m_backscat_space[3][2]=0;
	m_scene_area = 0;
	m_scene_numtri = 0;
	m_light_area = 0;
	m_light_numtri = 0;
	m_xml_name = filename;
	if(doc.load(filename) != 1) return 0;
	
	m_frame = doc.getIntAttribByName("frame");
	
	doc.setChildren();
	
	while(doc.isLastNode() != 1)
	{
		if(doc.checkNodeName("mesh") == 1)
		{
			FXMLMesh* pmesh = new FXMLMesh();
			
			pmesh->load(filename, doc.getAttribByName("name"));
			
			if(pmesh->hasAttrib("asLightsource") !=1 && pmesh->hasAttrib("invisible") !=1) 
			{
				m_scene_area += pmesh->getArea();
				m_scene_numtri += pmesh->getNumTriangle();
				m_mesh.push_back(pmesh);
			}
			else if(pmesh->hasAttrib("asLightsource") ==1)
			{
				m_light_area += pmesh->getArea();
				m_light_numtri += pmesh->getNumTriangle();
				m_light.push_back(pmesh);
			}
		}
		doc.nextNode();
	}
	doc.setParent();
	doc.setChildren();
	if(XMLUtil::findByNameAndType("backscat_camera", "camera", doc))
	{
		doc.getFloat3AttribByName("X", m_backscat_space[0][0], m_backscat_space[0][1], m_backscat_space[0][2]);
		doc.getFloat3AttribByName("Y", m_backscat_space[1][0], m_backscat_space[1][1], m_backscat_space[1][2]);
		doc.getFloat3AttribByName("Z", m_backscat_space[2][0], m_backscat_space[2][1], m_backscat_space[2][2]);
		doc.getFloat3AttribByName("W", m_backscat_space[3][0], m_backscat_space[3][1], m_backscat_space[3][2]);
	}
	doc.free();
	
	return 1;
}

void FXMLScene::dice(int n_cell)
{
	for(unsigned i=0; i<m_mesh.size(); i++) 
	{
		if(m_mesh[i]->hasAttrib("noShadow") !=1)
		{
			cout<<" dicing "<<m_mesh[i]->getMeshName();

			int estimate_ncell = m_mesh[i]->getNumTriangle()*n_cell*2;
			estimate_ncell += estimate_ncell/9;
			
			pcdSample* samp = new pcdSample[estimate_ncell];
			int n_samp = 0;
		
			float epsilon = sqrt(m_mesh[i]->getArea()/m_mesh[i]->getNumTriangle()/2/n_cell);
			m_mesh[i]->dice(epsilon, samp, n_samp);
			
			cout<<" n samples: "<<n_samp<<endl;
			
			FQSPLAT* psplat = new FQSPLAT();
			psplat->createNoColor(samp, n_samp);
			
			m_shadow_tree.push_back(psplat);
			delete[] samp;
		}
	}
}

void FXMLScene::diceWithRTandColor(int n_cell)
{
	int estimate_ncell = m_scene_numtri*n_cell*2;
	estimate_ncell += estimate_ncell/9;
	
	pcdSample* samp = new pcdSample[estimate_ncell];
	CoeRec* coe_samp = new CoeRec[estimate_ncell];
	
	int n_samp = 0;
	for(unsigned i=0; i<m_mesh.size(); i++) 
	{
		if(m_mesh[i]->hasAttrib("noShadow") !=1)
		{
			cout<<" dicing "<<m_mesh[i]->getMeshName();

			//int estimate_ncell = m_mesh[i]->getNumTriangle()*n_cell*2;
			//estimate_ncell += estimate_ncell/9;
			
			//pcdSample* samp = new pcdSample[estimate_ncell];
			//CoeRec* coe_samp = new CoeRec[estimate_ncell];
			//int n_samp = 0;
		
			float epsilon = sqrt(m_mesh[i]->getArea()/m_mesh[i]->getNumTriangle()/2/n_cell);
			m_mesh[i]->diceWithRT(epsilon, samp, coe_samp, n_samp);
			
			
			
			//FQSPLAT* psplat = new FQSPLAT();
			//psplat->createRTandColor(samp, coe_samp, n_samp);
			
			//m_shadow_tree.push_back(psplat);
			//delete[] samp;
			//delete[] coe_samp;
		}
	}
	cout<<" n samples: "<<n_samp<<endl;
	
	m_rt_tree = new FQSPLAT();
	m_rt_tree->createRTandColor(samp, coe_samp, n_samp);
	
	delete[] samp;
	delete[] coe_samp;
}

void FXMLScene::dice(int n_cell, FQSPLAT* cloud) const
{
	int estimate_ncell = m_scene_numtri*n_cell*2;
	estimate_ncell += estimate_ncell/9;
	
	pcdSample* samp = new pcdSample[estimate_ncell];
	int n_samp = 0;
	
	
	
	//float epsilon = sqrt(m_scene_area/m_scene_numtri/2/n_cell);
	//cout<<" global grid: "<<epsilon<<endl;
	
	cout<<" dicing... ";
	
	for(unsigned i=0; i<m_mesh.size(); i++) 
	{
		float epsilon = sqrt(m_mesh[i]->getArea()/m_mesh[i]->getNumTriangle()/2/n_cell);
		m_mesh[i]->dice(epsilon, samp, n_samp);
	}

	cout<<"\r n samples: "<<n_samp<<endl;
	cloud->createNoColor(samp, n_samp);
	delete[] samp;
}

void FXMLScene::diceLight(int n_cell, FQSPLAT* cloud) const
{
	int estimate_ncell = m_light_numtri*n_cell*2;
	estimate_ncell += estimate_ncell/9;
	
	pcdSample* samp = new pcdSample[estimate_ncell];
	int n_samp = 0;
	
	cout<<" dicing... ";
	
	for(unsigned i=0; i<m_light.size(); i++) 
	{
		float epsilon = sqrt(m_light[i]->getArea()/m_light[i]->getNumTriangle()/2/n_cell);
		m_light[i]->dice(epsilon, samp, n_samp);
	}

	cout<<"\r n light samples: "<<n_samp<<endl;
	cloud->create(samp, n_samp);
	delete[] samp;
}

void FXMLScene::diceWithRT(int n_cell, FQSPLAT* cloud) const
{
	int estimate_ncell = m_scene_numtri*n_cell*2;
	estimate_ncell += estimate_ncell/9;
	
	pcdSample* samp = new pcdSample[estimate_ncell];
	CoeRec* coe_samp = new CoeRec[estimate_ncell];
	int n_samp = 0;
	
	cout<<" dicing... ";
	
	for(unsigned i=0; i<m_mesh.size(); i++) 
	{
		float epsilon = sqrt(m_mesh[i]->getArea()/m_mesh[i]->getNumTriangle()/2/n_cell);
		m_mesh[i]->diceWithRT(epsilon, samp, coe_samp, n_samp);
	}

	cout<<"\r n samples: "<<n_samp<<endl;
	cloud->createRT(samp, coe_samp, n_samp);
	delete[] samp;
	delete[] coe_samp;
}

void FXMLScene::removeTree()
{
	m_shadow_tree.clear();
}

void FXMLScene::removeRTTree()
{
	if(m_rt_tree) 
	{
		delete m_rt_tree;
		m_rt_tree = NULL;
	}
}
//:~
