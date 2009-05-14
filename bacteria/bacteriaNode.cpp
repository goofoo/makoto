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

#include "bacteriaNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"

MTypeId bacteriaNode::id( 0x0003718 );

MObject bacteriaNode::aOscillate;
MObject bacteriaNode::aFlapping;
MObject bacteriaNode::aBending;
MObject bacteriaNode::aLength;
MObject bacteriaNode::acachename;
MObject bacteriaNode::atime;
MObject bacteriaNode::aFrequency;
//MObject bacteriaNode::apos;
MObject bacteriaNode::aoutput;

bacteriaNode::bacteriaNode():m_num_fish(0), m_num_bone(20)
{
}

bacteriaNode::~bacteriaNode() 
{
}

MStatus bacteriaNode::compute( const MPlug& plug, MDataBlock& data )
{    
    MStatus status;
	if(plug == aoutput)
	{
		//MDoubleArray ptc_time_offset;
		//MDoubleArray ptc_amplitude;
		//MDoubleArray ptc_bend;
		//MDoubleArray ptc_scale;
		//MDoubleArray masses;
		//
		//MString cacheName = data.inputValue(acachename, &status).asString();
		//MTime currentTime = data.inputValue(atime, &status).asTime();
		//cacheName = cacheName+"."+250*int(currentTime.value())+".pdc";
		//
		//double flapping = zGetDoubleAttr(data, aFlapping);
		//double bending= zGetDoubleAttr(data, aBending);
		//double oscillate= zGetDoubleAttr(data, aOscillate);
		//double length = zGetDoubleAttr(data, aLength);
		//m_fish_length = length;
		//double frequency = zGetDoubleAttr(data, aFrequency);
		//
		//unsigned int nptc = ptc_positions.length();
		//MPointArray vertices;
		MTime currentTime = data.inputValue(atime, &status).asTime();
		int frame = (int)currentTime.value();
		
		//ptc_positions = zWorks::getVectorArrayAttr(data, apos);
		//MGlobal::displayInfo("get pos");
		//MDataHandle outputHandle = data.outputValue(aoutput, &status);
		//zCheckStatus(status, "ERROR getting polygon data handle\n");
		
		data.setClean( plug );
	}
	
	return MS::kSuccess;
}

void* bacteriaNode::creator()
{
	return new bacteriaNode();
}

/////////////////////////////////////
// Attribute Setup and Maintenance //
/////////////////////////////////////





MStatus bacteriaNode::initialize()
{
	MStatus stat;
	
	//zWorks::createVectorArrayAttr(apos, "inPosition","inpos");
	//addAttribute( apos );
	
	MFnNumericAttribute nAttr; 
	aoutput = nAttr.create( "output", "output", MFnNumericData::kInt, 1 );
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	nAttr.setKeyable(false);
	addAttribute( aoutput );
	
	stat = zCreateKeyableDoubleAttr(aFrequency, MString("frequency"), MString("feq"), 1.0);
	zCheckStatus(stat, "failed to add frequency attr");
	stat = addAttribute( aFrequency);
	
	stat = zCreateKeyableDoubleAttr(aFlapping, MString("flapping"), MString("fpn"), 1.0);
	zCheckStatus(stat, "failed to add flapping attr");
	stat = addAttribute(aFlapping);
	
	stat = zCreateKeyableDoubleAttr(aBending, MString("bending"), MString("bdn"), 1.0);
	zCheckStatus(stat, "failed to add bending attr");
	stat = addAttribute(aBending);
	
	stat = zCreateKeyableDoubleAttr(aOscillate, MString("oscillate"), MString("osc"), 1.0);
	zCheckStatus(stat, "failed to add oscillate attr");
	stat = addAttribute(aOscillate);
	
	stat = zCreateKeyableDoubleAttr(aLength, MString("length"), MString("l"), 10.0);
	zCheckStatus(stat, "failed to add length attr");
	stat = addAttribute(aLength);
	
	stat = zCreateTypedAttr(acachename, "cacheName", "cchn", MFnData::kString);
	zCheckStatus(stat, "ERROR creating cache name");
	zCheckStatus(addAttribute(acachename), "ERROR adding cache name");

	stat = zCreateTimeAttr(atime, "currentTime", MString("ct"), 1.0);
	zCheckStatus(stat, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	//stat = attributeAffects( aFrequency, outMesh );
	//stat = attributeAffects( aFlapping, outMesh );
	//stat = attributeAffects( aBending, outMesh );
	//stat = attributeAffects( aOscillate, outMesh );
	//stat = attributeAffects( aLength, outMesh );
	attributeAffects( atime, aoutput );
	//attributeAffects( apos, aoutput );
	
	return MS::kSuccess;
}


MStatus bacteriaNode::connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc )
{
	
	return MS::kUnknownParameter;
}

bool bacteriaNode::isBounded() const
{ 
	return false;
}

void bacteriaNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	unsigned num_ptc = ptc_positions.length();

	glBegin(GL_LINES);
	glColor3f(0,0,1);
	for(unsigned i=0; i<num_ptc; i++)
	{
		glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
		glVertex3f(ptc_positions[i].x+0, ptc_positions[i].y+1, ptc_positions[i].z+0);
	}
	//glColor3f(0,1,0);
	//for(unsigned i=0; i<num_ptc; i++)
	//{
	//	glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
	//	glVertex3f(ptc_positions[i].x+ptc_ups[i].x*m_fish_length, ptc_positions[i].y+ptc_ups[i].y*m_fish_length, ptc_positions[i].z+ptc_ups[i].z*m_fish_length);
	//}
	//glColor3f(1,0,0);
	//for(unsigned i=0; i<num_ptc; i++)
	//{
	//	glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
	//	glVertex3f(ptc_positions[i].x-ptc_views[i].x*m_fish_length, ptc_positions[i].y-ptc_views[i].y*m_fish_length, ptc_positions[i].z-ptc_views[i].z*m_fish_length);
	//}
	glEnd();
	glPopAttrib();
	view.endGL();
}
//:~