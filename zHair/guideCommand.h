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
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

class MArgList;


class guide : public MPxCommand
{

public:
				guide();
	virtual		~guide();
    static      MSyntax newSyntax();
	MStatus		doIt( const MArgList& );
	//MStatus		redoIt();
	static		void* creator();

private:
	int num_seg, num_strand;
    double seglength, strand_dist;
	// Store the data you will need to undo the command here
	//
	void strandCurve();
	void createCurve();
};

#endif
