#include "prtAttribNode.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

MTypeId     prtAttribNode::id( 0x00025257 );
MObject		prtAttribNode::askipidir;
MObject		prtAttribNode::askipscat;
MObject		prtAttribNode::askipepid;
MObject		prtAttribNode::aaslight;    
MObject		prtAttribNode::aklight; 
MObject		prtAttribNode::anoshadow;
MObject		prtAttribNode::aasghost;   
MObject     prtAttribNode::output;       

prtAttribNode::prtAttribNode() {}
prtAttribNode::~prtAttribNode() {}

MStatus prtAttribNode::compute( const MPlug& plug, MDataBlock& data )
{
	
	MStatus returnStatus;
 
	if( plug == output )
	{
		float result = 1.0f;
		MDataHandle outputHandle = data.outputValue( prtAttribNode::output );
		outputHandle.set( result );
		data.setClean(plug);
		
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* prtAttribNode::creator()
{
	return new prtAttribNode();
}

MStatus prtAttribNode::initialize()
{
	MFnNumericAttribute numAttr;
	MStatus				stat;
	
	askipidir = numAttr.create( "skipIndirect", "skind", MFnNumericData::kBoolean, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( askipidir );
	
	askipepid = numAttr.create( "skipScatter", "skscat", MFnNumericData::kBoolean, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( askipepid );
	
	askipscat = numAttr.create( "skipBackscatter", "skbscat", MFnNumericData::kBoolean, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( askipscat );
	
	aaslight = numAttr.create( "asLightsource", "asl", MFnNumericData::kBoolean, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aaslight );
	
	aklight = numAttr.create( "lightIntensity", "li", MFnNumericData::kDouble, 2.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aklight );
	
	anoshadow = numAttr.create( "castNoShadow", "cns", MFnNumericData::kBoolean, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( anoshadow );
	
	aasghost = numAttr.create( "asGhost", "asg", MFnNumericData::kBoolean, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aasghost );

	output = numAttr.create( "output", "out", MFnNumericData::kFloat, 0.0 );
	numAttr.setWritable(false);
	numAttr.setStorable(false);
	stat = addAttribute( output );
		if (!stat) { stat.perror("addAttribute"); return stat;}
		
	attributeAffects( askipidir, output );
	attributeAffects( askipepid, output );
	attributeAffects( askipscat, output );
	attributeAffects( aaslight, output );
	attributeAffects( aklight, output );
	attributeAffects( aasghost, output );
	attributeAffects( anoshadow, output );

	return MS::kSuccess;
}
