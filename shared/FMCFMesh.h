#ifndef _FMCFMESH_H
#define _FMCFMESH_H

#pragma once
#include "zData.h"
#include <string>
using namespace std;

struct meshInfo {
	unsigned int numPolygons, numVertices, numFaceVertices, numUVs;
	int skip_interreflection, skip_scattering;
};

class FMCFMesh
{
public:
	FMCFMesh(void);
	~FMCFMesh(void);
	int load(const char* _filename);
	void release();
	char isNull() const {return is_null;}
	void draw();
	void drawUV();
	void drawTangentSpace(const float size);

	unsigned int getNumVertex()  const {return m_num_vertices;}
	unsigned int getNumUV()  const {return m_num_uvs;}
	unsigned int getNumFace()  const {return npolys;}
	unsigned int getNumFaceVertex()  const {return m_num_face_vertices;}
	
	int getFaceCount(int id) const {return faceCount[id];}
	int getVertexId(int id) const {return vertices[id];}
	int getUVId(int id) const {return uvs[id];}
	double getS(int id) const {return coord_s[id]; }
	double getT(int id) const {return coord_t[id]; }
	
	void getVertAndNormal(XYZ& p, XYZ& n, unsigned int id) const
	{
		p = cvs[id];
		n = normals[id];
	}
	
	void getNormal(XYZ& n, unsigned int id) const
	{
		n = normals[id];
	}
	
	void getTangent(XYZ& t, unsigned int id) const
	{
		t = tangents[id];
	}

	void getVertex(XYZ& p, unsigned int id) const
	{
		p = cvs[id];
	}
	
	void getMovedVertex(XYZ& p, const float d, unsigned int id) const
	{
		p = disp_cvs[id];
		p +=normals[id]*d;
	}
	
	void setDrawColor(const XYZ& col, unsigned int id)
	{
		draw_colors[id] = col;
	}
	
	int toSkipInterreflection() const { return i_skip_interreflection; }
	int toSkipScattering() const { return i_skip_scattering; }
	
	string filename;
	
	int save(const int n_vertex, 
			const int n_face_vertex, 
			const int n_face, 
			const int n_uv, 
			const int i_skip_intr,
			const int i_skip_scat,
			const int* face_count, 
			const int* vertex_id, 
			const int* uv_id,
			const XYZ* pcv,
			const XYZ* pdcv,
			const XYZ* pnor,
			const XYZ* ptang,
			const XYZ* pcol,
			const double* pu,
			const double* pv,
			const char* _filename);
	
	void getCenterAndSize(XYZ& center, float& size) const;
	
private:
	unsigned int m_num_vertices, m_num_face_vertices, m_num_uvs, npolys;
	int* faceCount;
	int* vertices;
	int* uvs;
	XYZ* cvs;
	XYZ* disp_cvs;
	XYZ* normals;
	XYZ* tangents;
	XYZ* colors;
	XYZ* draw_colors;
	double* coord_s;
	double* coord_t;
	char is_null;
	int i_skip_interreflection, i_skip_scattering;
};
#endif