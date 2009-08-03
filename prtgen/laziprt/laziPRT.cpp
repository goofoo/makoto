/*
Syntax:

laziPRT.exe 
		-path \\servername\shared\
		-start 1
		-end 25
		-step 5
		inputfile
*/			
#include <iostream>
#include <cstring>
#include "../../sh_lighting/SHWorks.h"
#include "../../shared/FXMLScene.h"
#include "../../shared/zGlobal.h"
using namespace std;

SHWorks* f_work;
FXMLScene* fscene;

int main (int argc, char * const argv[]) {
    // insert code here...
    std::cout << "Lazi PRT 0.1.0 080309"<<endl;
    
	// check out
	if(argc !=10 || strcmp(argv[1], "-path")!=0 || strcmp(argv[3], "-start")!=0 || strcmp(argv[5], "-end")!=0 || strcmp(argv[7], "-step")!=0) {
		cout<<"Argument not correctly set, do nothing! ";
		return 0;
	}

// parse argument
	string sRoot(argv[2]);
	if(sRoot[sRoot.size()-1] != '\\') sRoot += '\\';
	
	string sFile(argv[9]);
	string sSceneFull = sRoot+sFile;
	
	int frame_start = atoi(argv[4]);
	int frame_end = atoi(argv[6]);
	int frame_step = atoi(argv[8]);
	
	cout<<" interpolating "<<sSceneFull<<" from "<<frame_start<<" to "<<frame_end<<" by "<<frame_step<<endl;

	f_work = new SHWorks();
	
	for(int i=frame_start; i<=frame_end; i++) {
	
		int frame_diff = i - frame_start;
		
		if(frame_diff%frame_step != 0) {
		
			fscene = new FXMLScene();
			
			zGlobal::changeFrameNumber(sSceneFull, i);
		
			if(fscene->load(sSceneFull.c_str()) != 1) {
				cout<<"ERROR: cannot open scene "<<sSceneFull.c_str()<<endl;
				delete fscene;
				continue;
			}
			
			int frame_lo = i - frame_diff%frame_step;
			int frame_hi = frame_lo + frame_step;
			if(frame_hi > frame_end) frame_hi = frame_lo;
			
			f_work->cheat(fscene, frame_lo, frame_hi, i);
			
			delete fscene;
		}
	}
	
	delete f_work;

	return 0;

}
//:~