#include <maya/MIOStream.h>
#include <math.h>
#include <stdlib.h>

#include "boomEmitter.h"

#include <maya/MDataHandle.h>
#include <maya/MFnDynSweptGeometryData.h>
#include <maya/MDynSweptLine.h>
#include <maya/MDynSweptTriangle.h>

#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnArrayAttrsData.h>
#include <maya/MFnMatrixData.h>

void sphereRand(MVector& q)
{	
	float noi = float(rand()%301)/301.f;
// 0 <= theta <= 2PI
	float theta = noi * (2*3.14);

// -1 <= u <= 1
	noi =  float(rand()%231)/231.f;
	float u = noi * 2 -1.0;
	 
	q.x = cos(theta) * sqrt(1.0-(u*u));
	q.y = sin(theta) * sqrt(1.0-(u*u));
	q.z = u;
}



MTypeId boomEmitter::id( 0x0004132c );


boomEmitter::boomEmitter()
:	lastWorldPoint(0, 0, 0, 1)
{
}


boomEmitter::~boomEmitter()
{
}


void *boomEmitter::creator()
{
    return new boomEmitter;
}


MStatus boomEmitter::initialize()
//
//	Descriptions:
//		Initialize the node, create user defined attributes.
//
{
	return( MS::kSuccess );
}


MStatus boomEmitter::compute(const MPlug& plug, MDataBlock& block)
//
//	Descriptions:
//		Call emit emit method to generate new particles.
//
{
	MStatus status;

	// Determine if we are requesting the output plug for this emitter node.
	//
	if( !(plug == mOutput) )
        return( MS::kUnknownParameter );

	// Get the logical index of the element this plug refers to,
	// because the node can be emitting particles into more 
    // than one particle shape.
	//
	int multiIndex = plug.logicalIndex( &status );
	McheckErr(status, "ERROR in plug.logicalIndex.\n");

	// Get output data arrays (position, velocity, or parentId)
	// that the particle shape is holding from the previous frame.
	//
	MArrayDataHandle hOutArray = block.outputArrayValue(mOutput, &status);
	McheckErr(status, "ERROR in hOutArray = block.outputArrayValue.\n");

	// Create a builder to aid in the array construction efficiently.
	//
	MArrayDataBuilder bOutArray = hOutArray.builder( &status );
	McheckErr(status, "ERROR in bOutArray = hOutArray.builder.\n");

	// Get the appropriate data array that is being currently evaluated.
	//
	MDataHandle hOut = bOutArray.addElement(multiIndex, &status);
	McheckErr(status, "ERROR in hOut = bOutArray.addElement.\n");

    // Get the data and apply the function set.
    //
    MFnArrayAttrsData fnOutput;
    MObject dOutput = fnOutput.create ( &status );
    McheckErr(status, "ERROR in fnOutput.create.\n");

	// Check if the particle object has reached it's maximum,
	// hence is full. If it is full then just return with zero particles.
	//
	bool beenFull = isFullValue( multiIndex, block );
	if( beenFull )
	{
		return( MS::kSuccess );
	}

	// Get deltaTime, currentTime and startTime.
	// If deltaTime <= 0.0, or currentTime <= startTime,
	// do not emit new pariticles and return.
	//
	MTime cT = currentTimeValue( block );
	MTime sT = startTimeValue( multiIndex, block );
	MTime dT = deltaTimeValue( multiIndex, block );
	if( (cT <= sT) || (dT <= 0.0) )
	{
		// We do not emit particles before the start time, 
		// and do not emit particles when moving backwards in time.
		// 

		// This code is necessary primarily the first time to 
		// establish the new data arrays allocated, and since we have 
		// already set the data array to length zero it does 
		// not generate any new particles.
		// 
		hOut.set( dOutput );
		block.setClean( plug );

		return( MS::kSuccess );
	}

	// Get speed, direction vector, and inheritFactor attributes.
	//
	//double speed = speedValue( block );
	//MVector dirV = directionVector( block );
	//double inheritFactor = inheritFactorValue( multiIndex, block );

	// Get the position and velocity arrays to append new particle data.
	//
	MVectorArray fnOutPos = fnOutput.vectorArray("position", &status);
	MVectorArray fnOutVel = fnOutput.vectorArray("velocity", &status);

	// Convert deltaTime into seconds.
	//
	//double dt = dT.as( MTime::kSeconds );

	// position,
	MVectorArray inPosAry;
	// velocity
	MVectorArray inVelAry;
	

	// Get the swept geometry data
	//
	MObject thisObj = this->thisMObject();
	MPlug sweptPlug( thisObj, mSweptGeometry );

	if ( sweptPlug.isConnected() ) {
		MDataHandle sweptHandle = block.inputValue( mSweptGeometry );
		// MObject sweptData = sweptHandle.asSweptGeometry();
		MObject sweptData = sweptHandle.data();
		MFnDynSweptGeometryData fnSweptData( sweptData );

		if (fnSweptData.triangleCount() > 0) {
			int numTriangles = fnSweptData.triangleCount();
		
			for ( int i=0; i<numTriangles; i++ ) {
				inPosAry.clear();
				inVelAry.clear();

				MDynSweptTriangle tri = fnSweptData.sweptTriangle( i );

				// get a triangle
				MVector p1 = tri.vertex( 0 );
				MVector p2 = tri.vertex( 1 );
				MVector p3 = tri.vertex( 2 );

				MVector center = p1 + p2 + p3;
				center /= 3.0;
				
				MVector dir = tri.normal();
				
				float aheight = 2 + 2*float(rand()%391)/391.f;

// generate new points
				for(int j=0; j<80; j++) {
					MVector noi, vnoi, addp;
					sphereRand(noi);
					
						addp =  dir + noi*0.25; 
						double fcenter = (addp.normal() * dir.normal() -0.5)/0.5;
						addp *= 1.f + 2*(float(rand()%491)/491.f - 0.5) * fcenter;
						addp *= aheight;
						float nf = float(rand()%491)/491.f;
						addp += (dir.normal()*addp.length() - addp)*nf*nf;
						
						addp = addp.normal()*2*addp.length()*float(rand()%191)/191.f;
						
						vnoi = addp.normal()*4;
						
						nf = float(rand()%491)/491.f;
						float nf1 = float(rand()%491)/491.f*(1-nf);
						
					
					inPosAry.append( p1*nf + p2*nf1 + p3*(1-nf-nf1) + addp );
					inVelAry.append( vnoi );
				}
// append to result
				emit( inPosAry, inVelAry, fnOutPos, fnOutVel );

			}
		}
	}

	// Update the data block with new dOutput and set plug clean.
	//
	hOut.set( dOutput );
	block.setClean( plug );

	return( MS::kSuccess );
}


void boomEmitter::emit
	(
		const MVectorArray &inPosAry,	// points where new particles from
		const MVectorArray &inVelAry,	// initial velocity of new particles
		
		MVectorArray &outPosAry,		// holding new particles position
		MVectorArray &outVelAry			// holding new particles velocity
	)
//
//	Descriptions:
//
{
	// check the length of input arrays.
	//
	int posLength = inPosAry.length();
	int velLength = inVelAry.length();
	
	if( posLength != velLength ) return;

	// Compute total emit count.
	//
	int index;

	// Start emission.
	//
	for( index = 0; index < posLength; index++ ) {

			// Add new data into output arrays.
			//
			outPosAry.append( inPosAry[index] );
			outVelAry.append( inVelAry[index] );
		
	}

}
