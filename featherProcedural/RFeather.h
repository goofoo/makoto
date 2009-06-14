#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>
#include "FXMLTriangleMap.h"

class RFeather {
public:
	RFeather(std::string& parameter);
    ~RFeather();
	void generateRIB(RtFloat detail);
    
private:
	char* m_cache_name;
	float m_n_dice, m_f_wh, m_thickness;
	float m_shutter_open, m_shutter_close;
	float m_fps;
	int m_is_blur, m_is_shd;
	XYZ* pp;
	FXMLTriangleMap* mesh;
	DFeather* feather;
	DFeather* feather_close;
};
