#include "RPTC.h"

RPTC::RPTC(const char* parameter):pCache(0),m_density(1.f),m_dusty(.5f)
{
	float m[4][4];
	int n = sscanf(parameter, "%s %f %f %f %f %d %d %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
	m_cachename, 
	&m_density,
	&m_dusty,
	&m_shutter_open, &m_shutter_close,
	&m_is_blur, &m_is_shd,
	&m_fov, &m_type_projection,
	&m[0][0], &m[0][1], &m[0][2], &m[0][3], 
	&m[1][0], &m[1][1], &m[1][2], &m[1][3], 
	&m[2][0], &m[2][1], &m[2][2], &m[2][3], 
	&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
	
	cameraspace.setIdentity();
	cameraspace.v[0][0] = m[0][0];
	cameraspace.v[0][1] = m[0][1];
	cameraspace.v[0][2] = m[0][2];
	cameraspace.v[1][0] = m[1][0];
	cameraspace.v[1][1] = m[1][1];
	cameraspace.v[1][2] = m[1][2];
	cameraspace.v[2][0] = m[2][0];
	cameraspace.v[2][1] = m[2][1];
	cameraspace.v[2][2] = m[2][2];
	cameraspace.v[3][0] = m[3][0];
	cameraspace.v[3][1] = m[3][1];
	cameraspace.v[3][2] = m[3][2];
}

void RPTC::init()
{
	pCache = new Z3DTexture();
	if(!pCache->load(m_cachename)) printf("ERROR: cannot load particle cache %s", m_cachename);
	
	pCache->setProjection(cameraspace, m_fov, m_type_projection);
}

unsigned RPTC::getNumGrid() const
{
	return pCache->getNumGrid();	
}

void RPTC::getGrid(GRIDNId* data, unsigned& n_data) const
{
	pCache->getLODGrid(data, n_data);
}

void RPTC::sampleById(unsigned idx, RGRID& data) const
{
	pCache->getGridById(idx, data);
}

SHB3COEFF* RPTC::getNamedSH(const char* name)
{
	return pCache->getNamedSH(name);
}
