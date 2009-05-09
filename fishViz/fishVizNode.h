#include <stdio.h>
#include <stdlib.h>
#include <maya/MPxNode.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include "../shared/fishBone.h"


#define Rad(x)	((x)*FPI/180.0F)
#define Deg(x)	((x)*180.0F/FPI)
#define FPI 3.14159265358979323846264338327950288419716939937510582F 

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }


class fishVizNode : public MPxLocatorNode
{
public:
	fishVizNode();
	virtual ~fishVizNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );
    virtual  MStatus  	connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc );

	static  void *          creator();
	static  MStatus         initialize();
	
	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;

public: 
	static	MTypeId		id;


	static MObject aInMesh;
	static MObject aNBone;
	static MObject aOscillate;
	static MObject aFrequency;
	static MObject aFlapping;
	static MObject aBending;
	static MObject aLength;
	static MObject aHead;
	static MObject acachename;
	static MObject atime;

    
    // Output mesh
    //
    static  MObject outMesh;

private:
	unsigned int m_num_fish, m_num_bone;
	CfishBone* m_pBone;
	
	void poseBones(float l, int n, float time, float freq, float ampl, float bend, float kf, float kb, float ko);
	
	MVectorArray ptc_positions;
	MVectorArray ptc_velocities;
	MVectorArray ptc_ups;
	MVectorArray ptc_views;
	float m_fish_length;
};