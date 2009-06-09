/*
 *  pluginMain.cpp
 *
 *  2.4.5 - 06.05.08
 *
 */

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "combCurveTool.h"

// The initializePlugin method is called by Maya when the
// plugin is loaded. It registers the hwbruiseMapShader node
// which provides Maya with the creator and initialize methods to be
// called when a hwbruiseMapShader is created.
//
MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Zhang", "0.4.1 - 06/05/09", "Any");
	
	status = plugin.registerContextCommand( "combCurveToolContext",
										    combCurveContextCmd::creator,
											"combCurveTool",
											combCurveTool::creator,
											combCurveTool::newSyntax);

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
	
	status = plugin.deregisterContextCommand( "combCurveToolContext", "combCurveTool");

	//MGlobal::executeCommand("zhairRemoveMenus;");

	return status;
}
