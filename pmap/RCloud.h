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
	void setDusty(float val) {dusty = val;}
	void setDetail(int val) {detail = (val+1)*2;}
	void setId(unsigned val) {idx = val;}
	void emit();
	void setCoe0(XYZ* data) {coe0 = data;}
	void setCoe1(XYZ* data) {coe1 = data;}
	
	void sphereRand(XYZ& q, int seed);
private:
	float radius, density, dusty;
	FNoise noise;
	int detail;
	unsigned idx;
	XYZ* coe0;
	XYZ* coe1;
};
#endif
