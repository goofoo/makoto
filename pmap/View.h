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

	static bool viewSize(const float boxSize,XYZ boxCenter,cameraParameter cameraPa); 
private:
	cameraParameter cameraPa;
};
