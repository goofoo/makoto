/*
 *  vxCacheDeformer.h
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

class vxCacheDeformer : public MPxDeformerNode
{
public:
					vxCacheDeformer();
	virtual				~vxCacheDeformer();

	static  void*			creator();
	static  MStatus			initialize();

	// deformation function
	//
	virtual MStatus      		deform(MDataBlock& block, MItGeometry& iter, const MMatrix& mat, unsigned int multiIndex);

	// local node attributes
	static  	MObject		path;
	static  	MObject		frame;
	static  	MObject 	aminframe;
	static  	MObject 	amaxframe;
	static  	MObject 	aframestep;
	static  MTypeId		id;
	
private:
	//FILE *fp;
	MPointArray vertexArray, vertexFArray;
};