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
 * fishGen:
 *  a simple ribgen example that places Ri Spheres and Cylinders
 *  into your RIB stream.
 *
 *  we leave it "as an excercise" to extend it for all fishGen and
 *  even to support caps, etc.
 *
 *  $Revision: #1 $
 */
#include <winsock2.h>
#include "fishGen.h"
//#include "../shared/zFMatrix44f.h"

//#include <string.h>
//#include <limits.h>
//#ifndef _WIN32
//#include <unistd.h>
//#endif
#include <math.h>
//#include <sys/stat.h>
#include "../shared/zString.h"
#include "../shared/zAttrWorks.h"
#include <maya/MAnimControl.h>
#include "../shared/zWorks.h"
#include "../shared/pdcFile.h"
#include "../shared/zData.h"

extern "C" RIBGen *
RIBGenCreate()
{
    RIBGen *gen = new fishGen();
    return gen;
}

extern "C" void
RIBGenDestroy( RIBGen *g )
{
    delete (fishGen *) g;
}

/*---------------------------------------------------------*/
fishGen::fishGen() :
    m_cache_body_0(0L),
    m_cache_body_1(0L),
    m_cache_body_2(0L),
    m_cache_body_3(0L),
    m_cache_teeth_top_0(0L),
    m_cache_teeth_top_1(0L),
    m_cache_teeth_top_2(0L),
    m_cache_teeth_top_3(0L),
    m_cache_teeth_bottom_0(0L),
    m_cache_teeth_bottom_1(0L),
    m_cache_teeth_bottom_2(0L),
    m_cache_teeth_bottom_3(0L),
    m_cache_eye_0(0L),
    m_cache_eye_1(0L),
    m_cache_eye_2(0L),
    m_cache_eye_3(0L),
    m_body_surf(0L),
    m_body_disp(0L),
    m_teeth_surf(0L),
    m_teeth_disp(0L),
    m_eye_surf(0L),
    m_eye_disp(0L),
    m_xmin(-1),
    m_xmax(1),
    m_ymin(-1),
    m_ymax(1),
    m_zmin(-1),
    m_zmax(1),
    m_k_flap(1), 
    m_k_bend(1),
    m_k_oscillate(1),
    m_length(20),
    m_frequency(1),
    m_displacementBound(1)
{   
}

fishGen::~fishGen()
{
    if(m_cache_body_0)
    {
    	delete [] m_cache_body_0;
    }
    if(m_cache_body_1)
    {
    	delete [] m_cache_body_1;
    }
    if(m_cache_body_2)
    {
    	delete [] m_cache_body_2;
    }
    if(m_cache_body_3)
    {
    	delete [] m_cache_body_3;
    }
    if(m_body_surf)
    {
    	delete [] m_body_surf;
    }
    if(m_body_disp)
    {
    	delete [] m_body_disp;
    }
    if(m_cache_teeth_top_0) delete m_cache_teeth_top_0;
    if(m_cache_teeth_top_1) delete m_cache_teeth_top_1;
    if(m_cache_teeth_top_2) delete m_cache_teeth_top_2;
    if(m_cache_teeth_top_3) delete m_cache_teeth_top_3;
    if(m_cache_teeth_bottom_0) delete m_cache_teeth_bottom_0;
    if(m_cache_teeth_bottom_1) delete m_cache_teeth_bottom_1;
    if(m_cache_teeth_bottom_2) delete m_cache_teeth_bottom_2;
    if(m_cache_teeth_bottom_3) delete m_cache_teeth_bottom_3;
    if(m_cache_eye_0) delete m_cache_eye_0;
    if(m_cache_eye_1) delete m_cache_eye_1;
    if(m_cache_eye_2) delete m_cache_eye_2;
    if(m_cache_eye_3) delete m_cache_eye_3;
    
    if(m_teeth_surf) delete m_teeth_surf;
    if(m_teeth_disp) delete m_teeth_disp;
    if(m_eye_surf) delete m_eye_surf;
    if(m_eye_disp) delete m_eye_disp;
    
    
}

