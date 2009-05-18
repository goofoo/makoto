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
#include "../shared/pdcFile.h"
#include "bacteriaNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"

MTypeId bacteriaNode::id( 0x0003018 );

MObject bacteriaNode::aOscillate;
MObject bacteriaNode::aFlapping;
MObject bacteriaNode::aBending;
MObject bacteriaNode::aLength;
MObject bacteriaNode::acachename;
MObject bacteriaNode::atime;
MObject bacteriaNode::aFrequency;
MObject bacteriaNode::aoutput;
MObject bacteriaNode::amatrix;
MObject bacteriaNode::anear;
MObject bacteriaNode::afar;
MObject bacteriaNode::ahapeture;
MObject bacteriaNode::avapeture;
MObject bacteriaNode::afocallength;

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
		MObject mat_obj = data.inputValue(amatrix, &status).data();
		MFnMatrixData mat_data( mat_obj );
		MMatrix cammat = mat_data.matrix();
		
		eyespace.setIdentity();
		eyespace.v[0][0] = cammat.matrix[0][0];
		eyespace.v[0][1] = cammat.matrix[0][1];
		eyespace.v[0][2] = cammat.matrix[0][2];
		eyespace.v[1][0] = cammat.matrix[1][0];
		eyespace.v[1][1] = cammat.matrix[1][1];
		eyespace.v[1][2] = cammat.matrix[1][2];
		eyespace.v[2][0] = cammat.matrix[2][0];
		eyespace.v[2][1] = cammat.matrix[2][1];
		eyespace.v[2][2] = cammat.matrix[2][2];
		eyespace.v[3][0] = cammat.matrix[3][0];
		eyespace.v[3][1] = cammat.matrix[3][1];
		eyespace.v[3][2] = cammat.matrix[3][2];
		eyespaceinv = eyespace;
		eyespaceinv.inverse();
		
		fsize = data.inputValue(aFrequency, &status).asDouble();
		frot = data.inputValue(aFlapping, &status).asDouble();
		
		nearClip = data.inputValue(anear, &status).asFloat();
		farClip = data.inputValue(afar, &status).asFloat();
		h_apeture = data.inputValue(ahapeture, &status).asFloat();
		v_apeture = data.inputValue(avapeture, &status).asFloat();
		fl = data.inputValue(afocallength, &status).asFloat();
		
		MTime currentTime = data.inputValue(atime, &status).asTime();
		MString cacheName = data.inputValue(acachename, &status).asString();
		cacheName = cacheName+"."+250*int(currentTime.value())+".pdc";
		
		pdcFile* fpdc = new pdcFile();
		if(fpdc->load(cacheName.asChar())==1)
		{
			fpdc->readPositionAndVelocity(ptc_positions, ptc_velocities);
		}
		else MGlobal::displayWarning(MString("Bacteria cannot open cache file: ")+cacheName);
		
		delete fpdc;
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
	
	MFnNumericAttribute nAttr; 
	aoutput = nAttr.create( "output", "output", MFnNumericData::kInt, 1 );
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	nAttr.setKeyable(false);
	addAttribute( aoutput );
	
	stat = zCreateKeyableDoubleAttr(aFrequency, MString("size"), MString("sz"), 1.0);
	zCheckStatus(stat, "failed to add size attr");
	stat = addAttribute( aFrequency);
	
	stat = zCreateKeyableDoubleAttr(aFlapping, MString("rotate"), MString("rot"), 0.0);
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
	
	ahapeture = nAttr.create( "horizontalFilmAperture", "hfa", MFnNumericData::kDouble );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( ahapeture );
	
	avapeture = nAttr.create( "verticalFilmAperture", "vfa", MFnNumericData::kDouble );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( avapeture );
	
	afocallength = nAttr.create( "focalLength", "fl", MFnNumericData::kDouble );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( afocallength );
	
	attributeAffects( amatrix, aoutput );
	attributeAffects( anear, aoutput );
	attributeAffects( afar, aoutput );
	attributeAffects( ahapeture, aoutput );
	attributeAffects( avapeture, aoutput );
	attributeAffects( afocallength, aoutput );
	attributeAffects( atime, aoutput );
	attributeAffects( aFrequency, aoutput );
	attributeAffects( aFlapping, aoutput );
	
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

	glBegin(GL_POINTS);
	//glColor3f(0,0,1);
	for(unsigned i=0; i<num_ptc; i++) glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
	glEnd();
	
	XYZ xvec(1,0,0), yvec, vert, zz(0,0,1), cen;
	
	xvec.rotateXY(frot);
	yvec = zz.cross(xvec);
	
	eyespace.transformAsNormal(xvec);
	eyespace.transformAsNormal(yvec);
	
	xvec *= fsize;
	yvec *= fsize;
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glBegin(GL_QUADS);
	for(unsigned i=0; i<num_ptc; i++)
	{
		cen =  XYZ(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
		vert = cen - xvec - yvec;
		glVertex3f(vert.x, vert.y, vert.z);
		vert =  cen + xvec - yvec;
		glVertex3f(vert.x, vert.y, vert.z);
		vert =  cen + xvec + yvec;
		glVertex3f(vert.x, vert.y, vert.z);
		vert = cen - xvec + yvec;
		glVertex3f(vert.x, vert.y, vert.z);
	}
	glEnd();
	glPopAttrib();
	view.endGL();
}
//:~