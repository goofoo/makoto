#include "RPTC.h"

RPTC::RPTC(const char* parameter):pCache(0)
{
	float val;
	float m[4][4];
	int n = sscanf(parameter, "%s %f %f %d %d %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
	m_cachename, 
	&m_shutter_open, &m_shutter_close,
	&m_is_blur, &m_is_shd,
	&val, &m_type_projection,
	&m[0][0], &m[0][1], &m[0][2], &m[0][3], 
	&m[1][0], &m[1][1], &m[1][2], &m[1][3], 
	&m[2][0], &m[2][1], &m[2][2], &m[2][3], 
	&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
}

void RPTC::init()
{
	pCache = new Z3DTexture();
	if(!pCache->load(m_cachename)) printf("ERROR: cannot load particle cache %s", m_cachename);
}

unsigned RPTC::getNumData() const
{
	return pCache->getNumLeaf();	
}

void RPTC::getIndex(HighNLow* data) const
{
	pCache->getLeaIndex(data);
}

void RPTC::sampleById(unsigned idx, RGRID& data) const
{
	pCache->getGridById(idx, data);
}
