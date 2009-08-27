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
	m_depth_file = new char[256];
}

hairGen::~hairGen()
{
	delete[] m_depth_file;
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
		else if(!strcmp(args[i], "float dofmn"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &dof_min);
		}
		else if(!strcmp(args[i], "float dofmx"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &dof_max);
		}
		else if(!strcmp(args[i], "float fov"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &fov);
		}
		else if(!strcmp(args[i], "float orthow"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &orthow);
		}
		else if(!strcmp(args[i], "string sdepth"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_depth_file);
		}
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
	RtMatrix mat;
	c->GetCamBasis(mat, 0);
	
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
	
	MGlobal::displayInfo(MString("frame ")+frame);
	
	double dtime0 = frame;
	MTime mt0;
	if(fps == 24) mt0 = MTime(dtime0, MTime::Unit::kFilm);
	else if(fps == 25) mt0 = MTime(dtime0, MTime::Unit::kPALFrame);
	else mt0 = MTime(dtime0, MTime::Unit::kNTSCFrame);
	
	MDGContext ctx0(mt0);
	
	zWorks::getDoubleAttributeByNameAndTime(fnode, "currentTime", ctx0, dtime0);
	
	double dtime1;
	
	zWorks::getDoubleAttributeByName(fnode, "currentTime", dtime1);
	
	float fframe0 = dtime0;
	float fframe1 = dtime1;
	
	//double dtime0, dtime1, t0;
	//t0 = (double)frame;
	
	//MTime mt0;
	//if(fps == 24) mt0 = MTime(t0, MTime::Unit::kFilm);
	//else if(fps == 25) mt0 = MTime(t0, MTime::Unit::kPALFrame);
	//else mt0 = MTime(t0, MTime::Unit::kNTSCFrame);
	
	//MDGContext ctx0(mt0);
	//zWorks::getDoubleAttributeByNameAndTime(fnode, "currentTime", ctx0, dtime0);
	
	//zWorks::getDoubleAttributeByName(fnode, "currentTime", dtime1);
	
	int iblur = 0;
	int ishd = 0;
	if(pass == RIBContext::rpShadow) ishd = 1;
	if(usingMotionBlur && pass != RIBContext::rpShadow) iblur = 1;
	
	//float fframe0 = frame;
	//float fframe1 = frame + (shutterClose - shutterOpen)*fps;
	
	MGlobal::displayInfo(MString("HairRIBGen emits ") + MString(vizname));
	if(iblur == 1) MGlobal::displayInfo(MString(" motion blur between ") + fframe0 + " and " + fframe1);
	
	string shairname = base->getCacheName();
	zGlobal::changeFrameNumber(shairname, int(fframe0));
	
	float val = fov;
	if(ishd == 1) val = orthow;
	
	char sbuf[1600];
	sprintf( sbuf, "%f %f %f %f %f %s %s %s %f %f %f %f %f %f %f %f %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
	ndice,
	rootwidth, tipwidth,
	dof_min, dof_max,
	shairname.c_str(), 
	base->getDensityName(), 
	m_depth_file,
	base->getFuzz(),
	base->getKink(),
	base->getClumping(),
	base->getBald(),
	shutterOpen, shutterClose,
	(float)fframe0, (float)fframe1,
	iblur,
	ishd,
	val,
	mat[0][0], mat[0][1], mat[0][2], mat[0][3], 
	mat[1][0], mat[1][1], mat[1][2], mat[1][3], 
	mat[2][0], mat[2][1], mat[2][2], mat[2][3], 
	mat[3][0], mat[3][1], mat[3][2], mat[3][3]
	);
	
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
