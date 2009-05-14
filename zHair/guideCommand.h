#ifndef _guideCommand
#define _guideCommand
//
// Copyright (C) YiLi
// 
// File: guideCmd.h
//
// MEL Command: guide
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxCommand.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPoint.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MPxCommand.h>
#include <maya/MObject.h>
#include <maya/MArgList.h>
#include <maya/MItMeshVertex.h>
#include <maya/M3dView.h>
#include <maya/MFnMesh.h>

class MArgList;
const short num_seg = 10;
const double seglength = 0.2;

class guide : public MPxCommand
{

public:
				guide();
	virtual		~guide();

	MStatus		doIt( const MArgList& );
	MStatus		redoIt();
	static		void* creator();

private:
	// Store the data you will need to undo the command here
	//
	MDagPath fDagPath;
	MPointArray pointArray;
	MVector tang;
	MVector nor;
	MVectorArray normalArray;
	MVectorArray tangentArray;
	int vertexCounts;
	MPoint point;

	//MObject component;
};

#endif
