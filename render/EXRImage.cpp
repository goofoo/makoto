#include "EXRImage.h"

EXRImage::EXRImage(void)
{
}

EXRImage::~EXRImage(void)
{
}

void EXRImage::saveRGB(const char* filename, int width, int height, const half* rPixels, const half* gPixels, const half* bPixels )
{
	Header header (width, height); 
 
	header.channels().insert ("R", Channel (HALF)); 
	header.channels().insert ("G", Channel (HALF)); 
	header.channels().insert ("B", Channel (HALF));
	
	OutputFile file (filename, header);
	 
	FrameBuffer frameBuffer; 
	
	frameBuffer.insert ("R",                                // name 
						Slice (HALF,                        // type 
							   (char *) rPixels,            // base 
							   sizeof (*rPixels) * 1,       // xStride 
							   sizeof (*rPixels) * width)); // yStride 
	frameBuffer.insert ("G",                                // name 
						Slice (HALF,                        // type 
							   (char *) gPixels,            // base 
							   sizeof (*gPixels) * 1,       // xStride 
							   sizeof (*gPixels) * width)); // yStride
	frameBuffer.insert ("B",                                // name 
						Slice (HALF,                        // type 
							   (char *) bPixels,            // base 
							   sizeof (*bPixels) * 1,       // xStride 
							   sizeof (*bPixels) * width)); // yStride
							   
	file.setFrameBuffer (frameBuffer); 
	file.writePixels (height);

	//RgbaOutputFile file (filename, width, height, WRITE_RGBA);
    //file.setFrameBuffer (&px[0][0], 1, width);
    //file.writePixels (height);
}
