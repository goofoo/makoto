#include "zFnEXR.h"

ZFnEXR::ZFnEXR(void)
{
}

ZFnEXR::~ZFnEXR(void)
{
}

void ZFnEXR::save(float* data, const char* filename, int width, int height)
{
	Array2D<Rgba> px (height, width);

	 for (int y = 0; y < height; ++y)
    {
		int invy = height-1-y;
	for (int x = 0; x < width; ++x)
	{
	    Rgba &p = px[y][x];
	    p.r = data[(invy*width+x)*4];
	    p.g = data[(invy*width+x)*4+1];
	    p.b = data[(invy*width+x)*4+2];
	    p.a = data[(invy*width+x)*4+3];
	}
    }

	RgbaOutputFile file (filename, width, height, WRITE_RGBA);
    file.setFrameBuffer (&px[0][0], 1, width);
    file.writePixels (height);
}
