#ifndef _CPRTDATA_H
#define _CPRTDATA_H

#include "../shared/zMath.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "../shared/zString.h"
#include "../sh_lighting/SHWorks.h"
#define BLOCKSIZE 55368
using namespace std;

class CPRTData {

public :
	CPRTData();
	~CPRTData ();

	char isNull() const {return m_isNull;}
	
	void initialize(string _hdrname, const float exposure, const XYZ& side, const XYZ& up, const XYZ& front );
	
	HDRtexture* m_hdrimg;
	SHWorks* f_sh;

private :

	char m_isNull;
};

#endif
