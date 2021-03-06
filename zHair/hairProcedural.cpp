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

float glb_attrib[5];

extern "C" {
    export RtPointer ConvertParameters(RtString paramstr);
    export RtVoid Subdivide(RtPointer data, RtFloat detail);
    export RtVoid Free(RtPointer data);
	export RtVoid SubdivideReal(RtPointer data, RtFloat detail);
	export RtVoid SubdivideReal1(RtPointer data, RtFloat detail);
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
	
	ddensmap fmap;
	fmap.isNil = 1;
	
	pdata->setDensity(fmap);
	
	if(pdata->isShadow() ==0) pdata->setDepthPersp();
	else pdata->setDepthOrtho();

	unsigned numtri = pdata->getNumTriangle();
	float box[6], dist;
	RtBound bound = {-1, 1, -1, 1, -1, 1};
	float kink = pdata->getKink();
	XYZ disp;

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
		
		if(pdata->isShadow() != 1) { if(!pdata->isVisible(box, 1)) continue; }
		else { if(!pdata->isVisible(box, 0)) continue; }
		
		bound[0] = box[0];
		bound[1] = box[1];
		bound[2] = box[2];
		bound[3] = box[3];
		bound[4] = box[4];
		bound[5] = box[5];
		
		pdata->setAttrib(glb_attrib);
/*
		if(pdata->has_densmap) {
			param->densities = new float[3];
			pdata->pHair->lookupDensity(i, param->densities);
		}
*/		
		param->densmap = fmap;
		
		if(pdata->isShadow() == 1) RiProcedural((RtPointer)param, bound, SubdivideReal, FreeReal);
		else {
			dist = pdata->clostestDistance(param->points);
			
			param->distantSimp = 1.f;
			
// depth of field simplification
				pdata->simplifyDOF(i, dist, param->distantSimp);
			
			RiTransformBegin();
			if(pdata->needMBlur()) {
				pdata->meanDisplace(i, disp);
				
// motion blur simplification
				pdata->simplifyMotion(i, param->distantSimp);
				
				RiMotionBegin(2, (RtFloat)pdata->getShutterOpen(), (RtFloat)pdata->getShutterClose());
				RiTranslate( 0,0,0 );
				RiTranslate(disp.x, disp.y, disp.z);
				RiMotionEnd();
			}
			
			RiProcedural((RtPointer)param, bound, SubdivideReal1, FreeReal);
			RiTransformEnd();
		}
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
	param->distantSimp = 1.f;
	if(detail < 9000) {
		if(detail <1000) param->distantSimp = 0.2;
		else param->distantSimp = 1.0 - 0.8*(9000 - detail)/8000;
	}
	param->emit(glb_attrib, (float)detail);
}

export RtVoid
SubdivideReal1(RtPointer data, RtFloat detail)
{
	BoundHair* param = static_cast<BoundHair*>(data);
	param->emit(glb_attrib, (float)detail);
}

export RtVoid
FreeReal(RtPointer data)
{
	BoundHair* param = static_cast<BoundHair*>(data);
	param->release();
    delete param;
}