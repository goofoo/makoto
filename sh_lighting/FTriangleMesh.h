#ifndef _F_TRIANGLEMESH_H
#define _F_TRIANGLEMESH_H

#include "../shared/zData.h"
#include <string>
#include <iostream>
using namespace std;
#include "../vertexCache/triangle_common.h"
#include "FQSplat.h"

class FTriangleMesh
{
public:
	FTriangleMesh(void);
	~FTriangleMesh(void);
	
	int load(const char* _filename);
	void release();
	void draw() const;
	void draw(XYZ* hdr) const;
	void drawCloud(int n_cell, XYZ* hdr) const;
	void dice(int n_cell, FQSPLAT* cloud) const;
	void diceCoe(int n_cell, FQSPLAT* cloud) const;
	int checkExistingRTFile() const;
	int readExistingRTFile(CoeRec* data) const;
	void saveRTFile(const XYZ* coe) const;
	int getVertexCount() const { return m_n_vertex; }
	
	void getVertex( XYZ& vec, int id ) const
	{
		vec = m_data[id].pos + m_data[id].nor*0.1f;
	}
	
	void getNormal( XYZ& vec, int id ) const
	{
		vec = m_data[id].nor;
	}
	
	void getTangent( XYZ& vec, int id ) const
	{
		vec = m_data[id].tang;
	}
	
	float getArea() const { return m_area; }
	
	void getCenterAndSize(XYZ& center, float& size) const;

	string filename;

private:
	int m_n_triangle, m_n_vertex;
	float m_area, m_size;
	int* m_vertexId;
	triangle_mesh_rec* m_data;
	XYZ m_center;
};
#endif