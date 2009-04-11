#include "FTriangleMesh.h"
#include "../shared/gBase.h"
#include "../shared/zString.h"
#include "FTriangle.h"

FTriangleMesh::FTriangleMesh(void):m_vertexId(0), m_data(0)
{
}

FTriangleMesh::~FTriangleMesh(void)
{
	release();
}

void FTriangleMesh::release()
{
	if(m_vertexId) delete[] m_vertexId;
	if(m_data) delete[] m_data;
}

int FTriangleMesh::load(const char* _filename)
{
	cout<<" Loading Triangle Mesh File: "<<_filename<<endl;
	FILE *fp = fopen( _filename, "rb");

	if( fp==NULL ) 
	{
		cout<<"Failed to open triangle mesh file: "<<_filename;
		return 0;
	}
	
	triangle_mesh_header header;
	
	int stat;
	
	stat = fread(&header,sizeof(struct triangle_mesh_header),1,fp);
	
	if(stat!=1) return 0;
		
	m_n_triangle = header.n_triangle;
	m_n_vertex = header.n_vertex;
	m_area = header.area;
	
	cout<<" N Triangle: "<<m_n_triangle<<endl;
	cout<<" N Vertex: "<<m_n_vertex<<endl;
	cout<<" Area: "<<m_area<<endl;
	
	release();
	
	m_vertexId = new int[m_n_triangle*3];
	
	fread(m_vertexId, m_n_triangle*3*sizeof(int),1,fp);
	
	m_data = new triangle_mesh_rec[m_n_vertex];
	
	fread(m_data, m_n_vertex*sizeof(triangle_mesh_rec),1,fp);
	
	fclose(fp);
	
	filename = _filename;
	
	return 1;
}

