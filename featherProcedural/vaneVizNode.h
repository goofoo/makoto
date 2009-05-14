/*
 *  vaneVizNode.h
 *  vaneVizNode
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
#include <maya/MDagPath.h>
#include "VVane.h"

class vaneVizNode : public MPxLocatorNode
{
public:
	vaneVizNode();
	virtual ~vaneVizNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const;

	static  void *          creator();
	static  MStatus         initialize();
	
		// Internally cached attribute handling routines
	virtual bool getInternalValueInContext( const MPlug&,
											  MDataHandle&,
											  MDGContext&);
    virtual bool setInternalValueInContext( const MPlug&,
											  const MDataHandle&,
											  MDGContext&);

		// Attributes
	static MObject acachename;
	static MObject aframe;
	static MObject aminframe;
	static MObject amaxframe;
	static MObject aratio;
	static MObject agrowth;
	static MObject acollision;
	static MObject atime;
	static MObject awind;
	static MObject aoutval;
	static  	MObject 	outMesh;
	static MTypeId id;
	
		// Cached internal values
	double m_time;
	MString m_cachename;
	float m_scale, m_wind;
	
private :
		VVane* m_base;
		bool	mAttributesChanged;
		int m_frame_pre;
		char i_show_v;
};