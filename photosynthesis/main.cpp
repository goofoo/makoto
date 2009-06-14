/*
Syntax:
*/
#include "../render/Buffer2D.h"
#include "../render/EXRImage.h"
#include <iostream>
using namespace std;

int main (int argc, char * const argv[]) {
    // insert code here...
    cout << "Photosynthesis 0.0.1 06/15/09"<<endl;
	
	Buffer2D* buffer = new Buffer2D();
	buffer->create(512, 512);
	
	TRIANGLE2D tri;
	tri.s[0] = 0.2f;
	tri.t[0] = 0.2f;
	
	tri.s[1] = 0.3f;
	tri.t[1] = 0.1f;
	
	tri.s[2] = 0.25f;
	tri.t[2] = 0.9f;
	
	buffer->rasterize(tri);
	
	tri.s[0] = 0.5f;
	tri.t[0] = 0.32f;
	
	tri.s[1] = 0.83f;
	tri.t[1] = 0.31f;
	
	tri.s[2] = 0.75f;
	tri.t[2] = 0.49f;
	
	buffer->rasterize(tri);
	
	EXRImage::saveRGB("foo.exr", buffer->getWidth(), buffer->getHeight(), buffer->getR(), buffer->getG(), buffer->getB());

	delete buffer;

	return 0;

}
//:~