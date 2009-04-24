/*
 *  bruiseMap.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "bruiseMap.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include "../shared/zFnEXR.h"
#include <maya/MItMeshVertex.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshPolygon.h>

bruiseMap::bruiseMap():has_base(0),has_guide(0),ddice(0),ddist(0),n_samp(0),map_dist(0)
{
}

bruiseMap::~bruiseMap() 
{
	if(ddice) delete[] ddice;
	if(ddist) delete[] ddist;
	if(map_dist) delete[] map_dist;
}

void bruiseMap::setBase(const MObject& mesh)
{
	pbase = mesh;
	has_base = 1;
}

void bruiseMap::setGuide(const MObject& mesh)
{
	pguide = mesh;
	has_guide = 1;
}

int bruiseMap::dice(float bias)
{
	if(!has_base || !has_guide) return 0;
	
	MStatus status;
	MFnMesh meshFn(pbase, &status );
	MItMeshVertex vertIter(pbase, &status);
	MItMeshPolygon faceIter(pbase, &status);
	MFnMesh guideFn(pguide, &status );
	
	MPointArray p_vert;
	MVectorArray p_ray;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	n_vert = meshFn.numVertices(); 
	
	hitArray.clear();
	hitparray.clear();
	hitArray.setLength(n_vert);
	hitparray.setLength(n_vert*2);
	
	for(unsigned i=0; !vertIter.isDone(); vertIter.next(), i++) 
	{
		hitArray[i] = -1;
		MPoint vertp = vertIter.position( MSpace::kObject);
		MVector vertn;
		vertIter.getNormal(vertn, MSpace::kObject);
		p_ray.append(vertn);
		vertp += vertn*bias;
		
		MPointArray Aphit;
		MIntArray Aihit;
		if(guideFn.intersect (vertp, -vertn, Aphit, 0, MSpace::kObject, &Aihit, &status)) 
		{
			MVector hitnormal;
			guideFn.getPolygonNormal ( Aihit[0], hitnormal,  MSpace::kObject  );
				
			if(hitnormal*-vertn > 0) hitArray[i] = 1;
			hitparray[i*2] = Aphit[0];
			hitparray[i*2+1] = vertp;
		}
		else 
		{
			hitparray[i*2] = MPoint(0,0,0);
			hitparray[i*2+1] = MPoint(0,0,0);
		}
	}
	
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
	
	float epsilon = sqrt(sum_area/n_tri/2/13);

	int estimate_ncell = n_tri*13*2;
	estimate_ncell += estimate_ncell/9;
	
	if(ddice) delete[] ddice;
	if(ddist) delete[] ddist;
	ddice = new DiceParam[estimate_ncell];
	ddist = new float[estimate_ncell];
	n_samp = 0;

	DiceTriangle ftri;
	int a, b, c;
	faceIter.reset();
	int seed = 12;
	XYZ va, vb, vc;
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		int hashit = 0;
		faceIter.getConnectedVertices ( vexlist );
		for(unsigned i=0; i< vexlist.length(); i++)
		{
			if(hitArray[vexlist[i]] >0) hashit =1;
		}
		
		faceIter.getVertices ( vexlist );
		for(unsigned i=0; i< vexlist.length(); i++)
		{
			if(hitArray[vexlist[i]] >0) hashit =1;
		}
		
		if(hashit==1)
		{
			for( int i=vexlist.length()-2; i >0; i-- ) 
			{
				a = vexlist[vexlist.length()-1];
				b = vexlist[i];
				c = vexlist[i-1];
				va =XYZ(p_vert[a].x, p_vert[a].y, p_vert[a].z);
				vb =XYZ(p_vert[b].x, p_vert[b].y, p_vert[b].z);
				vc =XYZ(p_vert[c].x, p_vert[c].y, p_vert[c].z);
				ftri.create(va, vb, vc);
				ftri.setId(a, b, c);
				ftri.rasterize(epsilon, ddice, n_samp, seed);seed++;
			}
		}

	}
	
	/*MPoint ori;
	MVector ray;
	for(unsigned i=0; i<n_samp; i++)
	{
		ori = p_vert[ddice[i].id0]*ddice[i].alpha + p_vert[ddice[i].id1]*ddice[i].beta + p_vert[ddice[i].id2]*ddice[i].gamma;
		ray = p_ray[ddice[i].id0]*ddice[i].alpha + p_ray[ddice[i].id1]*ddice[i].beta + p_ray[ddice[i].id2]*ddice[i].gamma;
		ray.normalize();
		ori += ray*bias;
		if(guideFn.intersect (ori, -ray, Aphit, 0.0, MSpace::kObject, &Aihit, &status)) 
		{
			if(Aphit.length()==1) 
			{
				MVector tohit = Aphit[0] - ori;
				ddist[i] = tohit.length();
			}
			else ddist[i] = -1;
		}
	}*/

	p_vert.clear();
	p_ray.clear();
	return n_samp;	
}

