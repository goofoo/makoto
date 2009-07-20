#include ".\view.h"
using namespace std;
View::View(void)
{
}

View::~View(void)
{
}

bool View::viewSize(const float boxSize,XYZ boxCenter,cameraParameter cameraPa)
{
	/*
	float dis = (boxCenter.x - cameraPa.eyePoint.x)*cameraPa.viewDirection.x +
		        (boxCenter.y - cameraPa.eyePoint.y)*cameraPa.viewDirection.y +
		        (boxCenter.z - cameraPa.eyePoint.z)*cameraPa.viewDirection.z ;*/

	cameraPa.mat.transform(boxCenter);
	if( boxCenter.z<cameraPa.clipNear || boxCenter.z>cameraPa.clipFar )
		return false;
	double h_length =  boxCenter.z*cameraPa.horizontalFieldOfView;
	double v_length =  boxCenter.z*cameraPa.verticalFieldOfView;
//	double h_dis = dis*cameraPa.horizontalFieldOfView;
//	double v_dis = dis*cameraPa.verticalFieldOfView;

	if(boxSize > 0.5*h_length  || boxSize > 0.5*v_length  )
		return false;
	return true;
}
