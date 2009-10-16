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
	printf("\n0: %f %f %f\n", hdrCoeff[0].x, hdrCoeff[0].y, hdrCoeff[0].z);
	printf("1: %f %f %f\n", hdrCoeff[1].x, hdrCoeff[1].y, hdrCoeff[1].z);
	printf("2: %f %f %f\n", hdrCoeff[2].x, hdrCoeff[2].y, hdrCoeff[2].z);
	printf("3: %f %f %f\n", hdrCoeff[3].x, hdrCoeff[3].y, hdrCoeff[3].z);
	printf("4: %f %f %f\n", hdrCoeff[4].x, hdrCoeff[4].y, hdrCoeff[4].z);
	printf("5: %f %f %f\n", hdrCoeff[5].x, hdrCoeff[5].y, hdrCoeff[5].z);
	printf("6: %f %f %f\n", hdrCoeff[6].x, hdrCoeff[6].y, hdrCoeff[6].z);
	printf("7: %f %f %f\n", hdrCoeff[7].x, hdrCoeff[7].y, hdrCoeff[7].z);
	printf("8: %f %f %f\n", hdrCoeff[8].x, hdrCoeff[8].y, hdrCoeff[8].z);
}
