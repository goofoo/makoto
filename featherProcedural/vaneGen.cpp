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
class vanes : public RIBGen
{
public:
		    vanes();
		    ~vanes();

    virtual int	    SetArgs( RIBContext *c,
    	    	    	int n, RtToken tokens[], RtPointer vals[] );
    virtual void    Bound( RIBContext *c, RtBound b );
    virtual int	    GenRIB( RIBContext *c );

private:
	RtFloat m_n_dice;
};

/*---------------------------------------------------------*/
RIBGen *
RIBGenCreate()
{
    RIBGen *gen = new vanes();
    return gen;
}

void
RIBGenDestroy( RIBGen *g )
{
    delete (vanes *) g;
}

/*---------------------------------------------------------*/
vanes::vanes() :
m_n_dice(100.0)
{   
}

vanes::~vanes()
{
}

int
vanes::SetArgs(RIBContext *c,
    int n, RtToken args[], RtPointer vals[])
{
    int err = 0, i = 0;    
    //c->ReportError( RIBContext::reInfo, 
	//    	"vanes setargs: %d", n);
    for(i=0;i<n;i++)
    {
		if( !strcmp(args[i], "float n_dice") )
		{
			RIBContextUtil::GetFloatValue(vals[i], &m_n_dice);
		}
		else
		{
			c->ReportError( RIBContext::reInfo, "unknown arg: %s (%d)", args[i], n);
		}
    }
    return err;
}

void
vanes::Bound( RIBContext *, RtBound  )
{
    // do nothing
}

int
vanes::GenRIB( RIBContext *c )
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
	
//	c->ReportError( RIBContext::reInfo, "vanes get motion: %f %f %f %i", shutterOpen, shutterClose, fps, frame);
	
	const char *vizname = c->GetObjName();
	MObject viznode;
	
	zWorks::getTypedNodeByName(MFn::kLocator, MString(vizname), viznode);
	
	if(viznode.isNull()) return err;
	
	MString cachename;
	
	MFnDependencyNode fnode(viznode);
	
	if(!zWorks::getStringAttributeByName(fnode, "cachePath", cachename)) return err;
	
	string sname(cachename.asChar());
	sname += '/';
	sname += fnode.name().asChar();
	sname += ".1.xvn";
	
	zGlobal::changeFrameNumber(sname, frame);
	
	MGlobal::displayInfo(MString("MeshRIBGen emits ") + MString(vizname) + " linked to " + sname.c_str());
	
	int iblur = 0;
	if(usingMotionBlur) iblur = 1;
	char sbuf[1024];
	sprintf( sbuf, "%s %f %f %f %f %d", 
	sname.c_str(), 
	m_n_dice,
	shutterOpen, shutterClose,
	fps,
	iblur );
	RtString args[] = { "vaneProcedural.dll", sbuf};
	
	RtBound mybound = { -1, 1, -1, 1, -1, 1 };
	
	XYZ bbmin(-1,-1,-1), bbmax(1,1,1);
	float delta;
	if(XMLUtil::readBBox(sname.c_str(), bbmin, bbmax))
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
