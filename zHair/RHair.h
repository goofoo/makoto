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
	float m_ndice, m_width0, m_width1;
	char* m_cache_name;
	char* m_dens_name;
	float m_clumping, m_fuzz, m_kink;
	float m_shutter_open, m_shutter_close;
	float m_hair_0, m_hair_1;
	float m_root_colorR, m_root_colorG, m_root_colorB; 
	float m_tip_colorR, m_tip_colorG, m_tip_colorB; 
	float m_mutant_colorR, m_mutant_colorG, m_mutant_colorB; 
	float m_mutant_scale;
	float m_bald;
	int m_is_blur, m_isInterpolate, m_issimple;
	float m_fract;
};
