#ifndef _FVANE_H
#define _FVANE_H

#include "../shared/zData.h"

class FVane
{
public:
	FVane(void);
	~FVane(void);
	
	void create(int num, XYZ* p, XYZ* up, XYZ* down, XYZ* normal);
	void clear();
	
	int getNStep(float delta) const;
	float getLength() const;
	XYZ getPointAt(float& S, float& T, XYZ& normal) const;
	
private:
	int n_cv;
	float grid;
	XYZ* pp;
	XYZ* pup;
	XYZ* pdown;
	XYZ* pnormal;
};
#endif
