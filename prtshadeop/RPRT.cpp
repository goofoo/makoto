#include "RPRT.h"

struct HDRLight : public SHFunction
{
	HDRLight(const HDRtexture* _img): img( _img) {}
	float getFloat(const SHSample& s) const
	{
		return 128.0f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r, g, b;
		img->getRGBByVector(s.vec[0], s.vec[1], s.vec[2], r,g,b);
		
		return XYZ(r,g,b);
	}
	
	const HDRtexture* img;
};

RPRT::RPRT() : m_isNull(1), f_hdr(0)
{
	f_sh = new sphericalHarmonics();
}
	
RPRT::~RPRT()
{
	delete f_sh;
	if(f_hdr) delete f_hdr;
}

void RPRT::initialize(const char* _hdrname, const float exposure, const XYZ& side, const XYZ& up, const XYZ& front)
{ 
	f_hdr = new HDRtexture(_hdrname);
	MATRIX44F space;
	space.setOrientations(side, up, front);
	space.inverse();
	f_hdr->setExposure(exposure);
	f_hdr->setSpace(space);
	
	HDRLight hl(f_hdr);
	f_sh->projectFunction(hl, hdrCoeff);	
	m_isNull = 0;
}
