#include "shadeop.h"
#include <stdio.h> 
#include "RPRT.h"
 
extern "C" { 
SHADEOP_TABLE(SHLighting) = 
{ 
	{"color shl_hdr(string, float, vector, vector, color, color, color, color, color, color, color, color, color )", "shl_init", "shl_cleanup"}, 
	{""} 
}; 
}

extern "C" SHADEOP_INIT(shl_init) 
{ 
	RPRT* data = new RPRT();
	return data;
}
 
extern "C" SHADEOP(shl_hdr) 
{ 
	float *result = (float *)argv[0]; 
	
	std::string hdr_name (*((char **) argv[1]));
	if(hdr_name.size() <1) return 0;

	float *in_exposure = (float *)argv[2];
	float *in_side = (float *)argv[3];
	float *in_up = (float *)argv[4];
	
	RPRT *db = ( RPRT *) initdata;
	
	if(db->isNull()) {
		XYZ side(in_side), up(in_up);
		XYZ front = side^up;
		db->initialize(hdr_name.c_str(), *in_exposure, side, up, front);
	}
	
	XYZ* light = db->getLight();
	
	XYZ coeff[9];
	for(int i =0; i<9; i++) coeff[i] = XYZ((float *)argv[5+i]);
	
	float r=0, g=0, b=0;
	for(int i =0; i<9; i++) {
		r += coeff[i].x * light[i].x;
		g += coeff[i].y * light[i].y;
		b += coeff[i].z * light[i].z;
	}
	
	//float *in_ce = (float *)argv[5];
	//printf("getl: %f\n", *in_ce);
	
	// output
	result[0] = r; 
	result[1] = g; 
	result[2] = b; 
	return 0; 
}

extern "C" SHADEOP_CLEANUP( shl_cleanup ) 
{ 
	RPRT* data = (RPRT*)initdata;
	delete data;
} 

