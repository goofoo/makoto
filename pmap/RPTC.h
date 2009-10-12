#ifndef _R_PARTICLECACHE_H
#define _R_PARTICLECACHE_H

#include <stdio.h>
#include <string>

class RPTC {
public:
	RPTC(std::string& parameter);
	~RPTC() {}
	void init() {}
private:
	char m_cachename[256];
	float m_shutter_open, m_shutter_close;
	int m_is_blur, m_is_shd;
};
#endif
