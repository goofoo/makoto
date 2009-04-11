#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "zData.h"
using namespace std;

typedef struct meshCacheInfo {
	unsigned int numPolygons, numVertices, numFaceVertices;
} meshCacheInfo;

class CrmcMesh
{
public:
	CrmcMesh(void);
	~CrmcMesh(void);
	int load(const char* filename);
	
	int export_(
			   const char* filename, 
			   unsigned int nface, 
			   unsigned int nvert, 
			   unsigned int nfacevert, 
			   unsigned int* nloops, 
			   unsigned int* vertices, 
			   float* cvs, 
			   float* normals, 
			   float* coord_s, 
			   float* coord_t
			   );
	
	unsigned int getVerticesCount() { return m_num_vertices; }
	unsigned int getNumberOfFaces() { return npolys; }
	unsigned int getNumberOfFaceVertices() { return m_num_face_vertices; }
	XYZ& getVertexById( int id ) { return cvs[id]; }
	XYZ& getNormalById( int id ) { return normals[id]; }
	int getFaceCountById(int id) {return nvertices[id];}
	int getFaceConnectionById(int id) {return vertices[id];}
	float getSById(int id) {return coord_s[id];}
	float getTById(int id) {return coord_t[id];}
	
	const int* getNLoops() {return nloops;}
	const int* getNVertices() {return nvertices;}
	const int* getVertices() {return vertices;}
	const XYZ* getCvs() {return cvs;}
	const XYZ* getNormals() {return normals;}
	const float* getCoordS() {return coord_s;}
	const float* getCoordT() {return coord_t;}
	
	void release();
	
private:
	unsigned int m_num_vertices, m_num_face_vertices;
	int npolys;
	int* nloops;
	int* nvertices;
	int* vertices;
	XYZ* cvs;
	XYZ* normals;
	float* coord_s;
	float* coord_t;
	int m_is_null;
};
