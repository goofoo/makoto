//
// Copyright (C) YiLi
// 
// File: pMapCmd.cpp
//
// MEL Command: pMapCmd
//
// Author: Maya Plug-in Wizard 2.0
//

#include "pMapCmd.h"
#include <assert.h>
#include <maya/MGlobal.h>
#include <string>

using namespace std;


MSyntax pMapCmd::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-n", "-name", MSyntax::kString);
	syntax.addFlag("-w", "-world", MSyntax::kBoolean);
	syntax.addFlag("-sg", "-single", MSyntax::kBoolean);
	
	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus pMapCmd::doIt( const MArgList& args)
//
//	Description:
//		implements the MEL pMapCmd command.
//
//	Arguments:
//		args - the argument list that was passes to the command from MEL
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - command failed (returning this value will cause the 
//                     MEL script that is being run to terminate unless the
//                     error is caught using a "catch" statement.
//
{
	// Typically, the doIt() method only collects the infomation required
	// to do/undo the action and then stores it in class members.  The 
	// redo method is then called to do the actuall work.  This prevents
	// code duplication.
	//

	MStatus status = parseArgs( args );
	
	if( status != MS::kSuccess ) return status;
	
	MArgDatabase argData(syntax(), args);
	
	MAnimControl timeControl;
	MTime time = timeControl.currentTime();
	int frame =int(time.value());
	MString proj;
	MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), proj );

    MString cache_path = proj + "/data";
	MString cache_name = "particleposition";
	worldSpace = false;
	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);
	if (argData.isFlagSet("-w")) argData.getFlagArgument("-w", 0, worldSpace);

    MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList list( slist, MFn::kParticle, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return status;
	}
    char filename[512];
	if(argData.isFlagSet("-sg")) sprintf( filename, "%s/%s.dat", cache_path.asChar(), cache_name.asChar() );
		else sprintf( filename, "%s/%s.%d.dat", cache_path.asChar(), cache_name.asChar(), frame );
	MObject component;

	if (list.isDone()) {
		displayError( "No components selected" );
		return MS::kFailure;
	}

	unsigned sum = 0,isum = 0;
	for(;!list.isDone();list.next())
	{
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
		sum += ps.count();
	}
	PosAndId *buf = new PosAndId[sum];
	//XYZ *color = new XYZ[sum];
	list.reset();
	for(;!list.isDone();list.next())
	{
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
 
		const unsigned int count = ps.count();
	    MIntArray ids;
	    ps.particleIds( ids );
	    MVectorArray positions;
		MVectorArray color;
	    ps.position( positions );
		if(ps.hasRgb())
			ps.rgb(color);
	    assert( positions.length() == count );
		for(unsigned i=0; i<positions.length(); i++,isum++ )
		{
			MVector p = positions[i];
			buf[isum].pos.x = p[0];
			buf[isum].pos.y = p[1];
			buf[isum].pos.z = p[2];
			if(ps.hasRgb()){
				MVector c = color[i];
				buf[isum].co.x = c[0];
			    buf[isum].co.y = c[1];
			    buf[isum].co.z = c[2];
			}
			else {
				buf[isum].co.x = 1.;
				buf[isum].co.y = 0.;
				buf[isum].co.z = 0.;
			}
			buf[isum].idx = isum;
		}
	}

	XYZ rootCenter;
	float rootSize;
    OcTree::getBBox(buf, sum, rootCenter, rootSize);
	mindist = 0.5;
	short maxlevel = 0;
	while(mindist<rootSize)
	{
		maxlevel++;
		mindist*=2;
	}
	OcTree* tree = new OcTree();
	tree->construct(buf, sum, rootCenter, rootSize, maxlevel);
	tree->saveFile(filename,tree,sum);
	delete[] buf;
	delete tree;
	return MS::kSuccess;
}

MStatus pMapCmd::redoIt()
//
//	Description:
//		implements redo for the MEL pMapCmd command. 
//
//		This method is called when the user has undone a command of this type
//		and then redoes it.  No arguments are passed in as all of the necessary
//		information is cached by the doIt method.
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - redoIt failed.  this is a serious problem that will
//                     likely cause the undo queue to be purged
//
{
	// Since this class is derived off of MPxCommand, you can use the 
	// inherited methods to return values and set error messages
	//
	setResult( "pMapCmd command executed!\n" );

	return MS::kSuccess;
}

MStatus pMapCmd::undoIt()
//
//	Description:
//		implements undo for the MEL pMapCmd command.  
//
//		This method is called to undo a previous command of this type.  The 
//		system should be returned to the exact state that it was it previous 
//		to this command being executed.  That includes the selection state.
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - redoIt failed.  this is a serious problem that will
//                     likely cause the undo queue to be purged
//
{

	// You can also display information to the command window via MGlobal
	//
    MGlobal::displayInfo( "pMapCmd command undone!\n" );

	return MS::kSuccess;
}

void* pMapCmd::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new pMapCmd();
}

pMapCmd::pMapCmd()
//
//	Description:
//		pMapCmd constructor
//
{
}

pMapCmd::~pMapCmd()
//
//	Description:
//		pMapCmd destructor
//
{
}

bool pMapCmd::isUndoable() const
//
//	Description:
//		this method tells Maya this command is undoable.  It is added to the 
//		undo queue if it is.
//
//	Return Value:
//		true if this command is undoable.
//
{
	return true;
}

MStatus pMapCmd::parseArgs( const MArgList& args )
{
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}

