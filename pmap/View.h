#pragma once

#include "../shared/zData.h"

class particleView
{
public:
	particleView(void);
	~particleView(void);
	
	void set(double horizontalFieldOfView,double verticalFieldOfView,double nearClippingPlane,double farClippingPlane,MATRIX44F matrix);
	bool needSplit(const float boxSize,XYZ boxCenter) const; 
private:
	double hov;
	double vov;
	double clipNear;
	double clipFar;
	MATRIX44F mat;
	double sigma;
};
