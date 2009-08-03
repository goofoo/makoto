/*
 *  FXMLMesh.cpp
 *  vertexCache
 *
 *  Created by zhang on 08-10-13.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "../sh_lighting/FTriangle.h"
#include "FXMLMesh.h"
#include "gBase.h"
#include "zGlobal.h"
#include <iostream>

const float constantCoeff[16] = { 3.543211, 0.000605, 0.000152, -0.003217, 
								0.000083, -0.002813, -0.000021, -0.001049,
								0.000144, -0.001950, -0.000261, 0.004375,
								0.001578, -0.004627, 0.001557, -0.000312};
								
const float hdrCoeff[48] = { 0.545337,0.615628,0.650480,
-0.410697,-0.509181,-0.586825,
-0.089776,-0.110368,-0.129048,
-0.134925,-0.125774,-0.088575,
0.065117,0.033042,-0.019713,
0.061583,0.089115,0.122649,
-0.100385,-0.116865,-0.129929,
0.112412,0.132619,0.135714,
0.022489,-0.020040,-0.085217,
-0.200834,-0.207858,-0.188677,
-0.072292,-0.082221,-0.080390,
-0.025514,-0.030657,-0.032725,
0.029278,0.033616,0.044302,
-0.003408,-0.020655,-0.049015,
-0.021611,-0.010354,0.013596,
-0.121166,-0.160075,-0.194946};

FXMLMesh::FXMLMesh():
pos_s(0), 
pos_d(0),
m_faceCount(0), 
m_vertices(0), 
m_triangleConn(0),
m_cvs(0),
//m_subd_cvs(0), 
m_normals(0),
m_tangents(0), m_binormals(0),
m_draw_color(0), m_color(0),
m_numFace(0), 
m_numFaceVertex(0), 
m_numVertex(0),
m_i_skip_second(0), m_i_skip_third(0),
m_pOpen(0), m_pClose(0),
m_grd(0),
m_isNull(1)
{}

FXMLMesh::FXMLMesh(const char* filename, const char* meshname):
pos_s(0), 
pos_d(0),
m_faceCount(0), 
m_vertices(0),
m_triangleConn(0), 
m_cvs(0),
//m_subd_cvs(0), 
m_normals(0),
m_tangents(0), m_binormals(0),
m_draw_color(0), m_color(0),
m_numFace(0), 
m_numFaceVertex(0), 
m_numVertex(0),
m_i_skip_second(0), m_i_skip_third(0),
m_pOpen(0), m_pClose(0),
m_grd(0),
m_isNull(1)
{
	resetHDRLighting();
	if(load(filename, meshname) == 1) m_isNull = 0;
}

FXMLMesh::~FXMLMesh()
{
	free();
}

void FXMLMesh::initMesh(const char* name, int num_triangles, float area)
{
	doc.begin();
	doc.elementBegin("mesh");
	doc.addAttribute("name", name);
	doc.addAttribute("num_triangle", num_triangles);
	doc.addAttribute("area", area);
}

void FXMLMesh::endMesh(const char* path)
{
	doc.elementEnd();
	//doc.end("/Users/jollyroger/Desktop/foo.xml");
	doc.end(path);
}

void FXMLMesh::addAttribute(const char* name, int val)
{
	doc.elementBegin("attribute");
	doc.addAttribute("name", name);
	doc.addAttribute("value", val);
	doc.elementEnd();
}

void FXMLMesh::staticBegin(const char* path)
{
	doc.elementBegin("static");
	//doc.addAttribute("path", path);
	
	static_file.open(path, ios::out | ios::binary);
}

void FXMLMesh::staticEnd()
{
	doc.elementEnd();
	static_file.close();
}
	
void FXMLMesh::dynamicBegin(const char* path)
{
	doc.elementBegin("dynamic");
	//doc.addAttribute("path", path);
	
	dynamic_file.open(path, ios::out | ios::binary);
}

void FXMLMesh::dynamicEnd()
{
	doc.elementEnd();
	dynamic_file.close();
}

void FXMLMesh::addFaceCount(int val, const int* data)
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

void FXMLMesh::addFaceConnection(int val, const int* data)
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

void FXMLMesh::addTriangleConnection(int val, const int* data)
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

void FXMLMesh::addP(int num, const XYZ* data)
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

void FXMLMesh::addN(int num, const XYZ* data)
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

void FXMLMesh::addTangent(int num, const XYZ* data)
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

void FXMLMesh::addStaticVectorEntry(const char* name, int num, const XYZ* data)
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

void FXMLMesh::addDynamicVectorEntry(const char* name, int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin(name);
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	dynamic_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLMesh::addIntAttrib(const char* name, int val)
{
	doc.addAttribute(name, val);
}

void FXMLMesh::addFloatParam(const char* name, int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("parameter");
	doc.addAttribute("name", name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, num*sizeof(float));
}

void FXMLMesh::uvSetBegin(const char* name)
{
	doc.elementBegin("uvSet");
	doc.addAttribute("name", name);
}

void FXMLMesh::uvSetEnd()
{
	doc.elementEnd();
}

void FXMLMesh::addS(const char* name, int num, const float* data)
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

void FXMLMesh::addT(const char* name, int num, const float* data)
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

void FXMLMesh::addVertexFloat(const char* name, int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("vertex_float");
	doc.addAttribute("name", name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, num*sizeof(float));
}

void FXMLMesh::addBBox(float x0, float y0, float z0, float x1, float y1, float z1)
{
	doc.elementBegin("bbox");
	doc.addAttribute("low", x0, y0, z0);
	doc.addAttribute("high", x1, y1, z1);
	doc.elementEnd();
}

void FXMLMesh::addSpace(float space[4][4])
{
	doc.elementBegin("space");
	doc.addAttribute("X", space[0][0], space[0][1], space[0][2]);
	doc.addAttribute("Y", space[1][0], space[1][1], space[1][2]);
	doc.addAttribute("Z", space[2][0], space[2][1], space[2][2]);
	doc.addAttribute("W", space[3][0], space[3][1], space[3][2]);
	doc.elementEnd();
}

void FXMLMesh::free()
{
	if(m_isNull==0)
	{
		if(m_faceCount) delete[] m_faceCount;
		if(m_vertices) delete[] m_vertices;
		if(m_triangleConn) delete[] m_triangleConn;
		if(m_cvs) delete[] m_cvs;
		//if(m_subd_cvs) delete[] m_subd_cvs;
		if(m_normals) delete[] m_normals;
		if(m_tangents) delete[] m_tangents;
		if(m_binormals) delete[] m_binormals;
		if(m_draw_color) delete[] m_draw_color;
		if(m_color) delete[] m_color;
		if(m_pOpen) delete[] m_pOpen;
		if(m_pClose) delete[] m_pClose;
		if(m_grd) delete[] m_grd;
		m_uvSet.clear();
		m_colorSet.clear();
		m_floatSet.clear();
		m_isNull = 1;
	}
}

int FXMLMesh::findMesh(const char* meshname)
{
	while(doc.isLastNode() != 1)
	{
		if(doc.checkNodeName("mesh") == 1)
		{
			if(strcmp ( doc.getAttribByName("name"), meshname )==0) return 1;
		}
		doc.nextNode();
	}
	return 0;
}

int FXMLMesh::load(const char* filename, const char* meshname)
{
	if(string(meshname)=="") return 0;
	free();
	m_xml_name = string(filename);
	if(doc.load(filename) != 1) { cout<<"cannot open scene "<<filename; return 0;}
	
	doc.setChildren();
	
	if(findMesh(meshname) !=1) { cout<<"cannot find mesh "<<meshname; return 0;}
	
	ifstream ffin;
	int pos, size;
	
	m_area = doc.getFloatAttribByName("area");
	m_numTriangle = doc.getIntAttribByName("num_triangle");
	m_meshName = string(doc.getAttribByName("name"));
	//m_grid = sqrt(m_area/m_numTriangle/2);
	
	m_bbox0.x = m_bbox0.y = m_bbox0.z = 0.0f;
	m_bbox1.x = m_bbox1.y = m_bbox1.z = 1.0f;

/*get arributes*/
	doc.setChildren();
	while(doc.isLastNode()!=1)
		{
			if(doc.checkNodeName("attribute") ==1)
			{
				//printf("mesh attrib: %s\n", doc.getAttribByName("name"));
				m_attrib.push_back(doc.getAttribByName("name"));
			}
			if(doc.checkNodeName("bbox") ==1)
			{
				doc.getFloat3AttribByName("low", m_bbox0.x, m_bbox0.y, m_bbox0.z);
				doc.getFloat3AttribByName("high", m_bbox1.x, m_bbox1.y, m_bbox1.z);
			}
			doc.nextNode();
		}
		doc.setParent();

		doc.getChildByName("static");
		
		m_staticName = m_xml_name;
		zGlobal::cutByFirstDot(m_staticName);
		m_staticName += ".sta";
		
		ffin.open(m_staticName.c_str(), ios::in | ios::binary);
		
			doc.getChildByName("faceCount");
			m_numFace = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_faceCount = new int[m_numFace];
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_faceCount, size);
			
		doc.setParent();
			doc.getChildByName("faceConnection");
			m_numFaceVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_vertices = new int[m_numFaceVertex];
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_vertices, size);
			
		doc.setParent();
		
			doc.getChildByName("triangleConnection");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_triangleConn = new int[m_numTriangle*3];
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_triangleConn, size);
			
		doc.setParent();
		
		doc.setChildren();
		while(doc.isLastNode()!=1)
		{
			if(doc.checkNodeName("vertex_float") ==1)
			{
				FUVSet* uv = new FUVSet();
				uv->name = string("vertex float ") + doc.getAttribByName("name");
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				uv->s = new float[size/4];
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)uv->s, size);
				m_vertexFloatSet.push_back(uv);
			}
			doc.nextNode();
		}
		doc.setParent();
		
		doc.setChildren();
		
		while(doc.isLastNode()!=1)
		{
			if(doc.checkNodeName("uvSet") ==1)
			{
				//printf("uvSet: %s\n", doc.getAttribByName("name"));
				
				FUVSet* uv = new FUVSet();
				uv->name = doc.getAttribByName("name");
				uv->s = new float[m_numFaceVertex];
				uv->t = new float[m_numFaceVertex];
				
				if(doc.isEmpty() != 1)
				{
					if(doc.getChildByName("s") == 1)
					{
						uv->sname = doc.getAttribByName("name");
						pos = doc.getIntAttribByName("loc");
						size = doc.getIntAttribByName("size");
						ffin.seekg( pos, ios::beg );
						ffin.read((char*)uv->s, size);
					}
					doc.setParent();
					
					if(doc.getChildByName("t") == 1)
					{
						uv->tname = doc.getAttribByName("name");
						pos = doc.getIntAttribByName("loc");
						size = doc.getIntAttribByName("size");
						ffin.seekg( pos, ios::beg );
						ffin.read((char*)uv->t, size);
					}
					doc.setParent();
				}
				
				m_uvSet.push_back(uv);
			}
			doc.nextNode();
		}
		
		doc.setParent();
		
			if(doc.getChildByName("vertex_color") != 0)
			{
				m_has_color = 1;
				m_numVertex = doc.getIntAttribByName("count");
				m_draw_color = new XYZ[m_numVertex];
				m_color = new XYZ[m_numVertex];
				
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_color, size);
			}
			doc.setParent();
			ffin.close();
	doc.setParent();
		doc.getChildByName("dynamic");
		
		m_dynamicName = m_xml_name;
		zGlobal::cutByLastDot(m_dynamicName);
		m_dynamicName += ".dyn";
		
		ffin.open(m_dynamicName.c_str(), ios::in | ios::binary);
		
			doc.getChildByName("P");
			m_numVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_cvs = new XYZ[m_numVertex];
			
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_cvs, size);
			
			//printf("%f %f %f \n", m_cvs[1].x, m_cvs[1].y, m_cvs[1].z);
		
		doc.setParent();
		
			m_normals = new XYZ[m_numVertex];
			
			if(doc.getChildByName("N") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_normals, size);
			}
		doc.setParent();
		
			m_tangents = new XYZ[m_numVertex];
			
			if(doc.getChildByName("Tangent") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_tangents, size);
			}
		doc.setParent();
		
			m_binormals = new XYZ[m_numVertex];
			for(unsigned iv = 0; iv<m_numVertex; iv++) {
				m_binormals[iv] = m_tangents[iv]^m_normals[iv];
				m_binormals[iv].normalize();
			}
		
			m_grd = new float[m_numVertex];
