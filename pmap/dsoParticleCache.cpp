#include <stdio.h> 
#include <stdlib.h> 

#include "RPTC.h"
#include "RCloud.h"

#ifdef _WIN32
#define export __declspec(dllexport)
#define strtok_r(str,delim,saveptr) strtok((str),(delim))
#else
#define export
#endif

extern "C" { 
/* Declarations */ 
	export RtPointer ConvertParameters(RtString paramstr); 
	export RtVoid Subdivide(RtPointer data, float detail); 
	export RtVoid Free(RtPointer data); 
	export RtVoid Subdivide_real(RtPointer data, float detail); 
	export RtVoid Free_real(RtPointer data); 
}

export RtPointer ConvertParameters(RtString paramstr) 
{
	RPTC *data = new RPTC(paramstr);
	return data; 
} 

export RtVoid Subdivide(RtPointer blinddata, RtFloat detailsize) 
{ 
	RPTC *data = static_cast<RPTC*>(blinddata);
	data->init();

	RCloud* realdata;
	RtBound bound = {-.5, .5, -.5, .5, -.5, .5}; 
	RGRID aptc;
	float r;
	
	unsigned num_range = data->getNumData();
	HighNLow* range = new HighNLow[num_range];
	data->getIndex(range);
	
	for(unsigned i=0; i< num_range; i++) {
		for(unsigned j=range[i].low; j<=range[i].high; j++) {
			realdata = new RCloud();
			
			realdata->setDensity(data->getDensity());
			
			data->sampleById(j, aptc);
			
			r = sqrt(aptc.area*0.25);
			
			realdata->setRadius(r);
			r+= r;
			
			RiTransformBegin(); 
			
			RiTranslate(aptc.pos.x, aptc.pos.y, aptc.pos.z);
			
			bound[0] = -r; bound[1] = r;
			bound[2] = -r; bound[3] = r;
			bound[4] = -r; bound[5] = r;
			
			RiProcedural(realdata, bound, Subdivide_real, Free_real); 
			
			RiTransformEnd();
		}
	}
	delete[] range;
} 

export RtVoid Subdivide_real(RtPointer blinddata, RtFloat detailsize) 
{ 
	RCloud *data = static_cast<RCloud*>(blinddata);
	data->emit(detailsize);
}
 
export RtVoid Free(RtPointer blinddata) 
{ 
	RPTC *data = static_cast<RPTC*>(blinddata); 
	delete data;
} 

export RtVoid Free_real(RtPointer blinddata) 
{ 
	RCloud *data = static_cast<RCloud*>(blinddata); 
	delete data;
}

