#ifndef _EXR_IMAGE_H
#define _EXR_IMAGE_H

#include <ImfRgbaFile.h>
#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace Imf;
using namespace Imath;

class EXRImage
{
public:
	EXRImage(void);
	~EXRImage(void);
	static void saveRGB(const char* filename, int width, int height, const half* rPixels, const half* gPixels, const half* bPixels);
};
#endif
