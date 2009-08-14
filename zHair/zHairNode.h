/*
 *  HairNode.h
 *  HairNode
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
#include "HairMap.h"

class HairNode : public MPxLocatorNode
{
public:
	HairNode();
	virtual ~HairNode(); 

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
	static MObject adraw;
	static MObject aoffset;
	static MObject adensitymap;
	static	MTypeId		id;

private :
	hairMap* m_base;
	bool	mAttributesChanged;
	int idraw;
};