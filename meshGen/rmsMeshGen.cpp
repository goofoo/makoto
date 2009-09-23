/*-______________________________________________________________________
** 
** Copyright (c) 1999 PIXAR.  All rights reserved.  This program or
** documentation contains proprietary confidential information and trade
** secrets of PIXAR.  Reverse engineering of object code is prohibited.
** Use of copyright notice is precautionary and does not imply
** publication.
** 
**                      RESTRICTED RIGHTS NOTICE
** 
** Use, duplication, or disclosure by the Government is subject to the 
** following restrictions:  For civilian agencies, subparagraphs (a) through
** (d) of the Commercial Computer Software--Restricted Rights clause at
** 52.227-19 of the FAR; and, for units of the Department of Defense, DoD
** Supplement to the FAR, clause 52.227-7013 (c)(1)(ii), Rights in
** Technical Data and Computer Software.
** 
** Pixar
** 1001 W Cutting Blvd
** Richmond, CA  94804
** 
** ______________________________________________________________________
*/

/*
 * quadrics:
 *  a simple ribgen example that places Ri Spheres and Cylinders
 *  into your RIB stream.
 *
 *  we leave it "as an excercise" to extend it for all quadrics and
 *  even to support caps, etc.
 *
 *  $Revision: #1 $
 */

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

private: // see .sli file for details on variables
    RtFloat m_i_hdr_shadowed, m_i_hdr_interreflection, m_i_hdr_subsurfacescat, m_i_hdr_backscat, m_i_lightsrc_shadowed, m_i_double_sided, m_has_tangentSpace;
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
quadrics::quadrics()
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
    //c->ReportError( RIBContext::reInfo, "quadrics setargs: %d", n);
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
	else
	{
	   c->ReportError( RIBContext::reInfo, 
	    	"unknown arg: %s (%d)", args[i], n);
	}
    }
    return err;
}

extern "C" {
    RtVoid meshProceduralFree(RtPointer data) {
	//RtString* sData = (RtString*) data;
	//free(sData[0]);
	//free(sData[1]);
	//free(sData);
    }
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
	if(pass == RIBContext::rpShadow) {
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
	
	c->ReportError(RIBContext::reInfo, "ribgen start");
	
	MGlobal::displayInfo(MString(" frame ")+frame);
	
	c->GetShutter(&shutterOpen, &shutterClose);
	c->GetMotionInfo(&shutterAngle, &fps, &subframeMotion, &blurCamera, &shutterTiming, &shutterConfig);
	
	
	const char *vizname = c->GetObjName(); MString svname(vizname);
	MObject viznode;
	
	zWorks::getTypedNodeByName(MFn::kLocator, svname, viznode);
	
	if(viznode.isNull()) {
		c->ReportError(RIBContext::reInfo, "skip unknown");
		return err;
	}
	
	MString cachename, meshname;
	
	MFnDependencyNode fnode(viznode);
	
	if(!zWorks::getStringAttributeByName(fnode, "cachePath", cachename)) return err;
	if(!zWorks::getStringAttributeByName(fnode, "meshName", meshname)) return err;
	
	double bbox[6];
	zWorks::getDoubleAttributeByName(fnode, "boundingBoxMinX", bbox[0]);
	zWorks::getDoubleAttributeByName(fnode, "boundingBoxMaxX", bbox[1]);
	zWorks::getDoubleAttributeByName(fnode, "boundingBoxMinY", bbox[2]);
	zWorks::getDoubleAttributeByName(fnode, "boundingBoxMaxY", bbox[3]);
	zWorks::getDoubleAttributeByName(fnode, "boundingBoxMinZ", bbox[4]);
	zWorks::getDoubleAttributeByName(fnode, "boundingBoxMaxZ", bbox[5]);
	double bboxfract = (bbox[1] - bbox[0] + bbox[3] - bbox[2] + bbox[5] - bbox[4])/11;
	
	string sname(cachename.asChar());
	
	string sprt = cachename.asChar();
	
	zGlobal::cutByLastSlash(sprt);
	sprt += meshname.asChar();
	sprt += ".1.prt";
	
	double dtime0 = frame;
	MTime mt0;
	if(fps == 24) mt0 = MTime(dtime0, MTime::kFilm);
	else if(fps == 25) mt0 = MTime(dtime0, MTime::kPALFrame);
	else mt0 = MTime(dtime0, MTime::kNTSCFrame);
	
	MDGContext ctx0(mt0);
	
	zWorks::getDoubleAttributeByNameAndTime(fnode, "currentTime", ctx0, dtime0);
	
	double dtime1;
	
	zWorks::getDoubleAttributeByName(fnode, "currentTime", dtime1);
	
	float fframe0 = dtime0;
	float fframe1 = dtime1;
	
	int iframe0 = zGlobal::safeConvertToInt(dtime0);
	
	zGlobal::changeFrameNumber(sname, iframe0);
	zGlobal::changeFrameNumber(sprt, iframe0);
	
	int iblur = 0;
	if(usingMotionBlur) iblur = 1;
	if(pass == RIBContext::rpShadow) iblur = 0;
	
	MGlobal::displayInfo(MString("MeshRIBGen emits ") + MString(vizname) + " linked to " + sname.c_str());
	if(iblur == 1) MGlobal::displayInfo(MString(" motion blur between ") + fframe0 + " and " + fframe1);
	
	char sbuf[4096];
	sprintf( sbuf, "%s %s %s %d %d %d %d %d %f %f %f %f %d %d", 
	sname.c_str(), 
	meshname.asChar(),
	sprt.c_str(),
	(int)m_i_hdr_shadowed,
	(int)m_i_hdr_interreflection,
	(int)m_i_hdr_subsurfacescat,
	(int)m_i_hdr_backscat,
	(int)m_i_lightsrc_shadowed,
	shutterOpen, shutterClose,
	(float)fframe0, (float)fframe1,
	iblur, (int)m_i_double_sided);

	RtString args[] = { "plugins/meshCacheProcedural.so", sbuf};
	
#if defined (_WIN32)
	args[0] = "plugins/meshCacheProcedural.dll";
#endif	
	
	RtBound mybound = { -1, 1, -1, 1, -1, 1 };
	
	mybound[0] = bbox[0] - bboxfract;
	mybound[1] = bbox[1] + bboxfract;
	mybound[2] = bbox[2] - bboxfract;
	mybound[3] = bbox[3] + bboxfract;
	mybound[4] = bbox[4] - bboxfract;
	mybound[5] = bbox[5] + bboxfract;
	
	c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(RIBContext::kDynamicLoad), meshProceduralFree);

	c->ReportError( RIBContext::reInfo, "ribgen complete");
    return err;
}
