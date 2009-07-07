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
#include "../shared/OcTree.h"
#include "../shared/zData.h"
 
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
	static  	MObject		alevel;
	static  	MObject		input;
	static  	MObject		frame;
	static  	MObject 	aminframe;
	static  	MObject 	amaxframe;
	static  	MObject 	aframestep;
	static      MObject     aoutval;
	double      m_time;
private:

	OcTree* tree;

};

#endif