void FTriangleMesh::draw() const
{
	XYZ p0, p1, p2;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0,.75,.2);
	glBegin(GL_TRIANGLES);
	for(int i=0; i<m_n_triangle; i++)
	{
		p0 = m_data[m_vertexId[i*3]].pos;
		p1 = m_data[m_vertexId[i*3+1]].pos;
		p2 = m_data[m_vertexId[i*3+2]].pos;
		glVertex3f(p0.x, p0.y, p0.z);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void FTriangleMesh::draw(XYZ* hdr) const
{
	CoeRec* coeff = new CoeRec[m_n_vertex];
	
	if(readExistingRTFile(coeff) != 1) return;
		
	glBegin(GL_TRIANGLES);
	for(int i=0; i<m_n_triangle; i++)
	{
		XYZ col0(0.f);
		XYZ col1(0.f);
		XYZ col2(0.f);
		for(int j=0; j<16; j++)
		{
			col0.x += coeff[m_vertexId[i*3]].data[j].x * hdr[j].x;
			col0.y += coeff[m_vertexId[i*3]].data[j].y * hdr[j].y;
			col0.z += coeff[m_vertexId[i*3]].data[j].z * hdr[j].z;
			
			col1.x += coeff[m_vertexId[i*3+1]].data[j].x * hdr[j].x;
			col1.y += coeff[m_vertexId[i*3+1]].data[j].y * hdr[j].y;
			col1.z += coeff[m_vertexId[i*3+1]].data[j].z * hdr[j].z;
			
			col2.x += coeff[m_vertexId[i*3+2]].data[j].x * hdr[j].x;
			col2.y += coeff[m_vertexId[i*3+2]].data[j].y * hdr[j].y;
			col2.z += coeff[m_vertexId[i*3+2]].data[j].z * hdr[j].z;
		}
		
		XYZ p0 = m_data[m_vertexId[i*3]].pos;
		XYZ p1 = m_data[m_vertexId[i*3+1]].pos;
		XYZ p2 = m_data[m_vertexId[i*3+2]].pos;
		glColor3f(col0.x, col0.y, col0.z);
		glVertex3f(p0.x, p0.y, p0.z);
		glColor3f(col1.x, col1.y, col1.z);
		glVertex3f(p1.x, p1.y, p1.z);
		glColor3f(col2.x, col2.y, col2.z);
		glVertex3f(p2.x, p2.y, p2.z);
	}
	glEnd();
	
	delete[] coeff;
}

void FTriangleMesh::dice(int n_cell, FQSPLAT* cloud) const
{
	cout<<" Dicing...";
	float average_area = 0;
	int a, b, c;
	FTriangle ftri;
	for(int i=0; i<m_n_triangle; i++)
	{
		a = m_vertexId[i*3];
		b = m_vertexId[i*3+1];
		c = m_vertexId[i*3+2];
		float ta = FTriangle::calculate_area(m_data[a].pos, m_data[b].pos, m_data[c].pos);
		
		average_area += ta;
	}
	average_area /= m_n_triangle;

	int real_n_cell = m_area/average_area*n_cell*2;
		
	cout<<" N Dice: "<<real_n_cell;
	
	float delta = sqrt(m_area/real_n_cell);
	
	pcdSample* samp = new pcdSample[real_n_cell+real_n_cell/7];
	int n_samp = 0;
	
	for(int i=0; i<m_n_triangle; i++)
	{
		a = m_vertexId[i*3];
		b = m_vertexId[i*3+1];
		c = m_vertexId[i*3+2];
		
		ftri.create(m_data[a].pos, m_data[b].pos, m_data[c].pos);
		ftri.setNormal(m_data[a].nor, m_data[b].nor, m_data[c].nor);
		ftri.setColor(m_data[a].col, m_data[b].col, m_data[c].col);
		ftri.setTangent(m_data[a].tang, m_data[b].tang, m_data[c].tang);
		
		ftri.rasterize(delta, samp, n_samp);
	}
	cout<<" N Samples: "<<n_samp;
	cloud->create(samp, n_samp);
	delete[] samp;
}

void FTriangleMesh::diceCoe(int n_cell, FQSPLAT* cloud) const
{
	CoeRec* vex_coe = new CoeRec[m_n_vertex];
	
	if(readExistingRTFile(vex_coe) != 1) return;
	
	cout<<" Dicing with RT...";
	float average_area = 0;
	int a, b, c;
	FTriangle ftri;
	for(int i=0; i<m_n_triangle; i++)
	{
		a = m_vertexId[i*3];
		b = m_vertexId[i*3+1];
		c = m_vertexId[i*3+2];
		float ta = FTriangle::calculate_area(m_data[a].pos, m_data[b].pos, m_data[c].pos);
		
		average_area += ta;
	}
	average_area /= m_n_triangle;

	int real_n_cell = m_area/average_area*n_cell*2;
		
	cout<<" N Dice: "<<real_n_cell;
	
	float delta = sqrt(m_area/real_n_cell);
	
	pcdSample* samp = new pcdSample[real_n_cell+real_n_cell/7];
	CoeRec* coe_samp = new CoeRec[real_n_cell+real_n_cell/7];
	int n_samp = 0;
	
	for(int i=0; i<m_n_triangle; i++)
	{
		a = m_vertexId[i*3];
		b = m_vertexId[i*3+1];
		c = m_vertexId[i*3+2];
		
		ftri.create(m_data[a].pos, m_data[b].pos, m_data[c].pos);
		ftri.setNormal(m_data[a].nor, m_data[b].nor, m_data[c].nor);
		ftri.setColor(m_data[a].col, m_data[b].col, m_data[c].col);
		ftri.setTangent(m_data[a].tang, m_data[b].tang, m_data[c].tang);
		ftri.setId(a, b, c);
		
		ftri.rasterize(delta, vex_coe, samp, coe_samp, n_samp);
	}
	cout<<" N Samples: "<<n_samp;
	cloud->create(samp, coe_samp, n_samp);
	delete[] samp;
	delete[] coe_samp;
	delete[] vex_coe;
}

int FTriangleMesh::readExistingRTFile(CoeRec* data) const
{
	string rtName = filename;
	changeFileNameExtension(rtName, ".coe");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_n_vertex*16*3*sizeof(float))
		{
			ifile.seekg(0, ios::beg);
			ifile.read((char*)data, m_n_vertex*16*3*sizeof(float));
			ifile.close();
		}
		else
		{
			ifile.close();
			cout<<" ERROR: existing RT file "<<rtName<<" is not usable! ";
			return 0;
		}
		
	}
	else
	{
		cout<<" ERROR: failed to open "<<rtName<<" to read! ";
		return 0;
	}
	
	return 1;
}

