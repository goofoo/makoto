#include <stdlib.h>
#include <ri.h>

#include "RVane.h"

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
}

export RtPointer
ConvertParameters(RtString paramstr)
{
	std::string parameters(paramstr);
    RVane* pdata = new RVane(parameters);
    return static_cast<RtPointer>(pdata);
}

export RtVoid
Subdivide(RtPointer data, RtFloat detail)
{
	RVane* pdata = static_cast<RVane*>(data);
	pdata->generateRIB(detail);
}

export RtVoid
Free(RtPointer data)
{
	RVane* pdata = static_cast<RVane*>(data);
    delete pdata;
}
