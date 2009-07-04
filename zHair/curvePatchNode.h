/*
 *  curvePatchNode.h
 *  curvePatchNode
 *
 *  Created by zhang on 07-10-12.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MPxNode.h> 
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include "../shared/zData.h"

class curvePatchNode : public MPxNode
{
public:
	curvePatchNode();
	virtual ~curvePatchNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	static  void *          creator();
	static  MStatus         initialize();
	
// Attributes
	static MObject  aSize;
	static MObject  atwist;
	static MObject  arotate;
	static MObject agrowth;
	static MObject aguide;
	static MObject aoutput;
	static	MTypeId		id;

private :
};