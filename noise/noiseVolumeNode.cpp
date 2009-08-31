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
#include "noiseVolumeNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

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

noiseVolumeNode::noiseVolumeNode():m_num_fish(0), m_num_bone(20)
{
	f_bac = new FNoiseVolume();
}

noiseVolumeNode::~noiseVolumeNode() 
{
	delete f_bac;
}

MStatus noiseVolumeNode::compute( const MPlug& plug, MDataBlock& data )
{    
    MStatus status;
	if(plug == aoutput)
	{
		MObject mat_obj = data.inputValue(amatrix, &status).data();
		MFnMatrixData mat_data( mat_obj );
		MMatrix cammat = mat_data.matrix();

		MATRIX44F eyespace;
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
		
		f_bac->setSpace(eyespace);
		
		float fsize;
		fsize = data.inputValue(aFrequency, &status).asDouble();
		f_bac->setGlobal(fsize);
		
		float nearClip, farClip, h_apeture, v_apeture, fl;
		nearClip = data.inputValue(anear, &status).asFloat();
		farClip = data.inputValue(afar, &status).asFloat();
		h_apeture = data.inputValue(ahapeture, &status).asFloat();
		v_apeture = data.inputValue(avapeture, &status).asFloat();
		fl = data.inputValue(afocallength, &status).asFloat();
		
		f_bac->updateCamera(nearClip, farClip, h_apeture, v_apeture, fl);
		
		MTime currentTime = data.inputValue(atime, &status).asTime();
		MString cacheName = data.inputValue(acachename, &status).asString();
		cacheName = cacheName+"."+250*int(currentTime.value())+".pdc";
		
		MVectorArray ptc_positions, ptc_velocities;
		MDoubleArray ptc_ages;
		pdcFile* fpdc = new pdcFile();
		if(fpdc->load(cacheName.asChar())==1)
		{
			fpdc->readPositionAndAge(ptc_positions,ptc_velocities,ptc_ages);
		}
		else MGlobal::displayWarning(MString("NoiseVolume cannot open cache file: ")+cacheName);
		delete fpdc;
		
		unsigned num_ptc = ptc_positions.length();
		XYZ* pp = new XYZ[num_ptc];
		XYZ* pv = new XYZ[num_ptc];
		float* pa = new float[num_ptc];
		for(unsigned i=0; i<num_ptc; i++)
		{
			pp[i].x = ptc_positions[i].x;
			pp[i].y = ptc_positions[i].y;
			pp[i].z = ptc_positions[i].z;
			pv[i].x = ptc_velocities[i].x;
			pv[i].y = ptc_velocities[i].y;
			pv[i].z = ptc_velocities[i].z;
			pa[i] = ptc_ages[i];
		}
		
		f_bac->updateData(num_ptc, pp, pv, pa);
		
		delete[] pp;
		delete[] pv;
		delete[] pa;
		ptc_positions.clear();
		ptc_velocities.clear();
		ptc_ages.clear();
		//ptc_positions = zWorks::getVectorArrayAttr(data, apos);
		//MGlobal::displayInfo("get pos");
		//MDataHandle outputHandle = data.outputValue(aoutput, &status);
		//zCheckStatus(status, "ERROR getting polygon data handle\n");
		
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
	
	attributeAffects( amatrix, aoutput );
	attributeAffects( anear, aoutput );
	attributeAffects( afar, aoutput );
	attributeAffects( ahapeture, aoutput );
	attributeAffects( avapeture, aoutput );
	attributeAffects( afocallength, aoutput );
	attributeAffects( atime, aoutput );
	attributeAffects( aFrequency, aoutput );
	
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
	unsigned num_ptc = f_bac->getNumNoiseVolume();
	XYZ cen;
	float size;
/*
	glBegin(GL_POINTS);
	
	for(unsigned i=0; i<num_ptc; i++) 
	{
		cen = f_bac->getPositionById(i);
		glVertex3f(cen.x, cen.y, cen.z);
	}
	glEnd();
*/
	MDagPath camera;
	view = M3dView::active3dView();
	view.getCamera(camera);
	MFnCamera fnCamera( camera );
	MVector viewDir = fnCamera.viewDirection( MSpace::kWorld );
	XYZ facing;
	facing.x = viewDir[0];facing.y = viewDir[1];facing.z = viewDir[2];

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glBegin(GL_POLYGON);
	XYZ xvec, yvec, vert;
	size = f_bac->getGlobalSize();
	for(unsigned i=0; i<num_ptc; i++)
	{
		if(f_bac->isInViewFrustum(i))
		{
			cen =  f_bac->getPositionById(i);
			double age= f_bac->getAgeById(i);
			while(age>1)
			{
				age = age-1;
			}
			glColor3f(age,age,age);
			gBase::drawLineCircleAt(cen,facing,size);
		}
	}
	glEnd();
	
	glPopAttrib();
	view.endGL();
}
//:~