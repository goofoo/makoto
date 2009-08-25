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

    result = (float) xRand/30269.0f + yRand/30307.0f + zRand/30323.0f;
    return result - ((int) result);
}

void FNoise::seedd(unsigned char nx, unsigned char ny, unsigned char nz)
{
	xRand = nx;
    yRand = ny;
    zRand = nz;
    randomd();
    randomd();
    randomd();
    //randomd();
}

float FNoise::randfint( int& i )
{
	int atoi = i;
	
	unsigned char seed = (unsigned char)atoi;
		
	seedd( seed, seed * 31, seed * 71 );
		
	float r = randomd();
	
	return r;

}
