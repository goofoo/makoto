/*
 *  shVizNode.h
 *  shVizNode
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
#include <maya/MFndagNode.h>
#include "../shared/zData.h"
#include "../sh_lighting/SphericalHarmonics.h"

class shVizNode : public MPxLocatorNode
{
public:
	shVizNode();
	virtual ~shVizNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const;

	static  void *          creator();
	static  MStatus         initialize();

// Attributes
	static MObject atype;
	static MObject alight0x;
	static MObject alight0y;
	static MObject alight0z;
	static MObject alight1x;
	static MObject alight1y;
	static MObject alight1z;
	static MObject alight2x;
	static MObject alight2y;
	static MObject alight2z;
	static MObject aoutput;
	static MTypeId		id;

private :
	sphericalHarmonics* m_sh;
	XYZ m_coeff[SH_N_BASES];
	int m_type;
};