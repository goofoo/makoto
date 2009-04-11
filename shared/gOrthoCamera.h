#ifndef _GORTHOCAMERA_H
#define _GORTHOCAMERA_H

#include "zData.h"

class gOrthoCamera {

public:
	gOrthoCamera();
	~gOrthoCamera() {}

	void reset();
	void setViewPort( int w, int h );
	void update();

	void track( float x, float y );
	void tumble( float x, float y );
	void dolly( float z );
						
private :
	float depth, _depth;
	
	XYZ eye;
	XYZ view;
	XYZ up;
	XYZ side;
	XYZ I;
	
	double fov,_fov;
	double clip_near, clip_far;

	int width, height;
	double aspect;
	double left,right,bottom,top;
	
};



#endif
