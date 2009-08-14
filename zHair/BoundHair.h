#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>

#include "../shared/zData.h"

class BoundHair {
public:
	BoundHair();
    ~BoundHair() {}
	
	void release();
	
	void calculateBBox(float *box) const;
    void emit(float detail) const;
	void emitGuider() const;
	void emitBBox() const;
	
	XYZ* points;
	XYZ* bindPoints;
	int seed;
	unsigned nsegs[3];
	XYZ* cvs_a;
	XYZ* cvs_b;
	XYZ* cvs_c;
	float epsilon;
	float* ucoord;
	float* vcoord;
	float* attrib;
	
	XYZ* velocities;
	float* shutters;
	float* densities;

private:
	void getPatParam(XYZ& p, const float& param, const unsigned& nseg, const XYZ* cvs) const;
};
