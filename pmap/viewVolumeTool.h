/*
 *  viewVolumeTool.h
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
#include <maya/MFnMesh.h>

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "../shared/zData.h"

class viewVolumeTool : public MPxToolCommand
{
public:
					viewVolumeTool(); 
	virtual			~viewVolumeTool(); 
	static void*	creator();

	MStatus			doIt(const MArgList& args);
	MStatus			parseArgs(const MArgList& args);
	static MSyntax	newSyntax();
	MStatus			finalize();

private:
};

const char helpString[] =
			"Click with left button or drag with middle button to select";

class viewVolumeContext : public MPxContext
{
public:
					viewVolumeContext();
	virtual void	toolOnSetup( MEvent & event );
	virtual MStatus	doPress( MEvent & event );
	virtual MStatus	doEnterRegion( MEvent & event );
	
	virtual	void	getClassName(MString & name) const;
	
private:
};

class viewVolumeContextCmd : public MPxContextCommand
{
public:	
						viewVolumeContextCmd();
	virtual MStatus		doEditFlags();
	virtual MStatus doQueryFlags();
	virtual MPxContext*	makeObj();
	static	void*		creator();
	virtual MStatus		appendSyntax();
	
protected:
    viewVolumeContext*		fContext;
};
