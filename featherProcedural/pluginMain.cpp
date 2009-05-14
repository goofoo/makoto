/*
 *  pluginMain.cpp
 *  
 *
 *  Created by zhang on 08-12-10.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include "featherVizNode.h"
#include "wingVizNode.h"
 
MStatus initializePlugin( MObject obj ) 
{
	MStatus status;

	MFnPlugin plugin( obj, "ZHANG JIAN", "build 0.5.6 03/17/09", "Any" );

	status = plugin.registerNode( "featherViz", featherVizNode::id, 
						 &featherVizNode::creator, &featherVizNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "wingViz", wingVizNode::id, 
						 &wingVizNode::creator, &wingVizNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
		
	MGlobal::executeCommand ( "source featherMenus.mel;featherCreateMenus;" );
 
	return MS::kSuccess;
}
 
MStatus uninitializePlugin( MObject obj ) 
{
	MStatus status;
	MFnPlugin plugin( obj );
	
	status = plugin.deregisterNode( featherVizNode::id );
	if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}

	status = plugin.deregisterNode( wingVizNode::id );
	if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
		
	MGlobal::executeCommand ( "featherRemoveMenus;" );

 return MS::kSuccess;
}


