#include "curvePatchNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

MTypeId curvePatchNode::id( 0x0002536 );

MObject	curvePatchNode::aSize;
MObject curvePatchNode::aoutput;
MObject curvePatchNode::agrowth;
MObject curvePatchNode::aguide;

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
		
		MArrayDataHandle hArray = data.inputArrayValue(aguide);
		int n_guide = hArray.elementCount();
		MObjectArray guidelist;
		for(int i=0; i<n_guide; i++)
		{
			guidelist.append(hArray.inputValue().asMesh());
			hArray.next();
		}
	    
		MObject outMeshData;
		
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
	
	zCheckStatus(zWorks::createTypedAttr(aoutput, MString("outMesh"), MString("om"), MFnData::kMesh), "ERROR creating out mesh");
	zCheckStatus(addAttribute(aoutput), "ERROR adding out mesh");
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedArrayAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kNurbsCurve);
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	attributeAffects( aSize, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	
	return MS::kSuccess;
}
//:~
