#include "RHair.h"
#include "../shared/zGlobal.h"
using namespace std;

RHair::RHair(std::string& parameter):
m_ndice(24),pHair(0),has_densmap(0)
{
	m_cache_name = new char[256];
	m_dens_name = new char[256];

	int n = sscanf(parameter.c_str(), "%f %f %f %s %s %f %f %f %f %f %f %f %f %d", 
	&m_ndice,
	&m_width0, &m_width1,
	//&m_root_colorR, &m_root_colorG, &m_root_colorB, 
	//&m_tip_colorR, &m_tip_colorG, &m_tip_colorB, 
	//&m_mutant_colorR, &m_mutant_colorG, &m_mutant_colorB, 
	//&m_mutant_scale,
	m_cache_name, 
	m_dens_name, 
	&m_fuzz, 
	&m_kink,
	&m_clumping,
	&m_bald,
	&m_shutter_open, &m_shutter_close,
	&m_hair_0, &m_hair_1,
	&m_is_blur);
}

RHair::~RHair() 
{
	delete[] m_cache_name;
	delete[] m_dens_name;
	if(pHair) delete pHair;
}

void RHair::init()
{
	pHair = new HairCache();
	
	string head = m_cache_name;
	zGlobal::cutByFirstDot(head);
	head += ".hairstart";
	pHair->loadStart(head.c_str());
	
	m_epsilon = pHair->getEpsilon(m_ndice);
	
	pHair->load(m_cache_name);
	if(m_is_blur==1) pHair->loadNext();
	
	if(strcmp(m_dens_name, "nil")!=0) has_densmap = pHair->setDensityMap(m_dens_name);
}

unsigned RHair::getNumTriangle() const
{
	return pHair->getNumTriangle();
}

void RHair::setAttrib(float* attrib) const
{
	attrib[0] = m_fuzz;
	attrib[1] = m_kink;
	attrib[2] = m_clumping;
	attrib[3] = m_width0;
	attrib[4] = m_width1;
}

void RHair::setMBlur(unsigned& idx, XYZ* velocities, float* shutters) const
{
	shutters[0] = m_shutter_open;
	shutters[1] = m_shutter_close;
	float fract = m_hair_1 - m_hair_0;
	pHair->lookupVelocity(idx, fract, velocities);
}
//:~