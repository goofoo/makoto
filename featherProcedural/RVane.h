#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>
#include "FXMLVaneMap.h"

class RVane {
public:
	RVane(std::string& parameter);
    ~RVane();
	void generateRIB(RtFloat detail);
    
private:
	char* m_cache_name;
	float m_g_width;
	float m_shutter_open, m_shutter_close;
	float m_fps;
	int m_is_blur;
	FXMLVaneMap* map;
};
