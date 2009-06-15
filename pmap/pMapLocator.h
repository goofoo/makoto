#ifndef _particlePositionNode
#define _particlePositionNode
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
#include <maya/MPoint.h>
#include <maya/MFndagNode.h>
#include ".\OcTree.h"

 
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

	static	MTypeId		id;
	static  MObject     alevel;
private:
	bool    loadParticlePosition() const;
	void    draw();
};

#endif
