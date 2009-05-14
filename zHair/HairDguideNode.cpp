//
// Copyright (C) YiLi
// 
// File: HairDguideNode.cpp
//
// Dependency Graph Node: HairDguide
//
// Author: Maya Plug-in Wizard 2.0
//


#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include "HairDguideNode.h"
#include "../shared/zData.h"
#include "HairMap.h"
#include <iostream>
#include <fstream>
using namespace std;

#include <maya/MGlobal.h>

using namespace std;

// You MUST change this to a unique value!!!  The id is a 32bit value used
// to identify this type of node in the binary file format.  
//
//#error change the following to a unique value and then erase this line 
MTypeId     HairDguideNode::id( 0x0002519 );

// Example attributes
// 
MObject     HairDguideNode::input;        
MObject     HairDguideNode::output;       

HairDguideNode::HairDguideNode() 
{
	m_base = new hairMap();
}
HairDguideNode::~HairDguideNode() 
{
	delete m_base;
}

MStatus HairDguideNode::compute( const MPlug& plug, MDataBlock& data )
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
	m_base->loadDguide();
	MStatus returnStatus;
 
	// Check which output attribute we have been asked to compute.  If this 
	// node doesn't know how to compute it, we must return 
	// MS::kUnknownParameter.
	// 
	if( plug == output )
	{
		// Get a handle to the input attribute that we will need for the
		// computation.  If the value is being supplied via a connection 
		// in the dependency graph, then this call will cause all upstream  
		// connections to be evaluated so that the correct value is supplied.
		// 
		MDataHandle inputData = data.inputValue( input, &returnStatus );

		if( returnStatus != MS::kSuccess )
			MGlobal::displayError( "Node HairDguideNode cannot get value\n" );
		else
		{
			// Read the input value from the handle.
			//
			float result = inputData.asFloat();

			// Get a handle to the output attribute.  This is similar to the
			// "inputValue" call above except that no dependency graph 
			// computation will be done as a result of this call.
			// 
			MDataHandle outputHandle = data.outputValue( HairDguideNode::output );
			// This just copies the input value through to the output.  
			// 
			outputHandle.set( result );
			// Mark the destination plug as being clean.  This will prevent the
			// dependency graph from repeating this calculation until an input 
			// of this node changes.
			// 
			data.setClean(plug);
		}
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}


void HairDguideNode::draw( M3dView & view, const MDagPath & path, 
                      M3dView::DisplayStyle style,
                      M3dView::DisplayStatus status )
{
	//loadDguide( );
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	m_base->drawGuide();
	glPopAttrib();
	view.endGL();
}

void* HairDguideNode::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new HairDguideNode();
}

MStatus HairDguideNode::initialize()
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
	MFnNumericAttribute nAttr;
	MStatus				stat;

	input = nAttr.create( "input", "in", MFnNumericData::kFloat, 0.0 );
	// Attribute will be written to files when this type of node is stored
 	nAttr.setStorable(true);
	// Attribute is keyable and will show up in the channel box
 	nAttr.setKeyable(true);

	output = nAttr.create( "output", "out", MFnNumericData::kFloat, 0.0 );
	// Attribute is read-only because it is an output attribute
	nAttr.setWritable(false);
	// Attribute will not be written to files when this type of node is stored
	nAttr.setStorable(false);

	// Add the attributes we have created to the node
	//
	stat = addAttribute( input );
		if (!stat) { stat.perror("addAttribute"); return stat;}
	stat = addAttribute( output );
		if (!stat) { stat.perror("addAttribute"); return stat;}

	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	//
	stat = attributeAffects( input, output );
		if (!stat) { stat.perror("attributeAffects"); return stat;}

	return MS::kSuccess;

}