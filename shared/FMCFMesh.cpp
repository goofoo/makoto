#include "FMCFMesh.h"
#include "gBase.h"
#include <iostream>
using namespace std;

FMCFMesh::FMCFMesh(void):is_null(1), 
faceCount(0), 
vertices(0),
uvs(0),
cvs(0),
coord_s(0),
coord_t(0),
//normals(0),
//tangents(0),
//colors(0),
 i_skip_interreflection(0), i_skip_scattering(0)
{
}

FMCFMesh::~FMCFMesh(void)
{
	release();
}

void FMCFMesh::release()
{
	if(!is_null)
	{
		if(vertices) delete[] vertices;
		if(cvs) delete[] cvs;
		//if(disp_cvs) delete[] disp_cvs;
		//if(normals) delete[] normals;
		if(coord_s) delete[] coord_s;
		if(coord_t) delete[] coord_t;
		if(faceCount) delete[] faceCount;
		if(uvs) delete[] uvs;
			//if(colors) delete[] colors;
				//if(tangents) delete[] tangents;
		is_null = 1;
	}
}

#include "zString.h"

int FMCFMesh::load(const char* _filename)
{
	FILE *fp = fopen( _filename, "rb");

	if( fp==NULL ) 
	{
		//char log[128];
		//sprintf( log, "Failed to open file %s \n", filename );
		//printf(log);
		return 0;
	}
	
	release();
	
	int stat;
	meshInfo mesh_info;
// read mesh info
	stat = fread(&mesh_info,sizeof(struct meshInfo),1,fp);
	
	if(stat!=1) return 0;

	m_num_face_vertices = mesh_info.numFaceVertices;
	m_num_vertices = mesh_info.numVertices;
	npolys = mesh_info.numPolygons;
	m_num_uvs = mesh_info.numUVs;
	i_skip_interreflection = mesh_info.skip_interreflection;
	i_skip_scattering = mesh_info.skip_scattering;
	
	//cout<<"load "<<m_num_vertices<<" vertex from "<<_filename<<endl;
	//cout<<" N Face:"<<npolys<<endl;
	//cout<<" N Facevertex:"<<m_num_face_vertices<<endl;
	//cout<<" N UV:"<<m_num_uvs<<endl;
	
	faceCount = new int[npolys];
	fread(faceCount, npolys*sizeof(int),1,fp);
	
	vertices = new int[m_num_face_vertices];
	fread(vertices, m_num_face_vertices*sizeof(int),1,fp);
	
	uvs = new int[m_num_face_vertices];
	fread(uvs, m_num_face_vertices*sizeof(int),1,fp);
	
	cvs = new XYZ[m_num_vertices];
	fread(cvs, m_num_vertices*sizeof(XYZ),1,fp);

	coord_s = new double[m_num_uvs];
	fread(coord_s, m_num_uvs*sizeof(double),1,fp);
	
	coord_t = new double[m_num_uvs];
	fread(coord_t, m_num_uvs*sizeof(double),1,fp);

	//normals = new XYZ[m_num_vertices];
	//fread(normals, m_num_vertices*sizeof(XYZ),1,fp);
	//
	//colors = new XYZ[m_num_vertices];
	//fread(colors, m_num_vertices*sizeof(XYZ),1,fp);
	//
	//disp_cvs = new XYZ[m_num_vertices];
	//fread(disp_cvs, m_num_vertices*sizeof(XYZ),1,fp);
	//
	//tangents = new XYZ[m_num_vertices];
	//fread(tangents, m_num_vertices*sizeof(XYZ),1,fp);
	//
	//draw_colors = new XYZ[m_num_vertices];
	//for(unsigned int i = 0; i < m_num_vertices; i++) draw_colors[i] = XYZ(0.5f) + normals[i]*0.5;
//
	is_null = 0;
	
	fclose(fp);	
	
	filename = string(_filename);
	
	return 1;
}

void FMCFMesh::draw()
{
	int acc=0;
	for(unsigned int i=0; i<npolys; i++)
	{
		glBegin(GL_POLYGON);
		for(int j=0; j<faceCount[i]; j++)
		{
			int vertId = vertices[acc];
			//glColor3d(draw_colors[vertId].x, draw_colors[vertId].y, draw_colors[vertId].z);
			glVertex3f(cvs[vertId].x, cvs[vertId].y, cvs[vertId].z);
			acc++;
		}
		glEnd();
	}
}

void FMCFMesh::drawUV()
{
	int acc=0;
	for(unsigned int i=0; i<npolys; i++)
	{
		glBegin(GL_LINES);
		for(int j=0; j<faceCount[i]; j++)
		{
			int vertId = vertices[acc+j];
			//glColor3d(draw_colors[vertId].x, draw_colors[vertId].y, draw_colors[vertId].z);
			int uvId = uvs[acc+j];
			glVertex3d(coord_s[uvId], coord_t[uvId], 0.f);
			
			int j1 = j+1;
			if(j1==faceCount[i]) j1 = 0;
			
			vertId = vertices[acc+j1];
			//glColor3d(draw_colors[vertId].x, draw_colors[vertId].y, draw_colors[vertId].z);
			uvId = uvs[acc+j1];
			glVertex3d(coord_s[uvId], coord_t[uvId], 0.f);
			
		}
		glEnd();
		acc+=faceCount[i];
	}
}