int
fishGen::SetArgs(RIBContext *c,
    int n, RtToken args[], RtPointer vals[])
{
    int err = 0, i = 0;    
    //c->ReportError( RIBContext::reInfo, "fishGen setargs: %d", n);
    
    for(i=0;i<n;i++)
    {
    	if( !strcmp(args[i], "string CacheFileBody0") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_body_0);
 	}
 	else if( !strcmp(args[i], "string CacheFileBody1") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_body_1);
 	}
 	else if( !strcmp(args[i], "string CacheFileBody2") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_body_2);
 	}
 	else if( !strcmp(args[i], "string CacheFileBody3") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_body_3);
 	}
 	else if( !strcmp(args[i], "string bodySurface") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_body_surf);
 	}
 	else if( !strcmp(args[i], "string bodyDisplacement") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_body_disp);
 	}
 //	else
	//if( !strcmp(args[i], "float XMin") )
	//{
	//    RIBContextUtil::GetFloatValue(vals[i], &m_xmin);
	//}
	//else
	//if( !strcmp(args[i], "float XMax") )
	//{
	//    RIBContextUtil::GetFloatValue(vals[i], &m_xmax);
	//}
	//else
	//if( !strcmp(args[i], "float YMin") )
	//{
	//    RIBContextUtil::GetFloatValue(vals[i], &m_ymin);
	//}
	//else
	//if( !strcmp(args[i], "float YMax") )
	//{
	//    RIBContextUtil::GetFloatValue(vals[i], &m_ymax);
	//}
	//else
	//if( !strcmp(args[i], "float ZMin") )
	//{
	//    RIBContextUtil::GetFloatValue(vals[i], &m_zmin);
	//}
	//else
	//if( !strcmp(args[i], "float ZMax") )
	//{
	//    RIBContextUtil::GetFloatValue(vals[i], &m_zmax);
	//}
	else
	if( !strcmp(args[i], "float Kflap") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_k_flap);
	}
	else if( !strcmp(args[i], "float Kbend") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_k_bend);
	}
	else if( !strcmp(args[i], "float Koscillate") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_k_oscillate);
	}
	else if( !strcmp(args[i], "float Length") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_length);
	}
	else if( !strcmp(args[i], "float bonecount") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_bonecount);
	}
	else if( !strcmp(args[i], "float Frequency") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_frequency);
	}
	else if( !strcmp(args[i], "float displacementBound") )
	{
	    RIBContextUtil::GetFloatValue(vals[i], &m_displacementBound);
	}
	else if( !strcmp(args[i], "string CacheFileTeethTop0") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_top_0);
	}
	else if( !strcmp(args[i], "string CacheFileTeethTop1") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_top_1);
	}
	else if( !strcmp(args[i], "string CacheFileTeethTop2") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_top_2);
	}
	else if( !strcmp(args[i], "string CacheFileTeethTop3") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_top_3);
	}
	else if( !strcmp(args[i], "string CacheFileTeethBottom0") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_bottom_0);
	}
	else if( !strcmp(args[i], "string CacheFileTeethBottom1") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_bottom_1);
	}
	else if( !strcmp(args[i], "string CacheFileTeethBottom2") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_bottom_2);
	}
	else if( !strcmp(args[i], "string CacheFileTeethBottom3") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_teeth_bottom_3);
	}
	else if( !strcmp(args[i], "string teethSurface") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_teeth_surf);
	}
	else if( !strcmp(args[i], "string teethDisplacement") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_teeth_disp);
	}
	else if( !strcmp(args[i], "string CacheFileEye0") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_eye_0);
	}
	else if( !strcmp(args[i], "string CacheFileEye1") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_eye_1);
	}
	else if( !strcmp(args[i], "string CacheFileEye2") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_eye_2);
	}
	else if( !strcmp(args[i], "string CacheFileEye3") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_cache_eye_3);
	}
	else if( !strcmp(args[i], "string eyeSurface") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_eye_surf);
	}
	else if( !strcmp(args[i], "string eyeDisplacement") )
	{
	    RIBContextUtil::GetStringValue(vals[i], &m_eye_disp);
	}
	else
	{
	   c->ReportError( RIBContext::reInfo, 
	    	"unknown arg: %s (%d)", args[i], n);
	}
    }
	
	gethostname(hostname, sizeof(hostname));
    //MGlobal::displayInfo(hostname);
    return err;
}

void
fishGen::Bound( RIBContext *, RtBound  )
{
    // do nothing
}

