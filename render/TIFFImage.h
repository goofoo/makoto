#ifndef _TIFF_IMAGE_H
#define _TIFF_IMAGE_H

#include <tiffio.h>
#include <half.h>

class TIFFImage
{
public:
	TIFFImage();
	TIFFImage(const char* filename);
	~TIFFImage();
	
	char load(const char* filename);
	
	int getWidth() const { return (int)width; }
	int getHeight() const { return (int)height; }
	int getSamplesPerPixel() const { return (int)samples_per_pixel; }
	
	const half* getData() const {return m_data;}

private:
	uint32 width, height;
	uint16 samples_per_pixel;
	uint16 bits_per_sample;
	uint32 size;
	uint16 config;

	half* m_data;
};
#endif