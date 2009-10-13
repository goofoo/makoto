#ifndef _R_CLOUD_H
#define _R_CLOUD_H

#include <stdio.h>
#include <string>
#include "ri.h"

class RCloud {
public:
	RCloud();
	~RCloud() {}
	void setRadius(float& val) {radius = val;}
	void emit();
private:
	float radius;
};
#endif