int
fishGen::GenRIB( RIBContext *c )
{
	MString sproj;
	MGlobal::executeCommand(MString ("string $p = `workspace -q -fn`"), sproj);
	
	std::string pp = sproj.asChar();
	int iclear = pp.find_last_of('/', pp.size()-1);
	
	MAnimControl ftime;
	MTime mayatime = ftime.currentTime();
	
	// determine motion-blur info
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

	MStatus stat;
	
	MVectorArray ptc_pos_0, ptc_vel_0, ptc_front_0, ptc_up_0;
	MDoubleArray ptc_time_0, ptc_scale, ptc_amplitude, ptc_bend, masses;
	
	const char *objName = c->GetObjName();
	
	MObject ocache = zGetShape(MString(objName), MFn::kLocator);
	
	//if(ptcObj== MObject::kNullObj)
	//{
	//	MGlobal::displayWarning(MString("ERROR fishGen skipped non-particle shape ") + MString(objName));
	//	return -1;
	//}
	
	MObject omesh;
	zWorks::getConnectedNode(omesh, MFnDependencyNode(ocache).findPlug("inMesh"));
	MString smesh = MFnDependencyNode(omesh).name();
	
	double t;
	MGlobal::executeCommand( MString("getAttr ")+ smesh +".boundingBoxMinX", t, false, false );
	m_xmin = t*1.1;
	
	MGlobal::executeCommand( MString("getAttr ")+ smesh +".boundingBoxMaxX", t, false, false );
	m_xmax = t*1.1;
	
	MGlobal::executeCommand( MString("getAttr ")+ smesh +".boundingBoxMinY", t, false, false );
	m_ymin = t*1.1;
	
	MGlobal::executeCommand( MString("getAttr ")+ smesh +".boundingBoxMaxY", t, false, false );
	m_ymax = t*1.1;
	
	MGlobal::executeCommand( MString("getAttr ")+ smesh +".boundingBoxMinZ", t, false, false );
	m_zmin = t*1.1;
	
	MGlobal::executeCommand( MString("getAttr ")+ smesh +".boundingBoxMaxZ", t, false, false );
	m_zmax = t*1.1;
	
	MString sfile;
	MFnDependencyNode(ocache).findPlug("cacheName").getValue(sfile);
	
	sfile = sfile + "." + 250*(int)mayatime.value() + ".pdc";
	
	//MString pdcfilename = MString(m_pdc_name)+"."+250*int(mayatime.value())+".pdc";

	pdcFile* fpdc = new pdcFile();
	if(fpdc->load(sfile.asChar())==1)
	{
		MGlobal::displayInfo(MString("FishGen reading data from: ")+sfile);
		ptc_pos_0.setLength(fpdc->getParticleCount());
		ptc_vel_0.setLength(fpdc->getParticleCount());
		ptc_up_0.setLength(fpdc->getParticleCount());
		ptc_front_0.setLength(fpdc->getParticleCount());
		ptc_time_0.setLength(fpdc->getParticleCount());
		ptc_amplitude.setLength(fpdc->getParticleCount());
		ptc_bend.setLength(fpdc->getParticleCount());
		ptc_scale.setLength(fpdc->getParticleCount());
		fpdc->readPositions(ptc_pos_0, ptc_vel_0, ptc_up_0, ptc_front_0, ptc_time_0, ptc_amplitude, ptc_bend, ptc_scale, masses);
				
	}
	delete fpdc;

	
	unsigned int n_ptc = ptc_pos_0.length();
	
	MGlobal::displayInfo(MString("Number of fish: ")+n_ptc);
	
	MATRIX44F mat;
	RtMatrix transform;
	//zFMatrix44f* fmat = new zFMatrix44f();
	RtBound mybound = { m_xmin, m_xmax, m_ymin, m_ymax, m_zmin, m_zmax } ;
	
	
	XYZ front, up, side;
	float delta_t;
	for(unsigned int i=0; i<n_ptc; i++)
	{
		c->AttributeBegin();
		
		mybound[0] = m_xmin*(float)ptc_scale[i];
		mybound[1] = m_xmax*(float)ptc_scale[i];
		mybound[2] = m_ymin*(float)ptc_scale[i];
		mybound[3] = m_ymax*(float)ptc_scale[i];
		mybound[4] = m_zmin*(float)ptc_scale[i];
		mybound[5] = m_zmax*(float)ptc_scale[i];
		
		if(usingMotionBlur)
		{
			c->MotionBegin(2, shutterOpen, shutterClose);
		}
			//fmat->reset(mat);
			
			front.x = (float)ptc_front_0[i].x;
			front.y = (float)ptc_front_0[i].y;
			front.z = (float)ptc_front_0[i].z;
			
			up.x = (float)ptc_up_0[i].x;
			up.y = (float)ptc_up_0[i].y;
			up.z = (float)ptc_up_0[i].z;
			
			side = front.cross(up);
			side.normalize();
			
			up = side.cross(front);
			up.normalize();
			mat.setIdentity();
			mat.setOrientations(side, up, front);
			mat.scale(ptc_scale[i]);
			mat.setTranslation(ptc_pos_0[i].x, ptc_pos_0[i].y, ptc_pos_0[i].z);
			//side = fmat->vcross(front, up);
			//fmat->vnormalize(side);
		
			//up = fmat->vcross(side, front);
			//fmat->vnormalize(up);
			
			//fmat->setOrientation(mat, side, up, front);
			//fmat->scale(mat, (float)ptc_scale[i]);
			
			//fmat->translate(mat, (float)ptc_pos_0[i].x, (float)ptc_pos_0[i].y, (float)ptc_pos_0[i].z);
			transform[0][0] = mat(0,0);
			transform[0][1] = mat(0,1);
			transform[0][2] = mat(0,2);
			transform[0][3] = mat(0,3);
			transform[1][0] = mat(1,0);
			transform[1][1] = mat(1,1);
			transform[1][2] = mat(1,2);
			transform[1][3] = mat(1,3);
			transform[2][0] = mat(2,0);
			transform[2][1] = mat(2,1);
			transform[2][2] = mat(2,2);
			transform[2][3] = mat(2,3);
			transform[3][0] = mat(3,0);
			transform[3][1] = mat(3,1);
			transform[3][2] = mat(3,2);
			transform[3][3] = mat(3,3);
			c->ConcatTransform(transform);
		if(usingMotionBlur)
		{	
			
			//fmat->reset(mat);
			
			front.x = (float)ptc_front_0[i].x;
			front.y = (float)ptc_front_0[i].y;
			front.z = (float)ptc_front_0[i].z;
			
			up.x = (float)ptc_up_0[i].x;
			up.y = (float)ptc_up_0[i].y;
			up.z = (float)ptc_up_0[i].z;
			
			//side = fmat->vcross(front, up);
			//fmat->vnormalize(side);
		
			//up = fmat->vcross(side, front);
			//fmat->vnormalize(up);
			
			//fmat->setOrientation(mat, side, up, front);
			//fmat->scale(mat, (float)ptc_scale[i]);
			side = front.cross(up);
			side.normalize();
			
			up = side.cross(front);
			up.normalize();
			mat.setIdentity();
			mat.setOrientations(side, up, front);
			mat.scale(ptc_scale[i]);
			
			
			delta_t = shutterClose - shutterOpen;
			
			mat.setTranslation((float)ptc_pos_0[i].x + delta_t*(float)ptc_vel_0[i].x, (float)ptc_pos_0[i].y + delta_t*(float)ptc_vel_0[i].y, (float)ptc_pos_0[i].z + delta_t*(float)ptc_vel_0[i].z);
			//fmat->translate(mat, (float)ptc_pos_0[i].x + delta_t*(float)ptc_vel_0[i].x, (float)ptc_pos_0[i].y + delta_t*(float)ptc_vel_0[i].y, (float)ptc_pos_0[i].z + delta_t*(float)ptc_vel_0[i].z);
			transform[0][0] = mat(0,0);
			transform[0][1] = mat(0,1);
			transform[0][2] = mat(0,2);
			transform[0][3] = mat(0,3);
			transform[1][0] = mat(1,0);
			transform[1][1] = mat(1,1);
			transform[1][2] = mat(1,2);
			transform[1][3] = mat(1,3);
			transform[2][0] = mat(2,0);
			transform[2][1] = mat(2,1);
			transform[2][2] = mat(2,2);
			transform[2][3] = mat(2,3);
			transform[3][0] = mat(3,0);
			transform[3][1] = mat(3,1);
			transform[3][2] = mat(3,2);
			transform[3][3] = mat(3,3);
			c->ConcatTransform(transform);
			c->MotionEnd();
			
		}

		std::string b0 = m_cache_body_0;
		std::string b1 = m_cache_body_1;
		std::string b2 = m_cache_body_2;
		std::string b3 = m_cache_body_3;
		std::string tt0 = m_cache_teeth_top_0;
		std::string tt1 = m_cache_teeth_top_1;
		std::string tt2 = m_cache_teeth_top_2;
		std::string tt3 = m_cache_teeth_top_3;
		std::string tb0 = m_cache_teeth_bottom_0;
		std::string tb1 = m_cache_teeth_bottom_1;
		std::string tb2 = m_cache_teeth_bottom_2;
		std::string tb3 = m_cache_teeth_bottom_3;
		std::string e0 = m_cache_eye_0;
		std::string e1 = m_cache_eye_1;
		std::string e2 = m_cache_eye_2;
		std::string e3 = m_cache_eye_3;
		
   // 		sharedpath(b0, iclear, hostname);
   // 		sharedpath(b1, iclear, hostname);
   // 		sharedpath(b2, iclear, hostname);
   // 		sharedpath(b3, iclear, hostname);
   // 		sharedpath(tb0, iclear, hostname);
   // 		sharedpath(tb1, iclear, hostname);
   // 		sharedpath(tb2, iclear, hostname);
   // 		sharedpath(tb3, iclear, hostname);
   // 		sharedpath(tt0, iclear, hostname);
   // 		sharedpath(tt1, iclear, hostname);
   // 		sharedpath(tt2, iclear, hostname);
   // 		sharedpath(tt3, iclear, hostname);
   // 		sharedpath(e0, iclear, hostname);
   // 		sharedpath(e1, iclear, hostname);
   // 		sharedpath(e2, iclear, hostname);
   // 		sharedpath(e3, iclear, hostname);
    		
    		//
		
		setupShaders(c, m_body_surf, m_body_disp);
		
		char sbuf[1200];
		sprintf(sbuf,"%s %s %s %s %f %f %f %f %f %f %f %i %f %f", 
		b0.c_str(), b1.c_str(), b2.c_str(), b3.c_str(), 
		ptc_time_0[i]/4.0f, ptc_amplitude[i]*600.0f*m_k_flap, ptc_bend[i]*1200.0f*m_k_bend, ptc_amplitude[i]*90.0f*m_k_oscillate, m_length, m_bonecount, m_frequency, 
		usingMotionBlur, 
		shutterOpen, shutterClose);
		
		RtString args[] = { "plugins/afish.dll", sbuf };
		
		c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
		
		/*
		setupShaders(c, m_teeth_surf, m_teeth_disp);
		
		sprintf(sbuf,"%s %s %s %s %f %f %f %f %f %f %f %i %f %f", 
		tt0.c_str(), tt1.c_str(), tt2.c_str(), tt3.c_str(), 
		ptc_time_0[i]/4.0f, ptc_amplitude[i]*600.0f*m_k_flap, ptc_bend[i]*1200.0f*m_k_bend, ptc_amplitude[i]*90.0f*m_k_oscillate, m_length, m_bonecount, m_frequency, 
		usingMotionBlur, 
		shutterOpen, shutterClose);
		
		c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
		
		setupShaders(c, m_teeth_surf, m_teeth_disp);
		
		sprintf(sbuf,"%s %s %s %s %f %f %f %f %f %f %f %i %f %f", 
		tb0.c_str(), tb1.c_str(), tb2.c_str(), tb3.c_str(), 
		ptc_time_0[i]/4.0f, ptc_amplitude[i]*600.0f*m_k_flap, ptc_bend[i]*1200.0f*m_k_bend, ptc_amplitude[i]*90.0f*m_k_oscillate, m_length, m_bonecount, m_frequency, 
		usingMotionBlur, 
		shutterOpen, shutterClose);

		c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
		
		setupShaders(c, m_eye_surf, m_eye_disp);
		
		sprintf(sbuf,"%s %s %s %s %f %f %f %f %f %f %f %i %f %f",  
		e0.c_str(), e1.c_str(), e2.c_str(), e3.c_str(), 
		ptc_time_0[i]/4.0f, ptc_amplitude[i]*600.0f*m_k_flap, ptc_bend[i]*1200.0f*m_k_bend, ptc_amplitude[i]*90.0f*m_k_oscillate, m_length, m_bonecount, m_frequency, 
		usingMotionBlur, 
		shutterOpen, shutterClose);

		c->Procedural((RtPointer)args, mybound, c->GetProcSubdivFunc(c->ProceduralSubdivFunction::kDynamicLoad), c->GetProcFreeFunc());
*/
		c->AttributeEnd();
	}
	
	//delete fmat;
	return 0;

}

void fishGen::setupShaders(RIBContext *c, const char* surface, const char* displacement)
{
	char* space = "shader";
	if(strcmp(surface, "null"))
	{
		c->TransformBegin();
		c->Surface((RtToken)surface, RI_NULL);
		c->TransformEnd();
	}
	if(strcmp(displacement, "null"))
	{
		c->TransformBegin();
		c->Attribute((RtToken)"displacementbound", (RtToken)"sphere", (RtPointer)&m_displacementBound, (RtToken)"coordinatesystem", (RtPointer)&space, RI_NULL);
		
		c->Displacement((RtToken)displacement, RI_NULL);
		c->TransformEnd();
	}
}