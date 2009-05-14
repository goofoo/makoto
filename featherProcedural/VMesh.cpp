/*
 *  VMesh.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "VMesh.h"
#include "FBend.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include <maya/MItMeshVertex.h>
#include "FXMLTriangleMap.h"
#include "../shared/zGlobal.h"
#include <maya/MFnMeshData.h>
#include <maya/MItMeshPolygon.h>

VMesh::VMesh():has_mesh(0),m_scale(1.0f),m_width(0.25f)
{
	fb = new FBend();
}
VMesh::~VMesh() {}

void VMesh::setGrow(const MObject& mesh)
{
	pgrow = mesh;
	has_mesh = 1;
}

void VMesh::getBBox(MPoint& corner1, MPoint& corner2)
{
	if(!has_mesh) return;
	
	corner1 = MPoint(10e6, 10e6, 10e6);
	corner2 = MPoint(-10e6, -10e6, -10e6);
	
	MStatus status;
	MItMeshVertex vertIter(pgrow, &status);
	
	MPoint S;
	for(; !vertIter.isDone(); vertIter.next())
	{
		S = vertIter.position(MSpace::kWorld);
		if( S.x < corner1.x ) corner1.x = S.x;
		if( S.y < corner1.y ) corner1.y = S.y;
		if( S.z < corner1.z ) corner1.z = S.z;
		if( S.x > corner2.x ) corner2.x = S.x;
		if( S.y > corner2.y ) corner2.y = S.y;
		if( S.z > corner2.z ) corner2.z = S.z;
	}
}

void VMesh::init()
{
	m_p_buf.clear();
	m_v_buf.clear();
	
	MStatus status;
	MItMeshVertex vertIter(pgrow, &status);
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		m_p_buf.append( vertIter.position(MSpace::kWorld) );
	}
	
	m_v_buf.setLength(m_p_buf.length());
	for( int i=0; i<m_v_buf.length(); i++ )
	{
		m_v_buf[i] = MVector(0,0,0);
	}
}

void VMesh::update()
{
	MStatus status;
	MItMeshVertex vertIter(pgrow, &status);
	MPoint cv;
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		cv = vertIter.position(MSpace::kWorld);
		
		m_v_buf[i] = m_p_buf[i] - cv;
		
		m_p_buf[i] = cv;
	}
}

void VMesh::draw()
{
	if(!has_mesh) return;
	
	MStatus status;
	MFnMesh meshFn(pgrow, &status );
	MItMeshVertex vertIter(pgrow, &status);
	
	MPoint S;
	MVector N, tang, ttang, binormal, dir, hair_up;
	MColor Cscale, Cerect, Crotate, Ccurl;
	float rot;
	MATRIX44F hair_space;
	
	MString setScale("fb_scale");
	MString setErect("fb_erect");
	MString setRotate("fb_rotate");
	MString setCurl("fb_curl");
	MIntArray conn_face;
	glBegin(GL_LINES);
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		//S = vertIter.position(MSpace::kWorld);
		//glVertex3f(S.x, S.y, S.z);
		
		vertIter.getNormal(N, MSpace::kWorld);
		N.normalize();
		
		vertIter.getColor(Cscale, &setScale);
		vertIter.getColor(Cerect, &setErect);
		vertIter.getColor(Crotate, &setRotate);
		vertIter.getColor(Ccurl, &setCurl);
		
		vertIter.getConnectedFaces(conn_face);
		tang = MVector(0,0,0);
		for(int j=0; j<conn_face.length(); j++)
		{
			meshFn.getFaceVertexTangent (conn_face[j], i, ttang,  MSpace::kWorld);
			ttang.normalize();
			tang += ttang;
		}
		tang /= conn_face.length();
		conn_face.clear();
		tang.normalize();
		tang = N^tang;
		tang.normalize();
		
		binormal = N^tang;
		
		if(Crotate.r<0.5)
		{
			rot = (0.5 - Crotate.r)*2;
			tang = tang + (binormal-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		else
		{
			rot = (Crotate.r-0.5)*2;
			tang = tang + (binormal*-1-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		
		dir = tang + (N - tang)*Cerect.r;
		dir.normalize();
		
		//S = S+dir*Cscale.r*m_scale;
		//glVertex3f(S.x, S.y, S.z);
		
		hair_up = dir^binormal;
		
		hair_space.setIdentity();
		hair_space.setOrientations(XYZ(binormal.x, binormal.y, binormal.z), XYZ(hair_up.x, hair_up.y, hair_up.z), XYZ(dir.x, dir.y, dir.z));
		
		S = vertIter.position(MSpace::kWorld);
		
		hair_space.setTranslation(XYZ(S.x, S.y, S.z));
		
		fb->create(Cscale.r*m_scale, 0, Cscale.r*m_scale*(Ccurl.r-0.5)*2);
		
		XYZ pw;
	
		for(int j=0; j<NUMBENDSEG; j++)
		{
			fb->getPoint(j, pw);
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
			
			fb->getPoint(j+1, pw);
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
		}

		
	}
	glEnd();
	
}

MObject VMesh::createFeather()
{
	MStatus stat;
	MFnMeshData dataCreator;
	MObject outMeshData = dataCreator.create(&stat);
	
	int numVertex = 0;
	int numPolygon = 0;
	MPointArray vertexArray;
	MFloatArray uArray, vArray;
	MIntArray polygonCounts, polygonConnects, polygonUVs;

	MFnMesh meshFn(pgrow, &stat);
	MItMeshVertex vertIter(pgrow, &stat);
	MItMeshPolygon faceIter(pgrow,  &stat);
	
	MPoint S;
	MVector N, tang, ttang, binormal, dir, hair_up;
	MColor Cscale, Cerect, Crotate, Ccurl, Cwidth;
	float rot, lengreal;
	MATRIX44F hair_space;
	
	MString setScale("fb_scale");
	MString setErect("fb_erect");
	MString setRotate("fb_rotate");
	MString setCurl("fb_curl");
	MString setWidth("fb_width");
	MIntArray conn_face;
	
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		if(vertIter.onBoundary()) continue;
		vertIter.getNormal(N, MSpace::kWorld);
		N.normalize();
		
		vertIter.getColor(Cscale, &setScale);
		vertIter.getColor(Cerect, &setErect);
		vertIter.getColor(Crotate, &setRotate);
		vertIter.getColor(Ccurl, &setCurl);
		vertIter.getColor(Cwidth, &setWidth);
		
		vertIter.getConnectedFaces(conn_face);
		tang = MVector(0,0,0);
		for(int j=0; j<conn_face.length(); j++)
		{
			meshFn.getFaceVertexTangent (conn_face[j], i, ttang,  MSpace::kWorld);
			ttang.normalize();
			tang += ttang;
		}
		tang /= conn_face.length();
		conn_face.clear();
		tang.normalize();
		tang = N^tang;
		tang.normalize();
		
		binormal = N^tang;
		
		if(Crotate.r<0.5)
		{
			rot = (0.5 - Crotate.r)*2;
			tang = tang + (binormal-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		else
		{
			rot = (Crotate.r-0.5)*2;
			tang = tang + (binormal*-1-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		
		dir = tang + (N - tang)*Cerect.r;
		dir.normalize();
		
		//S = S+dir*Cscale.r*m_scale;
		//glVertex3f(S.x, S.y, S.z);
		
		hair_up = dir^binormal;
		
		hair_space.setIdentity();
		hair_space.setOrientations(XYZ(binormal.x, binormal.y, binormal.z), XYZ(hair_up.x, hair_up.y, hair_up.z), XYZ(dir.x, dir.y, dir.z));
		
		S = vertIter.position(MSpace::kWorld);
		
		hair_space.setTranslation(XYZ(S.x, S.y, S.z));
		
		lengreal = Cscale.r*m_scale;
		
		fb->create(lengreal, 0, lengreal*(Ccurl.r-0.5)*2);
		
		XYZ pw;
		MPoint pvx;
		
		MPoint pright = S + binormal*Cwidth.r*m_width*lengreal;
		MPoint pleft = S - binormal*Cwidth.r*m_width*lengreal;
		
		MPoint phit;
		int polyhit;
		meshFn.getClosestPoint (pright,  phit,  MSpace::kObject, &polyhit);
		MVector topright = phit - S;
		topright.normalize();
		topright *= Cwidth.r*m_width*lengreal;
		
		meshFn.getClosestPoint (pleft,  phit,  MSpace::kObject, &polyhit);
		MVector topleft = phit - S;
		topleft.normalize();
		topleft *= Cwidth.r*m_width*lengreal;
		
		//tws_binormal  = binormal*cos(0.2) + hair_up*sin(0.2);
	
		for(int j=0; j<NUMBENDSEG+1; j++)
		{
			fb->getPoint(j, pw);
			hair_space.transform(pw);
			
			pvx = MPoint(pw.x, pw.y, pw.z) + topleft;//- tws_binormal*Cwidth.r*m_width*lengreal;
			vertexArray.append(pvx);
			pvx = MPoint(pw.x, pw.y, pw.z);
			vertexArray.append(pvx);
			pvx = MPoint(pw.x, pw.y, pw.z) + topright;//tws_binormal*Cwidth.r*m_width*lengreal;
			vertexArray.append(pvx);
			
			uArray.append(0.0);
			vArray.append((float)j/NUMBENDSEG);
			
			uArray.append(0.5);
			vArray.append((float)j/NUMBENDSEG);
			
			uArray.append(1.0);
			vArray.append((float)j/NUMBENDSEG);
		}

		for(int j=0; j<NUMBENDSEG; j++)
		{
			polygonConnects.append(j*3 + numVertex);
			polygonConnects.append(j*3+3 + numVertex);
			polygonConnects.append(j*3+4 + numVertex);
			polygonConnects.append(j*3+1 + numVertex);
			
			polygonCounts.append(4);
			
			polygonConnects.append(j*3+1 + numVertex);
			polygonConnects.append(j*3+4 + numVertex);
			polygonConnects.append(j*3+5 + numVertex);
			polygonConnects.append(j*3+2 + numVertex);
			
			polygonCounts.append(4);
			
			polygonUVs.append(j*3 + numVertex);
			polygonUVs.append(j*3+3 + numVertex);
			polygonUVs.append(j*3+4 + numVertex);
			polygonUVs.append(j*3+1 + numVertex);
			
			polygonUVs.append(j*3+1 + numVertex);
			polygonUVs.append(j*3+4 + numVertex);
			polygonUVs.append(j*3+5 + numVertex);
			polygonUVs.append(j*3+2 + numVertex);
		}
		
		numVertex += (NUMBENDSEG+1)*3;
		numPolygon += NUMBENDSEG*2;
	}
	
	MIntArray connvert; 
	int idxpre;
	float averg_scale, averg_erect, averg_rotate, averg_curl, averg_width;
	for( int i=0; !faceIter.isDone(); faceIter.next(), i++ )
	{
		if(faceIter.onBoundary()) continue;
		faceIter.getNormal(N, MSpace::kWorld);
		N.normalize();
		
		faceIter.getVertices(connvert);
		
		averg_scale=averg_erect=averg_rotate=averg_curl=0;
		for(int j=0; j<connvert.length(); j++)
		{
			vertIter.setIndex(connvert[j], idxpre);
			
			vertIter.getColor(Cscale, &setScale);
			vertIter.getColor(Cerect, &setErect);
			vertIter.getColor(Crotate, &setRotate);
			vertIter.getColor(Ccurl, &setCurl);
			vertIter.getColor(Cwidth, &setWidth);
			
			averg_scale += Cscale.r;
			averg_erect += Cerect.r;
			averg_rotate += Crotate.r;
			averg_curl += Ccurl.r;
			averg_width += Cwidth.r;
		}
		
		averg_scale /= connvert.length();
		averg_erect /= connvert.length();
		averg_rotate /= connvert.length();
		averg_curl /= connvert.length();
		averg_width /= connvert.length();
		
		tang = MVector(0,0,0);
		for(int j=0; j<connvert.length(); j++)
		{
			meshFn.getFaceVertexTangent (i, connvert[j], ttang,  MSpace::kWorld);
			ttang.normalize();
			tang += ttang;
		}
		//tang /= conn_face.length();
		connvert.clear();
		tang.normalize();
		tang = N^tang;
		tang.normalize();
		
		binormal = N^tang;
		
		if(averg_rotate<0.5)
		{
			rot = (0.5 - averg_rotate)*2;
			tang = tang + (binormal-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		else
		{
			rot = (averg_rotate-0.5)*2;
			tang = tang + (binormal*-1-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		
		dir = tang + (N - tang)*averg_erect;
		dir.normalize();
		
		//S = S+dir*Cscale.r*m_scale;
		//glVertex3f(S.x, S.y, S.z);
		
		hair_up = dir^binormal;
		
		hair_space.setIdentity();
		hair_space.setOrientations(XYZ(binormal.x, binormal.y, binormal.z), XYZ(hair_up.x, hair_up.y, hair_up.z), XYZ(dir.x, dir.y, dir.z));
		
		S = faceIter.center(MSpace::kWorld);
		
		hair_space.setTranslation(XYZ(S.x, S.y, S.z));
		
		lengreal = Cscale.r*m_scale;
		
		fb->create(lengreal, 0, lengreal*(averg_curl-0.5)*2);
		
		MPoint pright = S + binormal*Cwidth.r*m_width*lengreal;
		MPoint pleft = S - binormal*Cwidth.r*m_width*lengreal;
		
		XYZ pw;
		MPoint pvx;
		
		MPoint phit;
		int polyhit;
		meshFn.getClosestPoint (pright,  phit,  MSpace::kObject, &polyhit);
		MVector topright = phit - S;
		topright.normalize();
		topright *= Cwidth.r*m_width*lengreal;
		
		meshFn.getClosestPoint (pleft,  phit,  MSpace::kObject, &polyhit);
		MVector topleft = phit - S;
		topleft.normalize();
		topleft *= Cwidth.r*m_width*lengreal;
		//tws_binormal  = binormal*cos(0.2) + hair_up*sin(0.2);
	
		for(int j=0; j<NUMBENDSEG+1; j++)
		{
			fb->getPoint(j, pw);
			hair_space.transform(pw);
			
			pvx = MPoint(pw.x, pw.y, pw.z) + topleft;//- tws_binormal*averg_width*m_width*lengreal;
			vertexArray.append(pvx);
			pvx = MPoint(pw.x, pw.y, pw.z);
			vertexArray.append(pvx);
			pvx = MPoint(pw.x, pw.y, pw.z) + topright;//tws_binormal*averg_width*m_width*lengreal;
			vertexArray.append(pvx);
			
			uArray.append(0.0);
			vArray.append((float)j/NUMBENDSEG);
			
			uArray.append(0.5);
			vArray.append((float)j/NUMBENDSEG);
			
			uArray.append(1.0);
			vArray.append((float)j/NUMBENDSEG);
		}

		for(int j=0; j<NUMBENDSEG; j++)
		{
			polygonConnects.append(j*3 + numVertex);
			polygonConnects.append(j*3+3 + numVertex);
			polygonConnects.append(j*3+4 + numVertex);
			polygonConnects.append(j*3+1 + numVertex);
			
			polygonCounts.append(4);
			
			polygonConnects.append(j*3+1 + numVertex);
			polygonConnects.append(j*3+4 + numVertex);
			polygonConnects.append(j*3+5 + numVertex);
			polygonConnects.append(j*3+2 + numVertex);
			
			polygonCounts.append(4);
			
			polygonUVs.append(j*3 + numVertex);
			polygonUVs.append(j*3+3 + numVertex);
			polygonUVs.append(j*3+4 + numVertex);
			polygonUVs.append(j*3+1 + numVertex);
			
			polygonUVs.append(j*3+1 + numVertex);
			polygonUVs.append(j*3+4 + numVertex);
			polygonUVs.append(j*3+5 + numVertex);
			polygonUVs.append(j*3+2 + numVertex);
		}
		
		numVertex += (NUMBENDSEG+1)*3;
		numPolygon += NUMBENDSEG*2;
		
	}

	
	MFnMesh meshCreateFn;
	meshCreateFn.create( numVertex, numPolygon, vertexArray, polygonCounts, polygonConnects, outMeshData, &stat );
	
	meshCreateFn.setUVs ( uArray, vArray );
	meshCreateFn.assignUVs ( polygonCounts, polygonUVs );
	
	return outMeshData;
}

void VMesh::drawVelocity()
{
if(!has_mesh) return;
	MStatus status;
	MFnMesh meshFn(pgrow, &status );
if(m_v_buf.length() != meshFn.numVertices()) return;
	MItMeshVertex vertIter(pgrow, &status);
	
	MPoint S;
	MVector N, tang, ttang, binormal, dir, hair_up;
	MColor Cscale, Cerect, Crotate, Cstiff, Ccurl;
	float rot;
	MATRIX44F hair_space, hair_space_inv;
	XYZ dP;
	
	
	MString setScale("fb_scale");
	MString setErect("fb_erect");
	MString setRotate("fb_rotate");
	MString setStiff("fb_stiffness");
	MString setCurl("fb_curl");
	MIntArray conn_face;
	glBegin(GL_LINES);
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		S = vertIter.position(MSpace::kWorld);
		
		vertIter.getNormal(N, MSpace::kWorld);
		N.normalize();
		
		vertIter.getColor(Cscale, &setScale);
		vertIter.getColor(Cerect, &setErect);
		vertIter.getColor(Crotate, &setRotate);
		vertIter.getColor(Cstiff, &setStiff);
		vertIter.getColor(Ccurl, &setCurl);
		if(Cstiff.r < 0.1) Cstiff.r = 0.1;
		
		vertIter.getConnectedFaces(conn_face);
		tang = MVector(0,0,0);
		for(int j=0; j<conn_face.length(); j++)
		{
			meshFn.getFaceVertexTangent (conn_face[j], i, ttang,  MSpace::kWorld);
			ttang.normalize();
			tang += ttang;
		}
		tang /= conn_face.length();
		conn_face.clear();
		tang.normalize();
		tang = N^tang;
		tang.normalize();
		
		binormal = N^tang;
		
		if(Crotate.r<0.5)
		{
			rot = (0.5 - Crotate.r)*2;
			tang = tang + (binormal-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		else
		{
			rot = (Crotate.r-0.5)*2;
			tang = tang + (binormal*-1-tang)*rot;
			tang.normalize();
			binormal = N^tang;
		}
		
		dir = tang + (N - tang)*Cerect.r;
		dir.normalize();
		
		//S = S+dir*Cscale.r*m_scale;
		
		hair_up = dir^binormal;
		
		hair_space.setIdentity();
		hair_space.setOrientations(XYZ(binormal.x, binormal.y, binormal.z), XYZ(hair_up.x, hair_up.y, hair_up.z), XYZ(dir.x, dir.y, dir.z));
		
		S = vertIter.position(MSpace::kWorld);
		
		hair_space.setTranslation(XYZ(S.x, S.y, S.z));
		hair_space_inv = hair_space;
		hair_space_inv.inverse();
		
		dP = XYZ(m_v_buf[i].x, m_v_buf[i].y, m_v_buf[i].z);
		hair_space_inv.transformAsNormal(dP);
		
		if(dP.y<0) dP.y *= 0.3;
		dP *= m_wind/Cstiff.r;
		
		fb->create(Cscale.r*m_scale, dP.x, dP.y+Cscale.r*m_scale*(Ccurl.r-0.5)*2);
		
		XYZ pw;
	
		for(int j=0; j<NUMBENDSEG; j++)
		{
			fb->getPoint(j, pw);
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
			
			fb->getPoint(j+1, pw);
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
		}
		
	}
	glEnd();
}

void VMesh::save(const char* filename, int isStart)
{
	MStatus status;
	MFnMesh meshFn(pgrow, &status );
	MItMeshPolygon faceIter( pgrow, &status );
	MItMeshVertex vertIter(pgrow, &status);
	
	int m_n_triangle=0, m_n_vertex=0;
	MIntArray m_vertex_id;
	MFloatArray facevarying_s, facevarying_t;
	MPointArray m_vertex_p;
	MVectorArray m_vertex_n, m_vertex_t;
	
	meshFn.getPoints(m_vertex_p, MSpace::kWorld);
	float auv[2];
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray vexlist;
		faceIter.getVertices ( vexlist );
		m_n_triangle += vexlist.length() - 2;
		for( unsigned int i=1; i < vexlist.length()-1; i++ ) 
		{
			m_vertex_id.append(vexlist[0]);
			m_vertex_id.append(vexlist[i]);
			m_vertex_id.append(vexlist[i+1]);
			faceIter.getUV(0, auv);
			facevarying_s.append(auv[0]);
			facevarying_t.append(auv[1]);
			faceIter.getUV(i, auv);
			facevarying_s.append(auv[0]);
			facevarying_t.append(auv[1]);
			faceIter.getUV(i+1, auv);
			facevarying_s.append(auv[0]);
			facevarying_t.append(auv[1]);
		}
	}
	
	XYZ* pbinormal = new XYZ[meshFn.numVertices()];
	XYZ* pv = new XYZ[meshFn.numVertices()];
	float* pscale = new float[meshFn.numVertices()];
	float* curlarray = new float[meshFn.numVertices()];
	float* widtharray = new float[meshFn.numVertices()];
	float* densityarray = new float[meshFn.numVertices()];
	
	MVector tnor, tang, ttang, binormal, dir, hair_up, hair_v;
	MPoint tpos;
	MColor Cscale, Cerect, Crotate, Cstiff, Ccurl, Cwidth, Cdensity;
	float rot;
	MATRIX44F hair_space, hair_space_inv;
	
	MString setScale("fb_scale");
	MString setErect("fb_erect");
	MString setRotate("fb_rotate");
	MString setStiff("fb_stiffness");
	MString setCurl("fb_curl");
	MString setWidth("fb_width");
	MString setDensity("fb_density");
	MIntArray conn_face;
	
	XYZ dP;
	
	vertIter.reset();
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		vertIter.getConnectedFaces(conn_face);
		
		vertIter.getNormal(tnor, MSpace::kWorld);
		tnor.normalize();
		m_vertex_n.append(tnor);
		
		vertIter.getColor(Cscale, &setScale);
		vertIter.getColor(Cerect, &setErect);
		vertIter.getColor(Crotate, &setRotate);
		vertIter.getColor(Cstiff, &setStiff);
		vertIter.getColor(Ccurl, &setCurl);
		vertIter.getColor(Cwidth, &setWidth);
		vertIter.getColor(Cdensity, &setDensity);
		if(Cstiff.r < 0.1) Cstiff.r = 0.1;
		
		tang = MVector(0,0,0);
		
		for(int j=0; j<conn_face.length(); j++)
		{
			meshFn.getFaceVertexTangent (conn_face[j], i, ttang,  MSpace::kWorld);
			ttang.normalize();
			tang += ttang;
		}
		tang /= conn_face.length();
		conn_face.clear();
		tang.normalize();
		
		tang = tnor^tang;
		tang.normalize();
		
		binormal = tnor^tang;
		
		if(Crotate.r<0.5)
		{
			rot = (0.5 - Crotate.r)*2;
			tang = tang + (binormal-tang)*rot;
			tang.normalize();
			binormal = tnor^tang;
		}
		else
		{
			rot = (Crotate.r-0.5)*2;
			tang = tang + (binormal*-1-tang)*rot;
			tang.normalize();
			binormal = tnor^tang;
		}
		
		pbinormal[i] = XYZ(binormal.x, binormal.y, binormal.z);
		pscale[i] = Cscale.r * m_scale;
		
		dir = tang + (tnor - tang)*Cerect.r;
		dir.normalize();
		
		m_vertex_t.append(dir);
		
		hair_up = dir^binormal;
		//pup[i] = XYZ(hair_up.x, hair_up.y, hair_up.z);
		
		hair_space.setIdentity();
		hair_space.setOrientations(XYZ(binormal.x, binormal.y, binormal.z), XYZ(hair_up.x, hair_up.y, hair_up.z), XYZ(dir.x, dir.y, dir.z));
		hair_space.setTranslation(XYZ(m_vertex_p[i].x, m_vertex_p[i].y, m_vertex_p[i].z));
		hair_space_inv = hair_space;
		hair_space_inv.inverse();
		
		hair_v = getVelocity(i);
		dP = XYZ(hair_v.x, hair_v.y, hair_v.z);
		hair_space_inv.transformAsNormal(dP);
		if(dP.y<0) dP.y = 0;
		dP *= m_wind/Cstiff.r;
		
		pv[i] = dP;
		
		curlarray[i] = Ccurl.r;
		widtharray[i] = Cwidth.r;
		densityarray[i] = Cdensity.r;
	}

	m_n_vertex = meshFn.numVertices();
	
	FXMLTriangleMap fmap;
	fmap.beginMap(filename);
	
// write vertex id
	string static_path(filename);
	zGlobal::cutByFirstDot(static_path);
	static_path.append(".hbs");
	
	if(isStart == 1)
	{
		fmap.staticBegin(static_path.c_str(), m_n_triangle);
	
		int* id_list = new int[m_n_triangle*3];
		for(int i=0; i<m_n_triangle*3; i++) id_list[i] = m_vertex_id[i];
	
		fmap.addVertexId(m_n_triangle*3, id_list);
		delete[] id_list;
		
		float* sarray = new float[m_n_triangle*3];
		float* tarray = new float[m_n_triangle*3];
		for(int i=0; i<m_n_triangle*3; i++) 
		{
			sarray[i] = facevarying_s[i];
			tarray[i] = facevarying_t[i];
		}
		fmap.addStaticFloatArray("root_s", m_n_triangle*3, sarray);
		fmap.addStaticFloatArray("root_t", m_n_triangle*3, tarray);
		
		delete[] sarray;
		delete[] tarray;
		
		XYZ* recpref = new XYZ[m_n_vertex];
	
		for(int i=0; i<m_n_vertex; i++) recpref[i] = XYZ(m_vertex_p[i].x, m_vertex_p[i].y, m_vertex_p[i].z);
		fmap.addPref(m_n_vertex, recpref);
		delete[] recpref;
		
		fmap.addStaticFloatArray("scale", m_n_vertex, pscale);
		
		fmap.addStaticFloatArray("curl", m_n_vertex, curlarray);

		fmap.addStaticFloatArray("width", m_n_vertex, widtharray);
	
		fmap.addStaticFloatArray("density", m_n_vertex, densityarray);
	
		fmap.staticEnd();
	}
	else
	{
		fmap.staticBeginNoWrite(static_path.c_str(), m_n_triangle);
	
		fmap.addVertexId(m_n_triangle*3);
		fmap.addStaticFloatArray("root_s", m_n_triangle*3);
		fmap.addStaticFloatArray("root_t", m_n_triangle*3);
		fmap.addPref(m_n_vertex);
		fmap.addStaticFloatArray("scale", m_n_vertex);
		fmap.addStaticFloatArray("curl", m_n_vertex);
		fmap.addStaticFloatArray("width", m_n_vertex);
		fmap.addStaticFloatArray("density", m_n_vertex);
		
		fmap.staticEndNoWrite();
	}
	
	delete[] curlarray;
	delete[] widtharray;
	delete[] densityarray;

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
	fmap.addDynamicVectorArray("direction", m_n_vertex, rect);
	delete[] rect;
	
// write vertex binormal
	fmap.addDynamicVectorArray("binormal", m_n_vertex, pbinormal);
	delete[] pbinormal;
	

	
// write vertex up
//	fmap.addDynamicVectorArray("up", m_n_vertex, pup);
//	delete[] pup;
	
// write vertex wind
	fmap.addDynamicVectorArray("wind", m_n_vertex, pv);
	delete[] pv;

// write bound box	
	MPoint corner_l, corner_h;
	
	getBBox(corner_l, corner_h);
	
	fmap.addBBox(corner_l.x, corner_l.y, corner_l.z, corner_h.x, corner_h.y, corner_h.z);
	
	fmap.dynamicEnd();
	
	fmap.endMap(filename);
	
	char info[256];
	sprintf(info, "hairBase writes %s", filename);
	MGlobal::displayInfo ( info );
	
	m_vertex_id.clear();
	m_vertex_p.clear();
	m_vertex_n.clear();
	m_vertex_t.clear();

	delete[] pscale;
}
//~: