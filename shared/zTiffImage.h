#ifndef _Z_TIFF_IMAGE_H
#define _Z_TIFF_IMAGE_H

#include "tiffio.h"

class zTiffImage
{
public:
	zTiffImage();
	zTiffImage(const char* filename);
	~zTiffImage();
	
	char load(const char* filename);
	
	int getWidth() { return (int)width; }
	int getHeight() { return (int)height; }
	int getSamplesPerPixel() { return (int)samples_per_pixel; }

private:
	uint32 width, height;
	uint16 samples_per_pixel;
	uint16 bits_per_sample;
	uint32 size;
	uint16 config;

	float* m_data;
};
#endif