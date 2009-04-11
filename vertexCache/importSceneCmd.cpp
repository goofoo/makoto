/// :
// meshCacheCmd.cpp
// Version 1.2.8
// Mesh cache file writer
// updated: 05/07/08
//
#include "../shared/zWorks.h"
#include "importSceneCmd.h"
#include <maya/MAnimControl.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include "../shared/zData.h"
#include "../shared/ZXMLDoc.h"
////////////////////////////////////////
//
// cache file generation Command Class
//
////////////////////////////////////////
 
  
XMLSceneCmd::XMLSceneCmd() {}

XMLSceneCmd::~XMLSceneCmd() {}

MSyntax XMLSceneCmd::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-m", "-mesh", MSyntax::kBoolean);
	syntax.addFlag("-t", "-transform", MSyntax::kBoolean);
	
	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus XMLSceneCmd::doIt( const MArgList& args ) 
{
	MStatus status = parseArgs( args );
	
	if( status != MS::kSuccess ) return status;
	
	MArgDatabase argData(syntax(), args);
	
	MAnimControl timeControl;
	MTime time = timeControl.currentTime();

	MString proj;
	MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), proj );
	
	MSelectionList selList;
	MGlobal::getActiveSelectionList ( selList );
	MItSelectionList iter( selList );

	MString cache_path = proj + "/data/";
	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	
	ZXMLDoc doc;
	if(doc.load(cache_path.asChar()) != 1) 
	{
		MGlobal::displayError(MString("Cannot open ")+cache_path);
		return MS::kSuccess;
	}
	
	doc.setChildren();
	
	if(argData.isFlagSet("-m"))
	{
		while(doc.isLastNode() != 1)
		{
			if(doc.checkNodeName("mesh") == 1)
			{
				appendToResult(doc.getAttribByName("name"));
			}
			doc.nextNode();
		}
	}
	
	if(argData.isFlagSet("-t"))
	{
		while(doc.isLastNode() != 1)
		{
			if(doc.checkNodeName("camera") == 1 || doc.checkNodeName("transform") == 1 )
			{
				appendToResult(doc.getAttribByName("name"));
			}
			doc.nextNode();
		}
	}
	
	doc.setParent();
	doc.free();

 return MS::kSuccess;
 }

void* XMLSceneCmd::creator() {
 return new XMLSceneCmd;
 }
 
MStatus XMLSceneCmd::parseArgs( const MArgList& args )
{
	// Parse the arguments.
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}
//:~