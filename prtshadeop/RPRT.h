#ifndef _R_PRT_H
#define _R_PRT_H

#include <stdlib.h>
#include "../shared/hdrtexture.h"
#include "../sh_lighting/SphericalHarmonics.h"

class RPRT {

public :
	RPRT();
	~RPRT();

	char isNull() const {return m_isNull;}
	
	void initialize(const char* _hdrname, const float exposure, const XYZ& side, const XYZ& up, const XYZ& front );
	XYZ* getLight() {return hdrCoeff;}

private :
	char m_isNull;
	HDRtexture* f_hdr;
	sphericalHarmonics* f_sh;
	XYZ hdrCoeff[9];
};
#endif
