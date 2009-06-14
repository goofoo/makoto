/*
 *  xmlMeshCacheCmd.cpp
 *  vertexCache
 *
 *  Created by zhang on 08-10-9.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "hairBaseCmd.h"
#include "../shared/zWorks.h"
#include <maya/MAnimControl.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnTransform.h>
#include "../shared/zData.h"
#include "../shared/zGlobal.h"
#include "FXMLTriangleMap.h"

////////////////////////////////////////
//
// cache file generation Command Class
//
////////////////////////////////////////
 
  
xmlMeshCache::xmlMeshCache() {}

xmlMeshCache::~xmlMeshCache() {}

MSyntax xmlMeshCache::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-n", "-name", MSyntax::kString);
	syntax.addFlag("-r", "-reference", MSyntax::kLong);
	//syntax.addFlag("-t", "-title", MSyntax::kString);
	//syntax.addFlag("-s", "-scene", MSyntax::kBoolean);
	
	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus xmlMeshCache::doIt( const MArgList& args ) 
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
	frame_reference = 0;
	
	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);
	if (argData.isFlagSet("-r")) argData.getFlagArgument("-r", 0, frame_reference);
		
		char file_name[512];
	    
		sprintf( file_name, "%s/%s.%d.xhb", cache_path.asChar(), scene_name.asChar(), frame );

		m_n_triangle = 0;
		m_n_vertex = 0;
		m_area = 0;
		
		iter.reset();
		for ( ; !iter.isDone(); iter.next() )
		{	
			MDagPath meshPath;		
			iter.getDagPath( meshPath );
			meshPath.extendToShape();
			if ( meshPath.hasFn(MFn::kMesh)) 
			{				
				appendTriangles(meshPath);
			}
		}

		if(m_n_triangle>0) saveTriangles(file_name, frame);


	if ( selList.length() == 0 )
	{
		MGlobal:: displayError ( "Nothing is selected!" );
		return MS::kSuccess;
	}
	

	
 return MS::kSuccess;
 }
 
 
 void* xmlMeshCache::creator() {
 return new xmlMeshCache;
 }
 
MStatus xmlMeshCache::parseArgs( const MArgList& args )
{
	// Parse the arguments.
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}
/*append triangles from selected mesh*/
void xmlMeshCache::appendTriangles(MDagPath meshDag)
{
	MStatus status;
	MFnMesh meshFn(meshDag, &status );
	MItMeshPolygon faceIter( meshDag, MObject::kNullObj, &status );
	MItMeshVertex vertIter(meshDag, MObject::kNullObj, &status);
	MItMeshEdge edgeIter(meshDag, MObject::kNullObj, &status);
	
	MPointArray cvs;
	meshFn.getPoints(cvs,  MSpace::kWorld);
	
	XYZ a,b,c;
	float tri_a;
	// output face loop
	//
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray vexlist;
		faceIter.getVertices ( vexlist );
		m_n_triangle += vexlist.length() - 2;
		for( unsigned int i=1; i < vexlist.length()-1; i++ ) 
		{
			m_vertex_id.append(vexlist[0] + m_n_vertex);
			m_vertex_id.append(vexlist[i] + m_n_vertex);
			m_vertex_id.append(vexlist[i+1] + m_n_vertex);
			
			a.x = cvs[vexlist[0]].x;
			a.y = cvs[vexlist[0]].y;
			a.z = cvs[vexlist[0]].z;
			
			b.x = cvs[vexlist[i]].x;
			b.y = cvs[vexlist[i]].y;
			b.z = cvs[vexlist[i]].z;
			
			c.x = cvs[vexlist[i+1]].x;
			c.y = cvs[vexlist[i+1]].y;
			c.z = cvs[vexlist[i+1]].z;
			
			tri_a = FTriangle::calculate_area(a, b, c);
			m_face_area.append(tri_a);
		}
		double area;
		faceIter.getArea( area,  MSpace::kWorld );
		m_area += area;
	}
	
	vertIter.reset();
	MVector tnor, tang, ttang;
	MPoint tpos;
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		MIntArray conn_face, conn_edge;
		vertIter.getConnectedFaces(conn_face);
		vertIter.getConnectedEdges(conn_edge);
		MPoint Q;
		for(int j=0; j<conn_face.length(); j++)
		{
			int pre_id;
			faceIter.setIndex(conn_face[j],pre_id);
			Q += faceIter.center(MSpace::kWorld);
		}
		Q = Q/(double)conn_face.length();
		
		MPoint R;
		int onbound = 0;
		for(int j=0; j<conn_edge.length(); j++)
		{
			int pre_id;
			edgeIter.setIndex(conn_edge[j], pre_id);
			if(edgeIter.onBoundary()) onbound = 1;
			R += edgeIter.center(MSpace::kWorld);
		}
		R = R/(double)conn_edge.length();
		
		MPoint S = vertIter.position(MSpace::kWorld);
		
		int nv = conn_edge.length();
		MPoint nS = (Q + R*2 + S*(nv-3))/nv;
		
		if(onbound == 1) nS = S;
		
		m_vertex_p.append(nS);
		
		vertIter.getNormal(tnor, MSpace::kWorld);
		tnor.normalize();
		m_vertex_n.append(tnor);
		
		tang = MVector(0,0,0);
		
		for(int j=0; j<conn_face.length(); j++)
		{
			meshFn.getFaceVertexTangent (conn_face[j], i, ttang,  MSpace::kWorld);
			ttang.normalize();
			tang += ttang;
		}
		tang /= conn_face.length();
		tang.normalize();
		
		tang = tnor^tang;
		tang.normalize();
		
		m_vertex_t.append(tang);
	}

	m_n_vertex += meshFn.numVertices();
}

