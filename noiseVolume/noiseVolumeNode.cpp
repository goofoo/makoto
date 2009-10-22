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
#include "noiseVolumeNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include "../shared/FNoise.h"

MTypeId noiseVolumeNode::id( 0x0003018 );


MObject noiseVolumeNode::acachename;
MObject noiseVolumeNode::atime;
MObject noiseVolumeNode::aFrequency;
MObject noiseVolumeNode::aoutput;
MObject noiseVolumeNode::amatrix;
MObject noiseVolumeNode::anear;
MObject noiseVolumeNode::afar;
MObject noiseVolumeNode::ahapeture;
MObject noiseVolumeNode::avapeture;
MObject noiseVolumeNode::afocallength;
MObject noiseVolumeNode::amaxage;

noiseVolumeNode::noiseVolumeNode():m_num_fish(0), m_num_bone(20),m_offset(0)
{
}

noiseVolumeNode::~noiseVolumeNode() 
{
}

MStatus noiseVolumeNode::compute( const MPlug& plug, MDataBlock& data )
{    
    MStatus status;
	if(plug == aoutput)
	{
		double time = data.inputValue(atime).asTime().value();
		m_offset = time;
		data.setClean( plug );
	}
	
	return MS::kSuccess;
}

void* noiseVolumeNode::creator()
{
	return new noiseVolumeNode();
}

/////////////////////////////////////
// Attribute Setup and Maintenance //
/////////////////////////////////////

MStatus noiseVolumeNode::initialize()
{
	MStatus stat;
	
	MFnNumericAttribute nAttr; 
	aoutput = nAttr.create( "output", "output", MFnNumericData::kInt, 1 );
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	nAttr.setKeyable(false);
	addAttribute( aoutput );
	
	stat = zCreateKeyableDoubleAttr(aFrequency, MString("size"), MString("sz"), 1.0);
	zCheckStatus(stat, "failed to add size attr");
	stat = addAttribute( aFrequency);
	
	stat = zCreateTypedAttr(acachename, "cacheName", "cchn", MFnData::kString);
	zCheckStatus(stat, "ERROR creating cache name");
	zCheckStatus(addAttribute(acachename), "ERROR adding cache name");

	stat = zCreateTimeAttr(atime, "currentTime", MString("ct"), 1.0);
	zCheckStatus(stat, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	MFnTypedAttribute matAttr;
	amatrix = matAttr.create( "cameraMatrix", "cm", MFnData::kMatrix );
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	addAttribute( amatrix );
	
	anear = nAttr.create( "nearClip", "nc", MFnNumericData::kFloat, 0.1 );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( anear );
	
	afar = nAttr.create( "farClip", "fc", MFnNumericData::kFloat, 1000.0 );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( afar );
	
	ahapeture = nAttr.create( "horizontalFilmAperture", "hfa", MFnNumericData::kFloat );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( ahapeture );
	
	avapeture = nAttr.create( "verticalFilmAperture", "vfa", MFnNumericData::kFloat );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( avapeture );
	
	afocallength = nAttr.create( "focalLength", "fl", MFnNumericData::kFloat );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( afocallength );

	amaxage= nAttr.create( "maxage", "ma", MFnNumericData::kFloat, 10.0 );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( amaxage );
	
	attributeAffects( amatrix, aoutput );
	attributeAffects( anear, aoutput );
	attributeAffects( afar, aoutput );
	attributeAffects( ahapeture, aoutput );
	attributeAffects( avapeture, aoutput );
	attributeAffects( afocallength, aoutput );
	attributeAffects( atime, aoutput );
	attributeAffects( aFrequency, aoutput );
	attributeAffects( amaxage, aoutput );
	
	return MS::kSuccess;
}


MStatus noiseVolumeNode::connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc )
{
	return MS::kUnknownParameter;
}

bool noiseVolumeNode::isBounded() const
{ 
	return false;
}

void noiseVolumeNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glBegin(GL_LINES);
	FNoise fnoi;
	float noi;
	for(int i=0; i< 512; i++) {
		noi = fnoi.randfint(i+m_offset);
		glVertex3f(0.08*i,0,0);
		glVertex3f(0.08*i, noi*2,0);
	}
	
	glEnd();

	glPopAttrib();
	view.endGL();
}
//:~