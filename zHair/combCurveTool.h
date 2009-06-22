/*
 *  combCurveTool.h
 *  hair
 *
 *  Created by jian zhang on 6/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <math.h>
#include <stdlib.h>

#include <maya/MString.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MDagPath.h>
 
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>

#include <maya/MPxContextCommand.h>
#include <maya/MPxContext.h>
#include <maya/MPxToolCommand.h> 
#include <maya/MEvent.h>
#include <maya/MToolsInfo.h>

#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>

#include <maya/MDagPathArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MMatrix.h>

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "../shared/zData.h"

class combCurveTool : public MPxToolCommand
{
public:
					combCurveTool(); 
	virtual			~combCurveTool(); 
	static void*	creator();

	MStatus			doIt(const MArgList& args);
	MStatus			parseArgs(const MArgList& args);
	static MSyntax	newSyntax();
	MStatus			finalize();

private:
	unsigned opt, nseg;
	float lseg;
};

const char helpString[] =
			"Click with left button or drag with middle button to select";

class combCurveContext : public MPxContext
{
public:
					combCurveContext();
	virtual void	toolOnSetup( MEvent & event );
	virtual MStatus	doPress( MEvent & event );
	virtual MStatus	doDrag( MEvent & event );
	virtual MStatus	doRelease( MEvent & event );
	virtual MStatus	doEnterRegion( MEvent & event );
	
	virtual	void	getClassName(MString & name) const;
	
	void setOperation(unsigned val);
	unsigned getOperation() const;
	void setNSegment(unsigned val);
	unsigned getNSegment() const;
	void setLSegment(float val);
	float getLSegment() const;
	void setCreationNoise(float val);
	float getCreationNoise() const;

private:
	short					start_x, start_y;
	short					last_x, last_y;

	MGlobal::ListAdjustment	listAdjustment;
	M3dView					view;
	MDagPathArray curveList;
	MDoubleArray curveLen;
	MATRIX44F mat;
	double clipNear, clipFar;
	unsigned mOpt, m_numSeg;
	float m_seg_len, m_seg_noise;
	
	void moveAll();
	void dragTip();
	void scaleAll();
	void deKink();
	void grow();
};

class combCurveContextCmd : public MPxContextCommand
{
public:	
						combCurveContextCmd();
	virtual MStatus		doEditFlags();
	virtual MStatus doQueryFlags();
	virtual MPxContext*	makeObj();
	static	void*		creator();
	virtual MStatus		appendSyntax();
	
protected:
    combCurveContext*		fContext;
};
