//
// Copyright (C) YiLi
// 
// File: guideCmd.cpp
//
// MEL Command: guide
//
// Author: Maya Plug-in Wizard 2.0
//

#include "guideCommand.h"

#include <maya/MGlobal.h>
#include <math.h>

guide::guide():num_seg(5),seglength(1.f)
//
//	Description:
//		guide constructor
//
{
}

guide::~guide()
//
//	Description:
//		guide destructor
//
{
}

MSyntax guide::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag("-ns", "-numSegment" ,MSyntax::kLong);
	syntax.addFlag("-sl", "-segmentLength",MSyntax::kDouble );

	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus guide::doIt( const MArgList& args)
//
//	Description:
//		implements the MEL guide command.
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
	MStatus status ;
	MArgDatabase argData(syntax(), args);
    
	if (argData.isFlagSet("-ns")) argData.getFlagArgument("-ns", 0, num_seg );
	if (argData.isFlagSet("-sl")) argData.getFlagArgument("-sl", 0, seglength );
	
    MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	
	MItSelectionList list( slist, MFn::kComponent, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return status;
	}

	MObject component;

	if (list.isDone()) {
		displayError( "No components selected" );
		return MS::kFailure;
	}
	pointArray.clear();
	normalArray.clear();
	tangentArray.clear();

	list.getDagPath (fDagPath, component);
	//switch(component.apiType())
	//{
		//case MFn::kMeshVertComponent:
		//{
	        MIntArray vertlist;
			MIntArray facelist;
			int index;
			MItMeshVertex vertexIter( fDagPath, component );
            MFnMesh meshFn( fDagPath );
			vertexIter.reset();
			if(vertexIter.isDone())
				MGlobal::displayError("No vertex or faceVertex selected");
			else
			for( int i = 0;!vertexIter.isDone();vertexIter.next() ,i++)
			{
				pointArray.append(vertexIter.position(MSpace::kWorld ));
				vertexIter.getConnectedFaces(facelist);
				index = vertexIter.index();
				meshFn.getFaceVertexTangent(facelist[0],index,tang,MSpace::kObject);
				
				vertexIter.getNormal(nor,MSpace::kObject);
				normalArray.append(nor);	
				tang = nor^tang;
				tang = tang^nor;
				tangentArray.append( tang );
			}
			//break;
		//}
	//}
 return redoIt();
}

MStatus guide::redoIt()
//
//	Description:
//		implements redo for the MEL guide command. 
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
	MStatus status;
	MFnMesh meshFn;
	
    MPointArray vertexArray;
	MIntArray polygonCounts;
    MIntArray polygonConnects;
	MFloatArray uarray, varray;
	vertexArray.clear();
	polygonCounts.clear();
	polygonConnects.clear();

	unsigned acc=0;
	for(unsigned i = 0;i < pointArray.length();i++)
	{
		
		for(unsigned j = 0;j < num_seg;j++)
		{
			polygonCounts.append(4);
			
			polygonConnects.append(acc+2*j);
			polygonConnects.append(acc+2*j+2);
			polygonConnects.append(acc+2*j+3);
			polygonConnects.append(acc+2*j+1);
		}
		
		acc += 2*(num_seg+1);
		
		for(unsigned j = 0;j <= num_seg;j++)
		{
			MPoint vert;
			vert = pointArray[i] - tangentArray[i]*0.5*seglength + normalArray[i]*seglength*j;
			vertexArray.append(vert);
			vert = pointArray[i] + tangentArray[i]*0.5*seglength + normalArray[i]*seglength*j;
			vertexArray.append(vert);
			uarray.append(0);
			uarray.append(1.f/(float)num_seg);
			varray.append(1.f/(float)num_seg*j);
			varray.append(1.f/(float)num_seg*j);
		}
	}
	MObject newMesh = meshFn.create((2*num_seg+2)*pointArray.length(), num_seg*pointArray.length(),vertexArray,polygonCounts,polygonConnects,MObject::kNullObj );
	meshFn.setUVs ( uarray, varray );
	meshFn.assignUVs ( polygonCounts, polygonConnects );
	return MS::kSuccess;	
}



void* guide::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new guide();
}




