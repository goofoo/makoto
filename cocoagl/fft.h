/*
 *  fft.h
 *  triangle
 *
 *  Created by jian zhang on 1/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

typedef struct {
   double real,imag;
} COMPLEX;

int FFT2D(COMPLEX *c,int nx,int ny,int dir);

