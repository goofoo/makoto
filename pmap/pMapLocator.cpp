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
#include <iostream>
#include <fstream>
#include <string>
#include <maya/MVector.h>

#include <maya/MGlobal.h>


MTypeId     pMapLocator::id( 0x00001 );

pMapLocator::pMapLocator() {}
pMapLocator::~pMapLocator() {}

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
	MStatus stat;
	
	MString path =  data.inputValue( input ).asString();	
	double time = data.inputValue( frame ).asTime().value();
	int minfrm = data.inputValue( aminframe ).asInt();
	int frmstep = data.inputValue( aframestep ).asInt();
	
	if( time < minfrm ) time = minfrm;
		
	int frame_lo = minfrm + int(time-minfrm)/frmstep*frmstep;
	int frame_hi = frame_lo+frmstep;

	char filename[256];
	sprintf( filename, "%s.%d.dat", path.asChar(), frame_lo );
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

bool pMapLocator::loadParticlePosition() const
{
	return true;	
}

void pMapLocator::draw()
{
}

bool pMapLocator::isBounded() const
{ 
    return true;
}

MBoundingBox pMapLocator::boundingBox() const
{
	MBoundingBox bbox;
    return bbox; 
} 

void pMapLocator::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);

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
	nAttr.setMax( 10 );
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
	return MS::kSuccess;

}

