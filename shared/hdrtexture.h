#ifndef _HDRTEXTURE_H
#define _HDRTEXTURE_H

#include "../shared/zMath.h"
#include <iostream>
#include <fstream>
using namespace std;
class HDRtexture {
public:
	HDRtexture(const char *filename);
	~HDRtexture();
  
	void load(const char *filename);
	bool isValid() { return m_valid; };
	int getWidth() {return m_width;}
	int getHeight() {return m_height;}
	void setExposure(const float val)
	{	
		m_exp = val;
		m_exposure = pow(2.0f, m_exp);
	}
	void changeExposure(const float val) { m_exp+= val; m_exposure = pow(2.0f, m_exp);}
  
	const float* getTexturePointer() {return m_data;}
  
	void getRGBAt(const double theta, const double phi, float& r, float& g, float& b) const
	{
		int y = (m_height-1)*theta/PI;
		int x = (m_width-1)*phi/PI/2;
		r = m_exposure* m_data[(y*m_width+x)*4];
		g = m_exposure* m_data[(y*m_width+x)*4+1];
		b = m_exposure* m_data[(y*m_width+x)*4+2];
	}
	
	void getRGBByVector(const double vx, const double vy, const double vz, float& r, float& g, float& b) const;

	void setSpace(const MATRIX44F& space) {m_space = space;}
  
private:
	bool m_valid;
	int m_format;
	float m_exposure, m_exp;
	int m_width, m_height;
	bool m_flip_vertical;
	float* m_data;
	MATRIX44F m_space;
};
#endif