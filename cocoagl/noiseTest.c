/*
 *  shader.c
 *  triangle
 *
 *  Created by jian zhang on 12/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <OpenGL/glext.h>

#include "noiseTest.h"
#include "fft.h"
#include "image.h"

const char *vert_source =

"varying vec3  TexCoord;"

"void main(void)"
"{"
"    TexCoord        = gl_MultiTexCoord0.xyz;"
"    gl_Position     = ftransform();"
"}";

const char *frag_source =
"uniform sampler1D FilterKernel;"
"uniform sampler2D WhiteNoise;"

"varying vec3  TexCoord;"

"void main (void)"
"{" 
"	float iter;"
"	float sum = 0.0;"
"	float s = floor(TexCoord.x * 32.0) /32.0;"
"	float t = floor(TexCoord.y * 32.0) /32.0;"
"	for(iter = 0.0; iter < 32.0; iter += 1.0) {"
"		sum += texture2D(WhiteNoise, vec2(s,t) + vec2((iter-16.0)/64.0, 0.0)).r  * texture1D(FilterKernel, (iter-16.0)/32.0).r;"
"		sum += texture2D(WhiteNoise, vec2(s,t) + vec2( 0.0, (iter-16.0)/64.0)).r  * texture1D(FilterKernel, (iter-16.0)/32.0).r;"
"}"
"sum /= 2.0;"
//"sum = texture2D(WhiteNoise, TexCoord.xy).r - sum;"
"    gl_FragColor = vec4 (sum, sum, sum,  1.0);"

"}";

GLuint program;
GLuint texname;
GLuint dnname;
GLuint upname;
GLuint fnname;
GLuint kerneltex;

float aCoeffs[32] = { 
0.000334,-0.001528, 0.000410, 0.003545,-0.000938,-0.008233, 0.002172, 0.019120, 
-0.005040,-0.044412, 0.011655, 0.103311,-0.025936,-0.243780, 0.033979, 0.655340, 
0.655340, 0.033979,-0.243780,-0.025936, 0.103311, 0.011655,-0.044412,-0.005040, 
0.019120, 0.002172,-0.008233,-0.000938, 0.003546, 0.000410,-0.001528, 0.000334};

float pCoeffs[4] = { 0.125, 0.375, 0.375, 0.125 };

#define DNFILTERSIZE 32
#define UPFILTERSIZE 4

#define WIDTH	512
#define HEIGHT	512

#define DNWIDTH	256
#define DNHEIGHT 256

int Modi(int x, int n) {int m=x%n; return (m<0) ? m+n : m;} 

float downsample(int x, int y, float *data)
{
	int i, u, v;
	float sum = 0;
	for(i = 0; i< DNFILTERSIZE; i++) {
		u = x*2 + i - 15; u = Modi(u, WIDTH);
		v = y*2;
		sum += data[v*WIDTH+ u] * aCoeffs[i];
	}
	for(i = 0; i< DNFILTERSIZE; i++) {
		u = x*2;
		v = y*2 + i - 15; v = Modi(v, HEIGHT);
		sum += data[v*WIDTH+ u] * aCoeffs[i];
	}
sum /= 2.0;
	return sum;
}

float upsample(int x, int y, float *data)
{
	int i, u, v;
	float sum = 0;
	for(i = 0; i< UPFILTERSIZE; i++) {
		u = x + i - 2; u = Modi(u/2, DNWIDTH);
		v = y/2;
		sum += data[v*DNWIDTH+ u] * pCoeffs[i];
	}
	
	
	for(i = 0; i< UPFILTERSIZE; i++) {
		u = x/2;
		v = y + i - 2; v = Modi(v/2, DNWIDTH);
		sum += data[v*DNWIDTH+ u] * pCoeffs[i];
	}

	sum /= 2.0;
	return sum;
}

GLint initShaders()
{
	GLuint vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_source, 0);
	glCompileShader(vertex_shader);

	GLuint fragment_shader   = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_source, 0);
	glCompileShader(fragment_shader);
	
	program = glCreateProgram();
	
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
	glLinkProgram(program);
	
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	
	return linked;
}

void initTextures()
{
	glGenTextures(1, &texname);	
	glBindTexture(GL_TEXTURE_2D, texname);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	float *texels = malloc(WIDTH*HEIGHT*sizeof(float));
	int u, v;
	
		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				texels[v*WIDTH+u] = (float)(random()%1931)/1931.f;
				texels[v*WIDTH+u] = sin(3.14*(float)(u+0.5)/128.f*16.f)*0.5+0.5;
				//texels[v*WIDTH+u] *= sin(3.14*(float)v/128.f*6.f+3.14)*0.5+0.5;
				//texels[v*WIDTH+u] = 0;
			}
		}
		
		for(v=50; v<78; v++) 
			for(u=50; u<78; u++) texels[v*WIDTH+u] = 1;
	readEXRRED("/Users/jianzhang/Desktop/Enterprise128.exr", WIDTH, HEIGHT, texels);
	readEXRRED("/Users/jianzhang/Desktop/tomcat.exr", WIDTH, HEIGHT, texels);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_FLOAT, texels);
	


float *dns = malloc(DNWIDTH*DNHEIGHT*sizeof(float));
	
		for(v=0; v<DNHEIGHT; v++) {
			for(u=0; u<DNWIDTH; u++) {
				dns[v*DNWIDTH+u] = downsample(u, v, texels);
			}
		}
		
			glGenTextures(1, &dnname);	
	glBindTexture(GL_TEXTURE_2D, dnname);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, DNWIDTH, DNHEIGHT, 0, GL_LUMINANCE, GL_FLOAT, dns);
	
		float *ups = malloc(WIDTH*HEIGHT*sizeof(float));

		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				ups[v*WIDTH+u] = upsample(u, v, dns);
			}
		}
		
		glGenTextures(1, &upname);	
	glBindTexture(GL_TEXTURE_2D, upname);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_FLOAT, ups);
	
	float *fine = malloc(WIDTH*HEIGHT*sizeof(float));
	
	float *tmp = malloc(WIDTH*HEIGHT*sizeof(float));
	
		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				fine[v*WIDTH+u] = texels[v*WIDTH+u] - ups[v*WIDTH+u];
				tmp[v*WIDTH+u] = fine[v*WIDTH+u];
			}
		}
		int offset = WIDTH/2 + 1;
		
		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				fine[v*WIDTH+u] +=tmp[Modi(v+offset, HEIGHT)*WIDTH+Modi(u+offset, WIDTH)];
				fine[v*WIDTH+u] = 0.5 + fine[v*WIDTH+u]*0.5;
			}
		}
		
		glGenTextures(1, &fnname);	
	glBindTexture(GL_TEXTURE_2D, fnname);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_FLOAT, fine);
	
	COMPLEX *comimg = malloc(WIDTH*HEIGHT*sizeof(COMPLEX));
	
	
	for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				comimg[v*WIDTH+u].real = 0;
				comimg[v*WIDTH+u].imag = texels[v*WIDTH+u];
			}
		}
		
	FFT2D(comimg, WIDTH, HEIGHT, 1);
	
	for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				fine[Modi(v+64,HEIGHT)*WIDTH+Modi(u+64,WIDTH)] = 64*sqrt(comimg[v*WIDTH+u].real*comimg[v*WIDTH+u].real + comimg[v*WIDTH+u].imag * comimg[v*WIDTH+u].imag);
			}
		}
glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_FLOAT, fine);
	

	free(comimg);	
	free(ups);
	free(texels);
	free(dns);
	free(fine);
	
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texname);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dnname);
	
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "WhiteNoise"), 0);
	glBindTexture(GL_TEXTURE_2D, texname);
	
	glGenTextures(1, &kerneltex);	
	glBindTexture(GL_TEXTURE_1D, kerneltex);	
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	
glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE32F_ARB, 32, 0, GL_LUMINANCE, GL_FLOAT, aCoeffs);

 //glEnable(GL_TEXTURE_1D);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_1D, kerneltex);
	
	glUniform1i(glGetUniformLocation(program, "FilterKernel"), 1);
}

void drawTexQuad()
{glUseProgram(0);
//glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texname);
	glColor4f(1,1,1,1);
	glTranslatef(-1.01,1.01,0);
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
	
//glActiveTexture(GL_TEXTURE1);	
glBindTexture(GL_TEXTURE_2D, dnname);
	glTranslatef(2.02,0,0);
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
	
	glBindTexture(GL_TEXTURE_2D, upname);
	glTranslatef(0,-2.02,0);
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
	
	glBindTexture(GL_TEXTURE_2D, fnname);
	glTranslatef(-2.02,0,0);
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