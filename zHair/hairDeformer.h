/*
 *  hairDeformer.h
 *  
 *
 *  Created by zhang on 08-1-12.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MPxDeformerNode.h> 
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include "../shared/zData.h"

class hairDeformer : public MPxDeformerNode
{
public:
					hairDeformer();
	virtual				~hairDeformer();

	static  void*			creator();
	static  MStatus			initialize();

	// deformation function
	//
	virtual MStatus      		deform(MDataBlock& block, MItGeometry& iter, const MMatrix& mat, unsigned int multiIndex);

	// local node attributes
	static  	MObject		abase;
	static  	MObject		frame;
	static MObject agrowth;
	static  	MObject 	aframestep;
	static  MTypeId		id;
	
private:
	XYZ* pobj;
	int* faceId;
	int num_hair;
	int* nsegbuf;
};