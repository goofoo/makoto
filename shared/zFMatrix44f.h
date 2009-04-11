#ifndef _zFMatrix44f_H
#define _zFMatrix44f_H

#include "zData.h"

class zFMatrix44f {

public :
	zFMatrix44f();
	~zFMatrix44f();
	
	void reset(MATRIX44F& mat);
	void setOrientation(MATRIX44F& mat, const XYZ& right, const XYZ& up, const XYZ& front);
	void setOrientationByVector(MATRIX44F& mat, XYZ& v);
	void setOrientationByXYZ(MATRIX44F& mat, float x, float y, float z);
	void translate(MATRIX44F& mat, float x, float y, float z);
	void scale(MATRIX44F& mat, float scale);
	void rotateY(MATRIX44F& mat, float angle);
	void multiply(MATRIX44F& mat, const MATRIX44F& other);
	void transform(XYZ& p, const MATRIX44F& mat);
	void ntransform(XYZ& n, const MATRIX44F& mat);
	void vnormalize(XYZ& v);
	XYZ vcross(const XYZ& vec, const XYZ& other);
	float vlength(const XYZ& v);
	
private :
	
};



#endif