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
	unsigned getNumGrid() const;
	void getGrid(GRIDNId* data, unsigned& n_data) const;
	void sampleById(unsigned idx, RGRID& data) const;
	float getDensity() const {return m_density;}
	float getDusty() const {return m_dusty;}
private:
	char m_cachename[256];
	float m_shutter_open, m_shutter_close, m_fov;
	int m_is_blur, m_is_shd, m_type_projection;
	
	Z3DTexture* pCache;
	float m_density, m_dusty;
	MATRIX44F cameraspace;
};
#endif