// set average grid size in case no data is saved
			float avg_sgrd = sqrt(m_area/m_numTriangle)/2;
			for(unsigned iv = 0; iv<m_numVertex; iv++) m_grd[iv] = avg_sgrd;
			
			if(doc.getChildByName("GridSize") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_grd, size);
			}
		doc.setParent();

		if(!m_color)
		{
			m_draw_color = new XYZ[m_numVertex];
			m_color = new XYZ[m_numVertex];
			m_has_color = 0;
		}
		resetColor();

		doc.setParent();
			
		ffin.close();

		doc.setParent();
		
	doc.free();
	m_isNull = 0;

	return 1;
}

void FXMLMesh::draw() const
{
	int acc=0;
	for(int i=0; i<m_numFace; i++)
	{
		glBegin(GL_POLYGON);
		for(int j=0; j<m_faceCount[i]; j++)
		{
			int vertId = m_vertices[acc];
			glColor3d(m_draw_color[vertId].x, m_draw_color[vertId].y, m_draw_color[vertId].z);
			glVertex3f(m_cvs[vertId].x, m_cvs[vertId].y, m_cvs[vertId].z);
			acc++;
		}
		glEnd();
	}
}

void FXMLMesh::drawNoColor() const
{
	int acc=0;
	for(int i=0; i<m_numFace; i++)
	{
		glBegin(GL_POLYGON);
		for(int j=0; j<m_faceCount[i]; j++)
		{
			int vertId = m_vertices[acc];
			glVertex3f(m_cvs[vertId].x, m_cvs[vertId].y, m_cvs[vertId].z);
			acc++;
		}
		glEnd();
	}
}

