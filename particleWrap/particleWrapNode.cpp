// fish bone and skining visualization

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MAngle.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MMatrix.h>

#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshVertex.h>
#include <maya/MString.h> 
#include <maya/MItMeshPolygon.h>
#include <maya/MFnNurbsSurfaceData.h>

#include "particleWrapNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"

MTypeId particleWrapNode::id( 0x0002517 );
MObject particleWrapNode::aInMesh;
MObject particleWrapNode::abindMesh;
MObject particleWrapNode::aPosition;
MObject particleWrapNode::astarttime;
MObject particleWrapNode::atime;
MObject particleWrapNode::aoutval;

particleWrapNode::particleWrapNode():pobj(0), faceId(0)
{
}

particleWrapNode::~particleWrapNode() 
{
	if(pobj) delete[] pobj;
	if(faceId) delete[] faceId;
}

MStatus particleWrapNode::compute( const MPlug& plug, MDataBlock& data )
{    
    MStatus status;
	if(plug == aoutval) {
		int startFrame = data.inputValue(astarttime, &status).asInt();
		MTime currentTime = data.inputValue(atime, &status).asTime();
		int frame = (int)currentTime.value();
		
		MObject obase = data.inputValue(aInMesh).asMesh();
		MFnMesh fbase(obase, &status);
		MItMeshPolygon itbase(obase, &status);
		if(!status) {
			MGlobal::displayWarning("no base mesh connected, do nothing");
			return MS::kUnknownParameter;
		}
		MObject obind = data.inputValue(abindMesh).asMesh();
		MFnMesh fbind(obind, &status);
		MItMeshPolygon itbind(obind, &status);
		if(!status) {
			MGlobal::displayWarning("no bind mesh connected, do nothing");
			return MS::kUnknownParameter;
		}
	
		MVectorArray ptc_positions = zGetVectorArrayAttr(data, aPosition);
		
		unsigned num_ptc = ptc_positions.length();
		
		MATRIX44F spaceinv, space;
		if(startFrame == frame) {
			if(pobj) delete[] pobj;
			if(faceId) delete[] faceId;
			
			pobj = new XYZ[num_ptc];
			faceId = new int[num_ptc];
			
			for(unsigned i=0; i<num_ptc; i++) {
// get nearest face space
				MPoint closestp;
				MVector closestn;
				int closestPolygonID;
				MIntArray vertexList;
				MVector tangent;
         
				fbase.getClosestPoint (ptc_positions[i], closestp, MSpace::kObject, &closestPolygonID );
				fbase.getPolygonVertices(closestPolygonID, vertexList);
// save nearest face id
				faceId[i] = closestPolygonID;
				
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

				pobj[i] = XYZ(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
				spaceinv.transform(pobj[i]);
			}
		}
		else {
			if(pobj && faceId) {
				for(unsigned i=0; i<num_ptc; i++) {
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
					
					space.setIdentity();
					space.setOrientations(xdir, ydir, zdir);
					space.setTranslation(cen);
					
					XYZ pp = pobj[i]; 
					space.transform(pp);
					
					ptc_positions[i] = MPoint(pp.x, pp.y, pp.z);
				}
			}
		}
		
		MDataHandle outputHandle = data.outputValue(aoutval, &status);
		outputHandle.set(1.0);
		data.setClean( plug );
	
	}
	return MS::kSuccess;
}

void* particleWrapNode::creator()
{
	return new particleWrapNode();
}

/////////////////////////////////////
// Attribute Setup and Maintenance //
/////////////////////////////////////





MStatus particleWrapNode::initialize()
{
	MStatus stat;
	MFnNumericAttribute numAttr;
	
	aoutval = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutval );
	
	stat = zCreateTypedAttr(aInMesh, MString("baseMesh"), MString("bsm"), MFnData::kMesh);
	zCheckStatus(stat, "failed to add in mesh attr");
	stat = addAttribute( aInMesh );
	
	stat = zCreateTypedAttr(abindMesh, MString("bindMesh"), MString("bdm"), MFnData::kMesh);
	zCheckStatus(stat, "failed to add in mesh attr");
	stat = addAttribute( abindMesh );
	
	stat = zCreateTypedAttr(aPosition, MString("position"), MString("pos"), MFnData::kVectorArray);
	zCheckStatus(stat, "failed to add position attr");
	stat = addAttribute(aPosition);

	stat = zCreateTimeAttr(atime, "currentTime", MString("ct"), 1.0);
	zCheckStatus(stat, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	astarttime = numAttr.create( "startFrame", "fst", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(astarttime);
	
	stat = attributeAffects( aPosition, aoutval );
	stat = attributeAffects( atime, aoutval );
	stat = attributeAffects( astarttime, aoutval );
	stat = attributeAffects( aInMesh, aoutval );
	stat = attributeAffects( abindMesh, aoutval );
	
	return MS::kSuccess;
}

