#ifndef ZFN_EXR_H
#define ZFN_EXR_H

#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>
#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfInputFile.h>

using namespace Imf;
using namespace Imath;

class ZFnEXR
{
public:
	ZFnEXR(void);
	~ZFnEXR(void);
	static void checkFile(const char* filename, int* width, int* height);
	static void readR(const char* filename, int width, int height, float* data);
	static void save(float* data, const char* filename, int width, int height);
};
#endif
