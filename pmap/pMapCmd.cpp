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

MStatus pMapCmd::doIt( const MArgList& )
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
	MStatus status;
    MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList list( slist, MFn::kParticle, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return status;
	}

	MObject component;

	if (list.isDone()) {
		displayError( "No components selected" );
		return MS::kFailure;
	}
    
	unsigned int sum = 0;
    ofstream outfile;
	outfile.open("C:/Temp/pMapCmd.dat",ios_base::out | ios_base::binary );
	if(!outfile.is_open())
	{
		MGlobal::displayWarning(MString("Cannot open file:  C:/Temp/pMapCmd.dat"));
		return MS::kFailure;
	}
	for(;!list.isDone();list.next())
	{
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
		sum += ps.count();
	}
	outfile.write((char*)&sum,sizeof(int));
	outfile.close();
	list.reset();

	pointArray.clear();
	for(;!list.isDone();list.next()){
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
 
		const unsigned int count = ps.count();
	    MIntArray ids;
	    ps.particleIds( ids );
	    MVectorArray positions;
	    ps.position( positions );
	    assert( positions.length() == count );

		outfile.open("C:/Temp/pMapCmd.dat",ios_base::out | ios_base::binary |ios_base::app );
	    if(!outfile.is_open())
		{
			MGlobal::displayWarning(MString("Cannot open file:  C:/Temp/pMapCmd.dat"));
		    return MS::kFailure;
	    }
		
		//outfile.write((char*)&count,sizeof(int));
		for(unsigned i=0; i<positions.length(); i++ )
		{
			MVector p = positions[i];
			outfile.write((char*)&p[0],sizeof(p[0]));
			outfile.write((char*)&p[1],sizeof(p[1]));
			outfile.write((char*)&p[2],sizeof(p[2]));
		}
		outfile.close();
	}
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
{}

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
