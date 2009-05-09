/* Code from prman AppNote#23 for DSOs */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string>
#include <ri.h>

using namespace std;

#include "fishData.h"

#ifdef _WIN32
#define export __declspec(dllexport)
#else
#define export
#endif


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

	export RtPointer ConvertParameters(RtString paramstr);
	export RtVoid Subdivide(RtPointer data, RtFloat detail);
	export RtVoid Free(RtPointer data);
	
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


RtPointer
ConvertParameters(RtString paramstr)
{
  string parameters(paramstr);
    fishData* pdata = new fishData(parameters);
    return static_cast<RtPointer>(pdata);
}

RtVoid
Subdivide(RtPointer data, RtFloat detail)
{	
	fishData* pdata = static_cast<fishData*>(data);
	pdata->generateRIB(detail);
}

RtVoid
Free(RtPointer data)
{
	fishData* pdata = static_cast<fishData*>(data);
    delete pdata;
 
}