void FXMLMesh::drawNormal() const
{
	glBegin(GL_LINES);
	for(int i=0; i<m_numVertex; i++)
	{
		glVertex3f(m_cvs[i].x, m_cvs[i].y, m_cvs[i].z);
		glVertex3f(m_cvs[i].x + m_normals[i].x, m_cvs[i].y + m_normals[i].y, m_cvs[i].z + m_normals[i].z);
	}
	glEnd();
}

void FXMLMesh::drawTangent() const
{
	glBegin(GL_LINES);
	for(int i=0; i<m_numVertex; i++)
	{
		glVertex3f(m_cvs[i].x, m_cvs[i].y, m_cvs[i].z);
		glVertex3f(m_cvs[i].x + m_tangents[i].x, m_cvs[i].y + m_tangents[i].y, m_cvs[i].z + m_tangents[i].z);
	}
	glEnd();
}

void FXMLMesh::drawUV(int id) const
{
	if(m_uvSet.size()<(id+1)) return;
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	int acc=0;
	for(int i=0; i<m_numFace; i++)
	{
		glBegin(GL_POLYGON);
		for(int j=0; j<m_faceCount[i]; j++)
		{
			glVertex3f(m_uvSet[id]->s[acc], m_uvSet[id]->t[acc], 0);
			acc++;
		}
		glEnd();
	}
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void FXMLMesh::drawTangentSpace() const
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glBegin(GL_LINES);
	for(int j=0; j<m_numVertex; j++)
	{
		glColor3d(0,1,0);
		glVertex3f(m_cvs[j].x, m_cvs[j].y, m_cvs[j].z);
			
		XYZ up = m_cvs[j] + m_normals[j]*m_grd[j];
		glVertex3f(up.x, up.y, up.z);
			
		glColor3d(1,0,0);
		glVertex3f(m_cvs[j].x, m_cvs[j].y, m_cvs[j].z);
		XYZ side = m_cvs[j] + m_tangents[j]*m_grd[j];
		glVertex3f(side.x, side.y, side.z);
	}
	glEnd();
	glPopAttrib();
}

