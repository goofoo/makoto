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
MObject	bruiseMapNode::aBias;
MObject bruiseMapNode::asavemap;
MObject	bruiseMapNode::amapsize;
MObject	bruiseMapNode::auvset;
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
		
		float bias = data.inputValue(aBias, &status).asFloat();

		if(frame == startFrame)
		{
			int mapsize = data.inputValue(amapsize, &status).asInt();
			m_base->init(mapsize);
		}
		
		int npt = m_base->dice(bias);

		int isave = data.inputValue(asavemap, &status).asInt();
		if(isave==1) 
		{
			MString cache_path;
			MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), cache_path);

			cache_path = cache_path + "/data/" + MFnDependencyNode(thisMObject()).name() + "."+frame+".exr";
			MGlobal::displayInfo(MString("Saving ")+cache_path);
			
			MString uvname = data.inputValue(auvset).asString();
			if(uvname == "") uvname = "map1";
			m_base->save(bias, cache_path.asChar(), uvname);
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
	glPointSize(2);
	if(m_base->hasBase()) m_base->draw();

	view.endGL();
}

bool bruiseMapNode::isBounded() const
{ 
	return false;
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
	
	aBias = nAttr.create("bias", "bs",
						  MFnNumericData::kFloat, 0.1f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( nAttr.setStorable(true));
    CHECK_MSTATUS( nAttr.setKeyable(true));
    CHECK_MSTATUS( nAttr.setMin(0.f));
	
	amapsize = nAttr.create( "mapsize", "mapsize", MFnNumericData::kInt, 1024 );
	nAttr.setStorable(false);
	nAttr.setKeyable(false);
	addAttribute( amapsize );
	
	auvset = tAttr.create("uvset", "uv",
	MFnData::kString, MObject::kNullObj, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( tAttr.setStorable(true));
    CHECK_MSTATUS( tAttr.setKeyable(false));
	tAttr.setCached( true );
	tAttr.setInternal( true );
	
	zWorks::createMatrixAttr(aworldSpace, "worldSpace", "ws");

	aoutput = nAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	nAttr.setKeyable(false);
	addAttribute( aoutput );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kMesh);
	zCheckStatus(addAttribute(aguide), "ERROR adding grow mesh");
	
	astartframe = nAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( astartframe );
	
	zWorks::createTimeAttr(acurrenttime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(addAttribute(acurrenttime), "ERROR adding time");
	
	asavemap = nAttr.create( "saveMap", "sm", MFnNumericData::kInt, 0);
	nAttr.setStorable(false);
	nAttr.setKeyable(true);
	addAttribute(asavemap);
	
	CHECK_MSTATUS( addAttribute(aBias));
	CHECK_MSTATUS( addAttribute(auvset));
	
	addAttribute(aworldSpace);
	attributeAffects( aBias, aoutput );
	attributeAffects( amapsize, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( astartframe, aoutput );
	attributeAffects( acurrenttime, aoutput );
	attributeAffects( aguide, aoutput );
	attributeAffects( asavemap, aoutput );
	
	return MS::kSuccess;
}

/* virtual */
bool 
bruiseMapNode::setInternalValueInContext( const MPlug& plug,
												  const MDataHandle& handle,
												  MDGContext&)
{
	bool handledAttribute = false;

	return handledAttribute;
}

/* virtual */
bool
bruiseMapNode::getInternalValueInContext( const MPlug& plug,
											  MDataHandle& handle,
											  MDGContext&)
{
	bool handledAttribute = false;

	return handledAttribute;
}

//:~