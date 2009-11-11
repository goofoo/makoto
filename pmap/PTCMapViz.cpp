//
// Copyright (C) YiLi
// 
// File: PTCMapLocator.cpp
//
// Dependency Graph Node: PTCMapLocator
//
// Author: Maya Plug-in Wizard 2.0
//

#include "PTCMapViz.h"
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

#include <ImfTiledOutputFile.h>
#include <ImfTiledInputFile.h>
#include <ImfChannelList.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace Imf;
using namespace Imath;

#ifndef __APPLE__
#include "../shared/gExtension.h"
#endif

MTypeId     PTCMapLocator::id( 0x0004091 );
MObject     PTCMapLocator::frame;
MObject     PTCMapLocator::adensity;
MObject     PTCMapLocator::amaxframe;
MObject     PTCMapLocator::aminframe;
MObject 	PTCMapLocator::aviewattrib;
MObject     PTCMapLocator::adrawtype;
MObject     PTCMapLocator::input;
MObject PTCMapLocator::aincoe;
MObject PTCMapLocator::anoise;
MObject     PTCMapLocator::adiffuse;
MObject     PTCMapLocator::alightposx;
MObject     PTCMapLocator::alightposy;
MObject     PTCMapLocator::alightposz;
MObject     PTCMapLocator::ameanradius;
MObject     PTCMapLocator::aoutval;
MObject     PTCMapLocator::akkey;
MObject     PTCMapLocator::akback;
MObject     PTCMapLocator::acloudr;
MObject     PTCMapLocator::acloudg;
MObject     PTCMapLocator::acloudb;
MObject     PTCMapLocator::alacunarity;
MObject     PTCMapLocator::adimension;
MObject PTCMapLocator::asaveimage;
MObject PTCMapLocator::aresolutionx;
MObject PTCMapLocator::aresolutiony;
MObject PTCMapLocator::acameraname;

#define TILEWIDTH 1024
#define TILEHEIGHT 1024

Array2D<half> rPixels(TILEHEIGHT, TILEWIDTH);
Array2D<half> gPixels(TILEHEIGHT, TILEWIDTH);
Array2D<half> bPixels(TILEHEIGHT, TILEWIDTH);
Array2D<half> aPixels(TILEHEIGHT, TILEWIDTH);

GLuint fbo;
GLuint depthBuffer;
GLuint img;
GLuint texname;

PTCMapLocator::PTCMapLocator() : fRenderer(0), fData(0), f_type(0), fSaveImage(0),
fImageWidth(800), fImageHeight(600)
{
	fRenderer = new Voltex();
}

PTCMapLocator::~PTCMapLocator() 
{
	if(fRenderer) delete fRenderer;
	if(fData) delete fData;
	
	if(fbo) glDeleteFramebuffersEXT(1, &fbo);
	if(depthBuffer) glDeleteRenderbuffersEXT(1, &fbo);
	glDeleteTextures(1, &img);
}

MStatus PTCMapLocator::compute( const MPlug& plug, MDataBlock& data )
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
	    
		int hascoe = 0;
		MVectorArray vcoe = zWorks::getVectorArrayAttr(data, aincoe);
		XYZ hdrCoeff[16];
		if(vcoe.length() == 16) {
			hascoe = 1;
			for(unsigned i=0; i<16; i++) hdrCoeff[i] = XYZ(vcoe[i].x, vcoe[i].y, vcoe[i].z);
		}
		
		if( time < minfrm ) time = minfrm;
		int frame_lo = time + 0.005;
		char filename[512];
		sprintf( filename, "%s.%d.pmap", path.asChar(), frame_lo );
		sprintf( exrname, "%s.%d.exr", path.asChar(), frame_lo );
		
		if(fData) delete fData;
		fData = new Z3DTexture();
		if(!fData->load(filename)) MGlobal::displayInfo("PTCMap cannot load file");

		fData->setDraw();
		
		if(hascoe==1) fData->setHDRLighting(hdrCoeff);
		
		float kwei = data.inputValue(anoise).asFloat();
		
		fRenderer->setKNoise(kwei);
		
		kwei = data.inputValue(adiffuse).asFloat();
		
		fRenderer->setKDiffuse(kwei);
		
		kwei = data.inputValue(adensity).asFloat();
		
		fRenderer->setDensity(kwei);
		
		kwei = data.inputValue(ameanradius).asFloat();
		
		if(fData) fData->setMeanRadius(kwei);
		
		kwei = data.inputValue(akkey).asFloat();
		if(fData) fData->setKeyScale(kwei);
		
		kwei = data.inputValue(akback).asFloat();
		if(fData) fData->setBackScale(kwei);
		
		kwei = data.inputValue(alacunarity).asFloat();
		fRenderer->setLacunarity(kwei);
		
		kwei = data.inputValue(adimension).asFloat();
		fRenderer->setDimension(kwei);
		
		float r, g, b;
		r = data.inputValue(alightposx).asFloat();
		g = data.inputValue(alightposy).asFloat();
		b = data.inputValue(alightposz).asFloat();
		
		fRenderer->setLightPos(r,g,b);
		
		r = data.inputValue(acloudr).asFloat();
		g = data.inputValue(acloudg).asFloat();
		b = data.inputValue(acloudb).asFloat();
		
		fRenderer->setCloudColor(r,g,b);
		
		fImageWidth = data.inputValue(aresolutionx).asInt();
		fImageHeight = data.inputValue(aresolutiony).asInt();
		
		data.setClean(plug);
		
		return MS::kSuccess;
	}
	
	return MS::kUnknownParameter;
}

