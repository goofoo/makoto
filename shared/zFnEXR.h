#ifndef ZFN_EXR_H
#define ZFN_EXR_H

#include <ImfRgbaFile.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfInputFile.h>
#include <ImfChannelList.h>
#include <ImfArray.h>

#include <ImfBoxAttribute.h>
#include <ImfChannelListAttribute.h>
#include <ImfCompressionAttribute.h>
#include <ImfChromaticitiesAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfEnvmapAttribute.h>
#include <ImfDoubleAttribute.h>
#include <ImfIntAttribute.h>
#include <ImfLineOrderAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfOpaqueAttribute.h>
#include <ImfStringAttribute.h>
#include <ImfVecAttribute.h>

using namespace std;
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
	static void saveCameraNZ(float* data, M44f mat, float fov, const char* filename, int width, int height);
	static void readCameraNZ(const char* filename, int width, int height, float* data, M44f& mat, float& fov);
};
#endif
