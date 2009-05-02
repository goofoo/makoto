#include "fishVizNode.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "CVFX | Zhang Jian", "1.5.1 02/05/09", "Any");

	status = plugin.registerNode( "fishVizNode", fishVizNode::id, 
						 &fishVizNode::creator, &fishVizNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

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

	return status;
}
