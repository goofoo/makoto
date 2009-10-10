#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxNode.h> 

#include <maya/MFnNumericAttribute.h>

#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
 
class fluidCacheNode : public MPxNode
{
public:
						fluidCacheNode();
	virtual				~fluidCacheNode(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:
	static  MObject		atime;
	static MObject ainposition;
	static MObject ainvelocity;
	static MObject ainage;
	static MObject ainmass;
	static MObject acachename;
	static  MObject		output;        // The output value.
	static	MTypeId		id;
};