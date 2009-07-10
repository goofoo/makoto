/// :
// meshCacheCmd.cpp
// Version 2.0.0
// Mesh cache file writer
// updated: 01/05/09
//
#include "../shared/zWorks.h"
#include "meshCacheCmd.h"
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

vxCache::vxCache() {}

vxCache::~vxCache() {}

MSyntax vxCache::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-n", "-name", MSyntax::kString);
	syntax.addFlag("-w", "-world", MSyntax::kBoolean);
	syntax.addFlag("-sg", "-single", MSyntax::kBoolean);
	
	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus vxCache::doIt( const MArgList& args ) 
{

	MStatus status = parseArgs( args );
	
	if( status != MS::kSuccess ) return status;
	
	MArgDatabase argData(syntax(), args);
	
	MAnimControl timeControl;
	MTime time = timeControl.currentTime();
	int frame =int(time.value());

	MString proj;
	MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), proj );
	
	MSelectionList selList;
	MGlobal::getActiveSelectionList ( selList );
	MItSelectionList iter( selList );

	MString cache_path = proj + "/data/";
	MString cache_name;
	MString scene_name = "untitled";
	worldSpace = false;
	
	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);
	if (argData.isFlagSet("-w")) argData.getFlagArgument("-w", 0, worldSpace);

	for ( ; !iter.isDone(); iter.next() )
	{								
		MDagPath meshPath;		
		iter.getDagPath( meshPath );
		
		meshPath.extendToShape();
		
		MObject meshObj = meshPath.node();
	
		MString surface = meshPath.partialPathName();
	
		zWorks::validateFilePath(surface);

		char filename[512];

		cache_name = surface;
		
		if(argData.isFlagSet("-sg")) sprintf( filename, "%s/%s.mcf", cache_path.asChar(), cache_name.asChar() );
		else sprintf( filename, "%s/%s.%d.mcf", cache_path.asChar(), cache_name.asChar(), frame );

		MDagPath surfDag;

		if ( meshPath.hasFn(MFn::kMesh)) 
		{
			writeMesh(filename, meshPath, meshObj);
			MGlobal::displayInfo ( MString("vxCache writes ") + filename);
		}
		else
			MGlobal::displayError ( surface + "- Cannot find mesh to write!" );
	}

	if ( selList.length() == 0 )
	{
		MGlobal:: displayError ( "Nothing is selected!" );
		return MS::kSuccess;
	}
	

	
 return MS::kSuccess;
 }

// write a normal mesh
//
MStatus vxCache::writeMesh(const char* filename, MDagPath meshDag, const MObject& meshObj)
{	
	struct meshInfo mesh;

	MString uvset("map1");
	
	MStatus status;
	MFnMesh meshFn(meshDag, &status );
	MItMeshPolygon faceIter( meshDag, MObject::kNullObj, &status );
	MItMeshVertex vertIter(meshDag, MObject::kNullObj, &status);
	MItMeshEdge edgeIter(meshDag, MObject::kNullObj, &status);
	
	mesh.numPolygons = meshFn.numPolygons();
	mesh.numVertices = meshFn.numVertices();
	mesh.numFaceVertices = meshFn.numFaceVertices();
	mesh.numUVs = meshFn.numUVs(uvset, &status);
	mesh.skip_interreflection = mesh.skip_scattering = 0;
	
	if(zWorks::hasNamedAttribute(meshObj, "_prt_ig_intr") == 1) mesh.skip_interreflection = 1;
	if(zWorks::hasNamedAttribute(meshObj, "_prt_ig_scat") == 1) mesh.skip_scattering = 1;

	//zWorks::displayIntParam("N Face", mesh.numPolygons);
	//zWorks::displayIntParam("N Vertex", mesh.numVertices);
	//zWorks::displayIntParam("N Facevertex", mesh.numFaceVertices);
	//zWorks::displayIntParam("N UV", mesh.numUVs);
	
	int *fcbuf = new int[mesh.numPolygons];
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		fcbuf[ faceIter.index() ] = faceIter.polygonVertexCount();
	}

	int* vertex_id = new int[mesh.numFaceVertices];
	int* uv_id = new int[mesh.numFaceVertices];
// output face loop
	int acc = 0;
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		for( unsigned int i=0; i < vexlist.length(); i++ ) 
		{
			vertex_id[acc] = vexlist[i];
			faceIter.getUVIndex ( i, uv_id[acc] );
			acc++;
		}
	}
	
// output vertices
	MPointArray pArray;
	
	if(worldSpace) meshFn.getPoints ( pArray, MSpace::kWorld);
	else meshFn.getPoints ( pArray, MSpace::kObject );
	
	XYZ *pbuf = new XYZ[pArray.length()];
	
	for( unsigned int i=0; i<pArray.length(); i++) 
	{
		pbuf[i].x = pArray[i].x;
		pbuf[i].y = pArray[i].y;
		pbuf[i].z= pArray[i].z;
	}

