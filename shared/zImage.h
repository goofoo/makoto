#include "tiffio.h"
#include "math.h"

inline float clamp(float x) 
{
	if(x<0.) x = 0.;
	else if(x>1.) x = 1.;
	return x;
}

class zImage
{
public:
	zImage(void);
	~zImage(void);
	
	void initialize(TIFF* tif);
	void read(float* data, TIFF* tif);
	void read(uint8* data, TIFF* tif);
	void write(const float* data, int w, int h, int s, TIFF* tif);
	void write(const uint8* data, int w, int h, int s, TIFF* tif);
	void getStrip(const uint8* in, uint8* out, int start_x, int start_y, int limit_x, int limit_y);
	void setStrip(const uint8* in, uint8* out, int start_x, int start_y, int limit_x, int limit_y);

	uint32 getWidth() { return width; }
	uint32 getHeight() { return height; }
	uint16 getSamples() { return samples_per_pixel; }
	uint32 getSize() { return size; }
	
private:
	uint32 width, height;
	uint16 samples_per_pixel;
	uint16 bits_per_sample;
	uint32 size;
	uint16 config;
};
