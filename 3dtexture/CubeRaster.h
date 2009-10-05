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

class CubeRaster
{
public:
	CubeRaster();
	~CubeRaster();
	
	void clear();
	void write(XYZ& ray);
	void write(XYZ& ray, float val);
	void draw() const;
	void getLight(const float& opacity, float& lighting) const;
	void readLight(XYZ& ray, float& lighting) const;
private:
	float* m_pOpacity;
};

#endif