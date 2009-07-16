#pragma once

#include "../shared/zData.h"

struct cameraParameter 
{
	double focalLength;
	double horizontalFieldOfView;
	double verticalFieldOfView;
	double clipNear;
	double clipFar;
	XYZ viewDirection;
	XYZ eyePoint; 
	MATRIX44F mat;
};

class View
{
public:
	View(void);
	~View(void);
	
	void set();

	bool needSplit(const float boxSize,XYZ boxCenter,cameraParameter cameraPa) const; 
private:
	
	double focalLength;
	double horizontalFieldOfView;
	double verticalFieldOfView;
	double clipNear;
	double clipFar;
	XYZ viewDirection;
	XYZ eyePoint; 
	MATRIX44F mat;
	
	double sigma;
};
