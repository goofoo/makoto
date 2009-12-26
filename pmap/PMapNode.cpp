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

//#define ZPUBLIC

MTypeId     PMapNode::id( 0x0004093 );
MObject     PMapNode::frame;
MObject     PMapNode::aradius;
MObject     PMapNode::adensity;
MObject     PMapNode::ashadowscale;
MObject     PMapNode::amaxframe;
MObject     PMapNode::aminframe;
MObject 	PMapNode::aviewattrib;
MObject     PMapNode::adrawtype;
MObject     PMapNode::input;
MObject     PMapNode::alightr;
MObject     PMapNode::alightg;
MObject     PMapNode::alightb;
MObject     PMapNode::ashadowr;
MObject     PMapNode::ashadowg;
MObject     PMapNode::ashadowb;
MObject     PMapNode::adownsample;
//MObject     PMapNode::adiffuse;
MObject     PMapNode::alightposx;
MObject     PMapNode::alightposy;
MObject     PMapNode::alightposz;
MObject     PMapNode::afreq;
MObject     PMapNode::aoutval;
//MObject     PMapNode::akkey;
//MObject     PMapNode::akback;
MObject     PMapNode::acloudr;
MObject     PMapNode::acloudg;
MObject     PMapNode::acloudb;
MObject     PMapNode::alacunarity;
MObject     PMapNode::adimension;
MObject     PMapNode::aoriginx;
MObject     PMapNode::aoriginy;
MObject     PMapNode::aoriginz;
MObject PMapNode::asaveimage;
MObject PMapNode::aresolutionx;
MObject PMapNode::aresolutiony;
MObject PMapNode::acameraname;

PMapNode::PMapNode() : f_type(0), fSaveImage(0), fSupported(0), fValid(0)
{
	fData = new FluidOctree();
	 renderer = new RenderParticle();
// set intial param
	 fParam.density = 1.f;
	 fParam.shadow_scale = 1.f;
	 fParam.radius = 1.f;
	 fParam.base_r = 0.1;
	 fParam.base_g = 0.1;
	 fParam.base_b = 0.1;
	 fParam.light_r = 1.0;
	 fParam.light_g = 1.0;
	 fParam.light_b = 1.0;
	 fParam.shadow_r = 0.0;
	 fParam.shadow_g = 0.0;
	 fParam.shadow_b = 0.0;
	 fParam.side_x = 1.0;
	 fParam.side_y = 0.0;
	 fParam.side_z = 0.0;
	 fParam.up_x = 0.0;
	 fParam.up_y = 1.0;
	 fParam.up_z = 0.0;
	 fParam.lacunarity = 2.0;
	 fParam.dimension = 2.0;
	 fParam.frequency = 1.0;
	 fParam.noise_x = -256.0;
	 fParam.noise_y = -256.0;
	 fParam.noise_z = -256.0;
}

