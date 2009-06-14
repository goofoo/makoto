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
#include "../shared/XMLUtil.h"

using namespace std;

/*---------------------------------------------------------*/
class feathers : public RIBGen
{
public:
		    feathers();
		    ~feathers();

    virtual int	    SetArgs( RIBContext *c,
    	    	    	int n, RtToken tokens[], RtPointer vals[] );
    virtual void    Bound( RIBContext *c, RtBound b );
    virtual int	    GenRIB( RIBContext *c );

private:
	RtFloat m_n_dice, m_f_wh, m_thickness;
};

/*---------------------------------------------------------*/
RIBGen *
RIBGenCreate()
{
    RIBGen *gen = new feathers();
    return gen;
}

void
RIBGenDestroy( RIBGen *g )
{
    delete (feathers *) g;
}

/*---------------------------------------------------------*/
feathers::feathers() :
m_n_dice(512.0), m_f_wh(0.25)
{   
}

feathers::~feathers()
{
}

int
feathers::SetArgs(RIBContext *c,
    int n, RtToken args[], RtPointer vals[])
{
    int err = 0, i = 0;    
    //c->ReportError( RIBContext::reInfo, 
	//    	"feathers setargs: %d", n);
    for(i=0;i<n;i++)
    {
		if( !strcmp(args[i], "float n_dice") )
		{
			RIBContextUtil::GetFloatValue(vals[i], &m_n_dice);
		}
		else if( !strcmp(args[i], "float f_wh") )
		{
			RIBContextUtil::GetFloatValue(vals[i], &m_f_wh);
		}
		else if( !strcmp(args[i], "float f_thickness") )
		{
			RIBContextUtil::GetFloatValue(vals[i], &m_thickness);
		}
		else
		{
			c->ReportError( RIBContext::reInfo, "unknown arg: %s (%d)", args[i], n);
		}
    }
    return err;
}

void
feathers::Bound( RIBContext *, RtBound  )
{
    // do nothing
}

int
feathers::GenRIB( RIBContext *c )
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
	
//	c->ReportError( RIBContext::reInfo, "feathers get motion: %f %f %f %i", shutterOpen, shutterClose, fps, frame);
	
	const char *vizname = c->GetObjName();
	MObject viznode;
	
	zWorks::getTypedNodeByName(MFn::kLocator, MString(vizname), viznode);
	
	if(viznode.isNull()) return err;
	
	MString cachename;
	
	MFnDependencyNode fnode(viznode);
	
	if(!zWorks::getStringAttributeByName(fnode, "cachePath", cachename)) return err;
	
	string sname(cachename.asChar());
	//sname += '/';
	//sname += fnode.name().asChar();
	//sname += ".1.xhb";
	if(pass != RIBContext::rpShadow) zGlobal::changeFilenameExtension(sname, "xhb");
	
	zGlobal::changeFrameNumber(sname, frame);
	
	MGlobal::displayInfo(MString("MeshRIBGen emits ") + MString(vizname) + " linked to " + sname.c_str());
	
	int ishd = 0;
	if(pass == RIBContext::rpShadow) ishd = 1;
	int iblur = 0;
	if(usingMotionBlur) iblur = 1;
	if(pass == RIBContext::rpShadow) iblur = 0;
	char sbuf[1024];
	sprintf( sbuf, "%s %f %f %f %f %f %f %d %d", 
	sname.c_str(), 
	m_n_dice,
	m_f_wh,
	m_thickness,
	shutterOpen, shutterClose,
	fps,
	iblur,
	ishd);
	RtString args[] = { "featherProcedural.dll", sbuf};
	if(pass == RIBContext::rpShadow) args[0] = "meshCacheProcedural.dll";
	
	RtBound mybound = { -1, 1, -1, 1, -1, 1 };
	
	XYZ bbmin(-1,-1,-1), bbmax(1,1,1);
	float delta;
	if(XMLUtil::readDynBBox(sname.c_str(), bbmin, bbmax))
	{
		delta = (bbmax.x - bbmin.x + bbmax.y - bbmin.y + bbmax.z - bbmin.z ) /10;
		mybound[0] = bbmin.x - delta;
		mybound[1] = bbmax.x + delta;
		mybound[2] = bbmin.y - delta;
		mybound[3] = bbmax.y + delta;
		mybound[4] = bbmin.z- delta;
		mybound[5] = bbmax.z + delta;
	}
	RtBasis RiBSplineBasis =
{ 
    -1/6.,  3/6., -3/6.,  1/6.,
     3/6., -6/6.,  3/6.,  0,
    -3/6.,  0,     3/6.,  0,
     1/6.,  4/6.,  1/6.,  0
};
	c->Basis(RiBSplineBasis, 1,
			RiBSplineBasis, 1);
	
	c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
	
    return err;
}
