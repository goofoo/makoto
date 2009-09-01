#include "PerspectiveView.h"

PerspectiveView::PerspectiveView(void)
{
}

PerspectiveView::~PerspectiveView(void)
{
}

void PerspectiveView::set(double horizontalFieldOfView,double verticalFieldOfView,double nearClippingPlane,double farClippingPlane,MATRIX44F matrix,double signal)
{
	hov = horizontalFieldOfView;
	vov = verticalFieldOfView;
	clipNear = nearClippingPlane;
	clipFar = farClippingPlane;
	mat = matrix;
	sigma = signal;
}

float PerspectiveView::deep(XYZ boxCenter) const
{
	 mat.transform(boxCenter);
	 return boxCenter.z;
}

bool PerspectiveView::needSplit(const float boxSize,XYZ boxCenter) const
{
	/*
	float dis = (boxCenter.x - cameraPa.eyePoint.x)*cameraPa.viewDirection.x +
		        (boxCenter.y - cameraPa.eyePoint.y)*cameraPa.viewDirection.y +
		        (boxCenter.z - cameraPa.eyePoint.z)*cameraPa.viewDirection.z ;*/

    mat.transform(boxCenter);
	if( boxCenter.z+boxSize<clipNear || boxCenter.z-boxSize>clipFar ) return false;
	double h_length =  (boxCenter.z-boxSize)*hov;
	//double v_length =  boxCenter.z-boxSize*vov;
//	double h_dis = dis*cameraPa.horizontalFieldOfView;
//	double v_dis = dis*cameraPa.verticalFieldOfView;
    
	if(boxSize > sigma*h_length) return true;
	return false;
}
