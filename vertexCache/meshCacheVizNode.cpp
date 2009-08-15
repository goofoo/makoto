#include "meshCacheVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/zGlobal.h"
#include "../shared/FXMLMesh.h"
#include "../shared/zData.h"

MTypeId meshCacheVizNode::id( 0x00025256 );

MObject meshCacheVizNode::ameshname;
MObject meshCacheVizNode::acachename;
MObject meshCacheVizNode::aframe;
MObject meshCacheVizNode::aminframe;
MObject meshCacheVizNode::amaxframe;
MObject meshCacheVizNode::amode;
MObject meshCacheVizNode::aincoe;
MObject meshCacheVizNode::aoutval;

meshCacheVizNode::meshCacheVizNode():m_pMesh(0),m_mode(-1)
{
}

meshCacheVizNode::~meshCacheVizNode()
{
	if(m_pMesh) delete m_pMesh;
}

MStatus meshCacheVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	if(plug == aoutval)
	{
		int hascoe = 0;
		MVectorArray vcoe = zWorks::getVectorArrayAttr(data, aincoe);
		XYZ hdrCoeff[16];
		if(vcoe.length() == 16)
		{
			hascoe = 1;
			for(unsigned i=0; i<16; i++) hdrCoeff[i] = XYZ(vcoe[i].x, vcoe[i].y, vcoe[i].z);
		}
		
		double dtime = data.inputValue( aframe ).asDouble();

		string sbuf(m_cachename.asChar());
		zGlobal::changeFrameNumber(sbuf, zGlobal::safeConvertToInt(dtime));
		if(m_pMesh) 
		{
			m_pMesh->updateFrom(sbuf.c_str());
			if(hascoe==1) m_pMesh->setHDRLighting(hdrCoeff);
			else m_pMesh->resetHDRLighting();
			m_pMesh->updateColor(m_mode);
		}
		else
		{
			m_cachename =  data.inputValue( acachename ).asString();
			m_meshname =  data.inputValue( ameshname ).asString();
			
			if(m_cachename != "" && m_meshname != "")
			{
				m_pMesh = new FXMLMesh(m_cachename.asChar(), m_meshname.asChar());

				if(m_pMesh->isNull()) 
				{
					delete m_pMesh;
					m_pMesh = 0;
				}
			}
		}
	}
	return MS::kUnknownParameter;
}

void meshCacheVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 
	view.beginGL(); 

	glPushAttrib( GL_POLYGON_BIT );
	if(m_mode<0) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glFrontFace(GL_CW);
	glShadeModel(GL_SMOOTH);

	if(m_pMesh) 
	{
		if(m_mode<-2) m_pMesh->drawUV(-3-m_mode);
		else if(m_mode<-1) 
		{
			glEnable(GL_CULL_FACE);
			m_pMesh->drawNoColor();
			glDisable(GL_CULL_FACE);
			m_pMesh->drawTangentSpace();
		}
		else if(m_mode<0) 
		{
			glEnable(GL_CULL_FACE);
			m_pMesh->drawNoColor();
			glDisable(GL_CULL_FACE);
		}
		else{
			m_pMesh->draw();
			m_pMesh->drawBBox();
		}
	}

	glPopAttrib();
	
	view.endGL();
}

bool meshCacheVizNode::isBounded() const
{ 
	return true;
}

MBoundingBox meshCacheVizNode::boundingBox() const
{ 
	MPoint corner1( 0,0,0 );
	MPoint corner2( 1,1,1 );
	
	if(m_mode>-3)
	{
		if(m_pMesh)
		{
			corner1.x = m_pMesh->getBBox0X();
			corner1.y = m_pMesh->getBBox0Y();
			corner1.z = m_pMesh->getBBox0Z();
			corner2.x = m_pMesh->getBBox1X();
			corner2.y = m_pMesh->getBBox1Y();
			corner2.z = m_pMesh->getBBox1Z();
		}
	}

	return MBoundingBox( corner1, corner2 );
}

void* meshCacheVizNode::creator()
{
	return new meshCacheVizNode();
}

MStatus meshCacheVizNode::initialize()
{ 
	MStatus				stat;
	MFnNumericAttribute numAttr;
	
	aframe = numAttr.create( "currentTime", "ct", MFnNumericData::kDouble, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframe );
	
	aminframe = numAttr.create( "minFrame", "mnf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aminframe );
	
	amaxframe = numAttr.create( "maxFrame", "mxf", MFnNumericData::kInt, 24 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( amaxframe );
	
	aoutval = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutval );
	
	MFnTypedAttribute   stringAttr;
	acachename = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	stringAttr.setInternal(true);
	addAttribute( acachename );
	
	ameshname = stringAttr.create( "meshName", "mn", MFnData::kString );
 	stringAttr.setStorable(true);
	stringAttr.setInternal(true);
	addAttribute(ameshname);
	
	amode = numAttr.create( "drawMode", "dm", MFnNumericData::kInt, -1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	numAttr.setInternal(true);
	addAttribute( amode );
	
	zWorks::createVectorArrayAttr(aincoe, "inCoeff", "icoe");
	addAttribute( aincoe );
	
	attributeAffects( aframe, aoutval );
	attributeAffects( aincoe, aoutval );
    	
	return MS::kSuccess;
}

/* virtual */
bool 
meshCacheVizNode::setInternalValueInContext( const MPlug& plug,
												  const MDataHandle& handle,
												  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == acachename)
	{
		handledAttribute = true;
		m_cachename = (MString) handle.asString();
		
		if(m_pMesh) delete m_pMesh;
		m_pMesh = new FXMLMesh(m_cachename.asChar(), m_meshname.asChar());

		if(m_pMesh->isNull()) 
		{
			delete m_pMesh;
			m_pMesh = 0;
		}
		else m_pMesh->updateColor(m_mode);
	}
	
	if (plug == ameshname)
	{
		handledAttribute = true;
		m_meshname = (MString) handle.asString();
	}
	
	if (plug == amode)
	{
		handledAttribute = true;
		m_mode = handle.asInt();
		if(m_pMesh) m_pMesh->updateColor(m_mode);
	}

	return handledAttribute;
}

/* virtual */
bool
meshCacheVizNode::getInternalValueInContext( const MPlug& plug,
											  MDataHandle& handle,
											  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == acachename)
	{
		handledAttribute = true;
		handle.set( m_cachename );
	}
	
	if (plug == ameshname)
	{
		handledAttribute = true;
		handle.set( m_meshname );
	}
	
	if (plug == amode)
	{
		handledAttribute = true;
		handle.set( m_mode );
	}
	
	return handledAttribute;
}

//:~