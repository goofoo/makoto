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
#include "zHairNode.h"
#include "FeatherNode.h"
#include "HairDguideNode.h"
#include "guideCommand.h"
#include "curvePatchNode.h"

// The initializePlugin method is called by Maya when the
// plugin is loaded. It registers the hwbruiseMapShader node
// which provides Maya with the creator and initialize methods to be
// called when a hwbruiseMapShader is created.
//
MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang", "0.3.3 - 06/03/09", "Any");

	status = plugin.registerNode( "ZHairViz", HairNode::id, 
						 &HairNode::creator, &HairNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = plugin.registerNode( "ZHairCache", HairDguideNode::id, &HairDguideNode::creator,
								  &HairDguideNode::initialize,MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerCommand( "ZHairGuider", guide::creator ,
		                             guide::newSyntax);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}
	
	status = plugin.registerNode( "ZFeatherViz", FeatherNode::id, 
						 &FeatherNode::creator, &FeatherNode::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "Curve2Patch", curvePatchNode::id, 
						 &curvePatchNode::creator, &curvePatchNode::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	MGlobal::executeCommand("source zhairMenus.mel; zhairCreateMenus;");

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
	
	status = plugin.deregisterNode( HairNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	status = plugin.deregisterNode( HairDguideNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterCommand( "ZHairGuider" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
	
	status = plugin.deregisterNode( FeatherNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterNode( curvePatchNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::executeCommand("zhairRemoveMenus;");

	return status;
}

