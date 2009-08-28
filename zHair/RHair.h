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
	int isShadow() const {return m_is_shd;}
	char setDepthPersp();
	char setDepthOrtho();
	char isVisible(const float *box, char isPersp) const;
	char isPointVisible(XYZ& Q, char isPersp) const;
	float clostestDistance(const XYZ* pos) const;
	float getShutterOpen() const {return m_shutter_open;}
	float getShutterClose() const {return m_shutter_close;}
	void meanDisplace(unsigned& idx, XYZ& disp) const;
	void simplifyDOF(unsigned& idx, float& depth, float& val) const;
	void simplifyMotion(unsigned& idx, float& val) const;
	
	HairCache* pHair;
	char has_densmap;
    
private:
	float m_ndice, m_width0, m_width1;
	char* m_cache_name;
	char* m_dens_name;
	char* m_depth_name;
	float m_fuzz, m_kink, m_clumping, m_bald, dof_min, dof_max; 
	float m_shutter_open, m_shutter_close;
	float m_hair_0, m_hair_1;
	int m_is_blur, m_is_shd;
	float m_epsilon;
	float* pDepthMap;
	MATRIX44F cameraspaceinv;
	float ffov, forthow;
	double tanhfov;
	float m[4][4];
};