void FXMLMesh::updateFrom(const char* filename)
{
	m_xml_name = string(filename);
	ifstream ffin;
	int pos, size;
	if(doc.load(filename) != 1) return;
	
	doc.setChildren();
	
	if(findMesh(m_meshName.c_str()) !=1) return;
	
		if(doc.getChildByName("bbox") != 0)
		{
			doc.getFloat3AttribByName("low", m_bbox0.x, m_bbox0.y, m_bbox0.z);
			doc.getFloat3AttribByName("high", m_bbox1.x, m_bbox1.y, m_bbox1.z);
		}
		
		doc.setParent();

		doc.getChildByName("dynamic");
		m_dynamicName = filename;
		zGlobal::changeFilenameExtension(m_dynamicName, "dyn");
			
		ffin.open(m_dynamicName.c_str(), ios::in | ios::binary);
		
			doc.getChildByName("P");
			m_numVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_cvs, size);
		
		doc.setParent();

			if(doc.getChildByName("N") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_normals, size);
			}
		doc.setParent();
		
			if(doc.getChildByName("Tangent") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_tangents, size);
			}
		doc.setParent();

		ffin.close();

	doc.free();

}

void FXMLMesh::setMotion(float open, float close)
{
	m_pOpen = new XYZ[m_numVertex];
	m_pClose = new XYZ[m_numVertex];
	
	for(int i=0; i<m_numVertex; i++) m_pOpen[i] = m_pClose[i] = m_cvs[i];
	
	zGlobal::changeFrameNumber(m_xml_name, int(close+1));
	
	ifstream ffin;
	int pos, size;
	if(doc.load(m_xml_name.c_str()) != 1) return;
	
	doc.setChildren();
	
	if(findMesh(m_meshName.c_str()) !=1) return;
	
		doc.getChildByName("dynamic");
		m_dynamicName = m_xml_name;
		zGlobal::changeFilenameExtension(m_dynamicName, "dyn");
		
		ffin.open(m_dynamicName.c_str(), ios::in | ios::binary);
		
			doc.getChildByName("P");
			m_numVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_pClose, size);
		
		doc.setParent();
		
		ffin.close();
		
	doc.free();
	
	XYZ pbuf, delta_v;
	float len_f = (int)close - (int)open + 1;
	float len_o = (open - (int)open)/len_f;
	float len_c = (close - (int)open)/len_f;
	for(int i=0; i<m_numVertex; i++) 
	{
		delta_v = m_pClose[i] - m_pOpen[i];
		pbuf = m_pOpen[i];
		m_pOpen[i] = pbuf + delta_v * len_o;
		m_pClose[i] = pbuf + delta_v * len_c;
	}
}

