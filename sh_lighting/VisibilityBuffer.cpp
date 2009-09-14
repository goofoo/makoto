#include "VisibilityBuffer.h"

CVisibilityBuffer::CVisibilityBuffer(void)
{
	//m_dist = new float[VIS_N_GRID];
	//m_color = new float[VIS_N_GRID*3]; 
	//m_id = new unsigned int[VIS_N_GRID];
	m_data = new VisPix[VIS_N_GRID];
	//m_scale = new float[VIS_N_GRID];
}

CVisibilityBuffer::~CVisibilityBuffer(void)
{
	//delete[] m_dist;
	//delete[] m_color;
	//delete[] m_id;
	if(m_data) delete[] m_data;
	//delete[] m_scale;
}
#include <iostream>
void CVisibilityBuffer::initialize()
{
	for(int i=0; i<VIS_N_GRID; i++) 
	{
		m_data[i].dist = 10e8;
		m_data[i].id = 938461273;
		m_data[i].scale = 1.f;
	}
	//delete[] m_data;
	//m_data = new VisPix[VIS_N_GRID];
}

void CVisibilityBuffer::setSpace(const XYZ& _origin, const XYZ& normal, const XYZ& tangent)
{
	XYZ binormal = normal.cross(tangent);binormal.normalize();
	m_space.setIdentity();
	m_space.setOrientations(tangent, binormal, normal);
	m_space.setTranslation(_origin);
	m_space.inverse();
	
	zenith = normal;
	origin = _origin;
}

void CVisibilityBuffer::HemisphereToST(const XYZ& vec, int& coord_s, int& coord_t)
{
	float px = vec.x/(vec.z+1);
	float py = vec.y/(vec.z+1);
	coord_s = VIS_SQRT_N_HALF + px*VIS_SQRT_N_HALF;
	coord_t = VIS_SQRT_N_HALF + py*VIS_SQRT_N_HALF;
}

void CVisibilityBuffer::write(XYZ& sample, int& bucket_size)
{
	XYZ ray = sample - origin;

	m_space.transformAsNormal(ray);
	ray.normalize();
	
	if(ray.z<0) return;

	HemisphereToST(ray, coord_s, coord_t);
	
	for(int j=coord_t-bucket_size;j<=coord_t+bucket_size;j++)
	{
		if(j<0 || j>VIS_SQRT_N_GRID_MINUSONE) continue;
		for(int i=coord_s-bucket_size;i<=coord_s+bucket_size;i++)
		{
			if(i<0 || i>VIS_SQRT_N_GRID_MINUSONE) continue;
			int id = j*VIS_SQRT_N_GRID+i;
			m_data[id].dist = 0;
		}
	}
}

void CVisibilityBuffer::write(XYZ& sample, float& distance, int& bucket_size)
{
	XYZ ray = sample - origin;

	m_space.transformAsNormal(ray);
	ray.normalize();
	
	if(ray.z<0) return;

	HemisphereToST(ray, coord_s, coord_t);
	
	for(int j=coord_t-bucket_size;j<=coord_t+bucket_size;j++)
	{
		if(j<0 || j>VIS_SQRT_N_GRID_MINUSONE) continue;
		for(int i=coord_s-bucket_size;i<=coord_s+bucket_size;i++)
		{
			if(i<0 || i>VIS_SQRT_N_GRID_MINUSONE) continue;
			int id = j*VIS_SQRT_N_GRID+i;
			if(distance < m_data[id].dist) 
			{
				m_data[id].dist = distance;
			}
		}
	}
}

void CVisibilityBuffer::write(XYZ& sample, float& distance, unsigned int node_id, float k, int& bucket_size)
{
	XYZ ray = sample - origin;

	m_space.transformAsNormal(ray);
	ray.normalize();

	if(ray.z<0) return;
	
	HemisphereToST(ray, coord_s, coord_t);
	
	for(int j=coord_t-bucket_size;j<=coord_t+bucket_size;j++)
	{
		if(j<0 || j>VIS_SQRT_N_GRID_MINUSONE) continue;
		for(int i=coord_s-bucket_size;i<=coord_s+bucket_size;i++)
		{
			if(i<0 || i>VIS_SQRT_N_GRID_MINUSONE) continue;
			int id = j*VIS_SQRT_N_GRID+i;
			if(distance < m_data[id].dist) 
			{
				m_data[id].dist = distance;
				m_data[id].id = node_id;
				m_data[id].scale = k;
			}
		}
	}
}

void CVisibilityBuffer::writeBlend(XYZ& sample, float& distance, unsigned int node_id, float k, int& bucket_size)
{
	XYZ ray = sample - origin;

	m_space.transformAsNormal(ray);
	ray.normalize();

	HemisphereToST(ray, coord_s, coord_t);
	
	for(int j=coord_t-bucket_size;j<=coord_t+bucket_size;j++)
	{
		if(j<0 || j>VIS_SQRT_N_GRID_MINUSONE) continue;
		for(int i=coord_s-bucket_size;i<=coord_s+bucket_size;i++)
		{
			if(i<0 || i>VIS_SQRT_N_GRID_MINUSONE) continue;
			int id = j*VIS_SQRT_N_GRID+i;
			if(distance < m_data[id].dist) 
			{
				if(m_data[id].dist < distance*16)  m_data[id].dist = (m_data[id].dist+distance)*.5f;
				else m_data[id].dist = distance;
				m_data[id].id = node_id;
				m_data[id].scale = k;
			}
		}
	}
}

void CVisibilityBuffer::read(const XYZ& ray, float& distance) const
{
	XYZ object_ray = ray;
	m_space.transformAsNormal(object_ray);object_ray.normalize();
	
	int coord_s,coord_t;
	HemisphereToST(object_ray, coord_s, coord_t);
	
	distance = m_data[coord_t*VIS_SQRT_N_GRID+coord_s].dist;
}

#include <iostream>

void CVisibilityBuffer::read(const XYZ& ray, float& distance, unsigned int& node_id, float& K) const
{
	XYZ object_ray = ray;
	m_space.transformAsNormal(object_ray);object_ray.normalize();
	
	int coord_s,coord_t;
	HemisphereToST(object_ray, coord_s, coord_t);
	
	distance = m_data[coord_t*VIS_SQRT_N_GRID+coord_s].dist;
	node_id = m_data[coord_t*VIS_SQRT_N_GRID+coord_s].id;
	K = m_data[coord_t*VIS_SQRT_N_GRID+coord_s].scale;
}

float CVisibilityBuffer::getDepth(const XYZ& p) const
{
	XYZ object_p = p;
	m_space.transform(object_p);
	return object_p.z;
}
//:~
