#ifndef _particlePositionCmd
#define _particlePositionCmd
//
// Copyright (C) YiLi
// 
// File: pMapCmd.h
//
// MEL Command: pMapCmd
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
#include <maya/M3dView.h>
#include <maya/MFnParticleSystem.h>
#include <iostream>
#include <fstream>
#include "E:\git_home\shared\zData.h"



class MArgList;

class pMapCmd : public MPxCommand
{

public:
				pMapCmd();
	virtual		~pMapCmd();

	MStatus		doIt( const MArgList& );
	MStatus		redoIt();
	MStatus		undoIt();
	bool		isUndoable() const;

	static		void* creator();

private:
	// Store the data you will need to undo the command here
	//
	MDagPath fDagPath;
	MPointArray pointArray;
	MObject particleNode;
};

#endif
