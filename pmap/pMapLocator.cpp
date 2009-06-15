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
MObject     pMapLocator::alevel;
OcTree      tree;
TreeNode    *t;
XYZ         *particle;
unsigned int sum;

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
	MStatus stat;
	MObject thisNode = thisMObject();
	MFnDagNode dagFn(thisNode);
	MPlug tPlug = dagFn.findPlug( alevel, &stat);
	short lv;
	tPlug.getValue( lv ); 
	
	ifstream infile;
	infile.open("C:/Temp/pMapCmd.dat", ios_base::in | ios_base::binary ); 
	if(!infile.is_open())
	{
		MGlobal::displayWarning(MString("Cannot open file:  C:/Temp/pMapCmd.dat"));
		return false;
	}
	infile.read((char*)&sum,sizeof(int));

	particle = new XYZ[sum];
    MVector p;
    for(unsigned int i = 0;i<sum;i++)
	{
		infile.read((char*)&p[0],sizeof(p[0]));
		particle[i].x = p[0];
		infile.read((char*)&p[1],sizeof(p[1]));
		particle[i].y = p[1];
		infile.read((char*)&p[2],sizeof(p[2]));
		particle[i].z = p[2];
	}
	infile.close();
	
	XYZ center;
	if( tree.sizegainarray == 0)
	{   tree.gainarray = new XYZ[sum*24];
		t = new TreeNode(0, sum-1);
	    tree.CreateOcTree(t,particle,0,sum - 1,center,0.0,10);
		tree.PrintOcTree(t);
	}
	tree.sizegainarray = 0;
	tree.GetChildren(t,lv);
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
	loadParticlePosition();

    unsigned int i;
    for ( i = 0; i < sum; i ++ ) 
		bbox.expand( MPoint( particle[i].x, particle[i].y, particle[i].z ) ); 
    return bbox; 
} 

void pMapLocator::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glBegin(GL_LINES);
	for( int i = 0;i<tree.sizegainarray;i++)
		glVertex3f(tree.gainarray[i].x,tree.gainarray[i].y,tree.gainarray[i].z);
	glEnd();
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

