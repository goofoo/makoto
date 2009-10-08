#ifndef _PTCMapCmd_H
#define _PTCMapCmd_H
//
// Copyright (C) YiLi
// 
// File: PTCMapCmd.h
//
// MEL Command: PTCMapCmd
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
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <iostream>
#include <fstream>

class PTCMapCmd : public MPxCommand
{

public:
				PTCMapCmd();
	virtual		~PTCMapCmd();

	MStatus		doIt( const MArgList& );
	static MSyntax newSyntax();
	bool		isUndoable() const;
	static		void* creator();
    
private:
	MStatus parseArgs ( const MArgList& args );
};

#endif
