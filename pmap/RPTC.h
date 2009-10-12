#ifndef _R_PARTICLECACHE_H
#define _R_PARTICLECACHE_H

#include <stdio.h>
#include <string>

#include "../3dtexture/Z3DTexture.h"

class RPTC {
public:
	RPTC(const char* parameter);
	~RPTC() {}
	void init();
	unsigned getNumData() const;
	void getIndex(HighNLow* data) const;
	void sampleById(unsigned idx, RGRID& data) const;
private:
	char m_cachename[256];
	float m_shutter_open, m_shutter_close;
	int m_is_blur, m_is_shd, m_type_projection;
	
	Z3DTexture* pCache;
};
#endif
