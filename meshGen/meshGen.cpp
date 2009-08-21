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
class quadrics : public RIBGen
{
public:
		    quadrics();
		    ~quadrics();

    virtual int	    SetArgs( RIBContext *c,
    	    	    	int n, RtToken tokens[], RtPointer vals[] );
    virtual void    Bound( RIBContext *c, RtBound b );
    virtual int	    GenRIB( RIBContext *c );

private:
	RtFloat m_i_hdr_shadowed, m_i_hdr_interreflection, m_i_hdr_subsurfacescat, m_i_hdr_backscat;
	RtFloat m_i_lightsrc_shadowed, m_i_lightsrc_interreflection, m_i_lightsrc_subsurfacescat, m_i_double_sided, m_has_tangentSpace;
};

/*---------------------------------------------------------*/
RIBGen *
RIBGenCreate()
{
    RIBGen *gen = new quadrics();
    return gen;
}

void
RIBGenDestroy( RIBGen *g )
{
    delete (quadrics *) g;
}

/*---------------------------------------------------------*/
quadrics::quadrics() :
m_i_hdr_shadowed(0),
m_i_hdr_interreflection(0),
m_i_hdr_subsurfacescat(0),
m_i_hdr_backscat(0),
m_i_lightsrc_shadowed(0),
m_i_lightsrc_interreflection(0),
m_i_lightsrc_subsurfacescat(0),
m_i_double_sided(0),m_has_tangentSpace(0)
{   
}

quadrics::~quadrics()
{
}

