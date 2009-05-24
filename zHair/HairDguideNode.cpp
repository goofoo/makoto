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
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include "HairDguideNode.h"
#include "../shared/zData.h"
#include "../shared/zGlobal.h"
#include "HairMap.h"
#include <iostream>
#include <fstream>
using namespace std;

// You MUST change this to a unique value!!!  The id is a 32bit value used
// to identify this type of node in the binary file format.  
//
//#error change the following to a unique value and then erase this line 
MTypeId     HairDguideNode::id( 0x0002519 );

// Example attributes
//        
MObject     HairDguideNode::output;
MObject HairDguideNode::acachename;
MObject HairDguideNode::aframe;
MObject HairDguideNode::afuzz;
MObject HairDguideNode::akink;
MObject HairDguideNode::atwist;
MObject HairDguideNode::aclump;      

HairDguideNode::HairDguideNode() : m_base(0)
{
	m_base = new hairMap();
	curname = "null";
}
HairDguideNode::~HairDguideNode() 
{
	if(m_base) delete m_base;
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
	MStatus returnStatus;
 
	// Check which output attribute we have been asked to compute.  If this 
	// node doesn't know how to compute it, we must return 
	// MS::kUnknownParameter.
	// 
	if( plug == output )
	{
		double dtime = data.inputValue( aframe ).asDouble();
		MString sname = data.inputValue( acachename).asString();
		string sbuf(sname.asChar());
		zGlobal::changeFrameNumber(sbuf, zGlobal::safeConvertToInt(dtime));
		if(m_base) 
		{
			if(curname != sname)
			{
				curname = sname;
				string head = sbuf;
				zGlobal::cutByFirstDot(head);
				head += ".hairstart";
				m_base->loadStart(head.c_str());
				MGlobal::displayInfo(MString("nsamp ")+m_base->dice());
				m_base->bind();
			}
			int iss = m_base->load(sbuf.c_str());
			if(iss != 1) MGlobal::displayWarning(MString("Cannot open file ")+sbuf.c_str());
			
			m_base->setTwist(data.inputValue(atwist).asFloat());
			m_base->setClumping(data.inputValue(aclump).asFloat());
			m_base->setFuzz(data.inputValue(afuzz).asFloat());
			m_base->setKink(data.inputValue(akink).asFloat());
		}
		
		MDataHandle outputHandle = data.outputValue(output); 
		outputHandle.set( 1.0 ); 
		data.setClean(plug);
		return MS::kSuccess;
	}
	return MS::kUnknownParameter;
}


void HairDguideNode::draw( M3dView & view, const MDagPath & path, 
                      M3dView::DisplayStyle style,
                      M3dView::DisplayStatus status )
{
	view.beginGL(); 
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	if(m_base) m_base->draw();
	//glPopAttrib();
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
	MFnNumericAttribute numAttr;
	MStatus				stat;
	
	afuzz = numAttr.create( "fuzz", "fuzz", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(afuzz);
	
	akink = numAttr.create( "kink", "kink", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(akink);
	
	atwist = numAttr.create( "twisting", "twt", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(atwist);
	
	aclump = numAttr.create( "clumping", "cmp", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(aclump);

	output = numAttr.create( "output", "out", MFnNumericData::kFloat, 0.0 );
	// Attribute is read-only because it is an output attribute
	numAttr.setWritable(false);
	// Attribute will not be written to files when this type of node is stored
	numAttr.setStorable(false);

	stat = addAttribute( output );
		if (!stat) { stat.perror("addAttribute"); return stat;}
		
	aframe = numAttr.create( "currentTime", "ct", MFnNumericData::kDouble, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframe );
	
	MFnTypedAttribute   stringAttr;
	acachename = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	stringAttr.setInternal(true);
	addAttribute( acachename );

	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	//
	attributeAffects( aframe, output );
	attributeAffects( acachename, output );
	attributeAffects( afuzz, output );
	attributeAffects( akink, output );
	attributeAffects( atwist, output );
	attributeAffects( aclump, output );
	
	return MS::kSuccess;

}