/*
Syntax:

prtServer.exe 
		-path \\servername\shared\
		-dice 4
		-operation 1 0 0 0
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
    std::cout << "PRT Server 1.8.1 080309"<<endl;
    
	// check out
	if(argc !=11 || strcmp(argv[1], "-path")!=0 || strcmp(argv[3], "-dice")!=0 || strcmp(argv[5], "-operation")!=0)	 
	{
		cout<<"Argument not correctly set, do nothing! ";
		return 0;
	}
// parse argument
	string sRoot(argv[2]);
	if(sRoot[sRoot.size()-1] != '\\') sRoot += '\\';
	
	string sFile(argv[10]);
	string sSceneFull = sRoot+sFile;
	cout<<" scene:"<<sSceneFull<<endl;
	
	int iFrame = zGlobal::getFrameNumber(sFile);
	cout<<" frame:"<<iFrame<<endl;
	
	int n_dice_count = atoi(argv[4]);
	
	int i_direct = atoi(argv[6]);
	int i_indirect = atoi(argv[7]);
	int i_scat = atoi(argv[8]);
	int i_backscat = atoi(argv[9]);
	
	f_work = new SHWorks();
	
	fscene = new FXMLScene();
	
	if(fscene->load(sSceneFull.c_str()) != 1) 
	{
		cout<<"ERROR: cannot open scene "<<sSceneFull.c_str()<<endl;
		return 0;
	}
	
	cout<<" scene area: "<<fscene->getSceneArea()<<endl;
	cout<<" scene triangle count: "<<fscene->getSceneNumTriangle()<<endl;
	
	if(i_direct == 1) 
	{
		fscene->dice(n_dice_count);
		if(!fscene->hasLight()) f_work->saveRT(fscene);
		else
		{
			FQSPLAT* d_light = new FQSPLAT();
			fscene->diceLight(n_dice_count, d_light);
			f_work->saveLightRT(fscene, d_light);
			delete d_light;
		}
		fscene->removeTree();
	}
	
	if(i_indirect== 1) 
	{
		fscene->diceWithRTandColor(4);
		f_work->saveIndirectRT(fscene);
		fscene->removeRTTree();
	}
	
	if(i_scat == 1 || i_backscat == 1)
	{
		f_work->saveScat(fscene, n_dice_count, i_scat, i_backscat);
	}
	return 0;

}
//:~