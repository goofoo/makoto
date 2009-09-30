#include "octreeVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

MTypeId octreeVizNode::id( 0x0003508 );
MObject octreeVizNode::afuzz;
MObject octreeVizNode::astartframe;
MObject octreeVizNode::acurrenttime;
MObject	octreeVizNode::aSize;
MObject	octreeVizNode::aHDRName;
MObject octreeVizNode::aworldSpace;
MObject octreeVizNode::aoutput;
MObject octreeVizNode::agrowth;
MObject octreeVizNode::aguide;
MObject octreeVizNode::alengthnoise;
MObject octreeVizNode::asavemap;
MObject octreeVizNode::astep;
MObject octreeVizNode::aalternativepatch;
MObject octreeVizNode::alevel;
MObject octreeVizNode::aarea;
MObject octreeVizNode::acount;
MObject octreeVizNode::adefinepositionX;
MObject octreeVizNode::adefinepositionY;
MObject octreeVizNode::adefinepositionZ;

octreeVizNode::octreeVizNode():m_pTex(0)
{
	m_gridname = "nil";
}

octreeVizNode::~octreeVizNode() 
{
	if(m_pTex) delete m_pTex;
}

MStatus octreeVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput) {
		MString sname = data.inputValue(aHDRName).asString();	
		if(sname != m_gridname)	{
			m_gridname = sname;
			if(m_pTex) delete m_pTex;
			
			m_pTex = new Z3DTexture;
			if(m_pTex->load(m_gridname.asChar())) {
				m_pTex->constructTree();
				m_pTex->computePower();
				zDisplayFloat(m_pTex->getNumGrid());
				zDisplayFloat(m_pTex->getNumVoxel());
				zDisplayFloat(m_pTex->getMaxLevel());
			}
		}
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void octreeVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	glPointSize(3);

	if(m_pTex) {
		XYZ view(0,0,1);
		m_pTex->drawGrid(view);
		m_pTex->draw();
	}
	
	glPopAttrib();
	view.endGL();
}

bool octreeVizNode::isBounded() const
{ 
	return false;
}

MBoundingBox octreeVizNode::boundingBox() const
{ 
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* octreeVizNode::creator()
{
	return new octreeVizNode();
}

MStatus octreeVizNode::initialize()
{ 
	MStatus			 status;
	MFnNumericAttribute numAttr;
    MFnTypedAttribute tAttr;

	alevel = numAttr.create("alevel","alr",MFnNumericData::kShort);
	numAttr.setDefault( 4 ); 
	numAttr.setMin( 0 );
	numAttr.setMax( 10 );
	numAttr.setKeyable( true ); 
    numAttr.setReadable( true ); 
    numAttr.setWritable( true ); 
    numAttr.setStorable( true ); 
	status = addAttribute(alevel);
	CHECK_MSTATUS( status );

    aarea = numAttr.create("aarea","aar",MFnNumericData::kFloat);
	numAttr.setDefault( 10.0 ); 
	numAttr.setKeyable( true ); 
    numAttr.setReadable( true ); 
    numAttr.setWritable( true ); 
    numAttr.setStorable( true ); 
	status = addAttribute(aarea);
	CHECK_MSTATUS( status );

	acount = numAttr.create("acount","aco",MFnNumericData::kInt);
	numAttr.setDefault( 100 ); 
	numAttr.setKeyable( true ); 
    numAttr.setReadable( true ); 
    numAttr.setWritable( true ); 
    numAttr.setStorable( true ); 
	status = addAttribute(acount);
	CHECK_MSTATUS( status );

	adefinepositionX = numAttr.create("adefinepositionX","poX",MFnNumericData::kFloat);
	numAttr.setDefault( 0.0 ); 
	numAttr.setKeyable( true ); 
    numAttr.setReadable( true ); 
    numAttr.setWritable( true ); 
    numAttr.setStorable( true ); 
	status = addAttribute(adefinepositionX);
	CHECK_MSTATUS( status );

	adefinepositionY = numAttr.create("adefinepositionY","poY",MFnNumericData::kFloat);
	numAttr.setDefault( 0.0 ); 
	numAttr.setKeyable( true ); 
    numAttr.setReadable( true ); 
    numAttr.setWritable( true ); 
    numAttr.setStorable( true ); 
	status = addAttribute(adefinepositionY);
	CHECK_MSTATUS( status );

	adefinepositionZ= numAttr.create("adefinepositionZ","poZ",MFnNumericData::kFloat);
	numAttr.setDefault( 0.0 ); 
	numAttr.setKeyable( true ); 
    numAttr.setReadable( true ); 
    numAttr.setWritable( true ); 
    numAttr.setStorable( true ); 
	status = addAttribute(adefinepositionZ);
	CHECK_MSTATUS( status );


	afuzz = numAttr.create( "fuzz", "fuzz", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(afuzz);
	
	alengthnoise = numAttr.create("kink", "kink",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(alengthnoise);
	
	aSize = numAttr.create("clumping", "clp",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
    CHECK_MSTATUS( numAttr.setDefault(0.f));
	numAttr.setCached( true );
	addAttribute(aSize);
	
	astep = numAttr.create( "drawStep", "dsp", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(astep);
	
	aHDRName = tAttr.create("cachePath", "cp",
	MFnData::kString, MObject::kNullObj, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( tAttr.setStorable(true));
    CHECK_MSTATUS( tAttr.setKeyable(false));
	tAttr.setCached( true );
	
	zWorks::createMatrixAttr(aworldSpace, "worldSpace", "ws");
	
	aoutput = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutput );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedArrayAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kMesh);
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	astartframe = numAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( astartframe );
	
	zWorks::createTimeAttr(acurrenttime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(addAttribute(acurrenttime), "ERROR adding time");
	
	asavemap = numAttr.create( "saveMap", "sm", MFnNumericData::kInt, 0);
	numAttr.setStorable(false);
	numAttr.setKeyable(true);
	addAttribute(asavemap);
	
	aalternativepatch = numAttr.create( "patchOrder", "pod", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(aalternativepatch);
	
	CHECK_MSTATUS( addAttribute(aHDRName));
	addAttribute(aworldSpace);
	attributeAffects( alengthnoise, aoutput );
	attributeAffects( aSize, aoutput );
	attributeAffects( aworldSpace, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	attributeAffects( astartframe, aoutput );
	attributeAffects( acurrenttime, aoutput );
	attributeAffects( asavemap, aoutput );
	attributeAffects( afuzz, aoutput );
	attributeAffects( astep, aoutput );
	attributeAffects( aalternativepatch, aoutput );
	attributeAffects( alevel, aoutput );
	attributeAffects( aarea, aoutput );
	attributeAffects( acount, aoutput );
	attributeAffects( adefinepositionX, aoutput );
	attributeAffects( adefinepositionY, aoutput );
	attributeAffects( adefinepositionZ, aoutput );
	attributeAffects( aHDRName, aoutput );
	
	return MS::kSuccess;
}
//:~
