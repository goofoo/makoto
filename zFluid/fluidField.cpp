#include <maya/MDagPathArray.h>
#include <maya/M3dView.h>
#include "fluidField.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"
#include "../shared/zMath.h"
#include "fluidDescData.h"
#include "../shared/gBase.h"

MTypeId fluidField::id( 0x0004132a );
MObject	fluidField::amatrix;
MObject	fluidField::astarttime;
MObject	fluidField::atime;
MObject	fluidField::aobstacle;
MObject	fluidField::ainDesc;
MObject	fluidField::ainAge;

fluidField::fluidField():
m_rx(32),
m_ry(32),
m_rz(32),
m_pSolver(0),
m_origin_x(0.0f), 
m_origin_y(0.0f), 
m_origin_z(0.0f)
{
	m_pSolver=new FluidSolver();
	m_pSolver->initialize(m_rx,m_ry,m_rz);
}
fluidField::~fluidField() 
{
	delete m_pSolver;
}

void *fluidField::creator()
{
    return new fluidField;
}


MStatus fluidField::initialize()
//
//	Descriptions:
//		Initialize the node, attributes.
//
{
	MStatus				status;

	status = zCreateMatrixAttr(amatrix, MString("localMatrix"), MString("lm"));
	zCheckStatus(status, "failed to add up vector attr");
	status = addAttribute(amatrix);
	
	status = zCreatePluginAttr(ainDesc, MString("inDesc"), MString("inDesc"));
	zCheckStatus(status, "ERROR creating input desc");
	status = addAttribute(ainDesc);
	
	status = zCreateTimeAttr(atime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(status, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	status = zCreateTimeAttr(astarttime, MString("startTime"), MString("st"), 0.0);
	zCheckStatus(status, "ERROR creating start time");
	zCheckStatus(addAttribute(astarttime), "ERROR adding start time");
	
	zCheckStatus(zWorks::createTypedArrayAttr(aobstacle, MString("obstacles"), MString("obst"), MFnData::kMesh), "ERROR creating obstacles");
	zCheckStatus(addAttribute(aobstacle), "ERROR adding obstacles");
	
	zCheckStatus(zCreateTypedAttr(ainAge, MString("age"), MString("age"), MFnData::kDoubleArray), "failed to add age");
	zCheckStatus(addAttribute(ainAge), "ERROR adding age");
	
	attributeAffects(aobstacle, mOutputForce);

	return( MS::kSuccess );
}


MStatus fluidField::compute(const MPlug& plug, MDataBlock& block)
//
//	Descriptions:
//		compute output force.
//
{
	MStatus status;
	int do_save_cache;
	
	if( plug == mOutputForce)
	{
        	int ison, rx, ry, rz;
		float Kbuoyancy, Kswirl, Kvelocity, Ktemperature, temperatureLoss, wind_x, wind_z;
		MDataHandle hdesc = block.inputValue(ainDesc);
		MObject odesc = hdesc.data();
		MFnPluginData fdesc(odesc);
		zjFluidDescData* dDesc = (zjFluidDescData*) fdesc.data();
		if(dDesc)
		{
			fluidDesc* pDesc = dDesc->getDesc();
			//MGlobal::displayInfo(MString("on: ")+pDesc->enable);
			m_pSolver->setGridSize(pDesc->gridSize);
			m_gridSize = pDesc->gridSize;
			Kbuoyancy = pDesc->buoyancy;
			Kswirl = pDesc->swirl;
			Kvelocity = pDesc->velocity;
			Ktemperature = pDesc->temperature;
			temperatureLoss = pDesc->temperatureLoss;
			rx = pDesc->resolution_x;
			ry = pDesc->resolution_y;
			rz = pDesc->resolution_z;
			ison = pDesc->enable;
			do_save_cache = pDesc->save_cache;
			wind_x = pDesc->wind_x;
			wind_z = pDesc->wind_z;
		}
		else return MS::kUnknownParameter;
		
		MTime currentTime = block.inputValue(atime, &status).asTime();
		if(!status) MGlobal::displayInfo("ERROR getting time attribute");
		
		MTime startTime = block.inputValue(astarttime, &status).asTime();
		if(!status) MGlobal::displayInfo("ERROR getting start time attribute");

	// get the logical index of the element this plug refers to.
	//
	int multiIndex = plug.logicalIndex( &status );
	McheckErr(status, "ERROR in plug.logicalIndex.\n");

	// Get input data handle, use outputArrayValue since we do not
	// want to evaluate both inputs, only the one related to the
	// requested multiIndex. Evaluating both inputs at once would cause
	// a dependency graph loop.
	//
	MArrayDataHandle hInputArray = block.outputArrayValue( mInputData, &status );
	McheckErr(status,"ERROR in hInputArray = block.outputArrayValue().\n");

	status = hInputArray.jumpToElement( multiIndex );
	McheckErr(status, "ERROR: hInputArray.jumpToElement failed.\n");

	// get children of aInputData.
	//
	MDataHandle hCompond = hInputArray.inputValue( &status );
	McheckErr(status, "ERROR in hCompond=hInputArray.inputValue\n");

	MDataHandle hPosition = hCompond.child( mInputPositions );
	MObject dPosition = hPosition.data();
	MFnVectorArrayData fnPosition( dPosition );
	MVectorArray points = fnPosition.array( &status );
	McheckErr(status, "ERROR in fnPosition.array(), not find points.\n");

	MDataHandle hVelocity = hCompond.child( mInputVelocities );
	MObject dVelocity = hVelocity.data();
	MFnVectorArrayData fnVelocity( dVelocity );
	MVectorArray velocities = fnVelocity.array( &status );
	McheckErr(status, "ERROR in fnVelocity.array(), not find velocities.\n");
	
	M3dView view = M3dView::active3dView();
	int pw = view.portWidth();
	int ph = view.portHeight();
	MDagPath cameraPath;
	view.getCamera ( cameraPath );
	MFnCamera cameraFn(cameraPath );
	
	MMatrix mat_world = zGetMatrixAttr(block, amatrix);
	m_origin_x = mat_world(3,0);
	m_origin_y = mat_world(3,1);
	m_origin_z = mat_world(3,2);
	m_pSolver->setOrigin(m_origin_x, m_origin_y, m_origin_z);

	m_pSolver->setBuoyancy(Kbuoyancy);
	m_pSolver->setConserveVelocity(Kvelocity);
	m_pSolver->setTemperature(Ktemperature);
	m_pSolver->setTemperatureLoss(temperatureLoss);
	m_pSolver->setSwirl(Kswirl);
	m_pSolver->setWind(wind_x, wind_z);
	
	// initialize velocity field
	if(currentTime.value() <= startTime.value() + 1){
		MGlobal::displayInfo(MString("Initialize fluid at frame ")+currentTime.value());
		
		if(isPowerof2(rx)==1&&isPowerof2(ry)==1&&isPowerof2(rz)==1)
		{
			if(rx!=m_rx || ry!=m_ry || rz!=m_rz)
			{
				m_rx = rx;
				m_ry = ry;
				m_rz = rz;
				
				m_pSolver->initialize(m_rx,m_ry,m_rz);
			}
		}
		
		m_pSolver->clear();
	}
	
	
	
	
	MDoubleArray ptc_age = zGetDoubleArrayAttr(block, ainAge);
	
	MArrayDataHandle hArray = block.inputArrayValue(aobstacle);
	
	int n_obs = hArray.elementCount();
	MObjectArray obslist;
	for(int iter=0; iter<n_obs; iter++)
	{
		obslist.append(hArray.inputValue().asMeshTransformed());
		hArray.next();
	}

	if(ison==1) {
// simulate !
		m_pSolver->processSources(points, velocities, ptc_age);
		m_pSolver->processObstacles(obslist);
		m_pSolver->update();
	}

	obslist.clear();

	// restore the perspective view port after drawing
	float aspect = (float)pw/(float)ph;
	glViewport(0, 0, pw, ph);
	
	double horizontalFOV,verticalFOV;
	cameraFn.getPortFieldOfView(pw, ph, horizontalFOV, verticalFOV );
	//zDisplayFloat2(horizontalFOV,verticalFOV);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	gluPerspective( rad2deg(verticalFOV), aspect, cameraFn.nearClippingPlane(), cameraFn.farClippingPlane() );
	glMatrixMode(GL_MODELVIEW);
	
	
	
	//zDisplayFloat3(mat_world(3,0), mat_world(3,1), mat_world(3,2));
	// Compute the output force.
	//
	MVectorArray forceArray;
	
	calculateForce( points, velocities, forceArray, do_save_cache, (int)currentTime.value(), ptc_age);
	
	// get output data handle
	//
	MArrayDataHandle hOutArray = block.outputArrayValue( mOutputForce, &status);
	McheckErr(status, "ERROR in hOutArray = block.outputArrayValue.\n");
	MArrayDataBuilder bOutArray = hOutArray.builder( &status );
	McheckErr(status, "ERROR in bOutArray = hOutArray.builder.\n");

	// get output force array from block.
	//
	MDataHandle hOut = bOutArray.addElement(multiIndex, &status);
	McheckErr(status, "ERROR in hOut = bOutArray.addElement.\n");

	MFnVectorArrayData fnOutputForce;
	MObject dOutputForce = fnOutputForce.create( forceArray, &status );
	McheckErr(status, "ERROR in dOutputForce = fnOutputForce.create\n");
	
	// update data block with new output force data.
	//
	hOut.set( dOutputForce );
	block.setClean( plug );

		return( MS::kSuccess );
	}
	else return MS::kUnknownParameter;
}

void fluidField::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView:: DisplayStatus )
{
	 view.beginGL();
	if(m_pSolver->isInitialized()) m_pSolver->showImpulse();
	 view.endGL ();
}

void fluidField::calculateForce
	(
		//MDataBlock& block,			// get field param from this block
		//const MMatrix& space,
		const MVectorArray &points,		// current position of Object
		const MVectorArray &velocities,		// current velocity of Object
		//const MDoubleArray &masses,		// mass of Object
		MVectorArray &OutputForce,		// output force
		int i_cache,
		int i_frame,
		const MDoubleArray &ages
	)
//
//	Descriptions:
//		Compute output force.
//
{
	MStatus status;
	// points and velocities should have the same length. If not return.
	//
	if( points.length() != velocities.length() )
		return;
	
	int receptorSize = points.length();
	
	float vx, vy, vz;
	MVectorArray dest_vels;
	// particle iterator
	//
	for (int ptIndex = 0; ptIndex < receptorSize; ptIndex ++ ) 
	{
		MPoint ploc(points[ptIndex]);
		MVector pvel(velocities[ptIndex]);
		getLocalPoint(ploc);
		if(isInField(ploc)==1)
		{
			m_pSolver->getVelocity(vx, vy, vz, ploc.x, ploc.y, ploc.z);

			OutputForce.append( 24.0f*(24.0f*MVector(vx, vy, vz)*m_gridSize - pvel) );
			dest_vels.append(24.0f*MVector(vx, vy, vz)*m_gridSize);
		}
		else
			OutputForce.append( MVector(0,0,0) );
	}
}

int fluidField::isInField(const MPoint& pt)
{
	if(pt.x <1) return 0;
	if(pt.x >m_rx-1) return 0;
	if(pt.y <1) return 0;
	if(pt.y >m_ry-1) return 0;
	if(pt.z <1) return 0;
	if(pt.z >m_rz-1) return 0;
	return 1;
}

MStatus fluidField::connectionMade (const  MPlug & plug, const  MPlug & otherPlug, bool asSrc)
{
	if(plug.partialName()==MString("rez"))
	{
		MGlobal::displayInfo(plug.partialName()+ " connect");
	}
	return MS::kUnknownParameter;
}

MStatus fluidField::connectionBroken(const  MPlug & plug, const  MPlug & otherPlug, bool asSrc)
{
	if(plug.partialName()==MString("rez"))
		MGlobal::displayInfo(plug.partialName()+" broke");
	return MS::kUnknownParameter;
}

void fluidField::getLocalPoint(MPoint& pt)
{
	pt.x = (pt.x-m_origin_x)/m_gridSize;
	pt.y = (pt.y-m_origin_y)/m_gridSize;
	pt.z = (pt.z-m_origin_z)/m_gridSize;
}

void fluidField::getLocalPoint(float& px, float& py, float& pz)
{
	px = (px-m_origin_x)/m_gridSize;
	py = (py-m_origin_y)/m_gridSize;
	pz = (pz-m_origin_z)/m_gridSize;
}

MStatus fluidField::iconSizeAndOrigin(	GLuint& width,
					GLuint& height,
					GLuint& xbo,
					GLuint& ybo   )
//
//	This method is not required to be overridden.  It should be overridden
//	if the plug-in has custom icon.
//
//	The width and height have to be a multiple of 32 on Windows and 16 on 
//	other platform.
//
//	Define an 8x8 icon at the lower left corner of the 32x32 grid. 
//	(xbo, ybo) of (4,4) makes sure the icon is center at origin.
//
{
	width = 32;
	height = 32;
	xbo = 4;
	ybo = 4;
	return MS::kSuccess;
}

