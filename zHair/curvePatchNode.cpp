#include "curvePatchNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnMeshData.h>

MTypeId curvePatchNode::id( 0x0002536 );

MObject	curvePatchNode::aSize;
MObject curvePatchNode::atwist;
MObject curvePatchNode::aoutput;
MObject curvePatchNode::agrowth;
MObject curvePatchNode::aguide;
MObject curvePatchNode::abinormal;

curvePatchNode::curvePatchNode()
{
}

curvePatchNode::~curvePatchNode() 
{
}

MStatus curvePatchNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput)
	{
		MObject ogrow = data.inputValue(agrowth).asMesh();
		
		float width = data.inputValue(aSize).asFloat();
		float twist = data.inputValue(atwist).asFloat();
		int isBinormal = data.inputValue(abinormal).asInt();
		
		MFnMesh fbase(ogrow, &status);
		if(!status)
		{
			MGlobal::displayWarning("no mesh connected, do nothing");
			return MS::kUnknownParameter;
		}
		
		MArrayDataHandle hArray = data.inputArrayValue(aguide);
		int n_guide = hArray.elementCount();
		MObjectArray curvelist;
		for(int i=0; i<n_guide; i++)
		{
			curvelist.append(hArray.inputValue().asNurbsCurve());
			hArray.next();
		}
		
		MPointArray vertexArray;
		MIntArray polygonCounts;
		MIntArray polygonConnects;
		MFloatArray uarray, varray;
		
		unsigned acc=0;
		int num_face = 0;
		for(unsigned i=0; i<curvelist.length(); i++)
		{
			MFnNurbsCurve fcurve(curvelist[i]);
		
			MPointArray cvs;
			fcurve.getCVs ( cvs, MSpace::kObject );
			
			unsigned num_seg = cvs.length() - 1;
			num_face += num_seg;
			for(unsigned j = 0;j < num_seg;j++)
			{
				polygonCounts.append(4);
				
				polygonConnects.append(acc+2*j);
				polygonConnects.append(acc+2*j+2);
				polygonConnects.append(acc+2*j+3);
				polygonConnects.append(acc+2*j+1);
			}
			
			acc += 2*(num_seg+1);
			
			MPoint closestp;
			MVector closestn;
			int closestPolygonID;
			MIntArray vertexList;
			MVector tangent;
         
			fbase.getClosestPointAndNormal (cvs[0], closestp, closestn, MSpace::kObject, &closestPolygonID );
			fbase.getPolygonVertices(closestPolygonID,vertexList);
			fbase.getFaceVertexTangent(closestPolygonID,vertexList[0],tangent,MSpace::kObject,NULL);
			//MVector dir = cvs[1] - cvs[0];
			//dir.normalize();
			//MVector tangent = closestn^dir;
			tangent.normalize();
			
			if(isBinormal == 1) {
				MVector nn;
				fbase.getPolygonNormal (closestPolygonID, nn, MSpace::kObject );
				nn.normalize();
				tangent = nn^tangent;
			}
			
			for(unsigned j = 0;j <= num_seg;j++)
			{
				MPoint vert;
				XYZ side(tangent.x, tangent.y, tangent.z);
				XYZ axis;
				if(j<num_seg)
				{
					axis.x = cvs[j+1].x - cvs[j].x;
					axis.y = cvs[j+1].y - cvs[j].y;
					axis.z = cvs[j+1].z - cvs[j].z;
				}
				else
				{
					axis.x = cvs[j].x - cvs[j-1].x;
					axis.y = cvs[j].y - cvs[j-1].y;
					axis.z = cvs[j].z - cvs[j-1].z;
				}
				axis.normalize();
				
				side.rotateAroundAxis(axis, twist/num_seg*j);
				
				vert = cvs[j] - MVector(side.x, side.y, side.z)*width;
				vertexArray.append(vert);
				
				vert = cvs[j] + MVector(side.x, side.y, side.z)*width;
				vertexArray.append(vert);
				
				uarray.append(0);
				uarray.append(1.f/(float)num_seg);
				varray.append(1.f/(float)num_seg*j);
				varray.append(1.f/(float)num_seg*j);
			}
		}
	    
		MFnMeshData dataCreator;
		MObject outMeshData = dataCreator.create(&status);
		
		MFnMesh meshFn;
		meshFn.create(vertexArray.length(), num_face, vertexArray, polygonCounts, polygonConnects, outMeshData );
		meshFn.setUVs ( uarray, varray );
		meshFn.assignUVs ( polygonCounts, polygonConnects );
		
		MDataHandle meshh = data.outputValue(aoutput, &status);
		meshh.set(outMeshData);
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void* curvePatchNode::creator()
{
	return new curvePatchNode();
}

MStatus curvePatchNode::initialize()
{ 
	MStatus			 status;
	MFnNumericAttribute numAttr;
    MFnTypedAttribute tAttr;
	
	aSize = numAttr.create("width", "wd",
						  MFnNumericData::kFloat, 1.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
    CHECK_MSTATUS( numAttr.setMin(0.f));
	addAttribute(aSize);
	
	atwist = numAttr.create("twist", "tw",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(atwist);
	
	abinormal = numAttr.create("useBinormal", "ub",
						  MFnNumericData::kLong, 0, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(abinormal);
	
	zCheckStatus(zWorks::createTypedAttr(aoutput, MString("outMesh"), MString("om"), MFnData::kMesh), "ERROR creating out mesh");
	zCheckStatus(addAttribute(aoutput), "ERROR adding out mesh");
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedArrayAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kNurbsCurve);
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	attributeAffects( aSize, aoutput );
	attributeAffects( atwist, aoutput );
	attributeAffects( abinormal, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	
	return MS::kSuccess;
}
//:~
