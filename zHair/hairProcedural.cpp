#include <stdlib.h>
#include <ri.h>

#include "RHair.h"

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
    RHair* pdata = new RHair(parameters);
    return static_cast<RtPointer>(pdata);
}

export RtVoid
Subdivide(RtPointer data, RtFloat detail)
{
	RHair* pdata = static_cast<RHair*>(data);
	pdata->generateRIB(detail);
}

export RtVoid
Free(RtPointer data)
{
	RHair* pdata = static_cast<RHair*>(data);
    delete pdata;
}
