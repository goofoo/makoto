#ifndef _hairDguideNode_H
#define _hairDguideNode_H

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
#include "zHairNode.h"

class HairDguideNode : public MPxLocatorNode
{
public:
						HairDguideNode();
	virtual				~HairDguideNode(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	virtual void        draw( M3dView & view, const MDagPath & path, 
                              M3dView::DisplayStyle style,
                              M3dView::DisplayStatus status );

	void    loadDguide();

public:

	// There needs to be a MObject handle declared for each attribute that
	// the node will have.  These handles are needed for getting and setting
	// the values later.
	//
	static  MObject		input;		// Example input attribute
	static  MObject		output;		// Example output attribute


	// The typeid is a unique 32bit indentifier that describes this node.
	// It is used to save and retrieve nodes of this type from the binary
	// file format.  If it is not unique, it will cause file IO problems.
	//
	static	MTypeId		id;
	Dguide* guide_data;
	unsigned num_guide;
};

#endif