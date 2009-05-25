#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>

class RHair {
public:
	RHair(std::string& parameter);
    ~RHair();
	void generateRIB(RtFloat detail);
    
private:
	float m_ndice;
	char* m_cache_name;
	float m_twist, m_clumping, m_fuzz, m_kink;
	float m_shutter_open, m_shutter_close;
	float m_hair_0, m_hair_1;
	int m_is_blur;
};
