/*
 *  pluginMain.cpp
 *
 *  2.4.5 - 06.05.08
 *
 */

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "depthMapCmd.h"

// The initializePlugin method is called by Maya when the
// plugin is loaded. It registers the hwbruiseMapShader node
// which provides Maya with the creator and initialize methods to be
// called when a hwbruiseMapShader is created.
//
MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang", "0.0.1 - 25/08/09", "Any");
	
	status = plugin.registerCommand( "EXRZDepth", depthMap::creator ,
		                             depthMap::newSyntax);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	//MGlobal::executeCommand("source zhairMenus.mel; zhairCreateMenus;");

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
	
	status = plugin.deregisterCommand( "EXRZDepth" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
	
	//MGlobal::executeCommand("zhairRemoveMenus;");

	return status;
}
