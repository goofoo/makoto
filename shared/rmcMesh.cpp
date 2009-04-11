#include "rmcmesh.h"

CrmcMesh::CrmcMesh(void):m_is_null(1)
{
}

CrmcMesh::~CrmcMesh(void)
{
	release();
}

void CrmcMesh::release()
{
	if(m_is_null != 1)
	{
		if(nloops != NULL) delete[] nloops;
		if(nvertices != NULL) delete[] nvertices;
		if(vertices != NULL) delete[] vertices;
		if(cvs != NULL) delete[] cvs;
		if(normals != NULL) delete[] normals;
		if(coord_s != NULL) delete[] coord_s;
		if(coord_t != NULL) delete[] coord_t;
		m_is_null = 1;
	}
}

int CrmcMesh::load(const char* filename)
{
	FILE *fp = fopen( filename, "rb");

	if( fp==NULL ) 
	{
		//char log[128];
		//sprintf( log, "Failed to open file %s \n", filename );
		//printf(log);
		return 0;
	}
	
	release();
	
	int stat;
	meshCacheInfo* mesh_info = new meshCacheInfo;
	// read mesh info
	stat = fread(mesh_info,sizeof(struct meshCacheInfo),1,fp);
	
	if(stat!=1) return 0;
	//char log[128];
	//sprintf(log,"filename: %s\nnpolys: %i\nvertices count: %i\nnvertices: %i\n", fileName, m_mesh_info->numPolygons, m_mesh_info->numVertices, m_mesh_info->numFaceVertices);
	//printf(log);
	m_num_face_vertices = mesh_info->numFaceVertices;
	m_num_vertices = mesh_info->numVertices;
	npolys = mesh_info->numPolygons;
	
	delete mesh_info;
	
	nloops = new int[npolys];
	for(unsigned int i=0; i<npolys; i++) nloops[i] =1;

	nvertices = new int[npolys];
	fread(nvertices, npolys*sizeof(unsigned int),1,fp);
	
	vertices = new int[m_num_face_vertices];
	fread(vertices, m_num_face_vertices*sizeof(unsigned int),1,fp);
	
	cvs = new XYZ[m_num_vertices];
	fread(cvs, 3*m_num_vertices*sizeof(float),1,fp);
	
	normals = new XYZ[m_num_vertices];
	fread(normals, 3*m_num_vertices*sizeof(float),1,fp);
	
	coord_s = new float[m_num_face_vertices];
	fread(coord_s, m_num_face_vertices*sizeof(float),1,fp);
	
	coord_t = new float[m_num_face_vertices];
	fread(coord_t, m_num_face_vertices*sizeof(float),1,fp);

	m_is_null = -1;
	
	fclose(fp);	
	
	return 1;
}

int CrmcMesh::export_(const char* filename, unsigned int nface, unsigned int nvert, unsigned int nfacevert, unsigned int* nloops, unsigned int* vertices, float* cvs, float* normals, float* coord_s, float* coord_t)
{
	FILE *fp = fopen( filename ,"wb" );
	if (fp == NULL) return -1;
	
	struct meshCacheInfo mesh;
	mesh.numPolygons = nface;
	mesh.numVertices = nvert;
	mesh.numFaceVertices = nfacevert;
	
	fwrite(&mesh,sizeof(struct meshCacheInfo),1,fp);
	
	fwrite(nloops, nface*sizeof(unsigned int),1,fp);
	fwrite(vertices, nfacevert*sizeof(unsigned int),1,fp);
	fwrite(cvs,3*nvert*sizeof(float),1,fp);
	fwrite(normals, 3*nvert*sizeof(float),1,fp);
	fwrite(coord_s, nfacevert*sizeof(float),1,fp);
	fwrite(coord_t, nfacevert*sizeof(float),1,fp);
	
	fclose(fp);

	return 1;
}