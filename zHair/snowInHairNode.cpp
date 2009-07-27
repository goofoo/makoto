//
// Copyright (C) YiLi
// 
// File: snowInHairNode.cpp
//
// Dependency Graph Node: snowInHair
//
// Author: Maya Plug-in Wizard 2.0
//


#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MFnMeshData.h>
#include "snowInHairNode.h"
#include "../shared/zData.h"
#include "../shared/zGlobal.h"
#include "../shared/zWorks.h"
#include "HairMap.h"
#include <iostream>
#include <fstream>
using namespace std;

// You MUST change this to a unique value!!!  The id is a 32bit value used
// to identify this type of node in the binary file format.  
//
//#error change the following to a unique value and then erase this line 
MTypeId     snowInHairNode::id( 0x0002529 );

// Example attributes
//        
MObject     snowInHairNode::aoutput;
MObject snowInHairNode::acachename;
MObject snowInHairNode::aframe;
MObject snowInHairNode::afuzz;
MObject snowInHairNode::akink;
MObject snowInHairNode::aclump; 
MObject snowInHairNode::astep;
MObject snowInHairNode::arootcolorr;
MObject snowInHairNode::arootcolorg;
MObject snowInHairNode::arootcolorb; 
MObject snowInHairNode::atipcolorr;
MObject snowInHairNode::atipcolorg;
MObject snowInHairNode::atipcolorb;
MObject snowInHairNode::amutantcolorr;
MObject snowInHairNode::amutantcolorg;
MObject snowInHairNode::amutantcolorb;  
MObject snowInHairNode::amutantcolorscale;
MObject snowInHairNode::adice;
MObject snowInHairNode::adraw;
MObject snowInHairNode::ainterpolate;
//MObject snowInHairNode::aoffset;

snowInHairNode::snowInHairNode() : m_base(0),isInterpolate(0),idice(0),idraw(0)
{
	m_base = new hairMap();
	curname = "null";
}
snowInHairNode::~snowInHairNode() 
{
	if(m_base) delete m_base;
}

MStatus snowInHairNode::compute( const MPlug& plug, MDataBlock& data )
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
	MStatus status;
 
	// Check which output attribute we have been asked to compute.  If this 
	// node doesn't know how to compute it, we must return 
	// MS::kUnknownParameter.
	// 
	if( plug == aoutput )
	{
		double dtime = data.inputValue( aframe ).asDouble();
		MString sname = data.inputValue( acachename).asString();
		string sbuf(sname.asChar());
		zGlobal::changeFrameNumber(sbuf, zGlobal::safeConvertToInt(dtime));
		int eta = data.inputValue(adice).asInt();
		idraw = data.inputValue(adraw).asInt();
		if(m_base) 
		{
			if(curname != sname || isInterpolate !=data.inputValue(ainterpolate).asInt() || eta != idice)
			{
				curname = sname;
				string head = sbuf;
				zGlobal::cutByFirstDot(head);
				head += ".hairstart";
				m_base->loadStart(head.c_str());
				idice = eta;
				MGlobal::displayInfo(MString("nsamp ")+m_base->dice(eta));
				isInterpolate = data.inputValue(ainterpolate).asInt();
				m_base->setInterpolate(isInterpolate);
				m_base->bind();
			}
			int iss = m_base->load(sbuf.c_str());
			if(iss != 1) MGlobal::displayWarning(MString("Cannot open file ")+sbuf.c_str());
			
			m_base->setClumping(data.inputValue(aclump).asFloat());
			m_base->setFuzz(data.inputValue(afuzz).asFloat());
			m_base->setKink(data.inputValue(akink).asFloat());
			//m_base->setDrawAccuracy(data.inputValue(astep).asInt());
			//m_base->setRootColor(data.inputValue(arootcolorr).asDouble(), data.inputValue(arootcolorg).asDouble(), data.inputValue(arootcolorb).asDouble());
			//m_base->setTipColor(data.inputValue(atipcolorr).asDouble(), data.inputValue(atipcolorg).asDouble(), data.inputValue(atipcolorb).asDouble());
			//m_base->setMutantColor(data.inputValue(amutantcolorr).asDouble(), data.inputValue(amutantcolorg).asDouble(), data.inputValue(amutantcolorb).asDouble());
			//m_base->setMutantColorScale(data.inputValue(amutantcolorscale).asDouble());
			
			MFnMeshData dataCreator;
			MObject outMeshData = dataCreator.create(&status);
			
			m_base->createSnow(outMeshData);
			
			MDataHandle meshh = data.outputValue(aoutput, &status);
			meshh.set(outMeshData);
			data.setClean(plug);
		}
		return MS::kSuccess;
	}
	return MS::kUnknownParameter;
}

void* snowInHairNode::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new snowInHairNode();
}

MStatus snowInHairNode::initialize()
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
	
	aclump = numAttr.create( "clumping", "cmp", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(aclump);
	
	astep = numAttr.create( "drawStep", "dsp", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(astep);

	zCheckStatus(zWorks::createTypedAttr(aoutput, MString("outMesh"), MString("om"), MFnData::kMesh), "ERROR creating out mesh");
	zCheckStatus(addAttribute(aoutput), "ERROR adding out mesh");
		
	aframe = numAttr.create( "currentTime", "ct", MFnNumericData::kDouble, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframe );
	
	MFnTypedAttribute   stringAttr;
	acachename = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	stringAttr.setInternal(true);
	addAttribute( acachename );
	
	zWorks::createDoubleAttr(arootcolorr, "rootColorR", "rootColorR", 0.0);
	zWorks::createDoubleAttr(arootcolorg, "rootColorG", "rootColorG", 0.0);
	zWorks::createDoubleAttr(arootcolorb, "rootColorB", "rootColorB", 0.0);
	addAttribute(arootcolorr);
	addAttribute(arootcolorg);
	addAttribute(arootcolorb);
	
	zWorks::createDoubleAttr(atipcolorr, "tipColorR", "tipColorR", 1.0);
	zWorks::createDoubleAttr(atipcolorg, "tipColorG", "tipColorG", 1.0);
	zWorks::createDoubleAttr(atipcolorb, "tipColorB", "tipColorB", 1.0);
	addAttribute(atipcolorr);
	addAttribute(atipcolorg);
	addAttribute(atipcolorb);
	
	zWorks::createDoubleAttr(amutantcolorr, "mutantColorR", "mutantColorR", 1.0);
	zWorks::createDoubleAttr(amutantcolorg, "mutantColorG", "mutantColorG", 1.0);
	zWorks::createDoubleAttr(amutantcolorb, "mutantColorB", "mutantColorB", 1.0);
	addAttribute(amutantcolorr);
	addAttribute(amutantcolorg);
	addAttribute(amutantcolorb);
	
	zWorks::createDoubleAttr(amutantcolorscale, "mutantColorScale", "mcs", 0.0);
	addAttribute(amutantcolorscale);
	
	adraw = numAttr.create( "drawType", "drt", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(adraw);
	
	adice = numAttr.create( "dice", "dc", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setMin(0);
	numAttr.setKeyable(true);
	addAttribute(adice);
	
	ainterpolate = numAttr.create( "interpolate", "ipl", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(ainterpolate);
	
	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	//
	attributeAffects( aframe, aoutput );
	attributeAffects( acachename, aoutput );
	attributeAffects( afuzz, aoutput );
	attributeAffects( akink, aoutput );
	attributeAffects( aclump, aoutput );
	attributeAffects( astep, aoutput );
	attributeAffects( arootcolorr, aoutput );
	attributeAffects( arootcolorg, aoutput );
	attributeAffects( arootcolorb, aoutput );
	attributeAffects( atipcolorr, aoutput );
	attributeAffects( atipcolorg, aoutput );
	attributeAffects( atipcolorb, aoutput );
	attributeAffects( amutantcolorr, aoutput );
	attributeAffects( amutantcolorg, aoutput );
	attributeAffects( amutantcolorb, aoutput );
	attributeAffects( amutantcolorscale, aoutput );
	attributeAffects( ainterpolate, aoutput );
	attributeAffects( adice, aoutput );
	attributeAffects( adraw, aoutput );
	
	return MS::kSuccess;

}