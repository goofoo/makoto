#include "RHair.h"
#include "HairCache.h"
#include "../shared/zGlobal.h"
using namespace std;

RHair::RHair(std::string& parameter):
m_ndice(24)
{
	m_cache_name = new char[256];

	int n = sscanf(parameter.c_str(), "%f %f %f %f %f %f %f %f %f %f %f %f %f %s %f %f %f %d %f %f %f %f %d", 
	&m_ndice,
	&m_width0, &m_width1,
	&m_root_colorR, &m_root_colorG, &m_root_colorB, 
	&m_tip_colorR, &m_tip_colorG, &m_tip_colorB, 
	&m_mutant_colorR, &m_mutant_colorG, &m_mutant_colorB, 
	&m_mutant_scale,
	m_cache_name, 
	&m_clumping, 
	&m_fuzz, 
	&m_kink,
	&m_isInterpolate,
	&m_shutter_open, &m_shutter_close,
	&m_hair_0, &m_hair_1,
	&m_is_blur);
}

RHair::~RHair() 
{
	delete[] m_cache_name;
}

void RHair::generateRIB(RtFloat detail)
{
	HairCache* pHair = new HairCache();
	pHair->setDiceNumber(m_ndice);
	
	string head = m_cache_name;
	zGlobal::cutByFirstDot(head);
	head += ".hairstart";
	pHair->loadStart(head.c_str());
	pHair->dice();
	pHair->setInterpolate(m_isInterpolate);
	pHair->bind();
	pHair->load(m_cache_name);
	
	pHair->setClumping(m_clumping);
	pHair->setFuzz(m_fuzz);
	pHair->setKink(m_kink);
	pHair->setWidth(m_width0, m_width1);
	pHair->setRootColor(m_root_colorR, m_root_colorG, m_root_colorB);
	pHair->setTipColor(m_tip_colorR, m_tip_colorG, m_tip_colorB);
	pHair->setMutantColor(m_mutant_colorR, m_mutant_colorG, m_mutant_colorB);
	pHair->setMutantColorScale(m_mutant_scale);
			
	pHair->create();
	
	float cwidth = 0.01f;
	RiCurves("cubic", (RtInt)pHair->getNumCurves(), (RtInt*)pHair->getNumVertices(), "nonperiodic", "P", (RtPoint*)pHair->points(), "width", (RtFloat*)pHair->getWidth(), 
	"uniform float s", (RtFloat*)pHair->getS(), "uniform float t", (RtFloat*)pHair->getT(), 
	"uniform color root_color", (RtColor*)pHair->getRootColors(), "uniform color tip_color", (RtColor*)pHair->getTipColors(), 
	"Os", (RtColor*)pHair->getOs(), RI_NULL);
	
	delete pHair;
	
	return;
}
//:~