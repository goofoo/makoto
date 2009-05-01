#include <stdio.h>
#include <stdlib.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include "../shared/fishBone.h"


#define Rad(x)	((x)*FPI/180.0F)
#define Deg(x)	((x)*180.0F/FPI)
#define FPI 3.14159265358979323846264338327950288419716939937510582F 

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }


class fishVizNode : public MPxNode
{
public:
	fishVizNode();
	virtual ~fishVizNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );
    virtual  MStatus  	connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc );

	static  void *          creator();
	static  MStatus         initialize();

public: 
	static	MTypeId		id;


	static MObject aInMesh;

	static MObject aPosition;
	static MObject aVelocity;
	static MObject aUpVector;
	static MObject aViewVector;
	static MObject aScalePP;	
	static MObject ainmass;
	static MObject aTimeOffset;
	static MObject aAmplitude;
	static MObject aBend;	
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
};