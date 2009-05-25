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
m_image_name(0L),
m_image_file1(0L),
m_image_file2(0L),
m_image_file3(0L),
m_image_file4(0L),
m_image_file5(0L)
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
		/*if(!strcmp(args[i], "string imagename"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_image_name);
		}
		else if(!strcmp(args[i], "string imagefile1"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_image_file1);
		}
		else if(!strcmp(args[i], "string imagefile2"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_image_file2);
		}
		else if(!strcmp(args[i], "string imagefile3"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_image_file3);
		}
		else if(!strcmp(args[i], "string imagefile4"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_image_file4);
		}
		else if(!strcmp(args[i], "string imagefile5"))
		{
			RIBContextUtil::GetStringValue(vals[i], &m_image_file5);
		}
		else if(!strcmp(args[i], "float maxframe1"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &maxframe1);
		}
		else if(!strcmp(args[i], "float maxframe2"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &maxframe2);
		}
		else if(!strcmp(args[i], "float maxframe3"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &maxframe3);
		}
		else if(!strcmp(args[i], "float maxframe4"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &maxframe4);
		}
		else if(!strcmp(args[i], "float maxframe5"))
		{
			RIBContextUtil::GetFloatValue(vals[i], &maxframe5);
		}
	else if( !strcmp(args[i], "float hdr_shadowed") )
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
	}*/
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
	sprintf( sbuf, "%s %f %f %f %f %d", 
	base->getCacheName(), 
	shutterOpen, shutterClose,
	(float)dtime0, (float)dtime1,
	iblur);
	
	RtString args[] = { "plugins/zhairProcedural.dll", sbuf};
	
	c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
/*	
	RtFloat vertices[12] = {-1, -1, 0, -1, 1, 0, 1, -1, 0, 1, 1, 0};
	
	std::string sname = "constant string ";
	sname += m_image_name;
	std::string sfile;
	
	unsigned num_ptc = base->getNumBacteria();
	XYZ cen, xvec, yvec, vert;
	float frame_offset;
	int iframe;
	for(unsigned i=0; i<num_ptc; i++)
	{
		if(base->isInViewFrustum(i))
		{
			cen =  base->getPositionById(i);
			base->getSideAndUpById(i, xvec, yvec);
			
			vert = cen - xvec - yvec;
			vertices[0] = vert.x;
			vertices[1] = vert.y;
			vertices[2] = vert.z;
			vert =  cen - xvec + yvec;
			vertices[3] = vert.x;
			vertices[4] = vert.y;
			vertices[5] = vert.z;
			vert =  cen + xvec - yvec;
			vertices[6] = vert.x;
			vertices[7] = vert.y;
			vertices[8] = vert.z;
			vert = cen + xvec + yvec;
			vertices[9] = vert.x;
			vertices[10] = vert.y;
			vertices[11] = vert.z;
			
			float noi = base->getNoise2ById(i);
			if(noi<0.2) 
			{
				sfile = m_image_file1;
				frame_offset = maxframe1 * base->getNoise3ById(i);
				iframe = int(frame+frame_offset)%(int)maxframe1;
			}
			else if(noi<0.4) 
			{
				sfile = m_image_file2;
				frame_offset = maxframe2 * base->getNoise3ById(i);
				iframe = int(frame+frame_offset)%(int)maxframe2;
			}
			else if(noi<0.6) 
			{
				sfile = m_image_file3;
				frame_offset = maxframe3 * base->getNoise3ById(i);
				iframe = int(frame+frame_offset)%(int)maxframe3;
			}
			else if(noi<0.8) 
			{
				sfile = m_image_file4;
				frame_offset = maxframe4 * base->getNoise3ById(i);
				iframe = int(frame+frame_offset)%(int)maxframe4;
			}
			else 
			{
				sfile = m_image_file5;
				frame_offset = maxframe5 * base->getNoise3ById(i);
				iframe = int(frame+frame_offset)%(int)maxframe5;
			}
			
			zGlobal::changeFrameNumberFistDot4Digit(sfile, iframe);
			const char* psfile = sfile.c_str();
			c->Patch("bilinear", "P", (RtPoint*)vertices, (RtToken)sname.c_str(), (RtPointer)&psfile, RI_NULL);
		}
	}
*/
    return err;
}
