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

MTypeId     PTCMapLocator::id( 0x0004091 );
MObject     PTCMapLocator::frame;
MObject     PTCMapLocator::aresolution;
MObject     PTCMapLocator::amaxframe;
MObject     PTCMapLocator::aminframe;
//MObject     PTCMapLocator::aposition;
MObject     PTCMapLocator::adrawtype;
MObject     PTCMapLocator::input;
MObject     PTCMapLocator::aoutval;

PTCMapLocator::PTCMapLocator() :tree(0),f_type(0)
{
}

PTCMapLocator::~PTCMapLocator() 
{
	if(tree) delete tree;
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
	    double time = data.inputValue( frame ).asTime().value();
	    int minfrm = data.inputValue( aminframe ).asInt();
		f_type = data.inputValue( adrawtype ).asInt();
	    //f_rez = 2.f/data.inputValue( aresolution ).asFloat();
		
		//XYZ pos = data.inputValue(aposition).asFloat3();
		
	    if( time < minfrm ) time = minfrm;
	    int frame_lo = time;
	    char filename[512];
		sprintf( filename, "%s.%d.pmap", path.asChar(), frame_lo );
		
		if(tree) delete tree;
	    tree = new Z3DTexture();
		if(!tree->load(filename)) MGlobal::displayInfo("PTCMap cannot load file");
		//if(!tree->hasAttrib("volume_occlusion")) MGlobal::displayInfo("has no volume occlusion");
		tree->setDraw("volume_occlusion");
		//MGlobal::displayInfo(tree->fisrtattrib());
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
	
	if(tree) tree->getBBox(corner1.x, corner2.x, corner1.y, corner2.y, corner1.z, corner2.z);
	
	return MBoundingBox( corner1, corner2 ); 
} 

void PTCMapLocator::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	MDagPath camera;
	view = M3dView::active3dView();
	view.getCamera(camera);
	MFnCamera fnCamera( camera );

	MVector viewDir = fnCamera.viewDirection( MSpace::kWorld );
	MPoint eyePos = fnCamera.eyePoint ( MSpace::kWorld );
	XYZ facing;
	facing.x = viewDir[0];facing.y = viewDir[1];facing.z = viewDir[2];
	
	double clipNear, clipFar;
	clipNear = fnCamera.nearClippingPlane();
	clipFar = fnCamera.farClippingPlane();

	double h_apeture, v_apeture;
	h_apeture = fnCamera.horizontalFilmAperture();
	v_apeture = fnCamera.verticalFilmAperture();
	MVector rightDir = fnCamera.rightDirection( MSpace::kWorld );
	MVector upDir = fnCamera.upDirection( MSpace::kWorld );
	double fl;
	fl = fnCamera.focalLength();
	//double h_fov = h_apeture * 0.5*25.4 /fl;
	//double v_fov = v_apeture * 0.5*25.4 /fl;

	MATRIX44F mat;
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
	mat.inverse();

	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if(tree){
		//glClearDepth(1.0);
	    //glEnable(GL_BLEND);
	    //glDepthFunc(GL_LEQUAL);
	    //glShadeModel(GL_SMOOTH);
	    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//
		//glPointSize(2.5);
		//glShadeModel(GL_SMOOTH);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(f_type == 1) tree->draw();
		else tree->draw(facing);		
		//
		//XYZ ori(0,0,0);
		//tree->testRaster(ori);
	}
	glPopAttrib();
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
/*
	aposition = nAttr.create("aposition","apo",MFnNumericData::k3Float);
	nAttr.setDefault( 0 );
	nAttr.setKeyable( true ); 
    nAttr.setReadable( true ); 
    nAttr.setWritable( true ); 
    nAttr.setStorable( true ); 
	stat = addAttribute(aposition);*/

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
	
	aresolution = nAttr.create( "resolution", "rez", MFnNumericData::kFloat, 512);
	nAttr.setMin(32);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( aresolution );
	
	MFnTypedAttribute   stringAttr;
	input = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( input );
	
	adrawtype = nAttr.create( "drawType", "dt", MFnNumericData::kInt, 0 );
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( adrawtype );
	
	//MFnTypedAttribute   meshAttr;
	aoutval = nAttr.create( "outval", "ov", MFnNumericData::kFloat ); 
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	addAttribute( aoutval );
    
	attributeAffects( input, aoutval );
	attributeAffects( frame, aoutval );
	attributeAffects( aminframe, aoutval );
	attributeAffects( amaxframe, aoutval );
	attributeAffects( aresolution, aoutval );
	attributeAffects( adrawtype, aoutval );
	return MS::kSuccess;

}

