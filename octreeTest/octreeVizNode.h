/*
 *  octreeVizNode.h
 *  octreeVizNode
 *
 *  Created by zhang on 07-10-12.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MPxLocatorNode.h> 
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
#include "../shared/Octree.h"

class octreeVizNode : public MPxLocatorNode
{
public:
	octreeVizNode();
	virtual ~octreeVizNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const;

	static  void *          creator();
	static  MStatus         initialize();

		// Attributes
	static MObject afuzz;
	static MObject  astartframe;
	static MObject  acurrenttime;
	static MObject  aHDRName;
	static MObject  aSize;
	static MObject aworldSpace;
	static MObject aoutput;
	static MObject agrowth;
	static MObject aguide;
	static MObject alengthnoise;
	static MObject asavemap;
	static MObject astep;
	static MObject aalternativepatch;
	static	MTypeId		id;

private :
	XYZ* raw_data;
	unsigned num_raw_data;
	OcTree* tree;
};