void FXMLMesh::getCenterAndSize(XYZ& center, float& size) const
{
	center = m_bbox0 + m_bbox1;
	center = center / 2.0f;
	float dx = m_bbox1.x - m_bbox0.x;
	float dy = m_bbox1.y - m_bbox0.y;
	float dz = m_bbox1.z - m_bbox0.z;
	
	if(dx > dy && dx >dz) size = dx;
		else if(dy > dx && dy >dz) size = dy;
			else size = dz;
}

void FXMLMesh::resetColor()
{
	if(m_has_color) for(int i = 0; i < m_numVertex; i++) m_draw_color[i] = m_color[i];
	else for(int i = 0; i < m_numVertex; i++) m_color[i] = m_draw_color[i] = XYZ(1.0f);
}

void FXMLMesh::updateColor(int opt)
{
	switch (opt)
	{
		case 0: 
			resetColor();
			break;
		case 1: 
			calcOcc();
			break;
		case 2: 
			calcHDRF("prt");
			break;
		case 3: 
			calcHDRC("prti");
			break;
		case 4: 
			calcHDRF("prts");
			break;
		case 5: 
			calcHDRF("prte");
			break;
		case 6: 
			calcHDRF("prtb");
			break;
		case 7: 
			calcSRC();
			break;
		default:       
		break;
	}
}

