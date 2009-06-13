///:
// dumpdata.cpp - code for the DSO Shadeop writing P values of shading point to a specific file.
// 
// Date: 01/19/08
// 
// Thanks Larry Gritz for his "bake" program in the SIGGRAPH 2002 course notes.
//

#include <shadeop.h>
#include "CPRTData.h"
// declare the DSO Shadeop in "C"
//
extern "C" SHADEOP_TABLE(SHLighting) = 
{ 
    { "color sh_lighting_shadowed (string, float, vector, vector, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color)", "init", "cleanup"},
    { "color sh_lighting_shadowed_displacement (string, float, vector, vector, normal, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color)", "init", "cleanup"},
    { "color sh_lighting_src (color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color)", "init", "cleanup"},
	{ "color sh_lighting_src_disp (normal, float, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color )", "init", "cleanup"},
    { "color sh_lighting_src_d (normal, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color )", "init", "cleanup"},
    { "float sh_lighting_occlusion (normal, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color)", "init", "cleanup"},
	{ "color sh_lighting_reflection (string, float, vector, vector, vector)", "init", "cleanup"},
    { "color sh_lighting_shadowed_f (string, float, vector, vector, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)", "init", "cleanup"},
    { "color sh_lighting_shadowed_displacement_f (string, float, vector, vector, normal, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)", "init", "cleanup"},
	{ "float sh_lighting_occlusion_f (normal, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)", "init", "cleanup"},
    { "color sh_lighting_src_d_f (normal, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float )", "init", "cleanup"},
	{ "" }  
};

// initialization function
// build and return the data block
//
extern "C" SHADEOP_INIT(init)
{
	CPRTData *data = new CPRTData;
	return data;
}

// shutdown function called after rendering is complete
// destroy the data block
//
extern "C" SHADEOP_CLEANUP(cleanup)
{
	CPRTData *db = ( CPRTData *) initdata;
	delete db;
}

// actual implementation of the shadeop
// take the arguments and call the routines
//
extern "C" SHADEOP(sh_lighting_shadowed) 
{
	float *result = (float *)argv[0];
	
	std::string hdr_name (*((char **) argv[1]));
	if(hdr_name.size() <1) return 0;

	float *in_exposure = (float *)argv[2];
	float *in_side = (float *)argv[3];
	float *in_up = (float *)argv[4];
	
	CPRTData *db = ( CPRTData *) initdata;
	
	if(db->isNull()) 
	{
		windowspath(hdr_name);
		XYZ side(in_side), up(in_up);
		XYZ front = side^up;
		db->initialize(hdr_name, *in_exposure, side, up, front);
	}
	
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[5+i]);
	}

	XYZ C = db->f_sh->computeSHLighting(coeff);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;

	return 0;
}

extern "C" SHADEOP(sh_lighting_shadowed_displacement) 
{
	float *result = (float *)argv[0];
	
	std::string hdr_name (*((char **) argv[1]));
	if(hdr_name.size() <1) return 0;

	float *in_exposure = (float *)argv[2];
	float *in_side = (float *)argv[3];
	float *in_up = (float *)argv[4];
	XYZ Nn((float *)argv[5]);
	
	CPRTData *db = ( CPRTData *) initdata;
	
	if(db->isNull()) 
	{
		windowspath(hdr_name);
		XYZ side(in_side), up(in_up);
		XYZ front = side^up;
		db->initialize(hdr_name, *in_exposure, side, up, front);
	}
	
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[6+i]);
	}

	XYZ C = db->f_sh->computeSHLighting(coeff, Nn);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_src) 
{
	float *result = (float *)argv[0];
	
	CPRTData *db = ( CPRTData *) initdata;
		
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[i+1]);
	}
	
	XYZ C = db->f_sh->computeSrcSHLighting(coeff);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_src_disp) 
{
	float *result = (float *)argv[0];
	
	XYZ Nn((float *)argv[1]);
	float *channel = (float *)argv[2];
	
	CPRTData *db = ( CPRTData *) initdata;
		
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[i+3]);
	}
	
	XYZ C = db->f_sh->computeSrcSHLighting(coeff, Nn, *channel);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_src_d) 
{
	float *result = (float *)argv[0];
	
	CPRTData *db = ( CPRTData *) initdata;
	
	XYZ Nn((float *)argv[1]);
		
	XYZ srcCoeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		srcCoeff[i] = XYZ((float *)argv[i+2]);
	}
	
	XYZ shdCoeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		shdCoeff[i] = XYZ((float *)argv[i+18]);
	}

	XYZ C = db->f_sh->computeSHLighting(srcCoeff, shdCoeff, Nn);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_occlusion) 
{
	float *result = (float *)argv[0];
	
	CPRTData *db = ( CPRTData *) initdata;
	
	XYZ Nn((float *)argv[1]);
	
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[i+2]);
	}

	float occ = db->f_sh->computeSrcSHLighting(coeff, Nn);
	
	// output
	result[0] = occ/3.14f;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_reflection)
{
	float *result = (float *)argv[0];
	
	std::string hdr_name (*((char **) argv[1]));
	if(hdr_name.size() <1) return 0;

	float *in_exposure = (float *)argv[2];
	float *in_side = (float *)argv[3];
	float *in_up = (float *)argv[4];
	XYZ in_r((float *)argv[5]);
	
	CPRTData *db = ( CPRTData *) initdata;
	
	if(db->isNull()) 
	{
		windowspath(hdr_name);
		XYZ side(in_side), up(in_up);
		XYZ front = side^up;
		db->initialize(hdr_name, *in_exposure, side, up, front);
	}

	float r, g, b;
	db->m_hdrimg->getRGBByVector(in_r.x, in_r.y, in_r.x, r, g, b);
	
	// output
	result[0] = r;
	result[1] = g;
	result[2] = b;
	
    return 0;
}