PMapNode::~PMapNode() 
{
	delete fData;
	delete renderer;
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
	    iFrame = int(time + 0.005);
	    int minfrm = data.inputValue( aminframe ).asInt();
		f_type = data.inputValue( adrawtype ).asInt();
		fSaveImage = data.inputValue( asaveimage ).asInt();
		iOutWidth = data.inputValue( aresolutionx ).asInt();
		iOutHeight = data.inputValue( aresolutiony ).asInt();
		
		if( time < minfrm ) time = minfrm;
		int frame_lo = time + 0.005;
		char filename[512];
		sprintf( filename, "%s.%d", path.asChar(), frame_lo );
		
		fValid = fData->load(filename);
		if(!fValid) MGlobal::displayInfo("PMap cannot load file");
		
		MGlobal::displayInfo(MString(" num_leaf ")+ fData->getNumLeaf() + MString(" max_level ")+ fData->getMaxLevel());
		
		float x, y, z;
	
		x = data.inputValue(alightposx).asFloat();
		y = data.inputValue(alightposy).asFloat();
		z = data.inputValue(alightposz).asFloat();

		renderer->setLightPoint(x, y, z);

		fParam.density = data.inputValue(adensity).asFloat();
		fParam.shadow_scale = data.inputValue(ashadowscale).asFloat();
		fParam.radius = data.inputValue(aradius).asFloat();
		fParam.base_r = data.inputValue(acloudr).asFloat();
		fParam.base_g = data.inputValue(acloudg).asFloat();
		fParam.base_b = data.inputValue(acloudb).asFloat();
		fParam.light_r = data.inputValue(alightr).asFloat();
		fParam.light_g = data.inputValue(alightg).asFloat();
		fParam.light_b = data.inputValue(alightb).asFloat();
		fParam.shadow_r = data.inputValue(ashadowr).asFloat();
		fParam.shadow_g = data.inputValue(ashadowg).asFloat();
		fParam.shadow_b = data.inputValue(ashadowb).asFloat();
		fParam.lacunarity = data.inputValue(alacunarity).asFloat();
		fParam.dimension = data.inputValue(adimension).asFloat();
		fParam.frequency = data.inputValue(afreq).asFloat();
		fParam.noise_x = data.inputValue(aoriginx).asFloat();
		fParam.noise_y = data.inputValue(aoriginy).asFloat();
		fParam.noise_z = data.inputValue(aoriginz).asFloat();
		
		renderer->setDownSample(data.inputValue(adownsample).asInt());		
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
	
	renderer->setViewVector(mat.v[2][0], mat.v[2][1], mat.v[2][2]);
	renderer->setEyePoint(mat.v[3][0], mat.v[3][1], mat.v[3][2]);
	renderer->setUpVector(mat.v[1][0], mat.v[1][1], mat.v[1][2]);
	
// update camera side and up
	fParam.side_x = -mat.v[0][0];
	fParam.side_y = -mat.v[0][1];
	fParam.side_z = -mat.v[0][2];
	fParam.up_x = mat.v[1][0];
	fParam.up_y = mat.v[1][1];
	fParam.up_z = mat.v[1][2];

	view.beginGL(); 
	
	if(!renderer->isInitialized()) {
#ifndef __APPLE__
		gExtensionInit();
#endif
		//string log;
		//fSupported = fRenderer->diagnose(log);
		//MGlobal::displayInfo(MString("Voltex log: ") + log.c_str());	
		//if(fSupported) {

		//}
	}
	
	int port[4];
	glGetIntegerv(GL_VIEWPORT, port);
	
// set image fbo
	renderer->setImageDim(port[2], port[3]);
	
// calculate display widnow	
	double ratio = (double)port[3] / (double)port[2];
	double radians = 0.0174532925 * fov * 0.5; // half aperture degrees to radians 
	double wd2 = clipNear * tan(radians);
	
	GLdouble left, right, top, bottom;
	left  = -wd2;
	right = left + wd2*2;
	
	top = wd2 * ratio;
	bottom = top -  wd2*2*ratio;
	
	renderer->setFrustum(left, right, bottom, top, clipNear, clipFar);
	renderer->setFov(fov);

	float x,y,z,w;
	if(fValid) {
		
		fData->getBBox(x,y,z,w);
		renderer->setLightSize(w*1.8f);
		renderer->setLightVector(x,y,z);

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
	
	// restore view port	
	glViewport(0,0,port[2],port[3]);
	
	renderer->setPrimProjection();
			  
	glBegin(GL_LINES);
	glVertex3f(x,y,z);
	renderer->getLightPoint(x,y,z);
	glVertex3f(x,y,z);
	glEnd();
	
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
	
	aradius = nAttr.create( "billboardSize", "bsz", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.01);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( aradius );
	
	adensity = nAttr.create( "density", "den", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.01);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( adensity );
	
	ashadowscale = nAttr.create( "shadowScale", "sds", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.01);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(ashadowscale);
	
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
	
	alightr = nAttr.create( "lightColorR", "lclr", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alightr);
	
	alightg = nAttr.create( "lightColorG", "lclg", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alightg);
	
	alightb = nAttr.create( "lightColorB", "lclb", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alightb);
	
		ashadowr = nAttr.create( "shadowColorR", "sclr", MFnNumericData::kFloat, 0.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(ashadowr);
	
	ashadowg = nAttr.create( "shadowColorG", "sclg", MFnNumericData::kFloat, 0.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(ashadowg);
	
	ashadowb = nAttr.create( "shadowColorB", "sclb", MFnNumericData::kFloat, 0.0);
	nAttr.setMin(0.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(ashadowb);
	
	adownsample = nAttr.create( "downSample", "dsp", MFnNumericData::kInt, 1 );
	nAttr.setMin(0);
	nAttr.setMax(2);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( adownsample );
	
	MFnTypedAttribute   stringAttr;
	input = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( input );
	
	aviewattrib = stringAttr.create( "viewAttrib", "va", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( aviewattrib );
	/*
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
	addAttribute(adiffuse);*/
	
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
	
	alacunarity = nAttr.create( "Lacunarity", "lcu", MFnNumericData::kFloat, 1.25);
	nAttr.setMin(1.0);
	nAttr.setMax(4.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(alacunarity);
	
	adimension = nAttr.create( "Dimension", "dem", MFnNumericData::kFloat, 0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(3.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(adimension);
	
	afreq = nAttr.create( "frequency", "freq", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.01);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(afreq);
	
		aoriginx = nAttr.create( "noisePosX", "nox", MFnNumericData::kFloat, -256.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(aoriginx);
	
	aoriginy = nAttr.create( "noisePosY", "noy", MFnNumericData::kFloat, -256.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(aoriginy);
	
	aoriginz = nAttr.create( "noisePosZ", "noz", MFnNumericData::kFloat, -256.0);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute(aoriginz);
/*
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
*/	

	
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
	attributeAffects( aradius, aoutval );
	attributeAffects( adensity, aoutval );
	attributeAffects( ashadowscale, aoutval );
	attributeAffects(adownsample, aoutval );
	//attributeAffects( adiffuse, aoutval );
	attributeAffects( alightposx, aoutval );
	attributeAffects( alightposy, aoutval );
	attributeAffects( alightposz, aoutval );
	attributeAffects( afreq, aoutval );
	//attributeAffects( akkey, aoutval );
	//attributeAffects( akback, aoutval );
	attributeAffects( acloudr, aoutval );
	attributeAffects( acloudg, aoutval );
	attributeAffects( acloudb, aoutval );
	attributeAffects( alightr, aoutval );
	attributeAffects( alightg, aoutval );
	attributeAffects( alightb, aoutval );
	attributeAffects( ashadowr, aoutval );
	attributeAffects( ashadowg, aoutval );
	attributeAffects( ashadowb, aoutval );
	attributeAffects( alacunarity, aoutval );
	attributeAffects( adimension, aoutval );
	attributeAffects( asaveimage, aoutval );
	attributeAffects( aoriginx, aoutval );
	attributeAffects( aoriginy, aoutval );
	attributeAffects( aoriginz, aoutval );
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
	if(!renderer->isInitialized()) renderer->initialize();
	long n_ptc = fData->getNumLeaf();
	
	renderer->setNumParticle(n_ptc);
	
	float* pVertex = new float[n_ptc*4];
	float* pCoord = new float[n_ptc*4];
	
	fData->pushVertex(pVertex, pCoord);
	
	renderer->setVertex(pVertex);

	renderer->setCoord(pCoord);
	
	renderer->sort();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	renderer->updateParam(fParam);
	
	renderer->render();
	
	renderer->compose();
	//renderer->showShadow();
	
	
	if(fSaveImage==1) {
#ifndef ZPUBLIC
		MString sfile;
		MGlobal::executeCommand(MString ("string $p = `workspace -q -fn`"), sfile);
		if(iFrame < 10) sfile = sfile + "/images/" + name() + ".000" + iFrame + ".exr";
		else if(iFrame < 100) sfile = sfile + "/images/" + name() + ".00" + iFrame + ".exr";
		else if(iFrame < 1000) sfile = sfile + "/images/" + name() + ".0" + iFrame + ".exr";
		else sfile = sfile + "/images/" + name() + "." + iFrame + ".exr";
		
		MGlobal::displayInfo(MString(" render zSmoke to ") + sfile);
				
		renderer->saveToFile(sfile.asChar(), iOutWidth, iOutHeight);
#else
		MGlobal::displayInfo("Public Beta skips render-to-image feature.");
#endif
	}
	
	glPopAttrib();

	delete[] pVertex;
	delete[] pCoord;

}
//~: