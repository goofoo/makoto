/*
 *  image.c
 *  EXRViewer
 *
 *  Created by jian zhang on 6/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 
#include "image.h"

void loadEXR(const char* filename, int* width, int* height)
{
	InputFile file(filename); 
	Box2i dw = file.header().dataWindow(); 
	*width  = dw.max.x - dw.min.x + 1;
	*height = dw.max.y - dw.min.y + 1;
}

void readEXR(const char* filename, int width, int height, float* data)
{
	InputFile file(filename); 
	Box2i dw = file.header().dataWindow();
	
	int size = (width)*(height);
	
	half *rPixels = new half[size];
	half *gPixels = new half[size];
	half *bPixels = new half[size];
	
	FrameBuffer frameBuffer; 
	frameBuffer.insert ("R",                                  // name 
						Slice (HALF,                          // type 
							   (char *) rPixels, 
							   sizeof (*rPixels) * 1,    // xStride 
							   sizeof (*rPixels) * (width),// yStride 
							   1, 1,                          // x/y sampling 
							   0.0));                         // fillValue 
							   
	frameBuffer.insert ("G",                                  // name 
						Slice (HALF,                          // type 
							   (char *) gPixels, 
							   sizeof (*gPixels) * 1,    // xStride 
							   sizeof (*gPixels) * (width),// yStride 
							   1, 1,                          // x/y sampling 
							   0.0));                         // fillValue 
							   
	frameBuffer.insert ("B",                                  // name 
						Slice (HALF,                          // type 
							   (char *) bPixels, 
							   sizeof (*bPixels) * 1,    // xStride 
							   sizeof (*bPixels) * (width),// yStride 
							   1, 1,                          // x/y sampling 
							   0.0));                         // fillValue 
	
	file.setFrameBuffer (frameBuffer); 
	file.readPixels (dw.min.y, dw.max.y); 
	
	for(int i=0; i<size; i++) {
		
		data[i*4] = rPixels[i];
		data[i*4+1] = gPixels[i];
		data[i*4+2] = bPixels[i];
		data[i*4+3] = 1.f;
	}
	
	delete[] rPixels;
	delete[] gPixels;
	delete[] bPixels;
}


