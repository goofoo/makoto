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
#include <maya/MEvent.h>

#include <maya/MDagPathArray.h>
#include <maya/MMatrix.h>

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "../shared/zData.h"

//////////////////////////////////////////////
// The user Context
//////////////////////////////////////////////
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
	
	void setOperation(unsigned val);

private:
	short					start_x, start_y;
	short					last_x, last_y;

	MGlobal::ListAdjustment	listAdjustment;
	M3dView					view;
	MDagPathArray curveList;
	MATRIX44F mat;
	double clipNear, clipFar;
	unsigned mOpt;
};

class combCurveContextCmd : public MPxContextCommand
{
public:	
						combCurveContextCmd();
	virtual MPxContext*	makeObj();
	static	void*		creator();
	virtual MStatus		doEditFlags();
	virtual MStatus		appendSyntax();
protected:
    combCurveContext*		fContext;
};