int FTriangleMesh::checkExistingRTFile() const
{
	string rtName = filename;
	changeFileNameExtension(rtName, ".coe");
	
	ifstream ifile;
	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
	
	if(ifile.is_open())
	{
		ifstream::pos_type size = ifile.tellg();
		
		if((int)size == m_n_vertex*48*sizeof(float)) ifile.close();
		else
		{
			ifile.close();
			cout<<" ERROR: existing RT file "<<rtName<<" is not usable! ";
			return 0;
		}
		
	}
	else
	{
		cout<<" ERROR: failed to open "<<rtName<<" to read! ";
		return 0;
	}
	
	return 1;
}

void FTriangleMesh::saveRTFile(const XYZ* coe) const
{
	string rtName = filename;
	changeFileNameExtension(rtName, ".coe");
	
	ofstream ffile;
	ffile.open(rtName.c_str(), ios::out | ios::binary);
	if(!ffile.is_open()) 
	{
		cout<<"ERROR: failed to open "<<rtName<<" to write! ";
		return;
	}
	
	ffile.write((char*)coe, m_n_vertex*16*3*sizeof(float));
	ffile.close();
}

void FTriangleMesh::drawCloud(int n_cell, XYZ* hdr) const
{
	CoeRec* vex_coe = new CoeRec[m_n_vertex];
	
	if(readExistingRTFile(vex_coe) != 1) return;
	
	cout<<" Dicing with RT...";
	float delta = sqrt(m_area/n_cell);

	int a, b, c;
	FTriangle ftri;
	
	pcdSample* samp = new pcdSample[n_cell+n_cell/10];
	CoeRec* coe_samp = new CoeRec[n_cell+n_cell/10];
	int n_samp = 0;
	glBegin(GL_POINTS);
	for(int i=0; i<m_n_triangle; i++)
	{
		a = m_vertexId[i*3];
		b = m_vertexId[i*3+1];
		c = m_vertexId[i*3+2];
		
		ftri.create(m_data[a].pos, m_data[b].pos, m_data[c].pos);
		ftri.setNormal(m_data[a].nor, m_data[b].nor, m_data[c].nor);
		ftri.setColor(m_data[a].col, m_data[b].col, m_data[c].col);
		ftri.setTangent(m_data[a].tang, m_data[b].tang, m_data[c].tang);
		ftri.setId(a, b, c);
		
		ftri.rasterize(delta, vex_coe, samp, coe_samp, n_samp, hdr);
	}
	glEnd();
	delete[] samp;
	delete[] coe_samp;
	delete[] vex_coe;
}
void FTriangleMesh::getCenterAndSize(XYZ& center, float& size) const
{
	XYZ cen(0.f);
	float xmin, ymin, zmin, xmax, ymax, zmax;
	xmin = ymin = zmin = 10000000.f;
	xmax = ymax = zmax = -10000000.f;
	XYZ p0, p1, p2;
	for(int i=0; i<m_n_triangle; i++)
	{
		p0 = m_data[m_vertexId[i*3]].pos;
		p1 = m_data[m_vertexId[i*3+1]].pos;
		p2 = m_data[m_vertexId[i*3+2]].pos;
		cen += p0;
		cen += p1;
		cen += p2;
		if(p0.x < xmin) xmin = p0.x;
		if(p0.x > xmax) xmax = p0.x;
			
		if(p1.y < ymin) ymin = p1.y;
		if(p1.y > ymax) ymax = p1.y;
			
		if(p2.z < zmin) zmin = p2.z;
		if(p2.z > zmax) zmax = p2.z;
	}
	
	cen /=(float)m_n_triangle*3; 
	
	center = cen;
	
	float dx = xmax - xmin;
	float dy = ymax - ymin;
	float dz = zmax - zmin;
	
	if(dx > dy && dx >dz) size = dx;
		else if(dy > dx && dy >dz) size = dy;
			else size = dz;
}
//:~