//output texture coordinate
	MFloatArray uArray, vArray;
	meshFn.getUVs ( uArray, vArray, &uvset );

	double* ubuf = new double[mesh.numUVs];
	double* vbuf = new double[mesh.numUVs];
	
	for( unsigned int i=0; i<uArray.length(); i++) 
	{
		ubuf[i] = uArray[i];
		vbuf[i] = vArray[i];
	}

	XYZ *norbuf = new XYZ[mesh.numVertices];
	vertIter.reset();
	MVector tnor;
	
	for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		if(worldSpace) vertIter.getNormal(tnor, MSpace::kWorld);
		else vertIter.getNormal(tnor, MSpace::kObject);
		tnor.normalize();
		norbuf[i].x = tnor.x;
		norbuf[i].y = tnor.y;
		norbuf[i].z = tnor.z;
	}

	MStatus hasAttr;
	MString sColorSet("set_prt_attr");
	meshFn.numColors( sColorSet,  &hasAttr );
	
	XYZ *colbuf = new XYZ[mesh.numVertices];
	vertIter.reset();
	if(hasAttr)
	{
		MColor col;
		for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
		{
			MIntArray conn_face;
			vertIter.getConnectedFaces(conn_face);
			vertIter.getColor(col, conn_face[0], &sColorSet);
			colbuf[i].x = col.r;
			colbuf[i].y = col.g;
			colbuf[i].z = col.b;
		}
	}
	else
	{
		for( unsigned int i=0; i<vertIter.count(); i++ ) colbuf[i] = XYZ(1.0f);
	}
	
	vertIter.reset();
	XYZ *vsbuf = new XYZ[mesh.numVertices];
	for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		MIntArray conn_face, conn_edge;
		vertIter.getConnectedFaces(conn_face);
		vertIter.getConnectedEdges(conn_edge);
		MPoint Q;
		for(unsigned j=0; j<conn_face.length(); j++)
		{
			int pre_id;
			faceIter.setIndex(conn_face[j],pre_id);
			Q += faceIter.center(MSpace::kWorld);
		}
		Q = Q/(double)conn_face.length();
		
		MPoint R;
		for(unsigned j=0; j<conn_edge.length(); j++)
		{
			int pre_id;
			edgeIter.setIndex(conn_edge[j], pre_id);
			R += edgeIter.center(MSpace::kWorld);
		}
		R = R/(double)conn_edge.length();
		
		MPoint S = vertIter.position(MSpace::kWorld);
		
		int nv = conn_edge.length();
		MPoint nS = (Q + R*2 + S*(nv-3))/nv;

		vsbuf[i].x = nS.x;
		vsbuf[i].y = nS.y;
		vsbuf[i].z = nS.z;
	}
	
	XYZ *tangbuf = new XYZ[mesh.numVertices];
	
	vertIter.reset();
	for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		MIntArray conn_face;
		MVector tang(0,0,0);
		vertIter.getConnectedFaces(conn_face);
		
		//for(int j = 0; j<conn_face.length(); j++) 
		{
			MVector ttang;
			meshFn.getFaceVertexTangent (conn_face[0], i,  ttang,  MSpace::kWorld, &uvset);
			tang += ttang;
		}
		tang.normalize();
		tangbuf[i].x = tang.x;
		tangbuf[i].y = tang.y;
		tangbuf[i].z = tang.z;
		
		tangbuf[i] = norbuf[i].cross(tangbuf[i]);
		tangbuf[i].normalize();
	}
	
	FMCFMesh fmesh;
	fmesh.save(mesh.numVertices, 
			mesh.numFaceVertices, 
			mesh.numPolygons, 
			mesh.numUVs, 
			mesh.skip_interreflection,
			mesh.skip_scattering,
			fcbuf, 
			vertex_id, 
			uv_id,
			pbuf,
			vsbuf,
			norbuf,
			tangbuf,
			colbuf,
			ubuf,
			vbuf,
			filename);
			
	delete[] fcbuf;
	delete[] vertex_id;
	delete[] uv_id;
	delete[] pbuf;
	delete[] vsbuf;
	delete[] norbuf;
	delete[] tangbuf;
	delete[] colbuf;
	delete[] ubuf;
	delete[] vbuf;

	return MS::kSuccess;
}
 
 
 void* vxCache::creator() {
 return new vxCache;
 }
 
MStatus vxCache::parseArgs( const MArgList& args )
{
	// Parse the arguments.
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}
//:~