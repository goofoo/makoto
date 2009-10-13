#ifndef _R_CLOUD_H
#define _R_CLOUD_H

#include <stdio.h>
#include <string>
#include "ri.h"

#include "../shared/zData.h"
#include "../shared/FNoise.h"

class RCloud {
public:
	RCloud();
	~RCloud() {}
	void setRadius(float& val) {radius = val;}
	void setDensity(float val) {density = val;}
	void emit(float detail);
	
	void sphereRand(XYZ& q, int seed);
private:
	float radius, density;
	FNoise noise;
};
#endif
