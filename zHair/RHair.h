#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>

#include "HairCache.h"

class RHair {
public:
	RHair(std::string& parameter);
    ~RHair();
	
	void init();
	
	float getKink() const {return m_kink;}
	float getEpislon() const {return m_epsilon;}
	unsigned getNumTriangle() const;
	void setAttrib(float* attrib) const;
	char needMBlur() const {if(m_is_blur==1) return 1; else return 0;}
	void setMBlur(unsigned& idx, XYZ* velocities, float* shutters) const;
	
	HairCache* pHair;
	char has_densmap;
    
private:
	float m_ndice, m_width0, m_width1;
	char* m_cache_name;
	char* m_dens_name;
	float m_fuzz, m_kink, m_clumping, m_bald; 
	float m_shutter_open, m_shutter_close;
	float m_hair_0, m_hair_1;
	int m_is_blur;
	float m_epsilon;
};
