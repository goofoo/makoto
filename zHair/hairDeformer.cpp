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
MObject     hairDeformer::frame;
MObject  hairDeformer::agrowth;
MObject     hairDeformer::abase;
MObject     hairDeformer::aframestep;
MObject     hairDeformer::abindbase;


hairDeformer::hairDeformer():faceId(0),pobj(0),nsegbuf(0)
{

}
hairDeformer::~hairDeformer() 
{
	if(pobj) delete[] pobj;
	if(faceId) delete[] faceId;
	if(nsegbuf) delete[] nsegbuf;
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
	
	aframestep = numAttr.create( "startFrame", "fst", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframestep );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedAttr(abase, MString("baseMesh"), MString("bm"), MFnData::kMesh);
	zCheckStatus(addAttribute(abase), "ERROR adding base mesh");
	
	zWorks::createTypedAttr(abindbase, MString("bindMesh"), MString("bnd"), MFnData::kMesh);
	zCheckStatus(addAttribute(abindbase), "ERROR adding bind mesh");
	
	attributeAffects( frame, hairDeformer::outputGeom );
	attributeAffects( agrowth, hairDeformer::outputGeom );
	attributeAffects( abase, hairDeformer::outputGeom );
	attributeAffects( abindbase, hairDeformer::outputGeom );
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
	
	MObject ogrow = block.inputValue(agrowth).asMesh();
	MObject obind = block.inputValue(abindbase).asMesh();
	MObject obase = block.inputValue(abase).asMesh();
	
	MFnMesh fbase(ogrow, &status);
	MItMeshPolygon itbase(ogrow, &status);
	if(!status)
	{
		MGlobal::displayWarning("no grow mesh connected, do nothing");
		return MS::kUnknownParameter;
	}
	
	MFnMesh fbind(obind, &status);
	MItMeshPolygon itbind(obind, &status);
	if(!status)
	{
		MGlobal::displayWarning("no base mesh connected, do nothing");
		return MS::kUnknownParameter;
	}
	
	MFnMesh fori(obase, &status);
	MItMeshPolygon itori(obase, &status);
	if(!status)
	{
		MGlobal::displayWarning("no bind mesh connected, do nothing");
		return MS::kUnknownParameter;
	}

	double time = block.inputValue( frame ).asTime().value();
	
	int startfrm = block.inputValue( aframestep ).asInt();
	if(startfrm == (int)time) {
		MGlobal::displayInfo("hair deformer init");
// init get number of hair

		num_hair = 0;
		int nconn;
		for(; !itori.isDone(); itori.next()) {
			itori.numConnectedFaces (nconn);
			if(nconn==1) num_hair++;
		}
		num_hair /= 2;

// store per-hair segment		
		if(nsegbuf) delete[] nsegbuf;
		nsegbuf = new int[num_hair];
		
		itori.reset();
		char isstart = 0;
		int nseg = 0, ihair = 0;
		for(; !itori.isDone(); itori.next()) {
			nseg++;
			itori.numConnectedFaces (nconn);
			if(nconn==1) {
				if(!isstart) isstart = 1;
				else {
					nsegbuf[ihair] = nseg;
					nseg = 0;
					ihair++;
					isstart = 0;
				}
			}
		}
		
		if(pobj) delete[] pobj;
		pobj = new XYZ[iter.count()];
		
		if(faceId) delete[] faceId;
		faceId = new int[num_hair];
		
		MATRIX44F spaceinv;
		int hair_id = -1, vert_acc = 0;
		for ( ; !iter.isDone(); iter.next()) {
		
			MPoint pt = iter.position();
			XYZ pos(pt.x , pt.y, pt.z);
			
			if(iter.index() == vert_acc) {
				hair_id++;
				vert_acc += (nsegbuf[hair_id]+1)*2;
				
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
	iter.reset();
		if(faceId && pobj && nsegbuf) {
			
			MATRIX44F* space = new MATRIX44F[num_hair];
			
			for(unsigned i=0; i<num_hair; i++) {
				
				MIntArray vertexList;
				MVector tangent;
         
				fbind.getPolygonVertices(faceId[i], vertexList);
				fbind.getFaceVertexTangent(faceId[i], vertexList[0], tangent, MSpace::kObject, NULL);
				
				int preid;
				itbind.setIndex (faceId[i], preid);
				
				MVector facen;
				itbind.getNormal(facen, MSpace::kObject );
				MPoint cc = itbind.center ( MSpace::kObject );
				
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
			
			int hair_id = -1, vert_acc = 0;
			for ( ; !iter.isDone(); iter.next()) {
				if(iter.index() == vert_acc) {
					hair_id++;
					vert_acc += (nsegbuf[hair_id]+1)*2;
				}
				
				XYZ pp = pobj[iter.index()];
				space[hair_id].transform(pp);
				
				MPoint pt(pp.x, pp.y, pp.z);
				iter.setPosition(pt);
			}
			
			delete[] space;
		}

	return status;
}
