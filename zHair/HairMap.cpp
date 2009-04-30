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

hairMap::hairMap():has_base(0),ddice(0),n_samp(0),has_guide(0),guide_data(0),bind_data(0)
{
}
hairMap::~hairMap() 
{
	if(ddice) delete[] ddice;
	if(guide_data) delete[] guide_data;
	if(bind_data) delete[] bind_data;
}

void hairMap::setBase(const MObject& mesh)
{
	obase = mesh;
	has_base = 1;
}

void hairMap::setGuide(const MObject& mesh)
{
	oguide = mesh;
	has_guide = 1;
}

int hairMap::dice()
{
	if(!has_base) return 0;
	
	MStatus status;
	MFnMesh meshFn(obase, &status );
	MItMeshVertex vertIter(obase, &status);
	MItMeshPolygon faceIter(obase, &status);
	
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
	if(!has_base || n_samp < 1) return;
	
	MStatus status;
	MFnMesh meshFn(obase, &status );
	
	if(n_vert != meshFn.numVertices()) return;
	
	MPointArray p_vert;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
 
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
		glColor3f(guide_data[bind_data[i]].dsp_col.x, guide_data[bind_data[i]].dsp_col.y, guide_data[bind_data[i]].dsp_col.z);
		glVertex3f(pbuf[i].x, pbuf[i].y, pbuf[i].z);
	}
	glEnd();
	
	delete[] pbuf;
}

void hairMap::initGuide()
{
	if(!has_guide) return;
	
	if(guide_data) delete[] guide_data;
	
	MStatus status;
	
	MItMeshPolygon faceIter(oguide, &status);
	
	num_guide = faceIter.count()/5;
	guide_data = new Dguide[num_guide];

	MPoint cen;
	float r,g,b;
	int acc=0;
	for(int i=0; !faceIter.isDone(); faceIter.next(), i++) 
	{
		if(i%5 ==0)
		{
			cen = faceIter.center (  MSpace::kObject);
			r = rand( )%31/31.f;
			g = rand( )%71/71.f;
			b = rand( )%11/11.f;
			guide_data[acc].root = XYZ(cen.x, cen.y, cen.z);
			guide_data[acc].dsp_col = XYZ(r,g,b);
			acc++;
		}
	}
}

void hairMap::bind()
{
	if(!has_guide || !guide_data || n_samp < 1) return;
	
	MStatus status;
	MFnMesh meshFn(obase, &status );
	
	if(n_vert != meshFn.numVertices()) return;
	
	MPointArray p_vert;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
 
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

	if(bind_data) delete[] bind_data;
	bind_data = new unsigned[n_samp];
	XYZ togd;
	float dist;
	for(unsigned i=0; i<n_samp; i++)
	{
// find the nearest guide
		float min_dist = 10e6;
		for(unsigned j=0; j<num_guide; j++)
		{
			togd = pbuf[i] - guide_data[j].root;
			dist = togd.length();
			
			if(dist < min_dist)
			{
				bind_data[i] = j;
				min_dist = dist;
			}
		}
	}
	
	delete[] pbuf;
}

void hairMap::drawGuide()
{
	if(!has_guide || !guide_data) return;

	glBegin(GL_POINTS);
	MPoint cen;
	float r,g,b;
	int acc=0;
	for(int i=0; i<num_guide; i++) 
	{
		glColor3f(guide_data[i].dsp_col.x, guide_data[i].dsp_col.y, guide_data[i].dsp_col.z);
		glVertex3f(guide_data[i].root.x, guide_data[i].root.y, guide_data[i].root.z);
	}
	
	glEnd();
}
//~: