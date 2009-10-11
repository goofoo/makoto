#ifndef _PTCDemViz
#define _PTCDemViz

#include <maya/MPxLocatorNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MPoint.h>
#include <maya/MFndagNode.h>
#include <iostream>
#include <fstream>
#include <string>
#include <maya/MFnCamera.h>
#include <maya/MDagPath.h>
#include "../3dtexture/Z3DTexture.h"
 
class PTCDemViz : public MPxLocatorNode
{
public:
						  PTCDemViz();
	virtual				  ~PTCDemViz(); 

	virtual MStatus		  compute( const MPlug& plug, MDataBlock& data );
	virtual void          draw( M3dView & view, const MDagPath & path, 
                                M3dView::DisplayStyle style,M3dView::DisplayStatus status );
    virtual bool          isBounded() const;
    virtual MBoundingBox  boundingBox() const; 

	static  void*		creator();
	static  MStatus		initialize();
	
	void drawCross(int last, MPoint center, float size);

public:

	static  	MTypeId		id;
	static  	MObject		asize;
	static  	MObject		alevel;
private:
	float f_size;
	int f_level;
};

#endif
