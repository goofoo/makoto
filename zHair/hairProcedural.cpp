#include <stdlib.h>
#include <ri.h>

#include "RHair.h"
#include "BoundHair.h"

#ifdef _WIN32
#define export __declspec(dllexport)
#define strtok_r(str,delim,saveptr) strtok((str),(delim))
#else
#define export
#endif

extern "C" {
    export RtPointer ConvertParameters(RtString paramstr);
    export RtVoid Subdivide(RtPointer data, RtFloat detail);
    export RtVoid Free(RtPointer data);
	export RtVoid SubdivideReal(RtPointer data, RtFloat detail);
	export RtVoid FreeReal(RtPointer data);
}

export RtPointer
ConvertParameters(RtString paramstr)
{
	std::string parameters(paramstr);
    RHair* pdata = new RHair(parameters);
    return static_cast<RtPointer>(pdata);
}

export RtVoid
Subdivide(RtPointer data, RtFloat detail)
{
	RHair* pdata = static_cast<RHair*>(data);
	pdata->init();

	unsigned numtri = pdata->getNumTriangle();
	float box[6];
	RtBound bound = {-1, 1, -1, 1, -1, 1};
	float kink = pdata->getKink();

	for(unsigned i=0; i<numtri; i++) {
		BoundHair* param = new BoundHair();
		param->epsilon = pdata->getEpislon();
		param->seed = i;
		pdata->pHair->lookupTriangleBind(i, param->bindPoints);
		pdata->pHair->lookupTriangle(i, param->points);
		pdata->pHair->lookupTriangleUV(i, param->ucoord, param->vcoord);
		
		pdata->pHair->lookupGuiderNSeg(i, param->nsegs);
		
		param->cvs_a = new XYZ[param->nsegs[0]+1];
		param->cvs_b = new XYZ[param->nsegs[1]+1];
		param->cvs_c = new XYZ[param->nsegs[2]+1];
		
		pdata->pHair->lookupGuiderCVs(i, kink, param->cvs_a, param->cvs_b, param->cvs_c);
		
		param->calculateBBox(box);
		
		bound[0] = box[0];
		bound[1] = box[1];
		bound[2] = box[2];
		bound[3] = box[3];
		bound[4] = box[4];
		bound[5] = box[5];
		
		pdata->setAttrib(param->attrib);
		
		if(pdata->needMBlur()) {
			param->velocities = new XYZ[3];
			param->shutters = new float[2];
			pdata->setMBlur(i, param->velocities, param->shutters);
		}
		
		if(pdata->has_densmap) {
			param->densities = new float[3];
			pdata->pHair->lookupDensity(i, param->densities);
		}
		
		RiProcedural((RtPointer)param, bound, SubdivideReal, FreeReal);
	}
}

export RtVoid
Free(RtPointer data)
{
	RHair* pdata = static_cast<RHair*>(data);
    delete pdata;
}

export RtVoid
SubdivideReal(RtPointer data, RtFloat detail)
{
	BoundHair* param = static_cast<BoundHair*>(data);
	//param->emitBBox();
	//param->emitGuider();
	param->emit((float)detail);
	//printf(" dtl: %f",(float)detail);
}

export RtVoid
FreeReal(RtPointer data)
{
	BoundHair* param = static_cast<BoundHair*>(data);
	param->release();
    delete param;
}