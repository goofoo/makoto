//:~
// PLUGIN NAME: fluidField
// FILE: fluidField.cpp
// DESCRIPTION: -Defines "fluidField" node.
//
// REFERENCES: -The field generate the force to simulate a 3D fluid.
//
// COMPILED AND TESTED ON: Maya 5.0 on Linux


#include <maya/MVector.h>
#include <maya/MObject.h>
#include <maya/MDataBlock.h>
#include <maya/MPxFieldNode.h>

#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>


#include <maya/MFnDependencyNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMesh.h>


#define McheckErr(stat, msg)		\
	if ( MS::kSuccess != stat )	\
	{				\
		cerr << msg;		\
		return MS::kFailure;	\
	}

#include "fluidSolver.h"

class fluidField: public MPxFieldNode
{
public:
			 fluidField();
	virtual 	~fluidField();

	static void		*creator();
	static MStatus		initialize();

	// will compute output force.
	//
	virtual MStatus	compute( const MPlug& plug, MDataBlock& block );
	
	virtual void draw (  M3dView  & view, const  MDagPath  & path,  M3dView::DisplayStyle  style, M3dView:: DisplayStatus );

	
	virtual MStatus connectionMade (const  MPlug & plug, const  MPlug & otherPlug, bool asSrc);
	virtual MStatus connectionBroken(const  MPlug & plug, const  MPlug & otherPlug, bool asSrc);

	// Other data members
	//
	static MTypeId	id;
	static MObject	amatrix;
	static MObject	atime;
	static MObject	astarttime;
	static MObject	aobstacle;
	static MObject	asourcegeo;
	static MObject	ainDesc;
	static MObject	ainAge;

private:

	// data
	//

	// methods to compute output force.
	//
	
	void	calculateForce( //MDataBlock& block,	const MMatrix& space,
							const MVectorArray &points,
							const MVectorArray &velocities,
							//const MDoubleArray &masses,
							MVectorArray &OutputForce);
							
	void getLocalPoint(MPoint& pt);
	void getLocalPoint(float& px, float& py, float& pz);
	
		virtual MStatus iconSizeAndOrigin(	GLuint& width,
						GLuint& height,
						GLuint& xbo,
						GLuint& ybo   );
	
	int m_rx, m_ry, m_rz;
	float m_gridSize;
	float m_origin_x, m_origin_y, m_origin_z;
	int isInField(const MPoint& pt);
	
	FluidSolver* m_pSolver;
	int m_sho_tex, m_slice_id;
};