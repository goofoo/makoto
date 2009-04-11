#pragma once
#include "zMath.h"

class rayCast
{
public:
	rayCast(void);
	~rayCast(void);
	
	static int ray_box_intersection(XYZ origin, XYZ ray, float boxmin[3], float boxmax[3], float offset, float& dist);
};
