/// :
// meshCacheNode.cpp
// Version 1.2.7
// Mesh cache file loader
// updated: 01/05/09
//

//#define REQUIRE_IOSTREAM
//#include <maya/MIOStream.h>

#include "../shared/zWorks.h"
#include "meshCacheNode.h"
#include <maya/MAnimControl.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include "../shared/zData.h"
#include "../shared/FMCFMesh.h"

////////////////////////////////////////
//
// cache node
//
////////////////////////////////////////

MTypeId     meshCacheNode::id( 0x00025255 );
MObject     meshCacheNode::input; 
MObject     meshCacheNode::frame;
MObject     meshCacheNode::outMesh;
MObject     meshCacheNode::aminframe; 
MObject     meshCacheNode::amaxframe;
MObject     meshCacheNode::aframestep; 

meshCacheNode::meshCacheNode() {}
meshCacheNode::~meshCacheNode() {}



MStatus meshCacheNode::compute( const MPlug& plug, MDataBlock& data )
{
	
	MStatus stat;
	
	MString path =  data.inputValue( input ).asString();
	
	double time = data.inputValue( frame ).asTime().value();
	int minfrm = data.inputValue( aminframe ).asInt();
	//int maxfrm = data.inputValue( amaxframe ).asInt();
	int frmstep = data.inputValue( aframestep ).asInt();
	
	if( time < minfrm ) time = minfrm;
		
	int frame_lo = minfrm + int(time-minfrm)/frmstep*frmstep;
	int frame_hi = frame_lo+frmstep;

	char filename[256];
	sprintf( filename, "%s.%d.mcf", path.asChar(), frame_lo );
	
	FMCFMesh mesh;
	if(mesh.load(filename) != 1)
	{
		sprintf( filename, "%s.mcf", path.asChar());
		if(mesh.load(filename) != 1)
		{
			MGlobal::displayError( MString("Failed to open file: ") + filename );
			return MS::kFailure;
		}
	}
	
	int lo_n_vertex = mesh.getNumVertex();

	vertexArray.clear();
	vertexFArray.clear();
	uArray.clear();
	vArray.clear();
	polygonCounts.clear();
	polygonConnects.clear();
	polygonUVs.clear();

	for(unsigned int i = 0; i < mesh.getNumFace(); i++ ) 
	{
		polygonCounts.append( mesh.getFaceCount(i) );
	}

	for(unsigned int i = 0; i < mesh.getNumFaceVertex(); i++) 
	{
		polygonConnects.append( mesh.getVertexId(i) );
		polygonUVs.append( mesh.getUVId(i) );
	}

	XYZ tp;
	for(unsigned int i = 0; i < mesh.getNumVertex(); i++) 
	{
		mesh.getVertex(tp, i);
		vertexArray.append( MPoint( tp.x, tp.y, tp.z ) );
	}

	for(unsigned int i = 0; i < mesh.getNumUV(); i++) 
	{
		uArray.append( mesh.getS(i) );
		vArray.append( mesh.getT(i) );
	}
	
	

	if( time > frame_lo ) 
	{
		sprintf( filename, "%s.%d.mcf", path.asChar(), frame_hi );

		if(mesh.load(filename) != 1)
		{
			MGlobal::displayError( MString("Failed to open file: ") + filename );
		}
		else if(mesh.getNumVertex() == lo_n_vertex)
		{
			XYZ tp;
			for(unsigned int i = 0; i < mesh.getNumVertex(); i++) 
			{
				mesh.getVertex(tp, i);
				vertexFArray.append( MPoint( tp.x, tp.y, tp.z ) );
			}
			
			double alpha = double(time-frame_lo) / (double)frmstep;

			for(unsigned int i = 0; i < mesh.getNumVertex(); i++) 
			{
				vertexArray[i] = vertexArray[i] + alpha * ( vertexFArray[i] - vertexArray[i] );
			}
		}
	}

	if( plug == outMesh ) 
	{
	
		MDataHandle meshh = data.outputValue(outMesh, &stat);
		
		MFnMeshData dataCreator;
		MObject outMeshData = dataCreator.create(&stat);

		int numVertex = vertexArray.length();
		int numPolygon = polygonCounts.length();
		
		MFnMesh meshFn;
		meshFn.create( numVertex, numPolygon, vertexArray, polygonCounts, polygonConnects, outMeshData, &stat );
		
			
		if( !stat ) 
		{
			char log[256];
			sprintf( log, "Failed to create mesh %s", filename );
			MGlobal::displayError( log );
			return MS::kFailure;
		}

		if( polygonUVs.length() != 0 ) 
		{
			meshFn.setUVs ( uArray, vArray );
			meshFn.assignUVs ( polygonCounts, polygonUVs );
		}

		meshh.set(outMeshData);
	    
		data.setClean(plug);

	} 
	else 
	{
		return MS::kUnknownParameter;
	}

		return MS::kSuccess;
}

void* meshCacheNode::creator()
{
	return new meshCacheNode();
}

MStatus meshCacheNode::initialize()
{
	MFnNumericAttribute numAttr;
	MStatus				stat;
	
	stat = zWorks::createTimeAttr(frame, "currentTime", "ct", 1.0);
	zCheckStatus(stat, "ERROR creating time");
	zCheckStatus(addAttribute(frame), "ERROR adding time");
	
	aminframe = numAttr.create( "minFrame", "mnf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aminframe );
	
	amaxframe = numAttr.create( "maxFrame", "mxf", MFnNumericData::kInt, 24 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( amaxframe );
	
	aframestep = numAttr.create( "frameStep", "fst", MFnNumericData::kInt, 1 );
	numAttr.setMin(1);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframestep );
	
	MFnTypedAttribute   stringAttr;
	input = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( input );
	
	MFnTypedAttribute   meshAttr;
	outMesh = meshAttr.create( "outMesh", "o", MFnData::kMesh ); 
	meshAttr.setStorable(false);
	meshAttr.setWritable(false);
	addAttribute( outMesh );
    
	attributeAffects( input, outMesh );
	attributeAffects( frame, outMesh );
	attributeAffects( aminframe, outMesh );
	attributeAffects( amaxframe, outMesh );
	attributeAffects( aframestep, outMesh );

	return MS::kSuccess;
}

//:~
