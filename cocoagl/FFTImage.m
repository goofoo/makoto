//
//  FFTImage.m
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "FFTImage.h"
#include "fft.h"
#include "image.h"

@implementation FFTImage
- (id) init
{
	[super init];
	modelName = @"FFTImage";
	glInited = 0;
	return self;
}

- (NSString *) name
{
	return @"FFTImage";
}

- (void) draw
{
	glBindTexture(GL_TEXTURE_2D, teximg);
	glBegin(GL_QUADS);
	glMultiTexCoord3f(GL_TEXTURE0, 0, 0, 0);
	glVertex3f(-1, -1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 1, 0, 0);
	glVertex3f(1, -1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 1, 1, 0);
	glVertex3f(1, 1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 0, 1, 0);
	glVertex3f(-1, 1, 0);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, fftimg);
	glTranslatef(2.01,0,0);
	glBegin(GL_QUADS);
	glMultiTexCoord3f(GL_TEXTURE0, 0, 0, 0);
	glVertex3f(-1, -1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 1, 0, 0);
	glVertex3f(1, -1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 1, 1, 0);
	glVertex3f(1, 1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 0, 1, 0);
	glVertex3f(-1, 1, 0);
	glEnd();
}

- (void)initGL
{
	int w, h, u, v;
	checkEXRDim("/Users/jianzhang/Desktop/tomcat.exr", &w, &h);
	float *texels = malloc(w*h*sizeof(float));
	readEXRRED("/Users/jianzhang/Desktop/tomcat.exr", w,h, texels);
	
	glGenTextures(1, &teximg);	
	glBindTexture(GL_TEXTURE_2D, teximg);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, w, h, 0, GL_LUMINANCE, GL_FLOAT, texels);
	
	COMPLEX *comimg = malloc(w*h*sizeof(COMPLEX));

	for(v=0; v<h; v++) {
			for(u=0; u<w; u++) {
				comimg[v*w+u].real = 0;
				comimg[v*w+u].imag = texels[v*w+u];
			}
		}
		
	FFT2D(comimg, w, h, 1);
	
	for(v=0; v<h; v++) {
			for(u=0; u<w; u++) {
				texels[Modi(v-h/2,h)*w+Modi(u-w/2,w)] = (w+h)/2*sqrt(comimg[v*w+u].real*comimg[v*w+u].real + comimg[v*w+u].imag * comimg[v*w+u].imag);
			}
		}
		
	glGenTextures(1, &fftimg);	
	glBindTexture(GL_TEXTURE_2D, fftimg);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, w, h, 0, GL_LUMINANCE, GL_FLOAT, texels);
	
	
	free(texels);
	
	glInited = 1;
}
@end
