/*
 *  hdrEnvNode.h
 *  hdrEnvNode
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

#include "hdrProgram.h"
#include "../shared/zData.h"

class HDRtexture;
class sphericalHarmonics;

class hdrEnvNode : public MPxLocatorNode
{
public:
	hdrEnvNode();
	virtual ~hdrEnvNode(); 

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
	static MObject  aExposure;
	static MObject  aHDRName;
	static MObject  aSize;
	static MObject aworldSpace;
	static MObject acullFace;
	static MObject aoutput;
	static	MTypeId		id;
	
		// Cached internal values
	float m_exposure;
	float m_size;
	MString m_hdrname;
	
private :
	hdrProgram* m_program;
		GLuint m_tex;
		bool	mAttributesChanged;
		int m_width, m_height;
		XYZ* m_pPos;
		XY* m_pST;
	void loadHDR(MString& hdrname);
	HDRtexture* f_hdr;
	sphericalHarmonics* f_sh;
	XYZ hdrCoeff[16];
};
