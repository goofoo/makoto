#include "fishVizNode.h"
#include <maya/MPlug.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>



MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "CVFX | Zhang Jian", "1.4", "Any");

	status = plugin.registerNode( "fishVizNode", fishVizNode::id, 
						 &fishVizNode::creator, &fishVizNode::initialize,
						 MPxNode::kDependNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	/*
	status = plugin.registerNode( "fishCacheNode", fishCacheNode::id, fishCacheNode::creator,
								  fishCacheNode::initialize );
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

	status = plugin.deregisterNode( fishVizNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	/*
	status = plugin.deregisterNode( fishCacheNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	*/
	return status;
}
