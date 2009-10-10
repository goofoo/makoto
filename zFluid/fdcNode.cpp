#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"
#include "fdcNode.h"
#include "../shared/fdcFile.h"

MTypeId     fluidCacheNode::id( 0x00027732 );
MObject     fluidCacheNode::atime;        
MObject     fluidCacheNode::output; 
MObject fluidCacheNode::ainposition;
MObject fluidCacheNode::ainvelocity;
MObject fluidCacheNode::ainage;
MObject fluidCacheNode::ainmass;
MObject fluidCacheNode::acachename;      

fluidCacheNode::fluidCacheNode() {}
fluidCacheNode::~fluidCacheNode() {}

MStatus fluidCacheNode::compute( const MPlug& plug, MDataBlock& data )
{
	
	MStatus status;
	MTime currentTime = data.inputValue(atime, &status).asTime();

	if( plug == output )
	{
		MString cacheName = data.inputValue(acachename, &status).asString();
		
		cacheName = cacheName+"."+int(currentTime.value())+".fdc";
		
		MVectorArray positions = zWorks::getVectorArrayAttr(data, ainposition);
		MVectorArray velocitis = zWorks::getVectorArrayAttr(data, ainvelocity);
		MDoubleArray ages = zWorks::getDoubleArrayAttr(data, ainage);
		MDoubleArray masses = zWorks::getDoubleArrayAttr(data, ainmass);
		
		if(cacheName != "") 
		{
			if(fdcFile::load(cacheName.asChar(), positions, velocitis, ages)==1)
				MGlobal::displayInfo(MString("ZFluid loaded cache: ")+cacheName);
			else
				MGlobal::displayWarning(MString("ZFluid failed to load cache: ")+cacheName);
		}
		
		int num_ptc = positions.length();
		masses.clear();
		for(int i=0; i<num_ptc; i++) masses.append(1.0);
		
		if(currentTime.value()!=int(currentTime.value())) 
		{
			float delta_t = currentTime.value()-int(currentTime.value());
			
			
			for(int i=0; i<num_ptc; i++)
			{
				positions[i] += velocitis[i]*delta_t/24.0f;
			}
		}
		
		MObject counto;
		zWorks::getConnectedNode(counto, MPlug(thisMObject(), ainmass));
		MFnDependencyNode(counto).findPlug("count").setValue(num_ptc);
		//MDataHandle outputHandle = data.outputValue( fluidCacheNode::output );
		//outputHandle.set( 0.0 );
		data.setClean(plug);

	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* fluidCacheNode::creator()
{
	return new fluidCacheNode();
}

MStatus fluidCacheNode::initialize()
{
	MFnNumericAttribute nAttr;
	MStatus				status;
	
	zCheckStatus(zWorks::createTypedAttr(ainposition, MString("position"), MString("position"), MFnData::kVectorArray), "ERROR creating positions");
	zCheckStatus(addAttribute(ainposition), "ERROR adding positions");
	
	zCheckStatus(zWorks::createTypedAttr(ainvelocity, MString("velocity"), MString("velocity"), MFnData::kVectorArray), "ERROR creating velocity");
	zCheckStatus(addAttribute(ainvelocity), "ERROR adding velocity");
	
	zCheckStatus(zWorks::createTypedAttr(ainage, MString("age"), MString("age"), MFnData::kDoubleArray), "ERROR creating age");
	zCheckStatus(addAttribute(ainage), "ERROR adding age");
	
	zCheckStatus(zWorks::createTypedAttr(ainmass, MString("mass"), MString("mass"), MFnData::kDoubleArray), "ERROR creating mass");
	zCheckStatus(addAttribute(ainmass), "ERROR adding mass");
	
	status = zCreateTypedAttr(acachename, "cacheName", "cchn", MFnData::kString);
	zCheckStatus(status, "ERROR creating cache name");
	zCheckStatus(addAttribute(acachename), "ERROR adding cache name");

	status = zCreateTimeAttr(atime, "currentTime", MString("ct"), 1.0);
	zCheckStatus(status, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");

	output = nAttr.create( "output", "out", MFnNumericData::kFloat, 0.0 );
	nAttr.setWritable(false);
	nAttr.setStorable(false);

	zCheckStatus(addAttribute( output ), "ERROR adding out");

	zCheckStatus(attributeAffects( atime, output ), "ERROR adding affect");

	return MS::kSuccess;
}


