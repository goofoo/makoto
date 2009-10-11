#ifndef _PTCMapLocator
#define _PTCMapLocator
//
// Copyright (C) YiLi
// 
// File: PTCMapLocator.h
//
// Dependency Graph Node: PTCMapLocator
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
#include "../3dtexture/Z3DTexture.h"
 
class PTCMapLocator : public MPxLocatorNode
{
public:
						  PTCMapLocator();
	virtual				  ~PTCMapLocator(); 

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
	//static  	MObject 	aresolution;
	static      MObject     adrawtype;
	static      MObject     aoutval;
	//static      MObject     aposition;
private:
	int f_type;
	Z3DTexture* tree;
};

#endif
