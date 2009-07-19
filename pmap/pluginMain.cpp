//
// Copyright (C) YiLi
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//

#include "pMapCmd.h"
#include "pMapLocator.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "YiLi", "0.1.2 7.20.09", "Any");

	status = plugin.registerCommand( "pMapCmd", pMapCmd::creator,pMapCmd::newSyntax );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}
	status = plugin.registerNode( "pMapViz", pMapLocator::id, 
                                   &pMapLocator::creator, &pMapLocator::initialize,
                                                 MPxNode::kLocatorNode );
   if (!status) {
		status.perror("registerCommand");
		return status;
	}

   MGlobal::executeCommand ( "source particleCacheMenus.mel;particleCacheMakeMenus;" );

	return status;
}

MStatus uninitializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand( "pMapCmd" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}

	status = plugin.deregisterNode( pMapLocator::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
    MGlobal::executeCommand ( "particleCacheRemoveMenus;" );

	return status;
}
