#include <stdlib.h>
#include <ri.h>

#include "RFeather.h"

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
    RFeather* pdata = new RFeather(parameters);
    return static_cast<RtPointer>(pdata);
}

export RtVoid
Subdivide(RtPointer data, RtFloat detail)
{
	RFeather* pdata = static_cast<RFeather*>(data);
	pdata->generateRIB(detail);
}

export RtVoid
Free(RtPointer data)
{
	RFeather* pdata = static_cast<RFeather*>(data);
    delete pdata;
}