void FMCFMesh::drawTangentSpace(const float size)
{
	//int acc=0;
	//for(unsigned int i=0; i<npolys; i++)
	//{
	//	glBegin(GL_LINES);
	//	for(int j=0; j<faceCount[i]; j++)
	//	{
	//		int vertId = vertices[acc];
	//		glColor3d(0,1,0);
	//		glVertex3f(cvs[vertId].x, cvs[vertId].y, cvs[vertId].z);
	//		
	//		XYZ up = cvs[vertId] + normals[vertId]*size;
	//		glVertex3f(up.x, up.y, up.z);
	//		
	//		glColor3d(1,0,0);
	//		glVertex3f(cvs[vertId].x, cvs[vertId].y, cvs[vertId].z);
	//		XYZ side = cvs[vertId] + tangents[vertId]*size;
	//		glVertex3f(side.x, side.y, side.z);
	//		
	//		acc++;
	//	}
	//	glEnd();
	//}
}

#include <fstream>
int FMCFMesh::save(const int n_vertex, 
			const int n_face_vertex, 
			const int n_face, 
			const int n_uv, 
			const int i_skip_intr,
			const int i_skip_scat,
			const int* face_count, 
			const int* vertex_id, 
			const int* uv_id,
			const XYZ* pcv,
			//const XYZ* pdcv,
			//const XYZ* pnor,
			//const XYZ* ptang,
			//const XYZ* pcol,
			const double* pu,
			const double* pv,
			const char* _filename)
{
	ofstream ffile;
	ffile.open(_filename, ios::out | ios::binary);
	if(!ffile.is_open()) return 0;
		
	struct meshInfo mesh;
	mesh.numPolygons = n_face;
	mesh.numVertices = n_vertex;
	mesh.numFaceVertices = n_face_vertex;
	mesh.numUVs = n_uv;
	mesh.skip_interreflection = i_skip_intr;
	mesh.skip_scattering = i_skip_scat;
	
	ffile.write((char*)&mesh, sizeof(struct meshInfo));
	ffile.write((char*)face_count, n_face*sizeof(int));
	ffile.write((char*)vertex_id, n_face_vertex*sizeof(int));
	ffile.write((char*)uv_id, n_face_vertex*sizeof(int));
	ffile.write((char*)pcv, n_vertex*sizeof(XYZ));
	ffile.write((char*)pu, n_uv*sizeof(double));
	ffile.write((char*)pv, n_uv*sizeof(double));
	//ffile.write((char*)pnor, n_vertex*sizeof(XYZ));
	//ffile.write((char*)pcol, n_vertex*sizeof(XYZ));
	//ffile.write((char*)pdcv, n_vertex*sizeof(XYZ));
	//ffile.write((char*)ptang, n_vertex*sizeof(XYZ));

	ffile.close();
	return 1;
}

void FMCFMesh::getCenterAndSize(XYZ& center, float& size) const
{
	XYZ cen(0.f);
	float xmin, ymin, zmin, xmax, ymax, zmax;
	xmin = ymin = zmin = 10000000.f;
	xmax = ymax = zmax = -10000000.f;
	for(unsigned int i=0; i<m_num_vertices; i++)
	{
		cen += cvs[i];
		if(cvs[i].x < xmin) xmin = cvs[i].x;
		if(cvs[i].x > xmax) xmax = cvs[i].x;
			
		if(cvs[i].y < ymin) ymin = cvs[i].y;
		if(cvs[i].y > ymax) ymax = cvs[i].y;
			
		if(cvs[i].z < zmin) zmin = cvs[i].z;
		if(cvs[i].z > zmax) zmax = cvs[i].z;
	}
	
	cen /=(float)m_num_vertices; 
	
	center = cen;
	
	float dx = xmax - xmin;
	float dy = ymax - ymin;
	float dz = zmax - zmin;
	
	if(dx > dy && dx >dz) size = dx;
		else if(dy > dx && dy >dz) size = dy;
			else size = dz;
}

void FMCFMesh::facevaryingS(float* res) const
{
	//for(int i=0; i<m_num_face_vertices; i++) res[i] = coord_s[uvs[i]];
	int acc = 0;
	for(int i=0; i<npolys; i++)
	{
		int cface = faceCount[i];
		for(int j=0; j<cface; j++)
		{
			res[acc+j] = coord_s[uvs[acc+cface-1-j]];
		}
		acc += cface;
	}
}

void FMCFMesh::facevaryingT(float* res) const
{
	//for(int i=0; i<m_num_face_vertices; i++) res[i] = 1.0 - coord_t[uvs[i]];
	int acc = 0;
	for(int i=0; i<npolys; i++)
	{
		int cface = faceCount[i];
		for(int j=0; j<cface; j++)
		{
			res[acc+j] = 1.0 - coord_t[uvs[acc+cface-1-j]];
		}
		acc += cface;
	}
}

void FMCFMesh::rmanVertices(int* res) const
{
	int acc = 0;
	for(int i=0; i<npolys; i++)
	{
		int cface = faceCount[i];
		for(int j=0; j<cface; j++)
		{
			res[acc+j] = vertices[acc+cface-1-j];
		}
		acc += cface;
	}
}
//:~