int
quadrics::SetArgs(RIBContext *c,
    int n, RtToken args[], RtPointer vals[])
{
    int err = 0, i = 0;    
    //c->ReportError( RIBContext::reInfo, 
	//    	"quadrics setargs: %d", n);
    for(i=0;i<n;i++)
    {
	if( !strcmp(args[i], "float hdr_shadowed") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_i_hdr_shadowed);
	}
	else if( !strcmp(args[i], "float hdr_indirect") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_i_hdr_interreflection);
	}
	else if( !strcmp(args[i], "float hdr_scat") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_i_hdr_subsurfacescat);
	}
	else if( !strcmp(args[i], "float hdr_backscat") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_i_hdr_backscat);
	}
	else if( !strcmp(args[i], "float lightsrc_shadowed") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_i_lightsrc_shadowed);
	}
	else if( !strcmp(args[i], "float double_sided") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_i_double_sided);
	}
	else if( !strcmp(args[i], "float tangent_space") )
	{
		RIBContextUtil::GetFloatValue(vals[i], &m_has_tangentSpace);
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
quadrics::Bound( RIBContext *, RtBound  )
{
    // do nothing
}

int
quadrics::GenRIB( RIBContext *c )
{
    int err = 0;
	RtBoolean phase;
	RIBContext::RenderingPass pass;
	c->GetRenderingPass(&pass, &phase);
	
	//if(pass == RIBContext::rpFinal) c->ReportError( RIBContext::reInfo, "pass is final");
	//else if(pass == RIBContext::rpShadow) c->ReportError( RIBContext::reInfo, "pass is shadow");
	//else  c->ReportError( RIBContext::reInfo, "other pass");
	if(pass == RIBContext::rpShadow)
	{
		m_i_hdr_shadowed = 0;
		m_i_hdr_interreflection = 0;
		m_i_hdr_subsurfacescat = 0;
		m_i_hdr_backscat = 0;
		m_i_lightsrc_shadowed = 0;
	}
	
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
	
	//c->ReportError( RIBContext::reInfo, "quadrics get motion: %f %f %f %i", shutterOpen, shutterClose, fps, frame);
	
	const char *vizname = c->GetObjName();
	MObject viznode;
	
	zWorks::getTypedNodeByName(MFn::kLocator, MString(vizname), viznode);
	
	if(viznode.isNull()) return err;
	
	MString cachename, meshname;
	
	MFnDependencyNode fnode(viznode);
	
	if(!zWorks::getStringAttributeByName(fnode, "cachePath", cachename)) return err;
	if(!zWorks::getStringAttributeByName(fnode, "meshName", meshname)) return err;
	
	string sname(cachename.asChar());
	
	string sprt = cachename.asChar();
	
	zGlobal::cutByLastSlash(sprt);
	sprt += meshname.asChar();
	sprt += ".1.prt";
	
	
	//float dtime0 = shutterOpen * fps;
	//float dtime1 = shutterClose * fps;
	//t0 = (double)frame;
	
	//MTime mt0;
	//if(fps == 24) mt0 = MTime(t0, MTime::Unit::kFilm);
	//else if(fps == 25) mt0 = MTime(t0, MTime::Unit::kPALFrame);
	//else mt0 = MTime(t0, MTime::Unit::kNTSCFrame);
	
	//MDGContext ctx0(mt0);
	//zWorks::getDoubleAttributeByNameAndTime(fnode, "currentTime", ctx0, dtime0);
	
	//t1 = (double)frame + ((float)shutterClose - (float)shutterOpen)*(float)fps;
	
	//zWorks::getDoubleAttributeByName(fnode, "currentTime", dtime1);
	
//	c->ReportError( RIBContext::reInfo, "quadrics get motion: %f %f", (float)dtime0, (float)dtime1);
	//int iframe = zGlobal::safeConvertToInt(dtime0);
	//int iframe = zGlobal::safeConvertToInt(shutterOpen*fps);
	float fframe0 = frame;
	float fframe1 = frame + (shutterClose - shutterOpen)*fps;
	zGlobal::changeFrameNumber(sname, frame);
	zGlobal::changeFrameNumber(sprt, frame);
	
	int iblur = 0;
	if(usingMotionBlur) iblur = 1;
	if(pass == RIBContext::rpShadow) iblur = 0;
	
	float mso = int(shutterOpen * 1000000 + 0.5)/1000000.f;
	float msc = int(shutterClose * 1000000 + 0.5)/1000000.f;
	
	MGlobal::displayInfo(MString("MeshRIBGen emits ") + MString(vizname) + " linked to " + sname.c_str());
	if(iblur == 1) MGlobal::displayInfo(MString(" motion blur between ") + fframe0 + " and " + fframe1);
	
	char sbuf[4096];
	sprintf( sbuf, "%s %s %s %d %d %d %d %d %f %f %f %f %d %d %d", 
	sname.c_str(), 
	meshname.asChar(),
	sprt.c_str(),
	(int)m_i_hdr_shadowed,
	(int)m_i_hdr_interreflection,
	(int)m_i_hdr_subsurfacescat,
	(int)m_i_hdr_backscat,
	(int)m_i_lightsrc_shadowed,
	mso, msc,
	(float)fframe0, (float)fframe1,
	iblur, (int)m_i_double_sided, (int)m_has_tangentSpace);
	
	RtString args[] = { "plugins/meshCacheProcedural.dll", sbuf};
	
	RtBound mybound = { -1, 1, -1, 1, -1, 1 };
	
	ZXMLDoc doc;
	if(XMLUtil::findByNameAndType(sname.c_str(), meshname.asChar(), "mesh", doc)) 
	{
		XYZ bbmin(-1,-1,-1), bbmax(1,1,1);
		if(XMLUtil::readBBox(bbmin, bbmax, doc))
		{
			float delta = (bbmax.x - bbmin.x + bbmax.y - bbmin.y + bbmax.z - bbmin.z ) /10;
			mybound[0] = bbmin.x - delta;
			mybound[1] = bbmax.x + delta;
			mybound[2] = bbmin.y - delta;
			mybound[3] = bbmax.y + delta;
			mybound[4] = bbmin.z - delta;
			mybound[5] = bbmax.z + delta;
		}
		doc.free();
	}
/*

*/

//c->ReportError( RIBContext::reInfo, "mesh cache ribgen complete %f %f %f %f %f %f", mybound[0], mybound[1], mybound[2], mybound[3], mybound[4], mybound[5]);
    //c->AttributeBegin();
		c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
	//c->AttributeEnd();
    
    return err;
}
