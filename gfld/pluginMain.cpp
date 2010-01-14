#include "fluidField.h"
#include "fluidViz.h"
#include "fluidDescData.h"
#include "boomEmitter.h"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>


MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang Jian", "1.7.2 Wed Jan 15 2010", "Any");
	
	status = plugin.registerData("fluidDescData", zjFluidDescData::id,
								 zjFluidDescData::creator);
	if (!status) {
		MGlobal::displayWarning("ERROR registering fluid Desc data.");
		return status;
	}

	status = plugin.registerNode( "fluidField", fluidField::id,
							fluidField::creator, fluidField::initialize,
							MPxNode::kFieldNode
							);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "fluidViz", fluidViz::id, 
						 &fluidViz::creator, &fluidViz::initialize,
						 MPxNode::kLocatorNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	status = plugin.registerNode( "jetEmitter", boomEmitter::id,
							&boomEmitter::creator, &boomEmitter::initialize,
							MPxNode::kEmitterNode );
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	
	MGlobal::executeCommand ( "source zFluidMenus.mel; zfluidCreateMenus;" );

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( fluidField::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterNode( fluidViz::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	status = plugin.deregisterData(zjFluidDescData::id);
	if (!status) {
		MGlobal::displayWarning("ERROR deregistering fluid desc data.");
		return status;
	}
	
	status = plugin.deregisterNode( boomEmitter::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
		
	MGlobal::executeCommand ( "zfluidRemoveMenus;" );
	return status;
}

