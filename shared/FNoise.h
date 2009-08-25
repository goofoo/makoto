/*
 *  FNoise.h
 *  hair
 *
 *  Created by jian zhang on 5/13/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _F_NOISE_H
#define _F_NOISE_H

class FNoise
{
public:
	FNoise() {}
	~FNoise() {}
	
	float randfint( int& i );
	
private:
	static unsigned int xRand, yRand, zRand;     /* seed */
	float randomd();
	void seedd(unsigned char nx, unsigned char ny, unsigned char nz);
};

#endif