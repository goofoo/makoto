//
// Copyright (C) YiLi
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
#include <maya/MFnPlugin.h>
#include "PTCMapCmd.h"
#include "PTCMapViz.h"
#include "PTCDemViz.h"

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
	MFnPlugin plugin( obj, "ZHANG JIAN", "0.3.7 Sat Oct 17 2009", "Any");

	status = plugin.registerCommand( "pmapCache", PTCMapCmd::creator, PTCMapCmd::newSyntax );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}
	status = plugin.registerNode( "pmapViz", PTCMapLocator::id, 
                                   &PTCMapLocator::creator, &PTCMapLocator::initialize,
                                                 MPxNode::kLocatorNode );
   if (!status) {
		status.perror("registerNode");
		return status;
	}
	status = plugin.registerNode( "pmapDem", PTCDemViz::id, 
                                   &PTCDemViz::creator, &PTCDemViz::initialize,
                                                 MPxNode::kLocatorNode );
   if (!status) {
		status.perror("registerNode");
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

	status = plugin.deregisterCommand( "pmapCache" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}

	status = plugin.deregisterNode( PTCMapLocator::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterNode( PTCDemViz::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
    MGlobal::executeCommand ( "particleCacheRemoveMenus;" );

	return status;
}
