#include "TIFFImage.h"

TIFFImage::TIFFImage(void): m_data(0)
{
}

TIFFImage::TIFFImage(const char* filename): m_data(0)
{
	load(filename);
}

TIFFImage::~TIFFImage(void)
{
	if(m_data) delete[] m_data;
}

char TIFFImage::load(const char* filename)
{
	TIFF *tif = TIFFOpen(filename, "r");
	if(!tif) return 0;
	
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width); 
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);

	if(samples_per_pixel == 3)
		printf("%i X %i %i-bit RGB image\n", width, height, bits_per_sample);
	if(samples_per_pixel == 4)
		printf("%i X %i %i-bit RGBA image\n", width, height, bits_per_sample);
	else if(samples_per_pixel == 1)
		printf("%i X %i %i-bit Grayscale image\n", width, height, bits_per_sample);
	
	if(m_data) delete[] m_data;
	m_data = new half[width*height*samples_per_pixel];

	switch(bits_per_sample)
	{
		case 8:
		{
			uint8* fscanline = new uint8[width*samples_per_pixel];
			uint32 acc = 0;
			if (config == PLANARCONFIG_CONTIG) 
			{
			    	for (uint32 row = 0; row < height; row++)
			    	{
					TIFFReadScanline(tif, fscanline, row);
					for(uint32 i = 0; i< width*samples_per_pixel; i++)
					{
						m_data[acc + i] = (float)fscanline[i]/255.0f;
					}
					acc += width*samples_per_pixel;
				}
			}
			delete[] fscanline;
			break;
		}
		case 16:
		{
			uint16* fscanline = new uint16[width*samples_per_pixel];
			uint32 acc = 0;
			if (config == PLANARCONFIG_CONTIG) 
			{
			    	for (uint32 row = 0; row < height; row++)
			    	{
					TIFFReadScanline(tif, fscanline, row);
					for(uint32 i = 0; i< width*samples_per_pixel; i++)
					{
						m_data[acc + i] = (float)fscanline[i]/65535.0f;
					}
					acc += width*samples_per_pixel;
				}
			}
			delete[] fscanline;
			break;
		}
		case 32:
		{
			float* fscanline = new float[width*samples_per_pixel];
			uint32 acc = 0;
			if (config == PLANARCONFIG_CONTIG) 
			{
			    	for (uint32 row = 0; row < height; row++)
			    	{
					TIFFReadScanline(tif, fscanline, row);
					for(uint32 i = 0; i< width*samples_per_pixel; i++)
					{
						m_data[acc + i] = fscanline[i];
					}
					acc += width*samples_per_pixel;
				}
			}
			delete[] fscanline;
			break;
		}
	}
	
	TIFFClose(tif);

	return 1;
}
//:~