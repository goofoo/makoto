#include "noiseVolumeNode.h"
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang Jian", "1.5.1 05/14/09", "Any");

	status = plugin.registerNode( "noiseVolumeViz", noiseVolumeNode::id, 
						 &noiseVolumeNode::creator, &noiseVolumeNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	MGlobal::executeCommand("source noiseVolumeMenus.mel; noiseVolumeVizCreateMenus;");

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( noiseVolumeNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::executeCommand("noiseVolumeVizRemoveMenus;");
	return status;
}
