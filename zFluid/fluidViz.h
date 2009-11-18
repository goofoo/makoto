#include <maya/MPxLocatorNode.h> 
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include "fluidDescData.h"

class fluidViz : public MPxLocatorNode
{
public:
	fluidViz();
	virtual ~fluidViz(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	
	virtual  MStatus  	connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc );
	
	static  void *          creator();
	static  MStatus         initialize();

	static MObject  asize;         // The size of the foot
	static MObject	resolution;
	static MObject	resolutionX;
	static MObject	resolutionY;
	static MObject	resolutionZ;
	static MObject	aenable;
	static MObject	abuoyancy;
	static MObject	aswirl;
	static MObject	aconserveVelocity;
	static MObject atemperature;
	static MObject	aconserveTemperature;
	static MObject	awindx;
	static MObject	awindz;
	static MObject	asaveCache;
	static MObject	aoutDesc;

public: 
	static	MTypeId		id;
private:
	fluidDesc* m_pDesc;
	
};