/*
Syntax:
*/
#include "../shared/zData.h"
#include "../render/EXRImage.h"
#include "../render/TIFFImage.h"
#include <iostream>
using namespace std;

int main (int argc, char * const argv[]) {
    // insert code here...
    cout << "Fringe Map 0.1.0 06/15/09"<<endl;
	
	TIFFImage* tiff = new TIFFImage("foo.tif");
	int img_w = tiff->getWidth();
	int img_h = tiff->getHeight();
	int spp = tiff->getSamplesPerPixel();
	
	const half* data = tiff->getData();
	
	float* opacity = new float[img_w*img_h];
	half* rPixels = new half[img_w*img_h];
	half* gPixels = new half[img_w*img_h];
	half* bPixels = new half[img_w*img_h];
	
	for(int j=0; j<img_h; j++)
		for(int i=0; i<img_w; i++)
		{
			opacity[j*img_w+i] = (float)data[(j*img_w+i)*spp];
			rPixels[j*img_w+i] = gPixels[j*img_w+i] = 0.5f;
			bPixels[j*img_w+i] = 1.f;
		}
		
	XYZ dir;
	float len;
	for(int j=1; j<img_h-1; j++)
		for(int i=1; i<img_w-1; i++)
		{
			dir.x = opacity[j*img_w+i-1] - opacity[j*img_w+i+1];
			dir.y = opacity[(j+1)*img_w+i] - opacity[(j-1)*img_w+i];
			
			len = sqrt(dir.x*dir.x + dir.y*dir.y);
			dir.z = 1.0 - len;
			if(dir.z < 0) dir.z = 0;
			dir.normalize();
			
			rPixels[j*img_w+i] = dir.x/2 + 0.5;
			gPixels[j*img_w+i] = dir.y/2 + 0.5;
			bPixels[j*img_w+i] = dir.z/2 + 0.5;
		}
	
	EXRImage::saveRGB("foo.exr", img_w, img_h, rPixels, gPixels, bPixels);

	return 0;

}
//:~