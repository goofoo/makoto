/*
 *  fft.c
 *  triangle
 *
 *  Created by jian zhang on 1/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "fft.h"

int Powerof2(int n,int *m,int *twopm)
{
	if (n <= 1) {
		*m = 0;
		*twopm = 1;
		return(0);
	}

   *m = 1;
   *twopm = 2;
   do {
      (*m)++;
      (*twopm) *= 2;
   } while (2*(*twopm) <= n);

   if (*twopm != n) 
		return(0);
	else
		return(1);
}


int FFT(double *x,double *y,int m, int dir)
{
   long nn,i,i1,j,k,i2,l,l1,l2;
   double c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   nn = 1;
   for (i=0;i<m;i++)
      nn *= 2;

   /* Do the bit reversal */
   i2 = nn >> 1;
   j = 0;
   for (i=0;i<nn-1;i++) {
      if (i < j) {
         tx = x[i];
         ty = y[i];
         x[i] = x[j];
         y[i] = y[j];
         x[j] = tx;
         y[j] = ty;
      }
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0;
   c2 = 0.0;
   l2 = 1;
   for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0;
      u2 = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<nn;i+=l2) {
            i1 = i + l1;
            t1 = u1 * x[i1] - u2 * y[i1];
            t2 = u1 * y[i1] + u2 * x[i1];
            x[i1] = x[i] - t1;
            y[i1] = y[i] - t2;
            x[i] += t1;
            y[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (dir == 1)
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      for (i=0;i<nn;i++) {
         x[i] /= (double)nn;
         y[i] /= (double)nn;
      }
   }

   return(1);
}



int FFT2D(COMPLEX *c,int nx,int ny,int dir)
{
   int i,j;
   int m,twopm;

   //double* real = new double[nx];
   double* real = malloc(nx*sizeof(double));
   //double* imag = new double[nx];
   double* imag = malloc(nx*sizeof(double));
   
   if (!Powerof2(nx,&m,&twopm) || twopm != nx)
      return(0);
   for (j=0;j<ny;j++) {
      for (i=0;i<nx;i++) {
         real[i] = c[j*nx+i].real;
         imag[i] = c[j*nx+i].imag;
      }
      FFT(real,imag,m,dir);
      for (i=0;i<nx;i++) {
         c[j*nx+i].real = real[i];
         c[j*nx+i].imag = imag[i];
      }
   }
   //delete[] real;
   free(real);
//delete[] imag;
	free(imag);
   
   //real = new double[ny];
  real = malloc(ny*sizeof(double));
   //imag = new double[ny];
   imag = malloc(ny*sizeof(double));
   
   if (!Powerof2(ny,&m,&twopm) || twopm != ny)
      return(0);
   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         real[j] = c[j*nx+i].real;
         imag[j] = c[j*nx+i].imag;
      }
      FFT(real,imag,m,dir);
      for (j=0;j<ny;j++) {
         c[j*nx+i].real = real[j];
         c[j*nx+i].imag = imag[j];
      }
   }
   //delete[] real;
   free(real);
//delete[] imag;
	free(imag);

   return(1);
}