void* PTCMapLocator::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new PTCMapLocator();
}

bool PTCMapLocator::isBounded() const
{ 
    return true;
}

MBoundingBox PTCMapLocator::boundingBox() const
{
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );
	
	if(fData) fData->getBBox(corner1.x, corner2.x, corner1.y, corner2.y, corner1.z, corner2.z);
	
	return MBoundingBox( corner1, corner2 ); 
}

void PTCMapLocator::draw( M3dView & view, const MDagPath & path, 
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
	
	if(!fRenderer->isDiagnosed()) {
#ifndef __APPLE__
		gExtensionInit();
#endif
		string log;
		fRenderer->diagnose(log);
		MGlobal::displayInfo(MString("Voltex log: ") + log.c_str());

	
// setup fbo
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

// Create the render buffer for depth	
		glGenRenderbuffersEXT(1, &depthBuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TILEWIDTH, TILEHEIGHT);

// Now setup the first texture to render to
		glGenTextures(1, &img);
		glBindTexture(GL_TEXTURE_2D, img);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB,  TILEWIDTH, TILEHEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

// And attach it to the FBO so we can render to it
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);
		
// Attach the depth render buffer to the FBO as it's depth attachment
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);


		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("Cannot create frame buffer object.");
// Unbind the FBO for now
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	
	int port[4];
	glGetIntegerv(GL_VIEWPORT, port);

	if(fData) {
// update camera
		fData->setProjection(mat, fov, ispersp);
		fData->setPortWidth(port[2]);
		
		if(f_type == 1) fData->drawCube();
		else {
			//glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glShadeModel(GL_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask( GL_FALSE );
			glEnable(GL_BLEND);
			fRenderer->start(fData);
			fData->drawSprite();
			fRenderer->end();
		
			glDisable(GL_BLEND);	
			glDepthMask( GL_TRUE );	
			//glPopAttrib();
		}
		
		if(fSaveImage==1) {
			MGlobal::displayInfo(MString(" render particle cache to ") + exrname);
				
			writeNebula(exrname);

		}
	}
	
	view.endGL();
}

MStatus PTCMapLocator::initialize()
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
	
	adrawtype = nAttr.create( "drawType", "dt", MFnNumericData::kInt, 0 );
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( adrawtype );
	
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

