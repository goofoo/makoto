/*
 *  pluginMain.cpp
 *  
 *
 *  Created by zhang on 08-3-31.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include "particleWrapNode.h"
 
MStatus initializePlugin( MObject obj ) 
{
MStatus status;
MFnPlugin plugin( obj, "ZHANG JIAN", "build 0.0.1 23/08/09", "Any" );
	
	status = plugin.registerNode( "particleWrap", particleWrapNode::id, 
						 &particleWrapNode::creator, &particleWrapNode::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	MGlobal::executeCommand ( "source particleWrapMenus.mel; ptcwrapCreateMenus;" );
 
 return MS::kSuccess;
 }
 
 MStatus uninitializePlugin( MObject obj ) 
 {
MStatus status;
MFnPlugin plugin( obj );
	
	status = plugin.deregisterNode( particleWrapNode::id );
	if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
		
	MGlobal::executeCommand ( "ptcwrapRemoveMenus;" );

 return MS::kSuccess;
 }


