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
MObject curvePatchNode::arotate;

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
		float rotate = data.inputValue(arotate).asFloat();
		
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
			
			unsigned num_seg = fcurve.length()/width/2;
			if(num_seg < 4) num_seg = 4;
			else if(num_seg > 25) num_seg = 25;
			
			double minparam, maxparam;
			fcurve.getKnotDomain(minparam, maxparam);
			double dparam = (maxparam - minparam)/num_seg;
			
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
         
			MPoint cent;
			fcurve.getPointAtParam(minparam, cent, MSpace::kObject );
			fbase.getClosestPointAndNormal (cent, closestp, closestn, MSpace::kObject, &closestPolygonID );
			fbase.getPolygonVertices(closestPolygonID,vertexList);
			fbase.getFaceVertexTangent(closestPolygonID,vertexList[0],tangent,MSpace::kObject,NULL);
			tangent.normalize();
			
			if(rotate != 0) {
				MVector nn;
				fbase.getPolygonNormal (closestPolygonID, nn, MSpace::kObject );
				nn.normalize();
				XYZ ax(nn.x, nn.y, nn.z);
				XYZ vt(tangent.x, tangent.y, tangent.z);
				vt.rotateAroundAxis(ax, rotate);
				tangent.x = vt.x;
				tangent.y = vt.y;
				tangent.z = vt.z;
			}
			
			XYZ side(tangent.x, tangent.y, tangent.z);
			for(unsigned j = 0;j <= num_seg;j++)
			{
				double param = minparam + dparam*j;
				
				fcurve.getPointAtParam( param, cent, MSpace::kObject );
				
				MPoint vert;
				
				MVector cdir = fcurve.tangent(param, MSpace::kObject);
				XYZ axis(cdir.x, cdir.y, cdir.z);
				
				side.rotateAroundAxis(axis, twist/num_seg*j);
				
				tangent.x = side.x*width;
				tangent.y = side.y*width;
				tangent.z = side.z*width;
				 
				vert = cent - tangent;
				vertexArray.append(vert);
				
				vert = cent + tangent;
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
    CHECK_MSTATUS( numAttr.setMin(0.01f));
	addAttribute(aSize);
	
	arotate = numAttr.create("rotate", "rot",
						  MFnNumericData::kFloat, 0, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(arotate);
	
	atwist = numAttr.create("twist", "tw",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(atwist);
	
	zCheckStatus(zWorks::createTypedAttr(aoutput, MString("outMesh"), MString("om"), MFnData::kMesh), "ERROR creating out mesh");
	zCheckStatus(addAttribute(aoutput), "ERROR adding out mesh");
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedArrayAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kNurbsCurve);
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	attributeAffects( aSize, aoutput );
	attributeAffects( atwist, aoutput );
	attributeAffects( arotate, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	
	return MS::kSuccess;
}
//:~
