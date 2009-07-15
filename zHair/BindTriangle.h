/*
 *  BindTriangle.h
 *  hair
 *
 *  Created by jian zhang on 6/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _BIND_TRIANGLE_H
#define _BIND_TRIANGLE_H

#include "../shared/zData.h"

struct triangle_bind_info
{
	unsigned idx[3];
	float wei[3];
};

class BindTriangle
{
public:
	BindTriangle() {}
	~BindTriangle() {}
	
	static char set(const XY* corner, const XY& at, triangle_bind_info& res);
	static float barycentric_coord(float ax, float ay, float bx, float by, float x, float y);
	
};
#endif
