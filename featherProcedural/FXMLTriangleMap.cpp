/*
 *  FXMLTriangleMap.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "FXMLTriangleMap.h"
#include "../shared/zGlobal.h"
#include "FDice.h"

FXMLTriangleMap::FXMLTriangleMap():
pos_s(0), 
pos_d(0),
m_isNull(1)
{

}

FXMLTriangleMap::FXMLTriangleMap(const char* filename):
pos_s(0), 
pos_d(0),
m_vertices(0), 
m_cvs(0),
m_normals(0),m_binormals(0),//m_ups(0),
m_winds(0),
m_curl(0),
m_directions(0),
m_scales(0),
m_density(0),
m_numFace(0),
m_numFaceVertex(0),
m_numVertex(0),
m_rootS(0),
m_rootT(0),
m_isNull(1)
{
	if(load(filename) == 1)	m_isNull = 0;
}

FXMLTriangleMap::~FXMLTriangleMap()
{
	free();
}

void FXMLTriangleMap::beginMap(const char* name)
{
	doc.begin();
	doc.elementBegin("triangle_map");
	doc.addAttribute("name", name);
}

void FXMLTriangleMap::endMap(const char* path)
{
	doc.elementEnd();
	doc.end(path);
}

void FXMLTriangleMap::staticBegin(const char* path, int n_tri)
{
	doc.elementBegin("static");
	doc.addAttribute("path", path);
	doc.addAttribute("num_triangle", n_tri);
	
	static_file.open(path, ios::out | ios::binary);
}

void FXMLTriangleMap::staticEnd()
{
	doc.elementEnd();
	static_file.close();
}

void FXMLTriangleMap::staticBeginNoWrite(const char* path, int n_tri)
{
	doc.elementBegin("static");
	doc.addAttribute("path", path);
	doc.addAttribute("num_triangle", n_tri);
}

void FXMLTriangleMap::staticEndNoWrite()
{
	doc.elementEnd();
}

void FXMLTriangleMap::addVertexId(int val, const int* data)
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

void FXMLTriangleMap::addVertexId(int val)
{
	int offset = (int)sizeof(int)*val;
	doc.elementBegin("connection");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", val);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLTriangleMap::addPref(int num, const XYZ* data)
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

void FXMLTriangleMap::addPref(int num)
{
	int offset = (int)sizeof(XYZ)*num;
	doc.elementBegin("Pref");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLTriangleMap::addFaceArea(int num, const float* data)
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

void FXMLTriangleMap::addFaceArea(int num)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin("face_area");
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLTriangleMap::addStaticFloatArray(const char* name, int num, const float* data)
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

void FXMLTriangleMap::addStaticFloatArray(const char* name, int num)
{
	int offset = (int)sizeof(float)*num;
	doc.elementBegin(name);
	doc.addAttribute("loc", pos_s);
	doc.addAttribute("size", offset);
	doc.addAttribute("count", num);
	doc.elementEnd();
	
	pos_s += offset;
}

void FXMLTriangleMap::dynamicBegin(const char* path)
{
	doc.elementBegin("dynamic");
	doc.addAttribute("path", path);
	
	dynamic_file.open(path, ios::out | ios::binary);
}

void FXMLTriangleMap::dynamicEnd()
{
	doc.elementEnd();
	dynamic_file.close();
}

void FXMLTriangleMap::addP(int num, const XYZ* data)
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

void FXMLTriangleMap::addBBox(float x0, float y0, float z0, float x1, float y1, float z1)
{
	doc.elementBegin("bbox");
	doc.addAttribute("low", x0, y0, z0);
	doc.addAttribute("high", x1, y1, z1);
	doc.elementEnd();
}

void FXMLTriangleMap::addN(int num, const XYZ* data)
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

void FXMLTriangleMap::addTangent(int num, const XYZ* data)
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

void FXMLTriangleMap::addDynamicVectorArray(const char* name, int num, const XYZ* data)
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

void FXMLTriangleMap::free()
{
	if(m_isNull==0)
	{
		if(m_vertices) delete[] m_vertices;
		if(m_cvs) delete[] m_cvs;
		if(m_normals) delete[] m_normals;
		if(m_binormals) delete[] m_binormals;
		if(m_directions) delete[] m_directions;
		if(m_scales) delete[] m_scales;
		if(m_curl) delete[] m_curl;
		if(m_winds) delete[] m_winds;
		if(m_density) delete[] m_density;
		if(m_rootS) delete[] m_rootS;
		if(m_rootT) delete[] m_rootT;
		m_isNull = 1;
	}
}

int FXMLTriangleMap::load(const char* filename)
{
	free();
	m_xml_name = string(filename);
	if(doc.load(filename) != 1) return 0;
	
	ifstream ffin;
	int pos, size;
	
	m_meshName = string(doc.getAttribByName("name"));

		doc.getChildByName("static");
		m_staticName = string(doc.getAttribByName("path"));
		m_numFace = doc.getIntAttribByName("num_triangle");
		
		ffin.open(doc.getAttribByName("path"), ios::in | ios::binary);

			doc.getChildByName("connection");
			m_numFaceVertex = doc.getIntAttribByName("count");
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			
			m_vertices = new int[m_numFaceVertex];
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_vertices, size);

			doc.setParent();
			
			doc.getChildByName("root_s");
			m_rootS = new float[m_numFaceVertex];
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_rootS, size);
			
			doc.setParent();
			
			doc.getChildByName("root_t");
			m_rootT = new float[m_numFaceVertex];
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_rootT, size);
			
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

			//for(int k=0; k<doc.getIntAttribByName("count");k++) printf("%f", m_scales[k]);
			
			doc.setParent();
			
			doc.getChildByName("curl");
			m_curl = new float[doc.getIntAttribByName("count")];
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_curl, size);
			
			doc.setParent();
			
			doc.getChildByName("density");
			m_density = new float[doc.getIntAttribByName("count")];
			
			pos = doc.getIntAttribByName("loc");
			size = doc.getIntAttribByName("size");
			ffin.seekg( pos, ios::beg );
			ffin.read((char*)m_density, size);
			
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
/*
void FXMLTriangleMap::draw() const
{
	glBegin(GL_TRIANGLES);
	for(int i=0; i<m_numFaceVertex; i++)
	{
			int vertId = m_vertices[i];
			glVertex3f(m_cvs[vertId].x, m_cvs[vertId].y, m_cvs[vertId].z);
	}
	glEnd();
}

void FXMLTriangleMap::drawTangent() const
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
*/
void FXMLTriangleMap::dice(float rate, DFeather* res) const
{
	CoeRec* vex_coe = new CoeRec[m_numVertex];
	
	string rtName = m_xml_name;
	zGlobal::changeFilenameExtension(rtName, "prt");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size >= m_numVertex*48*sizeof(float))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)vex_coe, m_numVertex*48*sizeof(float));
			ifile.close();
		}
	}
	
	FDice fd;
	int a,b,c;
	
	float a_area, mini;

	int pool_size = m_numFace*rate*4;//printf("pool size %i ", pool_size);
	
	DHair* samp = new DHair[pool_size];
	CoeRec* res_coe = new CoeRec[pool_size];
	int n_samp = 0;
	
	//printf("a area %f", mini);
	
	DHair ha, hb, hc;
	
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
		ha.curl = m_curl[a];
		ha.wind = m_winds[a];
		ha.density = m_density[a];
		ha.s = m_rootS[i*3];
		ha.t = m_rootT[i*3];
		ha.id = a;
		
		hb.root = m_cvs[b];
		hb.normal = m_normals[b];
		hb.binormal = m_binormals[b];
		hb.tangent = m_directions[b];
		hb.scale = m_scales[b];
		hb.curl = m_curl[b];
		hb.wind = m_winds[b];
		hb.density = m_density[b];
		hb.s = m_rootS[i*3+1];
		hb.t = m_rootT[i*3+1];
		hb.id = b;
		
		hc.root = m_cvs[c];
		hc.normal = m_normals[c];
		hc.binormal = m_binormals[c];
		hc.tangent = m_directions[c];
		hc.scale = m_scales[c];
		hc.curl = m_curl[c];
		hc.wind = m_winds[c];
		hc.density = m_density[c];
		hc.s = m_rootS[i*3+2];
		hc.t = m_rootT[i*3+2];
		hc.id = c;
		
		fd.setData(ha, hb, hc);
				
		a_area = FDice::calculate_area(m_pref[a], m_pref[b], m_pref[c]);
		
		mini = sqrt(a_area)/rate;
		
		fd.rasterize(mini, vex_coe, samp, res_coe, n_samp);//printf("smap %i ", n_samp);
	}
	
	res->init(samp, res_coe, n_samp);
	
	delete[] samp;
	delete[] res_coe;
	delete[] vex_coe;
}

