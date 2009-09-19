/*
Syntax:
photosynth -dice 4 -opt 1 0 0 0 inputfile
*/			
#include <stdio.h>
#include "../sh_lighting/SHWorks.h"
#include "../shared/FXMLScene.h"
#include "../shared/zGlobal.h"
using namespace std;

SHWorks* f_work;
FXMLScene* fscene;

int main (int argc, char * const argv[]) {
    // insert code here...
    printf("Photosynth 1.0.0 19/09/09\n");
    
	// check out
	if(argc !=9 || strcmp(argv[1], "-dice")!=0 || strcmp(argv[3], "-opt")!=0)	 
	{
		printf(" argument not correctly set, exit! ");
		return 0;
	}
// parse argument
	string sSceneFull = argv[8];
	printf(" scene: %s", sSceneFull.c_str());
	
	int iFrame = zGlobal::getFrameNumber(sSceneFull);
	printf(" frame: %i", iFrame);
	
	int n_dice_count = atoi(argv[2]);
	
	int i_direct = atoi(argv[4]);
	int i_indirect = atoi(argv[5]);
	int i_scat = atoi(argv[6]);
	int i_backscat = atoi(argv[7]);
	
	f_work = new SHWorks();
	
	fscene = new FXMLScene();
	
	if(fscene->load(sSceneFull.c_str()) != 1) 
	{
		printf("ERROR: cannot open scene %s", sSceneFull.c_str());
		return 0;
	}
	
	printf(" scene area: %f\n",fscene->getSceneArea());
	printf(" scene triangle count: %i\n",fscene->getSceneNumTriangle());
	
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