void PTCMapLocator::writeNebula(const char filename[])
{
	MDagPath pcamera;
	zWorks::getTypedPathByName(MFn::kCamera, fEyeCamera, pcamera);
	
	MATRIX44F mat;
	double clipNear, clipFar, fov;
	int ispersp;
	
	parseCamera(pcamera, mat, clipNear, clipFar, fov, ispersp);
	
	fData->setProjection(mat, fov, ispersp);
	fData->setPortWidth(fImageWidth);

// calculate display widnow	
	double ratio = (double)fImageHeight / (double)fImageWidth;
	double radians = 0.0174532925 * fov * 0.5; // half aperture degrees to radians 
	double wd2 = clipNear * tan(radians);
	double displayWidth = wd2 + wd2;
	double tile_inc_h = displayWidth * (double)TILEWIDTH / (double)fImageWidth;
	double tile_inc_v = displayWidth * ratio * (double)TILEWIDTH / (double)fImageHeight;
	
	Header header(fImageWidth, fImageHeight);
    header.channels().insert ("R", Channel (HALF));
    header.channels().insert ("G", Channel (HALF));
    header.channels().insert ("B", Channel (HALF));
	header.channels().insert ("A", Channel (HALF));
    header.setTileDescription( TileDescription (TILEWIDTH, TILEHEIGHT, ONE_LEVEL));
	
	TiledOutputFile file (filename, header);
	for(int Y=0; Y < file.numYTiles(); ++Y) {
		for(int X=0; X < file.numXTiles(); ++X) {
    	     	     
			Box2i range = file.dataWindowForTile (X, Y);

// First we bind the FBO so we can render to it
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

// Save the view port and set it to the size of the texture
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glViewport(0,0,TILEWIDTH,TILEHEIGHT);

// Then render as normal
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClearDepth(1.0f);		  
	glClearColor(.0f, .0f, .0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	

// calculate view for this tile
	GLdouble left, right, top, bottom;

	left  = -wd2 + tile_inc_h * X;
	right = left + tile_inc_h;
	
	top = wd2 * ratio - tile_inc_v * Y;
	bottom = top - tile_inc_v;	
	

	glFrustum (left, right, bottom, top, clipNear, clipFar);
	
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	
	gluLookAt (mat.v[3][0], mat.v[3][1], mat.v[3][2],
			   mat.v[3][0] + mat.v[2][0], mat.v[3][1] + mat.v[2][1], mat.v[3][2] + mat.v[2][2],
			  mat.v[1][0], mat.v[1][1], mat.v[1][2]);
	
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
glBlendFuncSeparate(GL_SRC_ALPHA, 
 	GL_ONE_MINUS_SRC_ALPHA, 
 	GL_ONE, 
 	GL_ONE_MINUS_SRC_ALPHA);
glDepthMask( GL_FALSE );
glDepthFunc(GL_LEQUAL);
glEnable(GL_DEPTH_TEST);
	
	fRenderer->start(fData);
	fData->drawSprite();
	fRenderer->end();
	
glDisable(GL_BLEND);
glDepthMask( GL_TRUE );		
	
	glPopAttrib();
// dump	
	float *data = new float[TILEWIDTH*TILEHEIGHT*4];
	glReadPixels( 0, 0, TILEWIDTH, TILEHEIGHT, GL_RGBA, GL_FLOAT, data );
	
	for(int j=0; j<TILEHEIGHT; j++) {
		int flip_j = TILEHEIGHT-1-j;
		for(int i=0; i<TILEWIDTH; i++) {
				rPixels[j][i] = data[(flip_j*TILEWIDTH+i)*4];
				gPixels[j][i] = data[(flip_j*TILEWIDTH+i)*4+1];
				bPixels[j][i] = data[(flip_j*TILEWIDTH+i)*4+2];
				aPixels[j][i] = data[(flip_j*TILEWIDTH+i)*4+3];
		}
	}
	delete[] data;
	
// unbind fbo
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
			FrameBuffer frameBuffer;
    	     	     
			frameBuffer.insert("R", 
			Slice(HALF,
			(char*) &rPixels[-range.min.y][-range.min.x],
			sizeof(rPixels[0][0])*1,	
			sizeof(rPixels[0][0])*TILEWIDTH));

			frameBuffer.insert("G", 
			Slice(HALF,
			(char*) &gPixels[-range.min.y][-range.min.x],
			sizeof(gPixels[0][0])*1,	
			sizeof(gPixels[0][0])*TILEWIDTH));

			frameBuffer.insert("B", 
			Slice(HALF,
			(char*) &bPixels[-range.min.y][-range.min.x],
			sizeof(bPixels[0][0])*1,	
			sizeof(bPixels[0][0])*TILEWIDTH));

			frameBuffer.insert("A", 
			Slice(HALF,
			(char*) &aPixels[-range.min.y][-range.min.x],
			sizeof(aPixels[0][0])*1,	
			sizeof(aPixels[0][0])*TILEWIDTH));


			file.setFrameBuffer (frameBuffer);
			file.writeTile (X, Y);
		}
    }
}

void PTCMapLocator::parseCamera(const MDagPath& camera, MATRIX44F& mat, double& clipnear, double& clipfar, double& fov, int& ispersp)
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
//~: