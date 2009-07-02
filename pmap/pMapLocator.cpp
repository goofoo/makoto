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
#include <iostream>
#include <fstream>
#include <string>
#include <maya/MVector.h>

#include <maya/MGlobal.h>


MTypeId     pMapLocator::id( 0x00001 );
MObject     pMapLocator::alevel;
MObject     pMapLocator::frame;
MObject     pMapLocator::aframestep;
MObject     pMapLocator::amaxframe;
MObject     pMapLocator::aminframe;
MObject     pMapLocator::input;
MObject     pMapLocator::aoutval;

pMapLocator::pMapLocator() :raw_data(0),num_raw_data(0),tree(0){}
pMapLocator::~pMapLocator() {if(tree) tree->DeleteTree();}

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
	
	    if( time < minfrm ) time = minfrm;
		
	    int frame_lo = minfrm + int(time-minfrm)/frmstep*frmstep;
	    int frame_hi = frame_lo+frmstep;

	    char filename[256];
	    sprintf( filename, "%s.%d.dat", path.asChar(), frame_lo );
	    ifstream infile;
	    infile.open(filename,ios_base::out | ios_base::binary );
	    if(!infile.is_open())
		{
			MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		    return MS::kFailure;
		}
		
		infile.read((char*)&num_raw_data,sizeof(unsigned));
	    if(raw_data) delete[] raw_data;
	    raw_data = new XYZ[num_raw_data];
	    for(unsigned int i = 0;i<num_raw_data;i++)
		{
			MVector p;
			infile.read((char*)&p[0],sizeof(p[0]));
			infile.read((char*)&p[1],sizeof(p[1]));
			infile.read((char*)&p[2],sizeof(p[2]));
			raw_data[i].x = p[0];raw_data[i].y = p[1];raw_data[i].z = p[2];
		}
		
		infile.close();
		
		XYZ rootCenter;
	    float rootSize;
	    OcTree::getBBox(raw_data, num_raw_data, rootCenter, rootSize);
	    if(tree) tree->DeleteTree();
	    tree = new OcTree();
	    tree->construct(raw_data, num_raw_data, rootCenter, rootSize,3);
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

	return MBoundingBox( corner1, corner2 ); 
} 

void pMapLocator::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	
	glPointSize(3);
	if(num_raw_data > 0 && raw_data) {
		glBegin(GL_POINTS);
		glColor3f(1,0,0);
		for(unsigned i=0; i<num_raw_data; i++) {
			
			glVertex3f(raw_data[i].x, raw_data[i].y, raw_data[i].z);
		}
		glEnd();
		
		glBegin(GL_LINES);
		if(tree) tree->draw();
		glEnd();
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
    
	attributeAffects( input, aoutval );
	attributeAffects( frame, aoutval );
	attributeAffects( aminframe, aoutval );
	attributeAffects( amaxframe, aoutval );
	attributeAffects( aframestep, aoutval );
	return MS::kSuccess;

}

