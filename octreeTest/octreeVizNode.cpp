#include "octreeVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include <maya/MItMeshVertex.h>

MTypeId octreeVizNode::id( 0x0003508 );

MObject octreeVizNode::afuzz;
MObject octreeVizNode::astartframe;
MObject octreeVizNode::acurrenttime;
MObject	octreeVizNode::aSize;
MObject	octreeVizNode::aHDRName;
MObject octreeVizNode::aworldSpace;
MObject octreeVizNode::aoutput;
MObject octreeVizNode::agrowth;
MObject octreeVizNode::aguide;
MObject octreeVizNode::alengthnoise;
MObject octreeVizNode::asavemap;
MObject octreeVizNode::astep;
MObject octreeVizNode::aalternativepatch;

octreeVizNode::octreeVizNode():raw_data(0),num_raw_data(0),tree(0)
{
}

octreeVizNode::~octreeVizNode() 
{
	if(tree) delete tree;
}

MStatus octreeVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput)
	{
		MArrayDataHandle hArray = data.inputArrayValue(aguide);
		int n_guide = hArray.elementCount();
		MObjectArray guidelist;
		for(int i=0; i<n_guide; i++) {
			guidelist.append(hArray.inputValue().asMesh());
			hArray.next();
		}
		
		unsigned num_vert = 0;
		for(unsigned iguide=0; iguide<guidelist.length(); iguide++) {
			MFnMesh meshFn(guidelist[iguide], &status);
			num_vert += meshFn.numVertices();
		}
		
		zDisplayFloat(num_vert);
		
		if(raw_data) delete[] raw_data;
		
		raw_data = new XYZ[num_vert];
		
		num_raw_data = 0;
		for(unsigned iguide=0; iguide<guidelist.length(); iguide++) {
			MItMeshVertex vertIter(guidelist[iguide], &status);
			
			for(; !vertIter.isDone(); vertIter.next()) {
				MPoint vert = vertIter.position ( MSpace::kObject);
				raw_data[num_raw_data] = XYZ(vert.x, vert.y, vert.z);
				num_raw_data++;
			}
		}
		
		XYZ rootCenter;
		float rootSize;
		OcTree::getBBox(raw_data, num_raw_data, rootCenter, rootSize);
		
		if(tree) delete tree;
		tree = new OcTree();
		tree->construct(raw_data, num_raw_data, rootCenter, rootSize);
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void octreeVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	glPointSize(3);
	if(num_raw_data > 0 && raw_data) {
		glBegin(GL_POINTS);
		glColor3f(1,0,0);
		for(unsigned i=0; i<num_raw_data; i++) {
			
			glVertex3f(raw_data[i].x, raw_data[i].y, raw_data[i].z);
		}
		glEnd();
		
		glBegin(GL_LINES);
		if(tree) tree->draw();
		glEnd();
	}
	
	glPopAttrib();
	view.endGL();
}

bool octreeVizNode::isBounded() const
{ 
	return false;
}

MBoundingBox octreeVizNode::boundingBox() const
{ 
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* octreeVizNode::creator()
{
	return new octreeVizNode();
}

MStatus octreeVizNode::initialize()
{ 
	MStatus			 status;
	MFnNumericAttribute numAttr;
    MFnTypedAttribute tAttr;
	
	afuzz = numAttr.create( "fuzz", "fuzz", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(afuzz);
	
	alengthnoise = numAttr.create("kink", "kink",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(alengthnoise);
	
	aSize = numAttr.create("clumping", "clp",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
    CHECK_MSTATUS( numAttr.setDefault(0.f));
	numAttr.setCached( true );
	addAttribute(aSize);
	
	astep = numAttr.create( "drawStep", "dsp", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(astep);
	
	aHDRName = tAttr.create("cachePath", "cp",
	MFnData::kString, MObject::kNullObj, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( tAttr.setStorable(true));
    CHECK_MSTATUS( tAttr.setKeyable(false));
	tAttr.setCached( true );
	
	zWorks::createMatrixAttr(aworldSpace, "worldSpace", "ws");
	
	aoutput = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutput );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedArrayAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kMesh);
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	astartframe = numAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( astartframe );
	
	zWorks::createTimeAttr(acurrenttime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(addAttribute(acurrenttime), "ERROR adding time");
	
	asavemap = numAttr.create( "saveMap", "sm", MFnNumericData::kInt, 0);
	numAttr.setStorable(false);
	numAttr.setKeyable(true);
	addAttribute(asavemap);
	
	aalternativepatch = numAttr.create( "patchOrder", "pod", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(aalternativepatch);
	
	CHECK_MSTATUS( addAttribute(aHDRName));
	addAttribute(aworldSpace);
	attributeAffects( alengthnoise, aoutput );
	attributeAffects( aSize, aoutput );
	attributeAffects( aworldSpace, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	attributeAffects( astartframe, aoutput );
	attributeAffects( acurrenttime, aoutput );
	attributeAffects( asavemap, aoutput );
	attributeAffects( afuzz, aoutput );
	attributeAffects( astep, aoutput );
	attributeAffects( aalternativepatch, aoutput );
	
	return MS::kSuccess;
}
//:~
