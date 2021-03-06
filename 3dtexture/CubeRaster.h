/*
 *  CubeRaster.h
 *  pmap
 *
 *  Created by jian zhang on 9/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CUBERASTER_H
#define _CUBERASTER_H

#include "../shared/zData.h"
#define CUBERASTER_MAXANGLE 0.125
#define CUBERASTER_MAXANGLEINV 10

class CubeRaster
{
public:
	CubeRaster();
	~CubeRaster();
	
	void clear();
	void compose();
	void writemip(XYZ& ray, float val, int detail);
	void write2(XYZ& ray, float val);
	void write4(XYZ& ray, float val);
	void write8(XYZ& ray, float val);
	void write(XYZ& ray, float val);
	void draw() const;
	void getLight(const float& opacity, float& lighting) const;
	void readLight(XYZ& ray, float& lighting) const;
private:
	float* m_pOpacity;
	float* fOpacity2;
	float* fOpacity4;
	float* fOpacity8;
};

#endif