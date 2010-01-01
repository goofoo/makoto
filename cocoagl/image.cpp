/*
 *  image.c
 *  triangle
 *
 *  Created by jian zhang on 1/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "image.h"


void checkEXRDim(const char* filename, int* width, int* height)
{
	InputFile file(filename); 
	Box2i dw = file.header().dataWindow(); 
	*width  = dw.max.x - dw.min.x + 1;
	*height = dw.max.y - dw.min.y + 1;
}

void readEXRRED(const char* filename, int width, int height, float* data)
{
	InputFile file(filename); 
	Box2i dw = file.header().dataWindow();
	
	int size = (width)*(height);
	
	half *rPixels = new half[size];
	
	FrameBuffer frameBuffer; 
	frameBuffer.insert ("R",                                  // name 
						Slice (HALF,                          // type 
							   (char *) rPixels, 
							   sizeof (*rPixels) * 1,    // xStride 
							   sizeof (*rPixels) * (width),// yStride 
							   1, 1,                          // x/y sampling 
							   0.0));                         // fillValue 
							   
	file.setFrameBuffer (frameBuffer); 
	file.readPixels (dw.min.y, dw.max.y); 
	
	for(int j=0; j<height; j++)
	for(int i=0; i<width; i++) {
		
		data[j*width+i] = rPixels[(height-1-j)*width+i];
		
	}
	
	delete[] rPixels;
}



