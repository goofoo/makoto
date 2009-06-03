//
// Copyright (C) YiLi
// 
// File: guideCmd.cpp
//
// MEL Command: guide
//
// Author: Maya Plug-in Wizard 2.0
//
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnNurbsCurve.h>
#include "guideCommand.h"
#include "../shared/zData.h"
#include <maya/MGlobal.h>
#include <math.h>

guide::guide():num_seg(5),seglength(1.f),num_strand(3),strand_dist(1.0)
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

	syntax.addFlag("-cc", "-createCurve", MSyntax::kBoolean);
	syntax.addFlag("-sc", "-strandCurve", MSyntax::kBoolean);
	syntax.addFlag("-sct", "-strandCount", MSyntax::kLong);
	syntax.addFlag("-sd", "-strandDistance", MSyntax::kDouble);
	syntax.addFlag("-ns", "-numSegment", MSyntax::kLong);
	syntax.addFlag("-sl", "-segmentLength", MSyntax::kDouble );

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
	MArgDatabase argData(syntax(), args);
    
	MPoint point;
	
	if (argData.isFlagSet("-ns")) argData.getFlagArgument("-ns", 0, num_seg );
	if (argData.isFlagSet("-sl")) argData.getFlagArgument("-sl", 0, seglength );
	if(argData.isFlagSet("-sct")) argData.getFlagArgument("-sct", 0, num_strand);
	if(argData.isFlagSet("-sd")) argData.getFlagArgument("-sd", 0, strand_dist);
	
	if(argData.isFlagSet("-sc")) strandCurve();
	if(argData.isFlagSet("-cc")) createCurve();
    			
	return MS::kSuccess;
}
/*
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
*/
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

void guide::strandCurve()
{
	MStatus status;
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList iter( slist );
	unsigned numsel = slist.length();
	
	MObjectArray ocurvelist;
	MObject omesh;
	for (unsigned i=0; !iter.isDone(); iter.next(), i++ )
    {											
        MObject ocurve;		
        iter.getDependNode(ocurve);
		
		if(i != numsel-1) ocurvelist.append(ocurve);
		else omesh =ocurve;
    }
	
	MFnMesh fbase(omesh, &status);
	if(!status)
	{
		MGlobal::displayWarning("no mesh selected, do nothing");
		return;
	}
	
	for(unsigned i=0; i<ocurvelist.length(); i++)
	{
		MFnNurbsCurve fcurve(ocurvelist[i]);
		
		MPointArray cvs;
		fcurve.getCVs ( cvs, MSpace::kObject );
		MDoubleArray knots;
		fcurve.getKnots ( knots );
		
		MVectorArray disp;
		MVector dir;
		for(unsigned j = 0; j<cvs.length()-1; j++) 
		{
			dir = cvs[j+1] - cvs[j];
			disp.append(dir);
		}
		
		MPoint closestp;
		MVector closestn;
		fbase.getClosestPointAndNormal (cvs[0], closestp, closestn, MSpace::kObject);
		dir = disp[0];
		dir.normalize();
		MVector tangent = closestn^dir;
		tangent.normalize();
		MVector binormal = tangent^closestn;
		binormal.normalize();

		for(unsigned k = 0; k<num_strand; k++)
		{
			MPoint moved = closestp + tangent*( rand( )%31/31.f - 0.5f)*strand_dist + binormal*( rand( )%71/71.f - 0.5f)*strand_dist;
			fbase.getClosestPoint(moved, cvs[0], MSpace::kObject);
			
			for(unsigned j = 0; j<cvs.length()-1; j++) 
			{
				XYZ noiv(disp[j].x, disp[j].y, disp[j].z);
				noiv.randRotate(0.4f*j);
				cvs[j+1] = cvs[j] + MVector(noiv.x, noiv.y, noiv.z);
			}
			
			fcurve.create ( cvs, knots, 1, MFnNurbsCurve::kOpen , 0, 0, MObject::kNullObj, &status );
		}
	}
}

void guide::createCurve()
{
	MStatus status;
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	
	MItSelectionList list( slist, MFn::kComponent, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return;
	}

	MObject component;

	if (list.isDone()) {
		displayError( "No components selected" );
		return;
	}
	
	MPointArray pointArray;
	MVector tang, nor;
	MVectorArray normalArray, tangentArray;

	MDagPath fDagPath;
	list.getDagPath (fDagPath, component);

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
	
	MFnNurbsCurve fcurve;
	MDoubleArray knots;
	for(unsigned j = 0;j <= num_seg;j++) knots.append(j);
		
	for(unsigned i = 0;i < pointArray.length();i++)
	{
		MPointArray cvs;
		MPoint vert;
		MVector binormal;
		for(unsigned j = 0;j <= num_seg;j++)
		{
			binormal = tangentArray[i]^normalArray[i];
			
			vert = pointArray[i] + (normalArray[i] + binormal*0.1)*seglength*j;
			cvs.append(vert);
		}
		fcurve.create ( cvs, knots, 1, MFnNurbsCurve::kOpen , 0, 0, MObject::kNullObj, &status );
	}
}




