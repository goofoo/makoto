#include <maya/MPxNode.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include "../shared/zData.h"

class particleWrapNode : public MPxNode
{
public:
	particleWrapNode();
	virtual ~particleWrapNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );
    static  void *          creator();
	static  MStatus         initialize();

public: 
	static	MTypeId		id;
	
	static MObject aInMesh;
	static MObject abindMesh;
	static MObject aPosition;
	static MObject astarttime;
	static MObject atime;
	static MObject aoutval;

private:
	XYZ* pobj;
	int* faceId;
};