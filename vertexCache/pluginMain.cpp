/*
 *  pluginMain.cpp
 *  
 *
 *  Created by zhang on 08-3-31.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MFnPlugin.h>
#include "obj2mcf.h"
#include "meshCacheNode.h"
#include "meshCacheCmd.h"
#include "vxCacheDeformer.h"
#include "xmlMeshCacheCmd.h"
#include "meshCacheVizNode.h"
#include "prtAttribNode.h"
#include "importSceneCmd.h"
#include "cacheTransform.h"
 
MStatus initializePlugin( MObject obj ) 
{
MStatus status;
MFnPlugin plugin( obj, "ZHANG JIAN", "build 3.5.1 06/05/09", "Any" );

	//if( licenseIsValid ( "C:/flexlm/shot_sculptor.lic" ) ) {
		status = plugin.registerCommand( "vxCache", vxCache::creator, vxCache::newSyntax);
		if (!status) {
			status.perror("registerCommand");
			return status;
		}
	//}
	status = plugin.registerCommand( "obj2mcf", obj2mcf::creator, obj2mcf::newSyntax);
		if (!status) {
			status.perror("registerCommand");
			return status;
	}

	status = plugin.registerNode( "meshCacheNode", meshCacheNode::id, meshCacheNode::creator,
								  meshCacheNode::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "vxCacheDeformer", vxCacheDeformer::id, vxCacheDeformer::creator, 
								  vxCacheDeformer::initialize, MPxNode::kDeformerNode );
	if (!status) {
	      status.perror("deregisterCommand");
	      return status;
	}
	
		status = plugin.registerCommand( "xmlMeshCache", xmlMeshCache::creator, xmlMeshCache::newSyntax);
		if (!status) {
			status.perror("registerCommand");
			return status;
		}
		
			status = plugin.registerNode( "meshCacheViz", meshCacheVizNode::id, 
						 &meshCacheVizNode::creator, &meshCacheVizNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "prtAttrib", prtAttribNode::id, prtAttribNode::creator,
								  prtAttribNode::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerCommand( "importSceneCache", XMLSceneCmd::creator, XMLSceneCmd::newSyntax);
		if (!status) {
			status.perror("registerCommand");
			return status;
		}
		
	status = plugin.registerTransform(	"cacheTransform", 
										rockingTransformNode::id, 
						 				&rockingTransformNode::creator, 
										&rockingTransformNode::initialize,
						 				&rockingTransformMatrix::creator,
										rockingTransformMatrix::id);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	MGlobal::executeCommand ( "source vertexCacheMenus.mel;meshCacheMakeMenus;" );
 
 return MS::kSuccess;
 }
 
 MStatus uninitializePlugin( MObject obj ) 
 {
MStatus status;
MFnPlugin plugin( obj );

	//if( licenseIsValid ( "C:/flexlm/shot_sculptor.lic" ) ) {
		status = plugin.deregisterCommand( "vxCache" );
		if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
		
		status = plugin.deregisterCommand( "obj2mcf" );
		if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
		
		status = plugin.deregisterNode( vxCacheDeformer::id );
		if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
	
	//}
	
		status = plugin.deregisterCommand( "xmlMeshCache" );
		if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}

status = plugin.deregisterNode( meshCacheNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterNode( meshCacheVizNode::id );
	if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
		
	status = plugin.deregisterNode( prtAttribNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
			status = plugin.deregisterCommand( "importSceneCache" );
		if (!status) {
		      status.perror("deregisterCommand");
		      return status;
		}
	
	status = plugin.deregisterNode( rockingTransformNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	MGlobal::executeCommand ( "meshCacheRemoveMenus;" );

 return MS::kSuccess;
 }