void bruiseMap::draw()
{
	if(!has_base || n_samp < 1) return;
	
	MStatus status;
	MFnMesh meshFn(pbase, &status );
	
	if(n_vert != meshFn.numVertices()) return;
	
	MItMeshVertex vertIter(pbase, &status);
	MItMeshPolygon faceIter(pbase, &status);
	
	MPointArray p_vert;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	
	MPoint ori;
	
	glBegin(GL_POINTS);
	for(unsigned i=0; i<n_samp; i++)
	{
		//if(ddist[i]>0) 
		//{
			ori = p_vert[ddice[i].id0]*ddice[i].alpha + p_vert[ddice[i].id1]*ddice[i].beta + p_vert[ddice[i].id2]*ddice[i].gamma;
			
			glVertex3f(ori.x, ori.y, ori.z);
		//}
	}
	glEnd();
	glBegin(GL_LINES);
	for(unsigned i=0; i<hitparray.length(); i++)
	{
			glVertex3f(hitparray[i].x, hitparray[i].y, hitparray[i].z);
	}
	glEnd();

	p_vert.clear();
}

void bruiseMap::init(int size)
{
	if(map_dist) delete[] map_dist;
	
	m_wh = size;
	m_wh_minus1 = m_wh -1;
	map_dist = new float[m_wh*m_wh];
	for(unsigned i=0; i<m_wh*m_wh; i++) map_dist[i] = 0;
}

void bruiseMap::write(int s, int t, float val, float* data)
{
	if(s<0 || s >m_wh_minus1) return;
	if(t<0 || t >m_wh_minus1) return;	
	if(data[t*m_wh+s]==0) data[t*m_wh+s]=val;
	else if(data[t*m_wh+s] < val) data[t*m_wh+s]=(val+ data[t*m_wh+s])/2;
}

