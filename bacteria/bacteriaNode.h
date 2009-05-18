#include <stdio.h>
#include <stdlib.h>
#include <maya/MPxNode.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MMatrix.h>
#include "../shared/zData.h"
#include "FBacteria.h"

#define Rad(x)	((x)*FPI/180.0F)
#define Deg(x)	((x)*180.0F/FPI)
#define FPI 3.14159265358979323846264338327950288419716939937510582F 

#define McheckErr(stat,msg)         \
    if ( MS::kSuccess != stat ) {   \
        cerr << msg;                \
        return MS::kFailure;        \
    }


class bacteriaNode : public MPxLocatorNode
{
public:
	bacteriaNode();
	virtual ~bacteriaNode(); 

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

	static MObject aOscillate;
	static MObject aFrequency;
	static MObject aFlapping;
	static MObject aBending;
	static MObject aLength;
	static MObject aHead;
	static MObject acachename;
	static MObject atime;
	static  MObject         amatrix;
	static  MObject         anear;
	static  MObject         afar;
	static	MObject		ahapeture;
	static	MObject		avapeture;
	static	MObject		afocallength;
    
    // Output
    static  MObject aoutput;
	
	const FBacteria* getBase() const {return f_bac;}

private:
	unsigned int m_num_fish, m_num_bone;

	float m_fish_length;
	
	FBacteria* f_bac;
};