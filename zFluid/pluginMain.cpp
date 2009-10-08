#include "fluidField.h"
#include "fluidViz.h"
#include "fluidDescData.h"
//#include "fdcNode.h"
#include <maya/MPlug.h>
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "CVFX | Zhang Jian", "1.4.0", "Any");
	
	status = plugin.registerData("fluidDescData", zjFluidDescData::id,
								 zjFluidDescData::creator);
	if (!status) {
		MGlobal::displayWarning("ERROR registering fluid Desc data.");
		return status;
	}

	status = plugin.registerNode( "fluidField", fluidField::id,
							fluidField::creator, fluidField::initialize,
							MPxNode::kFieldNode
							);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "fluidViz", fluidViz::id, 
						 &fluidViz::creator, &fluidViz::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
/*
	status = plugin.registerNode( "fluidCacheNode", fluidCacheNode::id, fluidCacheNode::creator,
								  fluidCacheNode::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}	
*/	
	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( fluidField::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterNode( fluidViz::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterData(zjFluidDescData::id);
	if (!status) {
		MGlobal::displayWarning("ERROR deregistering fluid desc data.");
		return status;
	}
/*
	status = plugin.deregisterNode( fluidCacheNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
*/		
	return status;
}

