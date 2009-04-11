#ifndef __APPLE__

#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace Imf;
using namespace Imath;

#include <iostream>

using namespace std;


#endif

class ZFnEXR
{
public:
	ZFnEXR(void);
	~ZFnEXR(void);
	static void save(float* data, const char* filename, int width, int height);
};
