/*
 *  math.c
 *  cocoagl
 *
 *  Created by jian zhang on 1/4/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "sampleMath.h"

#define DOWNSAMPLEFILTERSIZE 32

float downSampleFilterCoeffs[32] = { 
0.000334,-0.001528, 0.000410, 0.003545,-0.000938,-0.008233, 0.002172, 0.019120, 
-0.005040,-0.044412, 0.011655, 0.103311,-0.025936,-0.243780, 0.033979, 0.655340, 
0.655340, 0.033979,-0.243780,-0.025936, 0.103311, 0.011655,-0.044412,-0.005040, 
0.019120, 0.002172,-0.008233,-0.000938, 0.003546, 0.000410,-0.001528, 0.000334};

#define UPSAMPLEFILTERSIZE 4

float upSampleFilterCoeffs[4] = { 0.125, 0.375, 0.375, 0.125 };

int ModIn(int x, int n) {int m=x%n; return (m<0) ? m+n : m;} 

float downSample2D(int x, int y, int width, int height, float *data)
{
	int i, u, v;
	float sum = 0;
	for(i = 0; i< DOWNSAMPLEFILTERSIZE; i++) {
		u = x*2 + i - 15; u = ModIn(u, width);
		v = y*2;
		sum += data[v*width+ u] * downSampleFilterCoeffs[i];
	}
	for(i = 0; i< DOWNSAMPLEFILTERSIZE; i++) {
		u = x*2;
		v = y*2 + i - 15; v = ModIn(v, height);
		sum += data[v*width+ u] * downSampleFilterCoeffs[i];
	}
	sum /= 2.0;
	return sum;
}

float upSample2D(int x, int y, int width, int height, float *data)
{
	int i, u, v;
	float sum = 0;
	for(i = 0; i< UPSAMPLEFILTERSIZE; i++) {
		u = x + i - 1; u = ModIn(u/2, width);
		v = y/2;
		sum += data[v*width+ u] * upSampleFilterCoeffs[i];
	}
	
	for(i = 0; i< UPSAMPLEFILTERSIZE; i++) {
		u = x/2;
		v = y + i - 1; v = ModIn(v/2, height);
		sum += data[v*width+ u] * upSampleFilterCoeffs[i];
	}

	sum /= 2.0;
	return sum;
}

float downSample3D(int x, int y, int z, int width, int height, int depth, float *data)
{
	int i, u, v, w;
	float sum = 0;
	for(i = 0; i< DOWNSAMPLEFILTERSIZE; i++) {
		u = x*2 + i - 15; u = ModIn(u, width);
		v = y*2;
		w = z*2;
		sum += data[w*width*height+ v*width + u] * downSampleFilterCoeffs[i];
	}
	for(i = 0; i< DOWNSAMPLEFILTERSIZE; i++) {
		u = x*2;
		v = y*2 + i - 15; v = ModIn(v, height);
		w = z*2;
		sum += data[w*width*height+ v*width+ u] * downSampleFilterCoeffs[i];
	}
	for(i = 0; i< DOWNSAMPLEFILTERSIZE; i++) {
		u = x*2;
		v = y*2;
		w = z*2 + i - 15; w = ModIn(w, depth);
		sum += data[w*width*height+ v*width+ u] * downSampleFilterCoeffs[i];
	}
	sum /= 3.0;
	return sum;
}

float upSample3D(int x, int y, int z, int width, int height, int depth, float *data)
{
	int i, u, v, w;
	float sum = 0;
	for(i = 0; i< UPSAMPLEFILTERSIZE; i++) {
		u = x + i - 1; u = ModIn(u/2, width);
		v = y/2;
		w = z/2;
		sum += data[w*width*height + v*width+ u] * upSampleFilterCoeffs[i];
	}
	
	for(i = 0; i< UPSAMPLEFILTERSIZE; i++) {
		u = x/2;
		v = y + i - 1; v = ModIn(v/2, height);
		w = z/2;
		sum += data[w*width*height + v*width+ u] * upSampleFilterCoeffs[i];
	}
	
	for(i = 0; i< UPSAMPLEFILTERSIZE; i++) {
		u = x/2;
		v = y/2;
		w = z + i - 1; w = ModIn(w/2, height);
		sum += data[w*width*height + v*width+ u] * upSampleFilterCoeffs[i];
	}

	sum /= 3.0;
	return sum;
}