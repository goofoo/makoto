#include "CPRTData.h"

CPRTData::CPRTData():
m_isNull(1),
m_hdrimg(0)
{
	f_sh = new SHWorks();
}
	
CPRTData::~CPRTData ()
{
	delete f_sh;
	if(m_hdrimg) delete m_hdrimg;
}

void CPRTData::initialize(string _hdrname, const float exposure, const XYZ& side, const XYZ& up, const XYZ& front)
{ 
	m_hdrimg = new HDRtexture(_hdrname.c_str());
	MATRIX44F space;
	space.setOrientations(side, up, front);
	space.inverse();
	m_hdrimg->setExposure(exposure);
	m_hdrimg->setSpace(space);
	f_sh->computeHDRCoeffNoRotate(m_hdrimg);
	m_isNull = 0;
}
//:~