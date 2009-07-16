#include ".\view.h"
using namespace std;
View::View(void)
{
}

View::~View(void)
{
}

bool View::needSplit(const float boxSize,XYZ boxCenter,cameraParameter cameraPa) const
{
	/*
	float dis = (boxCenter.x - cameraPa.eyePoint.x)*cameraPa.viewDirection.x +
		        (boxCenter.y - cameraPa.eyePoint.y)*cameraPa.viewDirection.y +
		        (boxCenter.z - cameraPa.eyePoint.z)*cameraPa.viewDirection.z ;*/

	cameraPa.mat.transform(boxCenter);
	if( boxCenter.z+size<cameraPa.clipNear || boxCenter.z-size>cameraPa.clipFar )
		return false;
	double h_length =  boxCenter.z*cameraPa.horizontalFieldOfView;
	double v_length =  boxCenter.z*cameraPa.verticalFieldOfView;
//	double h_dis = dis*cameraPa.horizontalFieldOfView;
//	double v_dis = dis*cameraPa.verticalFieldOfView;

	if(boxSize > sigma*h_length  || boxSize > sigma*v_length  )
		return false;
	return true;
}
