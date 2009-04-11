#include "zimage.h"

zImage::zImage(void)
{
}

zImage::~zImage(void)
{
	//destroy();
}

void zImage::initialize(TIFF* tif)
{
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width); 
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
	
	printf("%i X %i %i-bit image\n", width, height, bits_per_sample);
	
	size = width * height * samples_per_pixel;
}

void zImage::read(float* data, TIFF* tif)
{
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
						data[acc + i] = (float)fscanline[i]/255.0f;
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
						data[acc + i] = (float)fscanline[i]/65535.0f;
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
						data[acc + i] = fscanline[i];
					}
					acc += width*samples_per_pixel;
				}
			}
			delete[] fscanline;
			break;
		}
	}
	
	return;
}

void zImage::read(uint8* data, TIFF* tif)
{
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
						data[acc + i] = fscanline[i];
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
						data[acc + i] = uint8((float)fscanline[i]/65535*255);
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
						data[acc + i] = uint8(fscanline[i]*255);
					}
					acc += width*samples_per_pixel;
				}
			}
			delete[] fscanline;
			break;
		}
	}
	
	return;
}

void zImage::write(const float* data, int w, int h, int s, TIFF* tif)
{
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, s);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	
	if(s==1) 
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	else 
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	
	int leng = w*h*s;
	uint8* buf = new uint8[leng];
	
	for(int i = 0; i< leng; i++)
	{
		buf[i] = uint8( 255*data[i]);
	}
	
	TIFFWriteEncodedStrip(tif, 0, buf, sizeof(uint8) * leng);
	
	delete[] buf;
}

void zImage::write(const uint8* data, int w, int h, int s, TIFF* tif)
{
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, s);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	
	if(s==1) 
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	else 
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	//TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	
	int leng = w*h*s;
	uint8* buf = new uint8[leng];
	
	for(int i = 0; i< leng; i++)
	{
		buf[i] = data[i];
	}
	
	TIFFWriteEncodedStrip(tif, 0, buf, sizeof(uint8) * leng);
	
	delete[] buf;
}

void zImage::getStrip(const uint8* in, uint8* out, int start_x, int start_y, int limit_x, int limit_y)
{
	int in_x, in_y;
	for(int j=0; j<1024; j++)
	{
		in_y = start_y + j;
		if(in_y>=limit_y) in_y = limit_y-1;
		
		for(int i=0; i<1024; i++)
		{
			in_x = start_x + i;
			if(in_x>=limit_x) in_x = limit_x-1;
		
			out[(i+j*1024)*3] = in[(in_x + in_y*limit_x)*3];
			out[(i+j*1024)*3+1] = in[(in_x + in_y*limit_x)*3+1];
			out[(i+j*1024)*3+2] = in[(in_x + in_y*limit_x)*3+2];
		}
	}
}

void zImage::setStrip(const uint8* in, uint8* out, int start_x, int start_y, int limit_x, int limit_y)
{
	int out_x, out_y;
	for(int j=0; j<1024; j++)
	{
		out_y = start_y + j;
		if(out_y<limit_y)
		{
			for(int i=0; i<1024; i++)
			{
				out_x = start_x + i;
				if(out_x<limit_x)
				{
					out[(out_x+out_y*limit_x)*3] = in[(i + j*1024)*3];
					out[(out_x+out_y*limit_x)*3+1] = in[(i + j*1024)*3+1];
					out[(out_x+out_y*limit_x)*3+2] = in[(i + j*1024)*3+2];
				}
			}
		}
	}
}
/*
GLuint zImage::createTextureSingle(GLenum target, GLenum internalformat, GLenum format, int width, int height, float* data)
{
	GLuint texid;
  glGenTextures(1, &texid);

  glBindTexture(target, texid);

  glTexImage2D(target, 0, internalformat, width, height, 0, format, GL_FLOAT, data);

    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
  return texid;

}
*/
//:~