void bruiseMap::save(float bias, const char* filename, MString& uvsetName)
{
	if(!has_base || !has_guide) return;
	
	MStatus status;
	MFnMesh meshFn(pbase, &status );
	MItMeshVertex vertIter(pbase, &status);
	MItMeshPolygon faceIter(pbase, &status);
	MFnMesh guideFn(pguide, &status );
	
	MPointArray p_vert;
	MVectorArray p_ray;
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	n_vert = meshFn.numVertices(); 
	
	hitArray.setLength(n_vert);
	MPointArray Aphit;
	MIntArray Aihit;
	for(unsigned i=0; !vertIter.isDone(); vertIter.next(), i++) 
	{
		hitArray[i] = -1;
		MPoint vertp = vertIter.position( MSpace::kObject);
		MVector vertn;
		vertIter.getNormal(vertn, MSpace::kObject);
		p_ray.append(vertn);
		vertp += vertn*bias;

		if(guideFn.intersect (vertp, -vertn, Aphit, 0, MSpace::kObject, &Aihit, &status)) 
		{
			MVector hitnormal;
			guideFn.getPolygonNormal ( Aihit[0], hitnormal,  MSpace::kObject  );
				
			if(hitnormal*-vertn > 0) hitArray[i] = 1;
		}
	}

	DiceTriangle ftri;
	int a, b, c;
	faceIter.reset();
	int seed = 12;

	float uva[2], uvb[2], uvc[2];
	MPoint ori;
	MVector ray, tohit;
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		int hashit = 0;
		faceIter.getConnectedVertices ( vexlist );
		for(unsigned i=0; i< vexlist.length(); i++)
		{
			if(hitArray[vexlist[i]] >0) hashit =1;
		}
		
		faceIter.getVertices ( vexlist );
		for(unsigned i=0; i< vexlist.length(); i++)
		{
			if(hitArray[vexlist[i]] >0) hashit =1;
		}
		
		if(hashit==1)
		{
			for( int i=vexlist.length()-2; i >0; i-- ) 
			{
				a = vexlist[vexlist.length()-1];
				b = vexlist[i];
				c = vexlist[i-1];
				
				faceIter.getUV(vexlist.length()-1, uva, &uvsetName);
				faceIter.getUV(i, uvb, &uvsetName);
				faceIter.getUV(i-1, uvc, &uvsetName);
				
				ftri.create2D(uva, uvb, uvc);
				
				int n_dice = ftri.getGrid2D(m_wh);
				
				Dice2DParam* param = new Dice2DParam[n_dice];
				
				int real_dice = ftri.rasterize2D(param, seed, m_wh);seed++;
				
				for(int k=0; k<real_dice; k++)
				{
					ori = p_vert[a]*param[k].alpha + p_vert[b]*param[k].beta + p_vert[c]*param[k].gamma;
					ray = p_ray[a]*param[k].alpha + p_ray[b]*param[k].beta + p_ray[c]*param[k].gamma;
					ray.normalize();
					ori += ray*bias;
					
					if(guideFn.intersect (ori, -ray, Aphit, 0, MSpace::kObject, &Aihit, &status)) 
					{
						MVector hitnormal;
						guideFn.getPolygonNormal ( Aihit[0], hitnormal,  MSpace::kObject  );
							
						if(hitnormal*-ray > 0)
						{
							int map_s = param[k].s;
							int map_t = param[k].t;
							tohit = Aphit[0] - ori;
							float dist = tohit.length();
							
							map_s=param[k].s-1;
							map_t=param[k].t-1;
							write(map_s, map_t, dist, map_dist);
							map_t++;
							write(map_s, map_t, dist, map_dist);
							map_t++;
							write(map_s, map_t, dist, map_dist);
							
							map_s++;
							map_t=param[k].t-1;
							write(map_s, map_t, dist, map_dist);
							map_t++;
							write(map_s, map_t, dist, map_dist);
							map_t++;
							write(map_s, map_t, dist, map_dist);
							
							map_s++;
							map_t=param[k].t-1;
							write(map_s, map_t, dist, map_dist);
							map_t++;
							write(map_s, map_t, dist, map_dist);
							map_t++;
							write(map_s, map_t, dist, map_dist);
							
						}
					}
				}
				
				delete[] param;
			}
		}

	}
	
	p_vert.clear();
	p_ray.clear();
	
	float* data = new float[m_wh*m_wh*4];
	for(unsigned j=0; j<m_wh; j++)
		for(unsigned i=0; i<m_wh; i++)
		{
			data[(j*m_wh+i)*4] = map_dist[j*m_wh+i];
			data[(j*m_wh+i)*4+1] = 0;
			data[(j*m_wh+i)*4+2] = 0;
			data[(j*m_wh+i)*4+3] = 1;
		}
	ZFnEXR::save(data, filename, m_wh, m_wh);
	delete[] data;
}
//~: