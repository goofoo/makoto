#ifndef _gMatrix44d_H
#define _gMatrix44d_H

#include "gVector3d.h"

class gMatrix44d {

public :
	gMatrix44d();
	~gMatrix44d();
	
	void setOrientation( gVector3d right, gVector3d up, gVector3d front );
	void setTranslation( gVector3d position );
	
	double * matrix();
	double * inverseMatrix();
private :
	double m[16];
	double mi[16];
	
	double determinant33( double a, double b, double c, double d, double e, double f, double g, double h, double i );

};



#endif
