/*
 *  hairDeformer.cpp
 *  
 *
 *  Created by zhang on 08-1-12.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "hairDeformer.h"

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

MTypeId		hairDeformer::id( 0x00006345 );
MObject 	hairDeformer::anumseg;
MObject     hairDeformer::frame;
MObject  hairDeformer::agrowth;
//MObject     hairDeformer::amaxframe;
MObject     hairDeformer::aframestep;



hairDeformer::hairDeformer():faceId(0),pobj(0)
{

}
hairDeformer::~hairDeformer() 
{
	if(pobj) delete[] pobj;
	if(faceId) delete[] faceId;
}

void* hairDeformer::creator()
{
	return new hairDeformer();
}

MStatus hairDeformer::initialize()
{
	MFnNumericAttribute	numAttr;
	MFnTypedAttribute   stringAttr;
	MStatus 		status;

	status = zWorks::createTimeAttr(frame, "currentTime", "ct", 1.0);
	zCheckStatus(status, "ERROR creating time");
	zCheckStatus(addAttribute(frame), "ERROR adding time");
	
	//path = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	//stringAttr.setStorable(true);
	//addAttribute( path );
	
	anumseg = numAttr.create( "numSegment", "nsg", MFnNumericData::kInt, 5 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( anumseg );
	
	//amaxframe = numAttr.create( "maxFrame", "mxf", MFnNumericData::kInt, 24 );
	//numAttr.setStorable(true);
	//numAttr.setKeyable(true);
	//addAttribute( amaxframe );
	
	aframestep = numAttr.create( "startFrame", "fst", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframestep );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	attributeAffects( frame, hairDeformer::outputGeom );
	attributeAffects( agrowth, hairDeformer::outputGeom );
	attributeAffects( anumseg, hairDeformer::outputGeom );
	//attributeAffects( amaxframe, hairDeformer::outputGeom );
	attributeAffects( aframestep, hairDeformer::outputGeom );

	return MStatus::kSuccess;
}


MStatus hairDeformer::deform( MDataBlock& block,
				MItGeometry& iter,
				const MMatrix& m,
				unsigned int multiIndex)
{
	MStatus status;
	
	MDataHandle envData = block.inputValue(envelope,&status);
	float env = envData.asFloat();
	if(env == 0) return status;
	
	int num_seg = block.inputValue(anumseg).asInt();
	
	int num_vert = (num_seg+1)*2;
	
	MObject ogrow = block.inputValue(agrowth).asMesh();
	
	MFnMesh fbase(ogrow, &status);
	MItMeshPolygon itbase(ogrow, &status);
	if(!status)
	{
		MGlobal::displayWarning("no mesh connected, do nothing");
		return MS::kUnknownParameter;
	}

	double time = block.inputValue( frame ).asTime().value();
	
	int startfrm = block.inputValue( aframestep ).asInt();
	if(startfrm == (int)time) {
// init
		MGlobal::displayInfo("hair deformer init");
		num_hair = iter.count()/num_vert;
		
		if(pobj) delete[] pobj;
		pobj = new XYZ[iter.count()];
		
		if(faceId) delete[] faceId;
		faceId = new int[num_hair];
		
		MATRIX44F spaceinv;
		for ( ; !iter.isDone(); iter.next()) {
		
			MPoint pt = iter.position();
			XYZ pos(pt.x , pt.y, pt.z);
			
			int hair_id = iter.index()/num_vert;
			if(iter.index()%num_vert == 0) {
// get nearest face space
				MPoint closestp;
				MVector closestn;
				int closestPolygonID;
				MIntArray vertexList;
				MVector tangent;
         
				fbase.getClosestPoint (pt, closestp, MSpace::kObject, &closestPolygonID );
				fbase.getPolygonVertices(closestPolygonID, vertexList);

// save nearest face id
				faceId[hair_id] = closestPolygonID;
				
				fbase.getFaceVertexTangent(closestPolygonID,vertexList[0], tangent, MSpace::kObject, NULL);
			
				int preid;
				itbase.setIndex (closestPolygonID , preid);
				
				itbase.getNormal(closestn, MSpace::kObject );
				MPoint cc = itbase.center ( MSpace::kObject );
				
				tangent = closestn^tangent;
				tangent.normalize();
				closestn.normalize();

				XYZ xdir(tangent.x, tangent.y, tangent.z);
				XYZ zdir(closestn.x, closestn.y, closestn.z);
				XYZ ydir = zdir.cross(xdir);
				XYZ cen(cc.x , cc.y, cc.z);
// set space
				spaceinv.setIdentity();
				spaceinv.setOrientations(xdir, ydir, zdir);
				spaceinv.setTranslation(cen);
				spaceinv.inverse();
			}
// save local pos
			pobj[iter.index()] = pos;
			spaceinv.transform(pobj[iter.index()]);
		}
	}
	else {
		if(faceId && pobj) {
		
			MATRIX44F* space = new MATRIX44F[num_hair];
			
			for(unsigned i=0; i<num_hair; i++) {
				
				MIntArray vertexList;
				MVector tangent;
         
				fbase.getPolygonVertices(faceId[i], vertexList);
				fbase.getFaceVertexTangent(faceId[i], vertexList[0], tangent, MSpace::kObject, NULL);
				
				int preid;
				itbase.setIndex (faceId[i], preid);
				
				MVector facen;
				itbase.getNormal(facen, MSpace::kObject );
				MPoint cc = itbase.center ( MSpace::kObject );
				
				tangent = facen^tangent;
				tangent.normalize();
				facen.normalize();

				XYZ xdir(tangent.x, tangent.y, tangent.z);
				XYZ zdir(facen.x, facen.y, facen.z);
				XYZ ydir = zdir.cross(xdir);
				XYZ cen(cc.x , cc.y, cc.z);
				
				space[i].setIdentity();
				space[i].setOrientations(xdir, ydir, zdir);
				space[i].setTranslation(cen);
			
			}
			
			for ( ; !iter.isDone(); iter.next()) {
				
				int hair_id = iter.index()/num_vert;
				
				XYZ pp = pobj[iter.index()];
				space[hair_id].transform(pp);
				
				MPoint pt(pp.x, pp.y, pp.z);
				iter.setPosition(pt);
			}
			
			delete[] space;
		}
	}
	return status;
}