/*save triangle map*/
void xmlMeshCache::saveTriangles(const char* filename, int frame)
{
	FXMLTriangleMap fmap;
	fmap.beginMap(filename);
	
// write vertex id
	string static_path(filename);
	zGlobal::cutByFirstDot(static_path);
	static_path.append(".hbs");
	
	if(frame == frame_reference)
	{
		fmap.staticBegin(static_path.c_str(), m_n_triangle);
		
		float* a_list = new float[m_n_triangle];
		for(int i=0; i<m_n_triangle; i++) a_list[i] = m_face_area[i];
		
		fmap.addFaceArea(m_n_triangle, a_list);
		delete[] a_list;
	
		int* id_list = new int[m_n_triangle*3];
		for(int i=0; i<m_n_triangle*3; i++) id_list[i] = m_vertex_id[i];
	
		fmap.addVertexId(m_n_triangle*3, id_list);
		delete[] id_list;
		
		XYZ* recpref = new XYZ[m_n_vertex];
	
		for(int i=0; i<m_n_vertex; i++) recpref[i] = XYZ(m_vertex_p[i].x, m_vertex_p[i].y, m_vertex_p[i].z);
		fmap.addPref(m_n_vertex, recpref);
		delete[] recpref;
	
		fmap.staticEnd();
	}
	else
	{
		fmap.staticBeginNoWrite(static_path.c_str(), m_n_triangle);
	
		fmap.addFaceArea(m_n_triangle);
		fmap.addVertexId(m_n_triangle*3);
		fmap.addPref(m_n_vertex);
		fmap.staticEndNoWrite();
	}

// write vertex p
	string dynamic_path(filename);
	zGlobal::cutByLastDot(dynamic_path);
	dynamic_path.append(".hbd");
	
	fmap.dynamicBegin(dynamic_path.c_str());
	
	XYZ* recp = new XYZ[m_n_vertex];
	
	for(int i=0; i<m_n_vertex; i++) recp[i] = XYZ(m_vertex_p[i].x, m_vertex_p[i].y, m_vertex_p[i].z);
	fmap.addP(m_n_vertex, recp);
	delete[] recp;
	
// write vertex n
	XYZ* recn = new XYZ[m_n_vertex];
	
	for(int i=0; i<m_n_vertex; i++) recn[i] = XYZ(m_vertex_n[i].x, m_vertex_n[i].y, m_vertex_n[i].z);
	fmap.addN(m_n_vertex, recn);
	delete[] recn;
	
// write veretx tangent
	XYZ* rect = new XYZ[m_n_vertex];
	
	for(int i=0; i<m_n_vertex; i++) rect[i] = XYZ(m_vertex_t[i].x, m_vertex_t[i].y, m_vertex_t[i].z);
	fmap.addTangent(m_n_vertex, rect);
	delete[] rect;

// write bound box	
	MPoint corner_l(10e6, 10e6, 10e6);
	MPoint corner_h(-10e6, -10e6, -10e6);
	
	for( unsigned int i=0; i<m_n_vertex; i++) 
	{
		if( m_vertex_p[i].x < corner_l.x ) corner_l.x = m_vertex_p[i].x;
		if( m_vertex_p[i].y < corner_l.y ) corner_l.y = m_vertex_p[i].y;
		if( m_vertex_p[i].z < corner_l.z ) corner_l.z = m_vertex_p[i].z;
		if( m_vertex_p[i].x > corner_h.x ) corner_h.x = m_vertex_p[i].x;
		if( m_vertex_p[i].y > corner_h.y ) corner_h.y = m_vertex_p[i].y;
		if( m_vertex_p[i].z > corner_h.z ) corner_h.z = m_vertex_p[i].z;
	}
	
	fmap.addBBox(corner_l.x, corner_l.y, corner_l.z, corner_h.x, corner_h.y, corner_h.z);
	
	fmap.dynamicEnd();
	
	fmap.endMap(filename);
	
	char info[256];
	sprintf(info, "hairBase writes %i triangles to scene %s", m_n_triangle, filename);
	MGlobal::displayInfo ( info );
}
//:~