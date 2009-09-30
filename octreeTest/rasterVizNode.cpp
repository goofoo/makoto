#include "rasterVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

MTypeId rasterVizNode::id( 0x0003698 );
MObject rasterVizNode::atype;
MObject rasterVizNode::alight0x;
MObject rasterVizNode::alight0y;
MObject rasterVizNode::alight0z;
MObject rasterVizNode::alight1x;
MObject rasterVizNode::alight1y;
MObject rasterVizNode::alight1z;
MObject rasterVizNode::alight2x;
MObject rasterVizNode::alight2y;
MObject rasterVizNode::alight2z;
MObject rasterVizNode::aoutput;

rasterVizNode::rasterVizNode():m_sh(0),m_type(0)
{
	m_sh = new sphericalHarmonics();
	
	m_sh->zeroCoeff(m_coeff);

	XYZ ray0(1,0,0); ray0.normalize();
	XYZ col0(0.9,0,0);
	XYZ ray1(0,1,0); ray1.normalize();
	XYZ col1(0,0.9,0);
	XYZ ray2(0,0,1); ray2.normalize();
	XYZ col2(0,0,0.9);
	
	for(unsigned int j=0; j<SH_N_SAMPLES; j++) {
		SHSample s = m_sh->getSample(j);
		float H = ray0.dot(s.vector);
		if(H>0) m_sh->projectASample(m_coeff, j, col0*H);
		
		H = ray1.dot(s.vector);
		if(H>0) m_sh->projectASample(m_coeff, j, col1*H);
		
		H = ray2.dot(s.vector);
		if(H>0) m_sh->projectASample(m_coeff, j, col2*H);
	}
	
	raster = new CubeRaster();
}

rasterVizNode::~rasterVizNode() 
{
	if(m_sh) delete m_sh;
	delete raster;
}

MStatus rasterVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput)
	{
		m_type = data.inputValue(atype).asInt();
		float x = data.inputValue(alight0x).asFloat();
		float y = data.inputValue(alight0y).asFloat();
		float z = data.inputValue(alight0z).asFloat();
		
		XYZ ray0(x,y,z); ray0.normalize();
		
		x = data.inputValue(alight1x).asFloat();
		y = data.inputValue(alight1y).asFloat();
		z = data.inputValue(alight1z).asFloat();
		
		XYZ ray1(x,y,z); ray1.normalize();
		
		x = data.inputValue(alight2x).asFloat();
		y = data.inputValue(alight2y).asFloat();
		z = data.inputValue(alight2z).asFloat();
		
		XYZ ray2(x,y,z); ray2.normalize();
		
		XYZ col0(0.9,0,0);
		XYZ col1(0,0.9,0);
		XYZ col2(0,0,0.9);
		
		m_sh->zeroCoeff(m_coeff);
		
		for(unsigned int j=0; j<SH_N_SAMPLES; j++) {
			SHSample s = m_sh->getSample(j);
			float H = ray0.dot(s.vector);
			if(H>0) m_sh->projectASample(m_coeff, j, col0*H);
			
			H = ray1.dot(s.vector);
			if(H>0) m_sh->projectASample(m_coeff, j, col1*H);
			
			H = ray2.dot(s.vector);
			if(H>0) m_sh->projectASample(m_coeff, j, col2*H);
		}
		
		raster->clear();
		raster->write(ray0);

		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void rasterVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	glPointSize(2);
	
	if(m_sh) {
		if(m_type ==0) m_sh->drawSamples();
		else if(m_type ==1) m_sh->drawBands();
		else m_sh->reconstructAndDraw(m_coeff);
	}
	
	raster->draw();
	
	glPopAttrib();
	view.endGL();
}

bool rasterVizNode::isBounded() const
{ 
	return false;
}

MBoundingBox rasterVizNode::boundingBox() const
{ 
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* rasterVizNode::creator()
{
	return new rasterVizNode();
}

MStatus rasterVizNode::initialize()
{ 
	MStatus			 status;
	MFnNumericAttribute numAttr;
    MFnTypedAttribute tAttr;
	
	atype = numAttr.create( "drawType", "dtp", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(atype);

	alight0x = numAttr.create( "light0X", "l0x", MFnNumericData::kFloat, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight0x);
	
	alight0y = numAttr.create( "light0Y", "l0y", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight0y);
	
	alight0z = numAttr.create( "light0Z", "l0z", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight0z);
	
	alight1x = numAttr.create( "light1X", "l1x", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight1x);
	
	alight1y = numAttr.create( "light1Y", "l1y", MFnNumericData::kFloat, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight1y);
	
	alight1z = numAttr.create( "light1Z", "l1z", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight1z);
	
	alight2x = numAttr.create( "light2X", "l2x", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight2x);
	
	alight2y = numAttr.create( "light2Y", "l2y", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight2y);
	
	alight2z = numAttr.create( "light2Z", "l2z", MFnNumericData::kFloat, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(alight2z);
	
	aoutput = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutput );
	
	attributeAffects( atype, aoutput );
	attributeAffects( alight0x, aoutput );
	attributeAffects( alight0y, aoutput );
	attributeAffects( alight0z, aoutput );
	attributeAffects( alight1x, aoutput );
	attributeAffects( alight1y, aoutput );
	attributeAffects( alight1z, aoutput );
	attributeAffects( alight2x, aoutput );
	attributeAffects( alight2y, aoutput );
	attributeAffects( alight2z, aoutput );
	
	return MS::kSuccess;
}
//:~