extern "C" SHADEOP(sh_lighting_shadowed_f) 
{
	float *result = (float *)argv[0];
	
	std::string hdr_name (*((char **) argv[1]));
	if(hdr_name.size() <1) return 0;

	float *in_exposure = (float *)argv[2];
	float *in_side = (float *)argv[3];
	float *in_up = (float *)argv[4];
	
	CPRTData *db = ( CPRTData *) initdata;
	
	if(db->isNull()) 
	{
		windowspath(hdr_name);
		XYZ side(in_side), up(in_up);
		XYZ front = side^up;
		db->initialize(hdr_name, *in_exposure, side, up, front);
	}
	
	XYZ coeff[SH_N_BASES];
	float *in_coe;
	for(int i =0; i<SH_N_BASES; i++) 
	{
		in_coe = (float *)argv[5+i];
		coeff[i] = *in_coe;
		coeff[i] = XYZ(*in_coe);
		//coeff[i] = (float *)argv[5+i];
	}

	XYZ C = db->f_sh->computeSHLighting(coeff);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	//result[0] = coeff[1];
	//result[1] = coeff[1];
	//result[2] = coeff[1];

	return 0;
}

extern "C" SHADEOP(sh_lighting_shadowed_displacement_f) 
{
	float *result = (float *)argv[0];
	
	std::string hdr_name (*((char **) argv[1]));
	if(hdr_name.size() <1) return 0;

	float *in_exposure = (float *)argv[2];
	float *in_side = (float *)argv[3];
	float *in_up = (float *)argv[4];
	XYZ Nn((float *)argv[5]);
	
	CPRTData *db = ( CPRTData *) initdata;
	
	if(db->isNull()) 
	{
		windowspath(hdr_name);
		XYZ side(in_side), up(in_up);
		XYZ front = side^up;
		db->initialize(hdr_name, *in_exposure, side, up, front);
	}
	
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[6+i]);
	}

	XYZ C = db->f_sh->computeSHLighting(coeff, Nn);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_occlusion_f) 
{
	float *result = (float *)argv[0];
	
	CPRTData *db = ( CPRTData *) initdata;
	
	XYZ Nn((float *)argv[1]);
	
	XYZ coeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		coeff[i] = XYZ((float *)argv[i+2]);
	}

	float occ = db->f_sh->computeSrcSHLighting(coeff, Nn);
	
	// output
	result[0] = occ/3.14f;
	
	return 0;
}

extern "C" SHADEOP(sh_lighting_src_d_f) 
{
	float *result = (float *)argv[0];
	
	CPRTData *db = ( CPRTData *) initdata;
	
	XYZ Nn((float *)argv[1]);
		
	XYZ srcCoeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		srcCoeff[i] = XYZ((float *)argv[i+2]);
	}
	
	XYZ shdCoeff[SH_N_BASES];
	for(int i =0; i<SH_N_BASES; i++) 
	{
		shdCoeff[i] = XYZ((float *)argv[i+18]);
	}

	XYZ C = db->f_sh->computeSHLighting(srcCoeff, shdCoeff, Nn);
	
	// output
	result[0] = C.x;
	result[1] = C.y;
	result[2] = C.z;
	
	return 0;
}
///:~