void FXMLTriangleMap::diceNoRT(float rate, DFeather* res) const
{
	FDice fd;
	int a,b,c;
	
	float a_area, mini;

	int pool_size = m_numFace*rate*4;//printf("pool size %i ", pool_size);
	
	DHair* samp = new DHair[pool_size];
	int n_samp = 0;
	
	//printf("a area %f", mini);
	
	DHair ha, hb, hc;
	
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
		ha.curl = m_curl[a];
		ha.wind = m_winds[a];
		ha.density = m_density[a];
		ha.s = m_rootS[i*3];
		ha.t = m_rootT[i*3];
		ha.id = a;
		
		hb.root = m_cvs[b];
		hb.normal = m_normals[b];
		hb.binormal = m_binormals[b];
		hb.tangent = m_directions[b];
		hb.scale = m_scales[b];
		hb.curl = m_curl[b];
		hb.wind = m_winds[b];
		hb.density = m_density[b];
		hb.s = m_rootS[i*3+1];
		hb.t = m_rootT[i*3+1];
		hb.id = b;
		
		hc.root = m_cvs[c];
		hc.normal = m_normals[c];
		hc.binormal = m_binormals[c];
		hc.tangent = m_directions[c];
		hc.scale = m_scales[c];
		hc.curl = m_curl[c];
		hc.wind = m_winds[c];
		hc.density = m_density[c];
		hc.s = m_rootS[i*3+2];
		hc.t = m_rootT[i*3+2];
		hc.id = c;
		
		fd.setData(ha, hb, hc);
				
		a_area = FDice::calculate_area(m_pref[a], m_pref[b], m_pref[c]);
		
		mini = sqrt(a_area)/rate;
		
		fd.rasterize(mini, samp, n_samp);
	}
	
	res->init(samp, n_samp);
	delete[] samp;
}
//:~