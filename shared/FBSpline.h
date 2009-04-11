#ifndef _FBSpline_H
#define _FBSpline_H

#include "zData.h"

class FBSpline
{
public:
	FBSpline(void);
	~FBSpline(void);
	
	void create(int num, float* cv);
	void clear();
	
	void setEnd(float v0, float v1);
	float getValue(float v) const;
	
private:
	int n_cv;
	float grid, end0, end1;
	float* pcv;
};

class FBSpline3D
{
public:
	FBSpline3D(void);
	~FBSpline3D(void);
	
	void create(int num, XYZ* cv);
	void clear();

	XYZ getValue(float v) const;
	
private:
	int n_cv;
	float grid;
	XYZ* pcv;
};
#endif
