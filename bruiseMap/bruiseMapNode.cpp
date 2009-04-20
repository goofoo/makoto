//-
// ==========================================================================
// Copyright (C) 1995 - 2005 Alias Systems Corp. and/or its licensors.  All 
// rights reserved. 
// 
// The coded instructions, statements, computer programs, and/or related 
// material (collectively the "Data") in these files are provided by Alias 
// Systems Corp. ("Alias") and/or its licensors for the exclusive use of the 
// Customer (as defined in the Alias Software License Agreement that 
// accompanies this Alias software). Such Customer has the right to use, 
// modify, and incorporate the Data into other products and to distribute such 
// products for use by end-users.
//  
// THE DATA IS PROVIDED "AS IS".  ALIAS HEREBY DISCLAIMS ALL WARRANTIES 
// RELATING TO THE DATA, INCLUDING, WITHOUT LIMITATION, ANY AND ALL EXPRESS OR 
// IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE. IN NO EVENT SHALL ALIAS BE LIABLE FOR ANY DAMAGES 
// WHATSOEVER, WHETHER DIRECT, INDIRECT, SPECIAL, OR PUNITIVE, WHETHER IN AN 
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, OR IN EQUITY, 
// ARISING OUT OF ACCESS TO, USE OF, OR RELIANCE UPON THE DATA.
// ==========================================================================
//+

#include "bruiseMapNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

MTypeId bruiseMapNode::id( 0x0002517 );

MObject bruiseMapNode::astartframe;
MObject bruiseMapNode::acurrenttime;
MObject	bruiseMapNode::aExposure;
MObject	bruiseMapNode::aSize;
MObject	bruiseMapNode::aHDRName;
MObject bruiseMapNode::aworldSpace;
MObject bruiseMapNode::aoutput;
MObject bruiseMapNode::agrowth;
MObject bruiseMapNode::aguide;

bruiseMapNode::bruiseMapNode()
{
	m_base = new bruiseMap();
}

bruiseMapNode::~bruiseMapNode() 
{
	delete m_base;
}

MStatus bruiseMapNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput)
	{
		MObject ogrow = data.inputValue(agrowth).asMesh();
		if(!ogrow.isNull()) m_base->setBase(ogrow);
		
		MObject oguide = data.inputValue(aguide).asMesh();
		if(!oguide.isNull()) m_base->setGuide(oguide);
		
		//MMatrix mat = data.inputValue(aworldSpace).asMatrix();
		MTime currentTime = data.inputValue(acurrenttime, &status).asTime();
		int frame = (int)currentTime.value();
		
		int startFrame = data.inputValue(astartframe, &status).asInt();
		
		if(startFrame == frame)
		{
			int npt = m_base->dice();
			MGlobal::displayInfo(MString("Bruise map diced ") + npt + " samples");
		}
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void bruiseMapNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 

	if(m_base->hasMesh()) m_base->draw();

	view.endGL();
}

bool bruiseMapNode::isBounded() const
{ 
	return true;
}

MBoundingBox bruiseMapNode::boundingBox() const
{ 
	MPoint corner1( -1.-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* bruiseMapNode::creator()
{
	return new bruiseMapNode();
}

MStatus bruiseMapNode::initialize()
{ 
	MStatus			 status;
	    MFnNumericAttribute nAttr; 
    MFnTypedAttribute tAttr;
	
	aExposure = nAttr.create("exposure", "exposure",
						  MFnNumericData::kFloat, 1.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( nAttr.setStorable(true));
    CHECK_MSTATUS( nAttr.setKeyable(true));
    CHECK_MSTATUS( nAttr.setDefault(1.f));
    CHECK_MSTATUS( nAttr.setSoftMin(-10.f));
    CHECK_MSTATUS( nAttr.setSoftMax(10.f));
	nAttr.setCached( true );
	nAttr.setInternal( true );
	
	aSize = nAttr.create("size", "size",
						  MFnNumericData::kFloat, 900.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( nAttr.setStorable(true));
    CHECK_MSTATUS( nAttr.setKeyable(true));
    CHECK_MSTATUS( nAttr.setDefault(900.f));
    CHECK_MSTATUS( nAttr.setMin(1.f));
	nAttr.setCached( true );
	nAttr.setInternal( true );
	
	aHDRName = tAttr.create("hdrName", "hdrname",
	MFnData::kString, MObject::kNullObj, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( tAttr.setStorable(true));
    CHECK_MSTATUS( tAttr.setKeyable(false));
	tAttr.setCached( true );
	tAttr.setInternal( true );
	
	zWorks::createMatrixAttr(aworldSpace, "worldSpace", "ws");
	
	MFnNumericAttribute numAttr;
	aoutput = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutput );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kMesh);
	zCheckStatus(addAttribute(aguide), "ERROR adding grow mesh");
	
	astartframe = numAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( astartframe );
	
	zWorks::createTimeAttr(acurrenttime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(addAttribute(acurrenttime), "ERROR adding time");
	
	CHECK_MSTATUS( addAttribute(aSize));
		CHECK_MSTATUS( addAttribute(aExposure));
	CHECK_MSTATUS( addAttribute(aHDRName));
	
	addAttribute(aworldSpace);
	attributeAffects( aExposure, aoutput );
	attributeAffects( aworldSpace, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( astartframe, aoutput );
	attributeAffects( acurrenttime, aoutput );
	attributeAffects( aguide, aoutput );
	
	return MS::kSuccess;
}

/* virtual */
bool 
bruiseMapNode::setInternalValueInContext( const MPlug& plug,
												  const MDataHandle& handle,
												  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == aHDRName)
	{
		handledAttribute = true;
		m_hdrname = (MString) handle.asString();
	}

	else if (plug == aExposure)
	{
		handledAttribute = true;
		float val = handle.asFloat();
		if (val != m_exposure)
		{
			m_exposure = val;
			mAttributesChanged = true;
		}
	}
	else if (plug == aSize)
	{
		handledAttribute = true;
		float val = handle.asFloat();
		if (val != m_size)
		{
			m_size = val;
			mAttributesChanged = true;
		}
	}

	return handledAttribute;
}

/* virtual */
bool
bruiseMapNode::getInternalValueInContext( const MPlug& plug,
											  MDataHandle& handle,
											  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == aExposure)
	{
		handledAttribute = true;
		handle.set( m_exposure );
	}
	else if (plug == aHDRName)
	{
		handledAttribute = true;
		handle.set( m_hdrname );
	}
	else if (plug == aSize)
	{
		handledAttribute = true;
		handle.set( m_size );
	}

	return handledAttribute;
}

//:~