void FXMLMesh::calcOcc()
{
	CoeRec* vex_coe = new CoeRec[m_numVertex];
	
	string rtName = m_xml_name;
	zGlobal::replacefilename(rtName, m_meshName);
	zGlobal::changeFilenameExtension(rtName, "prtu");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_numVertex*16*sizeof(float))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe, m_numVertex*16*sizeof(float));
			ifile.close();
		}
	}
	
	float ov;
	for(int i = 0; i < m_numVertex; i++) 
	{
		ov = 0;
		for(int j = 0; j < 16; j++) ov += vex_coe[i].data[j]*constantCoeff[j];
		ov /= 6.28;
		m_draw_color[i] = XYZ(ov);
	}
	
	delete[] vex_coe;
}

void FXMLMesh::calcHDRF(const char* ext)
{
	CoeRec* vex_coe = new CoeRec[m_numVertex];
	
	string rtName = m_xml_name;
	zGlobal::replacefilename(rtName, m_meshName);
	zGlobal::changeFilenameExtension(rtName, ext);
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_numVertex*16*sizeof(float))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe, m_numVertex*16*sizeof(float));
			ifile.close();
		}
	}
	
	float r, g, b;
	for(int i = 0; i < m_numVertex; i++) 
	{
		r =g =b = 0;
		for(int j = 0; j < 16; j++) 
		{
			r += vex_coe[i].data[j]*usr_hdrCoeff[j].x;
			g += vex_coe[i].data[j]*usr_hdrCoeff[j].y;
			b += vex_coe[i].data[j]*usr_hdrCoeff[j].z;
		}
		m_draw_color[i] = XYZ(r,g,b);
	}
	
	delete[] vex_coe;
}

void FXMLMesh::calcHDRC(const char* ext)
{
	XYZ* vex_coe = new XYZ[m_numVertex*16];
	
	string rtName = m_xml_name;
	zGlobal::replacefilename(rtName, m_meshName);
	zGlobal::changeFilenameExtension(rtName, ext);
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_numVertex*16*sizeof(XYZ))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe, m_numVertex*16*sizeof(XYZ));
			ifile.close();
		}
	}
	
	float r, g, b;
	for(int i = 0; i < m_numVertex; i++) 
	{
		r =g =b = 0;
		for(int j = 0; j < 16; j++) 
		{
			r += vex_coe[i*16+j].x*usr_hdrCoeff[j].x;
			g += vex_coe[i*16+j].y*usr_hdrCoeff[j].y;
			b += vex_coe[i*16+j].z*usr_hdrCoeff[j].z;
		}
		m_draw_color[i] = XYZ(r,g,b);
	}
	
	delete[] vex_coe;
}

void FXMLMesh::calcSRC()
{
	XYZ* vex_coe = new XYZ[m_numVertex*16];
	
	string rtName = m_xml_name;
	zGlobal::replacefilename(rtName, m_meshName);
	zGlobal::changeFilenameExtension(rtName, "prtl");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_numVertex*16*sizeof(XYZ))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe, m_numVertex*16*sizeof(XYZ));
			ifile.close();
		}
	}
	
	float r, g, b;
	for(int i = 0; i < m_numVertex; i++) 
	{
		r =g =b = 0;
		for(int j = 0; j < 16; j++) 
		{
			r += vex_coe[i*16+j].x*constantCoeff[j];
			g += vex_coe[i*16+j].y*constantCoeff[j];
			b += vex_coe[i*16+j].z*constantCoeff[j];
		}
		m_draw_color[i] = XYZ(r,g,b);
	}
	
	delete[] vex_coe;
}

