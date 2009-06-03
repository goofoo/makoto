#include "RIBGen.h"
#include "RIBContext.h"
#include "RIBContextUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <math.h>
#include <sys/stat.h>
#include "../shared/zGlobal.h"
#include "../shared/zWorks.h"
#include "hairGen.h"
#include "HairDguideNode.h"
#include "HairMap.h"

using namespace std;

/*---------------------------------------------------------*/
RIBGen *
RIBGenCreate()
{
    RIBGen *gen = new hairGen();
    return gen;
}

void
RIBGenDestroy( RIBGen *g )
{
    delete (hairGen *) g;
}

/*---------------------------------------------------------*/
hairGen::hairGen() :
ndice(24),rootwidth(0.034f), tipwidth(0.0f)
{   
}

hairGen::~hairGen()
{
}

int
hairGen::SetArgs(RIBContext *c,
    int n, RtToken args[], RtPointer vals[])
{
    int err = 0, i = 0;    
    //c->ReportError( RIBContext::reInfo, 
	//    	"hairGen setargs: %d", n);
    for(i=0;i<n;i++)
    {
		if(!strcmp(args[i], "float dice"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &ndice);
		}
		else if(!strcmp(args[i], "float rootwidth"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &rootwidth);
		}
		else if(!strcmp(args[i], "float tipwidth"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &tipwidth);
		}
		//else if(!strcmp(args[i], "string imagefile1"))
		//{
		//	RIBContextUtil::GetStringValue(vals[i], &m_image_file1);
		//}
		else
		{
		   c->ReportError( RIBContext::reInfo, 
				"unknown arg: %s (%d)", args[i], n);
		}
    }
    return err;
}

void
hairGen::Bound( RIBContext *, RtBound  )
{
    // do nothing
}

int
hairGen::GenRIB( RIBContext *c )
{
    int err = 0;
	RtBoolean phase;
	RIBContext::RenderingPass pass;
	c->GetRenderingPass(&pass, &phase);
	
	//if(pass == RIBContext::rpFinal) c->ReportError( RIBContext::reInfo, "pass is final");
	//else if(pass == RIBContext::rpShadow) c->ReportError( RIBContext::reInfo, "pass is shadow");
	//else  c->ReportError( RIBContext::reInfo, "other pass");
	
	RtBoolean usingMotionBlur = c->GetMotionBlur();
	// Query motion related settings
	RtFloat shutterOpen, shutterClose;
	RtFloat shutterAngle, fps;
	RtBoolean subframeMotion;
	RtBoolean blurCamera;
	RIBContext::ShutterTiming shutterTiming;
	RIBContext::ShutterConfig shutterConfig;
	int frame = c->GetFrame();
	
	c->GetShutter(&shutterOpen, &shutterClose);
	c->GetMotionInfo(&shutterAngle, &fps, &subframeMotion, &blurCamera, &shutterTiming, &shutterConfig);

	const char *vizname = c->GetObjName();
	MObject viznode;
	
	zWorks::getTypedNodeByName(MFn::kLocator, MString(vizname), viznode);
	
	if(viznode.isNull()) return err;
	
	MFnDependencyNode fnode(viznode);

	HairDguideNode* viz = (HairDguideNode*)fnode.userNode();
	const hairMap* base = viz->getBase();
	
	RtBound mybound = { -1, 1, -1, 1, -1, 1 };
	mybound[0] = base->getBBox0X();
	mybound[1] = base->getBBox1X();
	mybound[2] = base->getBBox0Y();
	mybound[3] = base->getBBox1Y();
	mybound[4] = base->getBBox0Z();
	mybound[5] = base->getBBox1Z();
	
	double dtime0, dtime1, t0;
	t0 = (double)frame;
	
	MTime mt0;
	if(fps == 24) mt0 = MTime(t0, MTime::Unit::kFilm);
	else if(fps == 25) mt0 = MTime(t0, MTime::Unit::kPALFrame);
	else mt0 = MTime(t0, MTime::Unit::kNTSCFrame);
	
	MDGContext ctx0(mt0);
	zWorks::getDoubleAttributeByNameAndTime(fnode, "currentTime", ctx0, dtime0);
	
	zWorks::getDoubleAttributeByName(fnode, "currentTime", dtime1);
	
	int iblur = 0;
	if(usingMotionBlur && pass != RIBContext::rpShadow) iblur = 1;
	
	char sbuf[1024];
	sprintf( sbuf, "%f %f %f %f %f %f %f %f %f %f %f %f %f %s %f %f %f %f %f %f %f %d", 
	ndice,
	rootwidth, tipwidth,
	base->getRootColorR(),
	base->getRootColorG(),
	base->getRootColorB(),
	base->getTipColorR(),
	base->getTipColorG(),
	base->getTipColorB(),
	base->getMutantColorR(),
	base->getMutantColorG(),
	base->getMutantColorB(),
	base->getMutantColorScale(),
	base->getCacheName(), 
	base->getClumping(),
	base->getFuzz(),
	base->getKink(),
	shutterOpen, shutterClose,
	(float)dtime0, (float)dtime1,
	iblur);
	
	RtString args[] = { "plugins/zhairProcedural.dll", sbuf};
	
	RtBasis RiBSplineBasis =
	{ 
		-1/6.,  3/6., -3/6.,  1/6.,
		 3/6., -6/6.,  3/6.,  0,
		-3/6.,  0,     3/6.,  0,
		 1/6.,  4/6.,  1/6.,  0
	};
	c->Basis(RiBSplineBasis, 1, RiBSplineBasis, 1);
	
	c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());

    return err;
}
