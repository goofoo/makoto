//
// Copyright (C) YiLi
// 
// File: snowInHairNode.h
//
// Dependency Graph Node: snowInHair
//
// Author: Maya Plug-in Wizard 2.0
//

#ifndef _SNOWINHAIRNODE_H
#define _SNOWINHAIRNODE_H

#include <maya/MPxLocatorNode.h> 
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MFnUnitAttribute.h>
#include "../shared/zData.h"
#include "HairMap.h"

class snowInHairNode : public MPxNode
{
public:
						snowInHairNode();
	virtual				~snowInHairNode(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

	// There needs to be a MObject handle declared for each attribute that
	// the node will have.  These handles are needed for getting and setting
	// the values later.
	//
	static  MObject		aoutput;		// Example output attribute

	static MObject acachename;
	static MObject aframe;
	static MObject afuzz;
	static MObject akink;
	static MObject aclump;
	static MObject astep;
	static MObject arootcolorr;
	static MObject arootcolorg;
	static MObject arootcolorb;
	static MObject atipcolorr;
	static MObject atipcolorg;
	static MObject atipcolorb;
	static MObject amutantcolorr;
	static MObject amutantcolorg;
	static MObject amutantcolorb;
	static MObject amutantcolorscale;
	static MObject adice;
	static MObject adraw;
	static MObject ainterpolate;
	
	// The typeid is a unique 32bit indentifier that describes this node.
	// It is used to save and retrieve nodes of this type from the binary
	// file format.  If it is not unique, it will cause file IO problems.
	//
	static	MTypeId		id;
	
	const hairMap* getBase() const {return m_base;}
private:
	hairMap* m_base;
	MString curname;
	int isInterpolate, idice, idraw;
};

#endif