void FXMLMesh::appendColorSet(const char* paramname, const char* filename)
{
	ifstream ffin;
	ffin.open(filename, ios::in | ios::binary | ios::ate);
	if(!ffin.is_open()) 
	{
		printf("ERROR: Cannot open %s\n", filename);
		return;
	}
	
	ifstream::pos_type size = ffin.tellg();
	
	char is_float = 0;
	if((int)size == sizeof(float)*16*m_numVertex) is_float = 1;
	
	ffin.seekg( 0, ios::beg );
	
	float* fbuf;

	if(is_float) 
	{
		fbuf = new float[m_numVertex*16];
		ffin.read( (char*)fbuf, sizeof(float)*16*m_numVertex );
	}
	else 
	{
		fbuf = new float[m_numVertex*48];
		ffin.read( (char*)fbuf, sizeof(float)*48*m_numVertex );
	}
	
	ffin.close();
	
	char sbuf[64];
	
	if(is_float) 
	{
		for(int j=0; j<16; j++)
		{
			FColorSet* colset = new FColorSet();
			sprintf(sbuf, "%s%i", paramname, j);
				
			colset->name = sbuf;
			colset->data = new XYZ[m_numVertex];
		
			for(int i=0; i<m_numVertex; i++) colset->data[i] = XYZ(fbuf[i*16+j]);
					
			m_colorSet.push_back(colset);
		}
	}
	else
	{
		for(int j=0; j<16; j++)
		{
			FColorSet* colset = new FColorSet();
			sprintf(sbuf, "%s%i", paramname, j);
				
			colset->name = sbuf;
			colset->data = new XYZ[m_numVertex];
		
			for(int i=0; i<m_numVertex; i++) colset->data[i] = XYZ(fbuf[i*48+j*3], fbuf[i*48+j*3+1], fbuf[i*48+j*3+2]);
					
			m_colorSet.push_back(colset);
		}
	}
	delete[] fbuf;
}

void FXMLMesh::appendFloatSet(const char* paramname, const char* filename)
{
	ifstream ffin;
	ffin.open(filename, ios::in | ios::binary | ios::ate);
	if(!ffin.is_open()) 
	{
		printf("ERROR: Cannot open %s\n", filename);
		return;
	}
	
	ifstream::pos_type size = ffin.tellg();
	
	char is_float = 0;
	if((int)size == sizeof(float)*16*m_numVertex) is_float = 1;
	
	ffin.seekg( 0, ios::beg );
	
	float* fbuf;

	if(is_float) 
	{
		fbuf = new float[m_numVertex*16];
		ffin.read( (char*)fbuf, sizeof(float)*16*m_numVertex );
	}
	else 
	{
		fbuf = new float[m_numVertex*48];
		ffin.read( (char*)fbuf, sizeof(float)*48*m_numVertex );
	}
	
	ffin.close();
	
	char sbuf[64];
	
	if(is_float) 
	{
		for(int j=0; j<16; j++)
		{
			FFloatSet* colset = new FFloatSet();
			sprintf(sbuf, "%s%i", paramname, j);
				
			colset->name = sbuf;
			colset->data = new float[m_numVertex];
		
			for(int i=0; i<m_numVertex; i++) colset->data[i] = fbuf[i*16+j];
					
			m_floatSet.push_back(colset);
		}
	}
	else
	{
		for(int j=0; j<16; j++)
		{
			FFloatSet* colset = new FFloatSet();
			sprintf(sbuf, "%s%i", paramname, j);
				
			colset->name = sbuf;
			colset->data = new float[m_numVertex];
		
			for(int i=0; i<m_numVertex; i++) colset->data[i] = fbuf[i*48+j*3];
					
			m_floatSet.push_back(colset);
		}
	}
	delete[] fbuf;
}

void FXMLMesh::dice(float epsilon, pcdSample* res, int& n_res)
{
	int a, b, c;
	FTriangle ftri;
	//float min_area = epsilon*epsilon/32;
	for(int i=0; i<m_numTriangle; i++)
	{
		a = m_triangleConn[i*3];
		b = m_triangleConn[i*3+1];
		c = m_triangleConn[i*3+2];
		
		ftri.create(m_cvs[a], m_cvs[b], m_cvs[c]);
		ftri.setNormal(m_normals[a], m_normals[b], m_normals[c]);
		ftri.setColor(m_color[a], m_color[b], m_color[c]);
		ftri.setTangent(m_tangents[a], m_tangents[b], m_tangents[c]);
		
		ftri.rasterize(epsilon, res, n_res);
	}
}

