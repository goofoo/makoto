#include <iostream>
#include <cstring>
#include "3dtexture.h"
using namespace std;

int main (int argc, char * const argv[]) {
	string gridname(argv[1]);
    // insert code here...
    cout <<"input grid file"<< gridname <<endl;
	
	z3dtexture* pTex = new z3dtexture;
	if(!pTex->loadGrid(gridname.c_str())) {
		cout <<"cannot load grid file "<<gridname <<endl;
		return 0;
	}
	
	pTex->constructTree();
	cout <<"num voxels "<<pTex->getNumVoxel()<<endl;
	
	pTex->doOcclusion();
	
	delete pTex;
    return 0;
}
