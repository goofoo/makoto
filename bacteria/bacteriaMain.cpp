#include "bacteriaNode.h"
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang Jian", "1.5.1 05/14/09", "Any");

	status = plugin.registerNode( "bacteriaViz", bacteriaNode::id, 
						 &bacteriaNode::creator, &bacteriaNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	MGlobal::executeCommand("source bacteriaMenus.mel; bacteriaVizCreateMenus;");

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( bacteriaNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::executeCommand("bacteriaVizRemoveMenus;");
	return status;
}
