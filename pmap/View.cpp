#include "view.h"
using namespace std;
particleView::particleView(void)
{
}

particleView::~particleView(void)
{
}

void particleView::set(double horizontalFieldOfView,double verticalFieldOfView,double nearClippingPlane,double farClippingPlane,MATRIX44F matrix,double signal)
{
	hov = horizontalFieldOfView;
	vov = verticalFieldOfView;
	clipNear = nearClippingPlane;
	clipFar = farClippingPlane;
	mat = matrix;
	sigma = signal;
}

bool particleView::needSplit(const float boxSize,XYZ boxCenter) const
{
	/*
	float dis = (boxCenter.x - cameraPa.eyePoint.x)*cameraPa.viewDirection.x +
		        (boxCenter.y - cameraPa.eyePoint.y)*cameraPa.viewDirection.y +
		        (boxCenter.z - cameraPa.eyePoint.z)*cameraPa.viewDirection.z ;*/

    mat.transform(boxCenter);
	if( boxCenter.z+boxSize<clipNear || boxCenter.z-boxSize>clipFar )
		return false;
	double h_length =  boxCenter.z*hov;
	double v_length =  boxCenter.z*vov;
//	double h_dis = dis*cameraPa.horizontalFieldOfView;
//	double v_dis = dis*cameraPa.verticalFieldOfView;
    
	if(boxSize > sigma*h_length  || boxSize > sigma*v_length  )
		return false;
	return true;
}
