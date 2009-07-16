//
// Copyright (C) YiLi
// 
// File: pMapLocator.cpp
//
// Dependency Graph Node: pMapLocator
//
// Author: Maya Plug-in Wizard 2.0
//

#include "pMapLocator.h"
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


MTypeId     pMapLocator::id( 0x00001 );
MObject     pMapLocator::alevel;
MObject     pMapLocator::frame;
MObject     pMapLocator::aframestep;
MObject     pMapLocator::amaxframe;
MObject     pMapLocator::aminframe;
MObject     pMapLocator::aposition;
MObject     pMapLocator::input;
MObject     pMapLocator::aoutval;

pMapLocator::pMapLocator() :tree(0)
{
}

pMapLocator::~pMapLocator() 
{
	if(tree) delete tree;
}

MStatus pMapLocator::compute( const MPlug& plug, MDataBlock& data )
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
	    int frmstep = data.inputValue( aframestep ).asInt();
		XYZ pos = data.inputValue(aposition).asFloat3();
		//int maxlevel =  data.inputValue( alevel ).asInt();
	    if( time < minfrm ) time = minfrm;
	    int frame_lo = minfrm + int(time-minfrm)/frmstep*frmstep;
	    sprintf( filename, "%s.%d.dat", path.asChar(), frame_lo );
		if(tree) delete tree;
	    tree = new OcTree();
		tree->loadFile(filename,tree);
		index = 0; 
		//tree->searchNearVoxel(tree,pos,index);
	}
	
	return MS::kUnknownParameter;
}

void* pMapLocator::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new pMapLocator();
}

bool pMapLocator::isBounded() const
{ 
    return true;
}

MBoundingBox pMapLocator::boundingBox() const
{
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );
	
	if(tree) {
		XYZ center; float size;
		tree->getRootCenterNSize(center, size);
		corner1.x = center.x -size;
		corner1.y = center.y -size;
		corner1.z = center.z -size;
		corner2.x = center.x +size;
		corner2.y = center.y +size;
		corner2.z = center.z +size;
	}
	return MBoundingBox( corner1, corner2 ); 
} 

void pMapLocator::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	MDagPath camera;
	view = M3dView::active3dView();
	view.getCamera(camera);
	MFnCamera fnCamera( camera );

	MVector viewDir = fnCamera.viewDirection( MSpace::kWorld );
	MPoint eyePos = fnCamera.eyePoint ( MSpace::kWorld );
	XYZ viewDirection,eyePoint;
	viewDirection.x = viewDir[0];viewDirection.y = viewDir[1];viewDirection.z = viewDir[2];
	eyePoint.x = eyePos.x;eyePoint.y = eyePos.y;eyePoint.z = eyePos.z;
	
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
	double h_fov = h_apeture * 0.5*25.4 /fl;
	double v_fov = v_apeture * 0.5*25.4 /fl;

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

	cameraParameter cameraPa;
	cameraPa.clipFar = clipFar;
	cameraPa.clipNear = clipNear;
	cameraPa.eyePoint = eyePoint;
	cameraPa.focalLength = fl;
	cameraPa.horizontalFieldOfView= h_fov;
	cameraPa.verticalFieldOfView = v_fov;
	cameraPa.viewDirection = viewDirection;
	cameraPa.mat = mat;


	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	
	new view 
	
	view->set();
	/*
	glPointSize(3);
	if(num_raw_data > 0 && raw_data) {
		glBegin(GL_POINTS);
		
		for(unsigned i=0; i<num_raw_data; i++) {
			
			glVertex3f(raw_data[i].x, raw_data[i].y, raw_data[i].z);
		}
		glEnd();
	 	*/

	if(tree){
		glBegin(GL_QUADS);
		//glColor3f(1.0,0.0,0.0);
		if(tree) tree->draw(view);
		glEnd();
		/*
		glColor3f(1.0,0.0,0.0);
		glBegin(GL_LINES);
		    glVertex3f(cen.x - size, cen.y - size, cen.z - size);
			glVertex3f(cen.x + size, cen.y - size, cen.z - size);
			glVertex3f(cen.x - size, cen.y + size, cen.z - size);
			glVertex3f(cen.x + size, cen.y + size, cen.z - size);
			glVertex3f(cen.x - size, cen.y - size, cen.z + size);
			glVertex3f(cen.x + size, cen.y - size, cen.z + size);
			glVertex3f(cen.x - size, cen.y + size, cen.z + size);
			glVertex3f(cen.x + size, cen.y + size, cen.z + size);
			
			glVertex3f(cen.x - size, cen.y - size, cen.z - size);
			glVertex3f(cen.x - size, cen.y - size, cen.z + size);
			glVertex3f(cen.x - size, cen.y + size, cen.z - size);
			glVertex3f(cen.x - size, cen.y + size, cen.z + size);
			glVertex3f(cen.x + size, cen.y - size, cen.z - size);
			glVertex3f(cen.x + size, cen.y - size, cen.z + size);
			glVertex3f(cen.x + size, cen.y + size, cen.z - size);
			glVertex3f(cen.x + size, cen.y + size, cen.z + size);
			
			glVertex3f(cen.x - size, cen.y - size, cen.z - size);
			glVertex3f(cen.x - size, cen.y + size, cen.z - size);
			glVertex3f(cen.x - size, cen.y - size, cen.z + size);
			glVertex3f(cen.x - size, cen.y + size, cen.z + size);
			glVertex3f(cen.x + size, cen.y - size, cen.z - size);
			glVertex3f(cen.x + size, cen.y + size, cen.z - size);
			glVertex3f(cen.x + size, cen.y - size, cen.z + size);
			glVertex3f(cen.x + size, cen.y + size, cen.z + size);
		glEnd();*/
	}
	    


	glPopAttrib();
	view.endGL();
}

MStatus pMapLocator::initialize()
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
	alevel = nAttr.create("alevel","alv",MFnNumericData::kShort );
	nAttr.setDefault( 4 ); 
	nAttr.setMin( 0 );
	nAttr.setMax( 16 );
    nAttr.setKeyable( true ); 
    nAttr.setReadable( true ); 
    nAttr.setWritable( true ); 
    nAttr.setStorable( true ); 
	stat = addAttribute(alevel);
	if(!stat)
	{
		stat.perror("Unable to add \"alevel\" attribute");
		return stat;
	}

	aposition = nAttr.create("aposition","apo",MFnNumericData::k3Float);
	nAttr.setDefault( 0 );
	nAttr.setKeyable( true ); 
    nAttr.setReadable( true ); 
    nAttr.setWritable( true ); 
    nAttr.setStorable( true ); 
	stat = addAttribute(aposition);

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
	
	aframestep = nAttr.create( "frameStep", "fst", MFnNumericData::kInt, 1 );
	nAttr.setMin(1);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( aframestep );
	
	MFnTypedAttribute   stringAttr;
	input = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( input );
	
	MFnTypedAttribute   meshAttr;
	aoutval = meshAttr.create( "aoutval", "o", MFnData::kMesh ); 
	meshAttr.setStorable(false);
	meshAttr.setWritable(false);
	addAttribute( aoutval );
    
	attributeAffects( alevel, aoutval );
	attributeAffects( input, aoutval );
	attributeAffects( frame, aoutval );
	attributeAffects( aminframe, aoutval );
	attributeAffects( amaxframe, aoutval );
	attributeAffects( aframestep, aoutval );
	attributeAffects( aposition, aoutval );
	return MS::kSuccess;

}

