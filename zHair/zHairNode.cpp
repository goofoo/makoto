#include "zHairNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

MTypeId HairNode::id( 0x0002518 );

MObject HairNode::astartframe;
MObject HairNode::acurrenttime;
MObject	HairNode::aExposure;
MObject	HairNode::aSize;
MObject	HairNode::aHDRName;
MObject HairNode::aworldSpace;
MObject HairNode::aoutput;
MObject HairNode::agrowth;
MObject HairNode::aguide;
MObject HairNode::alengthnoise;

HairNode::HairNode()
{
	m_base = new hairMap();
}

HairNode::~HairNode() 
{
	delete m_base;
}

MStatus HairNode::compute( const MPlug& plug, MDataBlock& data )
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
			MGlobal::displayInfo(MString("ZHair diced ") + npt + " samples");
			m_base->initGuide();
			//npt = m_base->saveDguide( );
			//if(npt == 1)
			//	MGlobal::displayInfo("Save Dguide data successed");
			//else
			//	MGlobal::displayInfo("Save Dguide data failed");
			m_base->bind();	
		}
		else m_base->updateGuide();
		
		m_base->setTwist(data.inputValue(aExposure).asFloat());
		m_base->setClumping(data.inputValue(aSize).asFloat());
		m_base->setNoise(data.inputValue(alengthnoise).asFloat());
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void HairNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPointSize(2);
	if(m_base->hasBase()) m_base->draw();
	m_base->drawGuide();
	glPopAttrib();
	view.endGL();
}

bool HairNode::isBounded() const
{ 
	return false;
}

MBoundingBox HairNode::boundingBox() const
{ 
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* HairNode::creator()
{
	return new HairNode();
}

MStatus HairNode::initialize()
{ 
	MStatus			 status;
	    MFnNumericAttribute nAttr; 
    MFnTypedAttribute tAttr;
	
	alengthnoise = nAttr.create("noise", "noi",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( nAttr.setStorable(true));
    CHECK_MSTATUS( nAttr.setKeyable(true));
    CHECK_MSTATUS( nAttr.setDefault(0.f));
	nAttr.setMin(0);
	nAttr.setMax(1);
	
	aExposure = nAttr.create("twist", "twt",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( nAttr.setStorable(true));
    CHECK_MSTATUS( nAttr.setKeyable(true));
    CHECK_MSTATUS( nAttr.setDefault(0.f));
	nAttr.setCached( true );
	nAttr.setInternal( true );
	nAttr.setMin(-1);
	nAttr.setMax(1);
	
	aSize = nAttr.create("clumping", "clp",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( nAttr.setStorable(true));
    CHECK_MSTATUS( nAttr.setKeyable(true));
    CHECK_MSTATUS( nAttr.setDefault(0.f));
    CHECK_MSTATUS( nAttr.setMin(-1.f));
	nAttr.setMax(1);
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
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	astartframe = numAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( astartframe );
	
	zWorks::createTimeAttr(acurrenttime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(addAttribute(acurrenttime), "ERROR adding time");
	
	addAttribute(alengthnoise);
	CHECK_MSTATUS( addAttribute(aSize));
		CHECK_MSTATUS( addAttribute(aExposure));
	CHECK_MSTATUS( addAttribute(aHDRName));
	addAttribute(aworldSpace);
	attributeAffects( alengthnoise, aoutput );
	attributeAffects( aExposure, aoutput );
	attributeAffects( aSize, aoutput );
	attributeAffects( aworldSpace, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	attributeAffects( astartframe, aoutput );
	attributeAffects( acurrenttime, aoutput );
	
	return MS::kSuccess;
}

/* virtual */
bool 
HairNode::setInternalValueInContext( const MPlug& plug,
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
HairNode::getInternalValueInContext( const MPlug& plug,
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
