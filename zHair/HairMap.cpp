/*
 *  hairMap.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "hairMap.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include <maya/MItMeshVertex.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshPolygon.h>

hairMap::hairMap():has_mesh(0),ddice(0),n_samp(0)
{
}
hairMap::~hairMap() 
{
	if(ddice) delete[] ddice;
}

void hairMap::setGrow(const MObject& mesh)
{
	pgrow = mesh;
	has_mesh = 1;
}

int hairMap::dice()
{
	if(!has_mesh) return 0;
	
	MStatus status;
	MFnMesh meshFn(pgrow, &status );
	MItMeshVertex vertIter(pgrow, &status);
	MItMeshPolygon faceIter(pgrow, &status);
	
	MPointArray p_vert;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	n_vert = meshFn.numVertices(); 
	XYZ* parray = new XYZ[n_vert];
	
	for(unsigned i=0; i<n_vert; i++) parray[i] = XYZ(p_vert[i].x, p_vert[i].y, p_vert[i].z);
	p_vert.clear();
	
	unsigned n_tri = 0;
	float sum_area = 0;
	double area;
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		n_tri += vexlist.length()-2;
		faceIter.getArea( area,  MSpace::kWorld );
		sum_area += (float)area;
	}
	
	float epsilon = sqrt(sum_area/n_tri/2/16);

	int estimate_ncell = n_tri*16*2;
	estimate_ncell += estimate_ncell/9;
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[estimate_ncell];
	n_samp = 0;

	DiceTriangle ftri;
	int a, b, c;
	faceIter.reset();
	int seed = 12;
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		for( int i=vexlist.length()-2; i >0; i-- ) 
		{
			a = vexlist[vexlist.length()-1];
			b = vexlist[i];
			c = vexlist[i-1];
			
			ftri.create(parray[a], parray[b], parray[c]);
			ftri.setId(a, b, c);
			ftri.rasterize(epsilon, ddice, n_samp, seed);seed++;
		}
	}
	
	delete[] parray;
	
	return n_samp;	
}

void hairMap::draw()
{
	if(!has_mesh || n_samp < 1) return;
	
	MStatus status;
	MFnMesh meshFn(pgrow, &status );
	
	if(n_vert != meshFn.numVertices()) return;
	
	MItMeshVertex vertIter(pgrow, &status);
	MItMeshPolygon faceIter(pgrow, &status);
	
	MPointArray p_vert;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	//n_vert = meshFn.numVertices(); 
	XYZ* parray = new XYZ[n_vert];
	
	for(unsigned i=0; i<n_vert; i++) parray[i] = XYZ(p_vert[i].x, p_vert[i].y, p_vert[i].z);
	p_vert.clear();
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++)
	{
		pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
	}
	
	delete[] parray;
	
	glBegin(GL_POINTS);
	for(unsigned i=0; i<n_samp; i++)
	{
		glVertex3f(pbuf[i].x, pbuf[i].y, pbuf[i].z);
	}
	glEnd();
	
	delete[] pbuf;
	
/*	
	glBegin(GL_POINTS);
	for(unsigned i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		S = vertIter.position(MSpace::kWorld);
		glVertex3f(S.x, S.y, S.z);
	}
	glEnd();

	
	MVector N, tang, ttang, binormal, dir, hair_up;
	MColor Cscale, Cerect, Crotate, Ccurl;
	float rot;
	MATRIX44F hair_space;
	
	MString setScale("fb_scale");
	MString setErect("fb_erect");
	MString setRotate("fb_rotate");
	MString setCurl("fb_curl");
	MIntArray conn_face;
	
	
	{
		
		
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
	
*/	
}
//~: