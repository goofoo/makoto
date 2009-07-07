/*
 *  vxCacheDeformer.cpp
 *  
 *
 *  Created by zhang on 08-1-12.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "vxCacheDeformer.h"

#include "../shared/zWorks.h"
#include <maya/MAnimControl.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>

#include "../shared/FMCFMesh.h"

MTypeId		vxCacheDeformer::id( 0x00025254 );
MObject     vxCacheDeformer::path; 
MObject     vxCacheDeformer::frame;
MObject     vxCacheDeformer::aminframe; 
MObject     vxCacheDeformer::amaxframe;
MObject     vxCacheDeformer::aframestep;



vxCacheDeformer::vxCacheDeformer() 
{

}
vxCacheDeformer::~vxCacheDeformer() 
{

}

void* vxCacheDeformer::creator()
{
	return new vxCacheDeformer();
}

MStatus vxCacheDeformer::initialize()
{
	MFnNumericAttribute	numAttr;
	MFnTypedAttribute   stringAttr;
	MStatus 		status;

	status = zWorks::createTimeAttr(frame, "currentTime", "ct", 1.0);
	zCheckStatus(status, "ERROR creating time");
	zCheckStatus(addAttribute(frame), "ERROR adding time");
	
	path = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( path );
	
	aminframe = numAttr.create( "minFrame", "mnf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aminframe );
	
	amaxframe = numAttr.create( "maxFrame", "mxf", MFnNumericData::kInt, 24 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( amaxframe );
	
	aframestep = numAttr.create( "frameStep", "fst", MFnNumericData::kInt, 1 );
	numAttr.setMin(1);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframestep );
	
	attributeAffects( frame, vxCacheDeformer::outputGeom );
	attributeAffects( path, vxCacheDeformer::outputGeom );
	attributeAffects( aminframe, vxCacheDeformer::outputGeom );
	attributeAffects( amaxframe, vxCacheDeformer::outputGeom );
	attributeAffects( aframestep, vxCacheDeformer::outputGeom );

	return MStatus::kSuccess;
}


MStatus vxCacheDeformer::deform( MDataBlock& block,
				MItGeometry& iter,
				const MMatrix& m,
				unsigned int multiIndex)
{
	MStatus returnStatus;
	
	MDataHandle envData = block.inputValue(envelope,&returnStatus);
	float env = envData.asFloat();	MGlobal::displayInfo(MString("z ")+env);
	if(env == 0) return returnStatus;
	
	double time = block.inputValue( frame ).asTime().value();
	
	MDataHandle inPathData = block.inputValue( path );
	MString str_path =  inPathData.asString();
	
	MDataHandle inMinFrmData = block.inputValue( aminframe );
	int minfrm = inMinFrmData.asInt();
	
	MDataHandle inMaxFrmData = block.inputValue( amaxframe );
	MDataHandle inFrmStepData = block.inputValue( aframestep );
	int frmstep = inFrmStepData.asInt();
	
	if( time < minfrm )
		time = minfrm;
		
	int frame_lo = minfrm + int(time-minfrm)/frmstep*frmstep;
	int frame_hi = frame_lo+frmstep;
	
	if( strlen( str_path.asChar() ) > 0 ) 
	{
	
		char filename[256];
		sprintf( filename, "%s.%d.mcf", str_path.asChar(), frame_lo );
	
		FMCFMesh mesh;
		if(mesh.load(filename) != 1)
		{
			MGlobal::displayError( MString("Failed to open file: ") + filename );
			return MS::kFailure;
		}
		
		int lo_n_vertex = mesh.getNumVertex();
		
		vertexArray.clear();
		vertexFArray.clear();
		
		XYZ tp;
		for(unsigned int i = 0; i < mesh.getNumVertex(); i++) {
			mesh.getVertex(tp, i);
			vertexArray.append( MPoint( tp.x, tp.y, tp.z ) );
		}

		if( time > frame_lo ) 
		{
			sprintf( filename, "%s.%d.mcf", str_path.asChar(), frame_hi );

			if(mesh.load(filename) != 1) MGlobal::displayError( MString("Failed to open file: ") + filename );

			else if(mesh.getNumVertex() == lo_n_vertex)
			{
				XYZ tp;
				for(unsigned int i = 0; i < mesh.getNumVertex(); i++) 
				{
					mesh.getVertex(tp, i);
					vertexFArray.append( MPoint( tp.x, tp.y, tp.z ) );
				}
				
				double alpha = double(time-frame_lo) / (double)frmstep;

				for(unsigned int i = 0; i < mesh.getNumVertex(); i++) {
					
					vertexArray[i] = vertexArray[i] + ( vertexFArray[i] - vertexArray[i] )*alpha;
					
				}
			}
		}

		// iterate through each point in the geometry
		//
		for ( ; !iter.isDone(); iter.next()) 
		{
			MPoint pt = iter.position();
			pt = pt + (vertexArray[iter.index()] - pt)*env;
			iter.setPosition(pt);
		}
	}
	
	return returnStatus;
}
