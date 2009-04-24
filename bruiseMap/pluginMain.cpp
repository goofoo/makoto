/*
 *  pluginMain.cpp
 *  hwbruiseMapShader
 *
 *  2.4.5 - 04.18.08
 *  Generalize sph2vec(), pointing zenith to -Z axis
 *
 */

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "bruiseMapNode.h"

// The initializePlugin method is called by Maya when the
// plugin is loaded. It registers the hwbruiseMapShader node
// which provides Maya with the creator and initialize methods to be
// called when a hwbruiseMapShader is created.
//
MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang", "0.1.1 - 04/25/09", "Any");

	status = plugin.registerNode( "bruiseMapViz", bruiseMapNode::id, 
						 &bruiseMapNode::creator, &bruiseMapNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	MGlobal::executeCommand("source bruisemapMenus.mel; bruisemapCreateMenus;");

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
	
	status = plugin.deregisterNode( bruiseMapNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::executeCommand("bruisemapRemoveMenus;");
	return status;
}