void FXMLMesh::diceWithRT(float epsilon, pcdSample* res, CoeRec* coe_res, int& n_res)
{
	CoeRec* vex_coe = new CoeRec[m_numVertex];
	
	string rtName = m_xml_name;
	zGlobal::replacefilename(rtName, m_meshName);
	zGlobal::changeFilenameExtension(rtName, "prt");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_numVertex*16*sizeof(float))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe, m_numVertex*16*sizeof(float));
			ifile.close();
		}
	}
	
	int a, b, c;
	FTriangle ftri;
	//float min_area = epsilon*epsilon/32;
	for(int i=0; i<m_numTriangle; i++)
	{
		a = m_triangleConn[i*3];
		b = m_triangleConn[i*3+1];
		c = m_triangleConn[i*3+2];
		
		ftri.create(m_cvs[a], m_cvs[b], m_cvs[c]);
		ftri.setNormal(m_normals[a], m_normals[b], m_normals[c]);
		ftri.setColor(m_color[a], m_color[b], m_color[c]);
		ftri.setTangent(m_tangents[a], m_tangents[b], m_tangents[c]);
		ftri.setId(a, b, c);
		
		ftri.rasterize(epsilon, vex_coe, res, coe_res, n_res);
	}	
	delete[] vex_coe;	
}

int FXMLMesh::checkExistingRT() const
{
	string rtName = m_xml_name;
	zGlobal::changeFilenameExtension(rtName, "prt");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(!ifile.is_open()) return 0;

	ifstream::pos_type size = ifile.tellg();
	
	if((int)size < m_numVertex*48*sizeof(float)) return 0;
	
	return 1;
}

int FXMLMesh::hasAttrib(const char* attribname) const
{
	AttribList list = m_attrib;
	for(AttribList::iterator it=list.begin(); it != list.end(); ++it)
	{
		if((*it)==string(attribname)) return 1;
	}
	return 0;
}

void FXMLMesh::setHDRLighting(const XYZ* coeff)
{
	for(unsigned i=0; i<16; i++) usr_hdrCoeff[i] = coeff[i];
}

void FXMLMesh::resetHDRLighting()
{
	for(unsigned i=0; i<16; i++) usr_hdrCoeff[i] = XYZ(hdrCoeff[i*16], hdrCoeff[i*16+1], hdrCoeff[i*16+2]);
}

void FXMLMesh::interpolateRT(int lo, int hi, int frame, float weight, const char* extension, int fpv)
{
	float* vex_coe = new float[m_numVertex*fpv];
	
	string rtName = m_xml_name;
	zGlobal::replacefilename(rtName, m_meshName);
	zGlobal::changeFilenameExtension(rtName, extension);
	zGlobal::changeFrameNumber(rtName, lo);
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(!ifile.is_open()) return;

	ifstream::pos_type size = ifile.tellg();
	
	if((int)size == m_numVertex*fpv*sizeof(float)) {
		ifile.seekg(0, ios::beg);
		ifile.read((char*)vex_coe, m_numVertex*fpv*sizeof(float));
		ifile.close();
	}
	
	if(weight < 1.f) {
		float wei0 = 1.f - weight;
		float* vex_coe1 = new float[m_numVertex*fpv];
	
		zGlobal::changeFrameNumber(rtName, hi);
		
		ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
		
		if(!ifile.is_open()) return;

		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_numVertex*fpv*sizeof(float)) {
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe1, m_numVertex*fpv*sizeof(float));
			ifile.close();
		}
		
		for(unsigned i=0; i<m_numVertex*fpv; i++) {
			vex_coe[i] = vex_coe1[i] * weight + vex_coe[i] * wei0;
		}
		delete[] vex_coe1;	
	}

	zGlobal::changeFrameNumber(rtName, frame);
	ofstream ofile;
	ofile.open(rtName.c_str(), ios::out | ios::binary);
	if(!ofile.is_open()) return;
	
	ofile.write((char*)vex_coe, m_numVertex*fpv*sizeof(float));
	ofile.close();
	
	delete[] vex_coe;	
}
//~:

