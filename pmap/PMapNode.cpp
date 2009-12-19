//
// Copyright (C) YiLi
// 
// File: PMapNode.cpp
//
// Dependency Graph Node: PMapNode
//
// Author: Maya Plug-in Wizard 2.0
//

#include "PMapNode.h"
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MGlobal.h>

#include "../shared/zWorks.h"

#ifndef __APPLE__
#include "../shared/gExtension.h"
#endif

#define ZPUBLIC

MTypeId     PMapNode::id( 0x0004093 );
MObject     PMapNode::frame;
MObject     PMapNode::adensity;
MObject     PMapNode::amaxframe;
MObject     PMapNode::aminframe;
MObject 	PMapNode::aviewattrib;
MObject     PMapNode::adrawtype;
MObject     PMapNode::input;
MObject PMapNode::aincoe;
MObject PMapNode::anoise;
MObject     PMapNode::adiffuse;
MObject     PMapNode::alightposx;
MObject     PMapNode::alightposy;
MObject     PMapNode::alightposz;
MObject     PMapNode::ameanradius;
MObject     PMapNode::aoutval;
MObject     PMapNode::akkey;
MObject     PMapNode::akback;
MObject     PMapNode::acloudr;
MObject     PMapNode::acloudg;
MObject     PMapNode::acloudb;
MObject     PMapNode::alacunarity;
MObject     PMapNode::adimension;
MObject PMapNode::asaveimage;
MObject PMapNode::aresolutionx;
MObject PMapNode::aresolutiony;
MObject PMapNode::acameraname;

PMapNode::PMapNode() : f_type(0), fSaveImage(0), fSupported(0), fValid(0)
{
	fData = new FluidOctree();
}

PMapNode::~PMapNode() 
{
	delete fData;
}

MStatus PMapNode::compute( const MPlug& plug, MDataBlock& data )
//
//	Description:
//		This method computes the value of the given output plug based
//		on the values of the input attributes.
//
//	Arguments:
//		plug - the plug to compute
//		data - object that provides access to the attributes for this node
//
{
	if(plug == aoutval)
	{
		MStatus stat;
		MString path =  data.inputValue( input ).asString();
		fEyeCamera = data.inputValue( acameraname ).asString();

	    double time = data.inputValue( frame ).asTime().value();
	    int minfrm = data.inputValue( aminframe ).asInt();
		f_type = data.inputValue( adrawtype ).asInt();
		fSaveImage = data.inputValue( asaveimage ).asInt();
	    
		//int hascoe = 0;
		//MVectorArray vcoe = zWorks::getVectorArrayAttr(data, aincoe);
		//XYZ hdrCoeff[16];
		//if(vcoe.length() == 16) {
		//	hascoe = 1;
		//	for(unsigned i=0; i<16; i++) hdrCoeff[i] = XYZ(vcoe[i].x, vcoe[i].y, vcoe[i].z);
		//}
		
		if( time < minfrm ) time = minfrm;
		int frame_lo = time + 0.005;
		char filename[512];
		sprintf( filename, "%s.%d", path.asChar(), frame_lo );
		
		fValid = fData->load(filename);
		if(!fValid) MGlobal::displayInfo("PMap cannot load file");
		
		MGlobal::displayInfo(MString(" num_leaf ")+ fData->getNumLeaf() + MString(" max_level ")+ fData->getMaxLevel());
		
		float kwei = data.inputValue(adiffuse).asFloat();
		
		
		
		kwei = data.inputValue(adensity).asFloat();
		
		
		
		kwei = data.inputValue(ameanradius).asFloat();
		
		
		
		kwei = data.inputValue(akkey).asFloat();
		
		
		kwei = data.inputValue(akback).asFloat();
		
		
		kwei = data.inputValue(alacunarity).asFloat();
		
		
		kwei = data.inputValue(adimension).asFloat();
		
		
		float r, g, b;
		r = data.inputValue(alightposx).asFloat();
		g = data.inputValue(alightposy).asFloat();
		b = data.inputValue(alightposz).asFloat();
		
		
		
		r = data.inputValue(acloudr).asFloat();
		g = data.inputValue(acloudg).asFloat();
		b = data.inputValue(acloudb).asFloat();
		
		data.setClean(plug);
		
		return MS::kSuccess;
	}
	
	return MS::kUnknownParameter;
}

void* PMapNode::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new PMapNode();
}

bool PMapNode::isBounded() const
{ 
    return true;
}

MBoundingBox PMapNode::boundingBox() const
{
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );
	
	if(fValid) fData->getBBox(corner1.x, corner2.x, corner1.y, corner2.y, corner1.z, corner2.z);
	
	return MBoundingBox( corner1, corner2 ); 
}

void PMapNode::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	MDagPath camera;
	view = M3dView::active3dView();
	view.getCamera(camera);
	
	MATRIX44F mat;
	double clipNear, clipFar, fov;
	int ispersp;
	
	parseCamera(camera, mat, clipNear, clipFar, fov, ispersp);

	view.beginGL(); 
/*	
	if(!fRenderer->isDiagnosed()) {
#ifndef __APPLE__
		gExtensionInit();
#endif
		string log;
		fSupported = fRenderer->diagnose(log);
		MGlobal::displayInfo(MString("Voltex log: ") + log.c_str());	
		if(fSupported) {

		}
	}
*/	
	int port[4];
	glGetIntegerv(GL_VIEWPORT, port);

	if(fValid) {
		switch(f_type) {
		case 0: 
			fData->drawCube();
			break;
		case 1: 
			drawPoint();
			break;
		case 2: 
			fData->drawVelocity();
			break;
		case 3: 
			fData->drawBox();
			break;
		}
	}
	
	view.endGL();
}

MStatus PMapNode::initialize()
//
//	Description:
//		This method is called to create and initialize all of the attributes
//      and attribute dependencies for this node type.  This is only called 
//		once when the node type is registered with Maya.
//
//	Return Values:
//		MS::kSuccess
//		MS::kFailure
//		
{
	// This sample creates a single input float attribute and a single
	// output float attribute.
	//
    MStatus stat;
	MFnNumericAttribute nAttr;
	
	adrawtype = nAttr.create( "drawType", "dt", MFnNumericData::kInt, 0 );
	nAttr.setMin(-1);
	nAttr.setMax(3);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( adrawtype );

	MFnUnitAttribute uAttr;
	frame = uAttr.create("currentTime", "ct", MFnUnitAttribute::kTime, 1.0);
	uAttr.setKeyable(true);
	uAttr.setAffectsWorldSpace(true);
	uAttr.setStorable(true);
	addAttribute( frame );
	
	aminframe = nAttr.create( "minFrame", "mnf", MFnNumericData::kInt, 1 );
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( aminframe );
	
	amaxframe = nAttr.create( "maxFrame", "mxf", MFnNumericData::kInt, 24 );
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( amaxframe );
	
	adensity = nAttr.create( "density", "den", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.1);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( adensity );
	
	MFnTypedAttribute   stringAttr;
	input = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( input );
	
	aviewattrib = stringAttr.create( "viewAttrib", "va", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( aviewattrib );
	
	zWorks::createVectorArrayAttr(aincoe, "inCoeff", "icoe");
	addAttribute( aincoe );
	
	anoise = nAttr.create( "KNoise", "knoi", MFnNumericData::kFloat, 0.5);
	nAttr.setMin(0.0);
	//nAttr.setMax(1.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(anoise);
	
	adiffuse = nAttr.create( "KDiffuse", "kdif", MFnNumericData::kFloat, 0.2);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(adiffuse);
	
	alightposx = nAttr.create( "LightPosX", "lposx", MFnNumericData::kFloat, 100.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alightposx);
	
	alightposy = nAttr.create( "LightPosY", "lposy", MFnNumericData::kFloat, 100.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alightposy);
	
	alightposz = nAttr.create( "LightPosZ", "lposz", MFnNumericData::kFloat, 100.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alightposz);
	
	ameanradius = nAttr.create( "MeanRadius", "mnr", MFnNumericData::kFloat, 4.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(ameanradius);
	
	acloudr = nAttr.create( "CloudColorR", "cclr", MFnNumericData::kFloat, 0.1);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(acloudr);
	
	acloudg = nAttr.create( "CloudColorG", "cclg", MFnNumericData::kFloat, 0.1);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(acloudg);
	
	acloudb = nAttr.create( "CloudColorB", "cclb", MFnNumericData::kFloat, 0.1);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(acloudb);
	
	akkey = nAttr.create( "KKey", "kky", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(akkey);
	
	akback = nAttr.create( "KBack", "kbc", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(akback);
	
	alacunarity = nAttr.create( "Lacunarity", "lcu", MFnNumericData::kFloat, 1.25);
	nAttr.setMin(1.25);
	nAttr.setMax(4.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alacunarity);
	
	adimension = nAttr.create( "Dimension", "dem", MFnNumericData::kFloat, 0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(3.5);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(adimension);
	
	asaveimage = nAttr.create( "saveImage", "smg", MFnNumericData::kInt, 0); 
	nAttr.setStorable(true);
	addAttribute(asaveimage);
	
	aresolutionx = nAttr.create( "resolutionX", "rzx", MFnNumericData::kInt, 800); 
	nAttr.setStorable(true);
	addAttribute(aresolutionx);
	
	aresolutiony = nAttr.create( "resolutionY", "rzy", MFnNumericData::kInt, 600); 
	nAttr.setStorable(true);
	addAttribute(aresolutiony);
	
	acameraname = stringAttr.create( "cameraName", "cmn", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( acameraname );
	
	aoutval = nAttr.create( "outval", "ov", MFnNumericData::kFloat ); 
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	addAttribute( aoutval );
    
	attributeAffects( input, aoutval );
	attributeAffects( frame, aoutval );
	attributeAffects( aminframe, aoutval );
	attributeAffects( amaxframe, aoutval );
	attributeAffects( aviewattrib, aoutval );
	attributeAffects( adrawtype, aoutval );
	attributeAffects( aincoe, aoutval );
	attributeAffects( adensity, aoutval );
	attributeAffects( anoise, aoutval );
	attributeAffects( adiffuse, aoutval );
	attributeAffects( alightposx, aoutval );
	attributeAffects( alightposy, aoutval );
	attributeAffects( alightposz, aoutval );
	attributeAffects( ameanradius, aoutval );
	attributeAffects( akkey, aoutval );
	attributeAffects( akback, aoutval );
	attributeAffects( acloudr, aoutval );
	attributeAffects( acloudg, aoutval );
	attributeAffects( acloudb, aoutval );
	attributeAffects( alacunarity, aoutval );
	attributeAffects( adimension, aoutval );
	attributeAffects( asaveimage, aoutval );
	return MS::kSuccess;

}

void PMapNode::parseCamera(const MDagPath& camera, MATRIX44F& mat, double& clipnear, double& clipfar, double& fov, int& ispersp)
{
	MFnCamera fnCamera( camera );

	clipnear = fnCamera.nearClippingPlane();
	clipfar = fnCamera.farClippingPlane();
	
	MVector viewDir = fnCamera.viewDirection( MSpace::kWorld );
	MPoint eyePos = fnCamera.eyePoint ( MSpace::kWorld );
	MVector rightDir = fnCamera.rightDirection( MSpace::kWorld );
	MVector upDir = fnCamera.upDirection( MSpace::kWorld );
	
	mat.setIdentity ();
	mat.v[0][0] = -rightDir.x;
	mat.v[0][1] = -rightDir.y;
	mat.v[0][2] = -rightDir.z;
	mat.v[1][0] = upDir.x;
	mat.v[1][1] = upDir.y;
	mat.v[1][2] = upDir.z;
	mat.v[2][0] = viewDir.x;
	mat.v[2][1] = viewDir.y;
	mat.v[2][2] = viewDir.z;
	mat.v[3][0] = eyePos.x;
	mat.v[3][1] = eyePos.y;
	mat.v[3][2] = eyePos.z;
	
	fov = fnCamera.horizontalFieldOfView();
	fov = fov/PI*180;
	ispersp = 1;
	
	if(fnCamera.isOrtho()) {
		ispersp = 0;
		fov = fnCamera.orthoWidth();
	}
}

void PMapNode::drawPoint()
{
	long n_ptc = fData->getNumLeaf();
	float* pVertex = new float[n_ptc*3];
	fData->pushVertex(pVertex);
	
	unsigned int* idxbuffer = new unsigned int[n_ptc];
	
	for(unsigned int i=0; i<n_ptc; i++) idxbuffer[i]=i;
	
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	unsigned int ibosize = n_ptc * sizeof(unsigned int);
	 glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibosize, idxbuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
		//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (float*)pVertex);
		//glTexCoordPointer(4, GL_FLOAT, 0, (float*)pColor);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);
        glDrawElements(GL_POINTS, n_ptc, GL_UNSIGNED_INT, 0 );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		
		//glDrawArrays(GL_POINTS, 0, n_ptc);
		
		
		//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	delete[] pVertex;
	delete[] idxbuffer;
	
	glBindBuffer(1, ibo);
		glDeleteBuffers(1, &ibo);
}
//~: