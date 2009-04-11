/*
 *  pluginMain.cpp
 *  hwHDREnvShader
 *
 *  2.4.5 - 04.18.08
 *  Generalize sph2vec(), pointing zenith to -Z axis
 *
 */

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "hdrEnvNode.h"

// The initializePlugin method is called by Maya when the
// plugin is loaded. It registers the hwHDREnvShader node
// which provides Maya with the creator and initialize methods to be
// called when a hwHDREnvShader is created.
//
MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang", "2.6.1 - 04/11/09", "Any");

	status = plugin.registerNode( "hwHDREnvViewer", hdrEnvNode::id, 
						 &hdrEnvNode::creator, &hdrEnvNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	MGlobal::executeCommand("source hdrviewerMenus.mel; hdrviewerCreateMenus;source slimcontrolMenus; slimcontrolCreateMenus();");

	return status;
}

// The unitializePlugin is called when Maya needs to unload the plugin.
// It basically does the opposite of initialize by calling
// the deregisterNode to remove it.
//
MStatus uninitializePlugin( MObject obj )
{
	MStatus   status;
	MFnPlugin plugin( obj );
	
	status = plugin.deregisterNode( hdrEnvNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::executeCommand("hdrviewerRemoveMenus;slimcontrolRemoveMenus;");
	return status;
}

