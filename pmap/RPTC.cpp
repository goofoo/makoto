#include "RPTC.h"

RPTC::RPTC(std::string& parameter)
{
	float val;
	float m[4][4];
	int n = sscanf(parameter.c_str(), "%s %f %f %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
	m_cachename, 
	&m_shutter_open, &m_shutter_close,
	&m_is_blur, &m_is_shd,
	&val,
	&m[0][0], &m[0][1], &m[0][2], &m[0][3], 
	&m[1][0], &m[1][1], &m[1][2], &m[1][3], 
	&m[2][0], &m[2][1], &m[2][2], &m[2][3], 
	&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
}
