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
#include <maya/MItMeshPolygon.h>
#include "guideCommand.h"
#include "../shared/zData.h"
#include "../shared/zWorks.h"
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
	syntax.addFlag("-cp", "-convertPatch", MSyntax::kBoolean);
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
	if(argData.isFlagSet("-cp")) convertPatch();
    			
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

void guide::convertPatch()
{
	MStatus status;
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList iter( slist, MFn::kMesh, &status);
	
	MDagPath 	mdagPath;
	MObject 	mComponent;
	
	for ( ; !iter.isDone(); iter.next() ) 
	{
		iter.getDagPath( mdagPath, mComponent );
		mdagPath.extendToShape ();
		
		MGlobal::displayInfo(mdagPath.fullPathName());
		
		MItMeshPolygon faceIter(mdagPath);
		int nend = 0;
		int nconn;
		for(; !faceIter.isDone(); faceIter.next()) 
		{
			faceIter.numConnectedFaces (nconn);
			if(nconn==1) nend++;
		}
		nend /= 2;
		
		int nseg = faceIter.count() / nend;
		
		
		
		MDoubleArray knots;
		for(unsigned j = 0;j <= nseg;j++) knots.append(j);
		
		MPointArray cvs;
		
		faceIter.reset();
		for(int i=0; !faceIter.isDone(); faceIter.next(), i++) 
		{
			cvs.append(faceIter.center (MSpace::kObject));
			if(i>0 && (i+1)%nseg==0)
			{
				MVector tip = cvs[nseg-2] - cvs[nseg-1];
				for(unsigned j = 0;j < nseg;j++)
				{
					if(j<nseg-1) cvs[j] += (cvs[j] - cvs[j+1])/2;
					else cvs[j] += tip/2;
				}
				
				MPoint last = cvs[nseg-1] - tip;
				cvs.append(last);
				
				MFnNurbsCurve fcurve;
				fcurve.create ( cvs, knots, 1, MFnNurbsCurve::kOpen , 0, 0, MObject::kNullObj, &status );
				cvs.clear();
			}
		}
		
		
	}
}
