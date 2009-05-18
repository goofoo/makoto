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
#include "bacteriaGen.h"
#include "bacteriaNode.h"
#include "FBacteria.h"

using namespace std;

/*---------------------------------------------------------*/
RIBGen *
RIBGenCreate()
{
    RIBGen *gen = new bacteriaGen();
    return gen;
}

void
RIBGenDestroy( RIBGen *g )
{
    delete (bacteriaGen *) g;
}

/*---------------------------------------------------------*/
bacteriaGen::bacteriaGen() :
m_image_name(0L),
m_image_file1(0L),
m_image_file2(0L),
m_image_file3(0L),
m_image_file4(0L),
m_image_file5(0L)
{   
}

bacteriaGen::~bacteriaGen()
{
}

int
bacteriaGen::SetArgs(RIBContext *c,
    int n, RtToken args[], RtPointer vals[])
{
    int err = 0, i = 0;    
    //c->ReportError( RIBContext::reInfo, 
	//    	"bacteriaGen setargs: %d", n);
    for(i=0;i<n;i++)
    {
		if(!strcmp(args[i], "string imagename"))
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
	/*else if( !strcmp(args[i], "float hdr_shadowed") )
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
	}*/
	else
	{
	   c->ReportError( RIBContext::reInfo, 
	    	"unknown arg: %s (%d)", args[i], n);
	}
    }
    return err;
}

void
bacteriaGen::Bound( RIBContext *, RtBound  )
{
    // do nothing
}

int
bacteriaGen::GenRIB( RIBContext *c )
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
	
	bacteriaNode* bacteria = (bacteriaNode*)fnode.userNode();
	const FBacteria* base = bacteria->getBase();
	
	RtFloat vertices[12] = {-1, -1, 0, -1, 1, 0, 1, -1, 0, 1, 1, 0};
	
	std::string sname = "constant string ";
	sname += m_image_name;
	std::string sfile;
	
	unsigned num_ptc = base->getNumBacteria();
	XYZ cen, xvec, yvec, vert;
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
			if(noi<0.2) sfile = m_image_file1;
			else if(noi<0.4) sfile = m_image_file2;
			else if(noi<0.6) sfile = m_image_file3;
			else if(noi<0.8) sfile = m_image_file4;
			else sfile = m_image_file5;
			
			const char* psfile = sfile.c_str();
			c->Patch("bilinear", "P", (RtPoint*)vertices, (RtToken)sname.c_str(), (RtPointer)&psfile, RI_NULL);
		}
	}

    return err;
}
