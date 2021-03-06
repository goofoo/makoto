#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ri.h>

#include "../shared/zData.h"

struct ddensmap {
	char isNil;
	int mapw, maph;
	float* data;
};

class BoundHair {
public:
	BoundHair();
    ~BoundHair() {}
	
	void release();
	
	void calculateBBox(float *box) const;
	void emit(const float* g_attrib, float detail) const;
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
	//float* attrib;
	
	//XYZ* velocities;
	//float* shutters;
	//float* densities;
	float distantSimp;
	ddensmap densmap;

private:
	void getPatParam(XYZ& p, const float& param, const unsigned& nseg, const XYZ* cvs) const;
};
