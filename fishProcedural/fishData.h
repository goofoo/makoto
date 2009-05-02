#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>

#include "../shared/FMCFMesh.h"

using namespace std;

class fishData {
public:
	fishData(string& parameter);
    ~fishData();
	void generateRIB(RtFloat detail);
    
private:
	
	char* m_cache_body_0;
	char* m_cache_body_1;
	char* m_cache_body_2;
	char* m_cache_body_3;
	float m_time_offset, m_angle_flap, m_angle_bend, m_angle_oscillate, m_length, m_bone_count, m_frequency, m_motion_begin, m_motion_end;
	int m_do_mblur;
	
	FMCFMesh* pMesh;
	
	void emit(const char* name);
};
