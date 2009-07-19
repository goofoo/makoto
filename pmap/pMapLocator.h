#ifndef _pMapLocator
#define _pMapLocator
//
// Copyright (C) YiLi
// 
// File: pMapLocator.h
//
// Dependency Graph Node: pMapLocator
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxLocatorNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MPoint.h>
#include <maya/MFndagNode.h>
#include <iostream>
#include <fstream>
#include <string>
#include <maya/MFnCamera.h>
#include <maya/MDagPath.h>
#include "../shared/OcTree.h"
#include "view.h"
 
class pMapLocator : public MPxLocatorNode
{
public:
						  pMapLocator();
	virtual				  ~pMapLocator(); 

	virtual MStatus		  compute( const MPlug& plug, MDataBlock& data );
	virtual void          draw( M3dView & view, const MDagPath & path, 
                                M3dView::DisplayStyle style,M3dView::DisplayStatus status );
    virtual bool          isBounded() const;
    virtual MBoundingBox  boundingBox() const; 

	static  void*		creator();
	static  MStatus		initialize();

public:

	static  	MTypeId		id;
	static  	MObject		input;
	static  	MObject		frame;
	static  	MObject 	aminframe;
	static  	MObject 	amaxframe;
	static  	MObject 	aresolution;
	static      MObject     aoutval;
	static      MObject     aposition;
private:
	float f_rez;
	OcTree* tree;
};

#endif
