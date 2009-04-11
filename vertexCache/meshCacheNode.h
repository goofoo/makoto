/*
 *  pluginMain.h
 *  
 *
 *  Created by zhang on 08-1-12.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MGlobal.h>
#include <maya/MPxDeformerNode.h> 
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
 
class meshCacheNode : public MPxNode
{
public:
						meshCacheNode();
	virtual				~meshCacheNode(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:
	static  	MObject		input;
	static  	MObject		frame;
	static  	MObject 	outMesh;
	static  	MObject 	aminframe;
	static  	MObject 	amaxframe;
	static  	MObject 	aframestep;
	static	MTypeId		id;
	
private:
	//FILE *fp;
	MPointArray vertexArray, vertexFArray;
	MIntArray polygonCounts, polygonConnects, polygonUVs;
	MFloatArray uArray, vArray;

};


