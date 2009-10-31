/*
 *  FNoise.cpp
 *  hair
 *
 *  Created by jian zhang on 5/13/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "FNoise.h"

unsigned int FNoise::xRand;
unsigned int FNoise::yRand;
unsigned int FNoise::zRand;

float FNoise::randomd()
{
	float result;
    unsigned int a = xRand/137, b = xRand%137;
    unsigned int c = yRand/139, d = yRand%139;
    unsigned int e = zRand/138, f = zRand%138;

    xRand = (131 * b - 2 * a) % 30253;
    yRand = (132 * d - 35 * c) % 30313;
    zRand = (130 * f - 63 * e) % 30329;

    result = (float) xRand/30269.0f +(float) yRand/30307.0f +(float) zRand/30323.0f;
    return result - ((int) result);
}

void FNoise::seedd(unsigned int nx, unsigned int ny, unsigned int nz)
{
	xRand = nx;
    yRand = ny;
    zRand = nz;
    randomd();
    randomd();
    randomd();
    //randomd();
}

float FNoise::randfint( int i )
{
	int atoi = i;
	
	unsigned int seed = (unsigned int)atoi;
		
	seedd( seed, seed * 13, seed * 61 );
		
	float r = randomd();
	
	return r;

}

void FNoise::sphereRand(float& x, float& y, float& z, float r, unsigned int &i)
{
	unsigned int seed = i;
		
	seedd( seed, seed * 13, seed * 61 );
	
	float theta = randomd() * 2.f * 3.1415927f;
	
	seedd( seed, seed * 23, seed * 41 );
	
	float u = randomd() * 2.f - 1.0f;
	
	x = cos(theta) * sqrt(1.0-(u*u)) * r;
	y = sin(theta) * sqrt(1.0-(u*u)) * r;
	z = u * r;
	
	seedd( seed, seed * 13, seed * 29 );
	
	float s = randomd();
	
	x *= s; y *= s; z *= s;
}