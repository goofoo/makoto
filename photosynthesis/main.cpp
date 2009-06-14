/*
Syntax:
*/
#include "../shared/zFnEXR.h"
#include <iostream>
using namespace std;

int main (int argc, char * const argv[]) {
    // insert code here...
    cout << "Photosynthesis 0.0.0 04/12/09"<<endl;
	float* data = new float[640*480*4];
	for(unsigned j=0; j<480; j++)
		for(unsigned i=0; i<640; i++)
		{
			data[(j*640+i)*4] = float(i)/640;
			data[(j*640+i)*4+1] = float(j)/480;
			data[(j*640+i)*4+2] = 0;
			data[(j*640+i)*4+3] = 1;
		}
	ZFnEXR::save(data, "foo.exr", 640, 480);



	return 0;

}
//:~