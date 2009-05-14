/*
 *  FXMLVaneMap.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "FXMLVaneMap.h"
#include "../shared/gBase.h"
#include "FDice.h"

FXMLVaneMap::FXMLVaneMap()
:m_isNull(1)
{

}

FXMLVaneMap::~FXMLVaneMap()
{
	free();
}

void FXMLVaneMap::beginMap(const char* name)
{
	doc.begin();
	doc.elementBegin("vane_map");
	doc.addAttribute("name", name);
}

void FXMLVaneMap::endMap(const char* path)
{
	doc.elementEnd();
	doc.end(path);
}

void FXMLVaneMap::dataBegin(const char* path)
{
	doc.addAttribute("data_path", path);
	pos_d =0;
	data_file.open(path, ios::out | ios::binary);
}

void FXMLVaneMap::dataEnd()
{
	
	data_file.close();
}

void FXMLVaneMap::vaneBegin(int n)
{
	doc.elementBegin("vane");
	doc.addAttribute("num_cv", n);
}

void FXMLVaneMap::vaneEnd()
{
	doc.elementEnd();
}

void FXMLVaneMap::addP(int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("P");
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	data_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLVaneMap::addVectorArray(const char* name, int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin(name);
	doc.addAttribute("loc", pos_d);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_d += offset;
	
	data_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLVaneMap::addBBox(float x0, float y0, float z0, float x1, float y1, float z1)
{
	doc.elementBegin("bbox");
	doc.addAttribute("low", x0, y0, z0);
	doc.addAttribute("high", x1, y1, z1);
	doc.elementEnd();
}

FXMLVaneMap::FXMLVaneMap(const char* filename)
:m_isNull(1)
{
	if(load(filename) == 1)	m_isNull = 0;
}

void FXMLVaneMap::free()
{
	if(m_isNull==0)
	{
		//if(m_vertices) delete[] m_vertices;
		//if(m_cvs) delete[] m_cvs;
		//if(m_normals) delete[] m_normals;
		//if(m_binormals) delete[] m_binormals;
		//if(m_directions) delete[] m_directions;
		//if(m_scales) delete[] m_scales;
		//if(m_ups) delete[] m_ups;
		//if(m_winds) delete[] m_winds;
		m_isNull = 1;
	}
}

int FXMLVaneMap::load(const char* filename)
{
	free();
	if(doc.load(filename) != 1) return 0;

	ffin.open(doc.getAttribByName("data_path"), ios::in | ios::binary);
	
	//printf(doc.getAttribByName("data_path"));
	
	doc.setChildren();
	
	return 1;
}

void FXMLVaneMap::finish()
{
	doc.free();
	ffin.close();
}

char FXMLVaneMap::getVane(int& num)
{
	while(doc.isLastNode()!=1)
	{
		if(doc.checkNodeName("vane")==1)
		{
			num = doc.getIntAttribByName("num_cv");			
			
			return 1;
		}
		else doc.nextNode();
	}
	return 0;
}

void FXMLVaneMap::next()
{
	doc.nextNode();
}

void FXMLVaneMap::getData(XYZ* p, XYZ* up, XYZ* down, XYZ* normal)
{
	int pos, size;
			doc.getChildByName("P");
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)p, size);
			
			doc.setParent();
			
			doc.getChildByName("up");
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)up, size);
			
			doc.setParent();
			
			doc.getChildByName("down");
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)down, size);
			
			doc.setParent();
			
			doc.getChildByName("surface_n");
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)normal, size);
			
			doc.setParent();

}
/*
void FXMLVaneMap::staticBeginNoWrite(const char* path, int n_tri)
{
	doc.elementBegin("static");
	doc.addAttribute("path", path);
	doc.addAttribute("num_Vane", n_tri);
}

void FXMLVaneMap::staticEndNoWrite()
{
	doc.elementEnd();
}

void FXMLVaneMap::addVertexId(int val, const int* data)
{
	int offset = (int)sizeof(int)*val;
	doc.elementBegin("connection");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", val);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, val*sizeof(int));
}

void FXMLVaneMap::addVertexId(int val)
{
	int offset = (int)sizeof(int)*val;
	doc.elementBegin("connection");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", val);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLVaneMap::addPref(int num, const XYZ* data)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("Pref");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, sizeof(XYZ)*num);
}

void FXMLVaneMap::addPref(int num)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("Pref");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLVaneMap::addFaceArea(int num, const float* data)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("face_area");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
	
	static_file.write((char*)data, sizeof(float)*num);
}

void FXMLVaneMap::addFaceArea(int num)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("face_area");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLVaneMap::addStaticFloatArray(const char* name, int num, const float* data)
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

void FXMLVaneMap::addStaticFloatArray(const char* name, int num)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin(name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLVaneMap::dynamicBegin(const char* path)
{
	doc.elementBegin("dynamic");
	doc.addAttribute("path", path);
	
	dynamic_file.open(path, ios::out | ios::binary);
}

void FXMLVaneMap::dynamicEnd()
{
	doc.elementEnd();
	dynamic_file.close();
}

void FXMLVaneMap::addP(int num, const XYZ* data)
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



void FXMLVaneMap::addN(int num, const XYZ* data)
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

void FXMLVaneMap::addTangent(int num, const XYZ* data)
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



void FXMLVaneMap::free()
{
	if(m_isNull==0)
	{
		if(m_vertices) delete[] m_vertices;
		if(m_cvs) delete[] m_cvs;
		if(m_normals) delete[] m_normals;
		if(m_binormals) delete[] m_binormals;
		if(m_directions) delete[] m_directions;
		if(m_scales) delete[] m_scales;
		//if(m_ups) delete[] m_ups;
		if(m_winds) delete[] m_winds;
		m_isNull = 1;
	}
}

int FXMLVaneMap::load(const char* filename)
{
	free();
	m_xml_name = string(filename);
	if(doc.load(filename) != 1) return 0;
	
	ifstream ffin;
	int pos, size;
	
	m_meshName = string(doc.getAttribByName("name"));

		doc.getChildByName("static");
		m_staticName = string(doc.getAttribByName("path"));
		m_numFace = doc.getIntAttribByName("num_Vane");
		
		ffin.open(doc.getAttribByName("path"), ios::in | ios::binary);

			doc.getChildByName("connection");
			m_numFaceVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_vertices = new int[m_numFaceVertex];
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_vertices, size);

			doc.setParent();
			
			doc.getChildByName("Pref");
			m_pref = new XYZ[doc.getIntAttribByName("count")];
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_pref, size);
			
			doc.setParent();
			
			doc.getChildByName("scale");
			m_scales = new float[doc.getIntAttribByName("count")];
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_scales, size);

			for(int k=0; k<doc.getIntAttribByName("count");k++) printf("%f", m_scales[k]);
			
			doc.setParent();
			ffin.close();
		doc.setParent();
		doc.getChildByName("dynamic");
		m_dynamicName = string(doc.getAttribByName("path"));
		
		ffin.open(doc.getAttribByName("path"), ios::in | ios::binary);
		
			doc.getChildByName("P");
			m_numVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_cvs = new XYZ[m_numVertex];
			
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_cvs, size);
		
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
		
			m_directions = new XYZ[m_numVertex];
			
			if(doc.getChildByName("direction") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_directions, size);
			}
		doc.setParent();
		
		m_binormals = new XYZ[m_numVertex];
			
			if(doc.getChildByName("binormal") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_binormals, size);
			}
		doc.setParent();
		
		m_winds = new XYZ[m_numVertex];
			
			if(doc.getChildByName("wind") != 0)
			{
				pos = doc.getIntAttribByName("loc");
				size = doc.getIntAttribByName("size");
				ffin.seekg( pos, ios::beg );
				ffin.read((char*)m_winds, size);
			}
		doc.setParent();
			
		ffin.close();
		
			if(doc.getChildByName("bbox") != 0)
			{
				doc.getFloat3AttribByName("low", m_bbox0.x, m_bbox0.y, m_bbox0.z);
				doc.getFloat3AttribByName("high", m_bbox1.x, m_bbox1.y, m_bbox1.z);
			}
			else
			{
				m_bbox0.x = m_bbox0.y = m_bbox0.z = 0.0f;
				m_bbox1.x = m_bbox1.y = m_bbox1.z = 1.0f;
			}
			
		doc.setParent();
		
	doc.free();
	m_isNull = 0;

	return 1;
}

void FXMLVaneMap::draw() const
{
	glBegin(GL_VaneS);
	for(int i=0; i<m_numFaceVertex; i++)
	{
			int vertId = m_vertices[i];
			glVertex3f(m_cvs[vertId].x, m_cvs[vertId].y, m_cvs[vertId].z);
	}
	glEnd();
}

void FXMLVaneMap::drawTangent() const
{
	XYZ vt;
	glBegin(GL_LINES);
	for(int i=0; i<m_numFaceVertex; i++)
	{
			int vertId = m_vertices[i];
			vt = m_cvs[vertId];
			glVertex3f(vt.x, vt.y, vt.z);
			vt += m_directions[vertId];
			glVertex3f(vt.x, vt.y, vt.z);//printf("%f %f %f", vt.x, vt.y, vt.z);
	}
	glEnd();
}

void FXMLVaneMap::dice(int n_cell, DFeather* res) const
{
	FDice fd;
	int a,b,c;
	
	float max_scale = 0;
	float sum_area = 0;
	for(int i=0; i<m_numFace; i++)
	{
		a = m_vertices[i*3];
		b = m_vertices[i*3+1];
		c = m_vertices[i*3+2];
		
		sum_area += FDice::calculate_area(m_pref[a], m_pref[b], m_pref[c]);
		
		if(m_scales[i] > max_scale) max_scale = m_scales[i];
	}
	
	float mini = sum_area/(n_cell);
	
	int pool_size = n_cell*7;printf("pool %i ", pool_size);
	
	DHair* samp = new DHair[pool_size];
	int n_samp = 0;
	
	//printf("a area %f", mini);
	
	DHair ha, hb, hc;
	float dens;
	
	for(int i=0; i<m_numFace; i++)
	{
		a = m_vertices[i*3];
		b = m_vertices[i*3+1];
		c = m_vertices[i*3+2];
		
		fd.create(m_pref[a], m_pref[b], m_pref[c]);
		
		ha.root = m_cvs[a];
		ha.normal = m_normals[a];
		ha.binormal = m_binormals[a];
		ha.tangent = m_directions[a];
		ha.scale = m_scales[a];
		//ha.up = m_ups[a];
		ha.wind = m_winds[a];
		
		hb.root = m_cvs[b];
		hb.normal = m_normals[b];
		hb.binormal = m_binormals[b];
		hb.tangent = m_directions[b];
		hb.scale = m_scales[b];
		//hb.up = m_ups[b];
		hb.wind = m_winds[b];
		
		hc.root = m_cvs[c];
		hc.normal = m_normals[c];
		hc.binormal = m_binormals[c];
		hc.tangent = m_directions[c];
		hc.scale = m_scales[c];
		//hc.up = m_ups[c];
		hc.wind = m_winds[c];
		
		fd.setData(ha, hb, hc);
		
		dens = (ha.scale + hb.scale + hc.scale)/3/max_scale;
		if(dens < 0.17) dens = 0.17;
		
		fd.rasterize(mini*dens, samp, n_samp);
	}
	printf("n smap %i ", n_samp);
	
	res->init(samp, n_samp);
}*/
//:~