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
#include "../shared/FNoise.h"
#include <iostream>
#include <fstream>
using namespace std;

hairMap::hairMap():has_base(0),ddice(0),n_samp(0),has_guide(0),guide_data(0),bind_data(0),guide_spaceinv(0),
parray(0),pconnection(0),uarray(0),varray(0),
sum_area(0.f)
{
}
hairMap::~hairMap() 
{
	if(ddice) delete[] ddice;
	if(guide_data) delete[] guide_data;
	if(bind_data) delete[] bind_data;
	if(guide_spaceinv) delete[] guide_spaceinv;
	if(parray) delete[] parray;
	if(pconnection) delete[] pconnection;
	if(uarray) delete[] uarray;
	if(varray) delete[] varray;
}

void hairMap::setBase(const MObject& mesh)
{
	obase = mesh;
	has_base = 1;
}

void hairMap::setGuide(const MObjectArray& meshes)
{
	oguide.clear();
	for(unsigned i=0; i<meshes.length(); i++)
	{
		if(!meshes[i].isNull()) oguide.append(meshes[i]);
	}
	if(oguide.length()>0) has_guide = 1;
	else  has_guide = 0;
}

void hairMap::updateBase()
{
	if(!has_base) return;
	MPointArray p_vert;
	MStatus status;
	MFnMesh meshFn(obase, &status );
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	n_vert = meshFn.numVertices();
	
	if(parray) delete[] parray; 
	parray = new XYZ[n_vert];
	
	for(unsigned i=0; i<n_vert; i++) parray[i] = XYZ(p_vert[i].x, p_vert[i].y, p_vert[i].z);
	
	p_vert.clear();
	
	MItMeshPolygon faceIter(obase, &status);
	n_tri = 0;
	sum_area = 0;
	double area;
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		n_tri += vexlist.length()-2;
		faceIter.getArea( area,  MSpace::kWorld );
		sum_area += (float)area;
	}
	
	if(pconnection) delete[] pconnection;
	pconnection = new int[n_tri*3];
	
	if(uarray) delete[] uarray;
	uarray = new float[n_tri*3];
	if(varray) delete[] varray;
	varray = new float[n_tri*3];
	
	int acc = 0;
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		MFloatArray ub, vb;
		faceIter.getVertices ( vexlist );
		faceIter.getUVs (ub, vb);
		for( int i=vexlist.length()-2; i >0; i-- ) 
		{
			pconnection[acc] = vexlist[vexlist.length()-1];
			pconnection[acc+1] = vexlist[i];
			pconnection[acc+2] = vexlist[i-1];
			
			uarray[acc] = ub[vexlist.length()-1];
			uarray[acc+1] = ub[i];
			uarray[acc+2] = ub[i-1];
			
			varray[acc] = vb[vexlist.length()-1];
			varray[acc+1] = vb[i];
			varray[acc+2] = vb[i-1];
			
			acc += 3;
		}
	}
}

int hairMap::dice()
{
	if(!pconnection || !parray) return 0;
		
	float epsilon = sqrt(sum_area/n_tri/2/3);

	int estimate_ncell = n_tri*3*2;
	estimate_ncell += estimate_ncell/9;
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[estimate_ncell];
	n_samp = 0;

	DiceTriangle ftri;
	int a, b, c;
	
	int seed = 12;
	for(unsigned i=0; i<n_tri; i++) 
	{
		a = pconnection[i*3];
		b = pconnection[i*3+1];
		c = pconnection[i*3+2];
		
		ftri.create(parray[a], parray[b], parray[c]);
		ftri.setId(a, b, c);
		ftri.setS(uarray[i*3], uarray[i*3+1], uarray[i*3+2]);
		ftri.setT(varray[i*3], varray[i*3+1], varray[i*3+2]);
		ftri.rasterize(epsilon, ddice, n_samp, seed);seed++;
	}
	
	return n_samp;	
}

void hairMap::draw()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray) return;
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++) pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;

	int g_seed = 13;
	FNoise fnoi;
	
	float noi;
	
	XYZ ppre, dv, axisobj, axisworld, guiderotaxis;
	glBegin(GL_LINES);
	for(unsigned i=0; i<n_samp; i++)
	{
		//glColor3f(guide_data[bind_data[i]].dsp_col.x, guide_data[bind_data[i]].dsp_col.y, guide_data[bind_data[i]].dsp_col.z);
		//glColor3f(ddice[i].coords, ddice[i].coordt, 0);
		ppre = pbuf[i]; 
		axisworld = axisobj = pbuf[i] -  guide_data[bind_data[i]].P[0];
		guide_spaceinv[bind_data[i]].transformAsNormal(axisobj);
		axisobj.x = 0;
		for(short j = 0; j< guide_data[bind_data[i]].num_seg; j++) 
		{
			dv = guide_data[bind_data[i]].dispv[j];
			MATRIX44F mat;
			
			XYZ binor = guide_data[bind_data[i]].N[j].cross(guide_data[bind_data[i]].T[j]);
			mat.setOrientations(guide_data[bind_data[i]].T[j], binor, guide_data[bind_data[i]].N[j]);
			
			XYZ rt = axisobj;
			rt.rotateAroundAxis(XYZ(1,0,0), twist*j*dv.length()/axisobj.length());
			mat.transformAsNormal(rt);

			axisworld = rt;
			//glVertex3f(guide_data[bind_data[i]].P[j].x, guide_data[bind_data[i]].P[j].y, guide_data[bind_data[i]].P[j].z);
			//glVertex3f(guide_data[bind_data[i]].P[j].x+axisworld.x, guide_data[bind_data[i]].P[j].y+axisworld.y, guide_data[bind_data[i]].P[j].z+axisworld.z);
			axisworld.normalize();
			
			glVertex3f(ppre.x, ppre.y, ppre.z);

			XYZ rot2p = ppre + dv -  guide_data[bind_data[i]].P[j];
			noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
			dv.rotateAlong(rot2p, -clumping*noi);

			noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
			dv.rotateAroundAxis(axisworld, -twist*noi);
			
			noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
			ppre += dv*noi;
			glVertex3f(ppre.x, ppre.y, ppre.z);
		}
	}
	glEnd();
	
	delete[] pbuf;
}

void hairMap::initGuide()
{
	if(!has_guide) return;

	num_guideobj = oguide.length();
	num_guide = 0;
	MStatus status;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++)
	{
		MFnMesh meshFn(oguide[iguide], &status);
		MItMeshPolygon faceIter(oguide[iguide], &status);
// find per-patch number of segment
		int nend = 0;
		int nconn;
		for(; !faceIter.isDone(); faceIter.next()) 
		{
			faceIter.numConnectedFaces (nconn);
			if(nconn==1) nend++;
		}
		nend /= 2;
		
		int nseg = meshFn.numPolygons()/nend;
		
		num_guide += faceIter.count()/nseg;
	}
	//MGlobal::displayInfo(MString(" ")+num_guide);
	
	if(guide_data) delete[] guide_data;
	guide_data = new Dguide[num_guide];
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];

	MPoint cen;
	MVector nor, tang;
	MIntArray vertlist;
	float r,g,b;
	int patch_id, patch_acc =0;
	XYZ pcur, ppre;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++)
	{
		MFnMesh meshFn(oguide[iguide], &status);
		MItMeshPolygon faceIter(oguide[iguide], &status);
// find per-patch number of segment
		int nend = 0;
		int nconn;
		for(; !faceIter.isDone(); faceIter.next()) 
		{
			faceIter.numConnectedFaces (nconn);
			if(nconn==1) nend++;
		}
		nend /= 2;
		
		int nseg = meshFn.numPolygons()/nend;
		faceIter.reset();
		for(int i=0; !faceIter.isDone(); faceIter.next(), i++) 
		{
			patch_id = i/nseg + patch_acc;
			
			if(i%nseg ==0)
			{
				guide_data[patch_id].num_seg = nseg;
				guide_data[patch_id].P = new XYZ[nseg];
				guide_data[patch_id].N = new XYZ[nseg];
				guide_data[patch_id].T = new XYZ[nseg];
				guide_data[patch_id].dispv = new XYZ[nseg];

				r = rand( )%31/31.f;
				g = rand( )%71/71.f;
				b = rand( )%11/11.f;
				guide_data[patch_id].dsp_col = XYZ(r,g,b);
			}
			
			cen = faceIter.center (  MSpace::kObject);
			pcur = XYZ(cen.x, cen.y, cen.z);
			
			guide_data[patch_id].P[i%nseg] = pcur;
			
			faceIter.getNormal ( nor,  MSpace::kObject );
			guide_data[patch_id].N[i%nseg] = XYZ(nor.x, nor.y, nor.z);
			
			faceIter.getVertices (vertlist);
			
			meshFn.getFaceVertexTangent (i, vertlist[0], tang,  MSpace::kObject);
			tang = nor^tang;
			tang.normalize();
			guide_data[patch_id].T[i%nseg] = XYZ(tang.x, tang.y, tang.z);
			
			MPoint corner0, corner1;
			meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
			meshFn.getPoint (vertlist[2], corner1, MSpace::kObject );
			
			MVector dv = corner0 - cen + corner1 - cen;
			
			guide_data[patch_id].dispv[i%nseg] = XYZ(dv.x, dv.y, dv.z);
			
			XYZ side = guide_data[patch_id].dispv[i%nseg].cross(guide_data[patch_id].N[i%nseg]);
			side.normalize();
			guide_data[patch_id].T[i%nseg] = guide_data[patch_id].N[i%nseg].cross(side);
			guide_data[patch_id].T[i%nseg].normalize();
			
			if(i%nseg ==0)
			{
				guide_spaceinv[patch_id].setIdentity();
				XYZ binor = guide_data[patch_id].N[0].cross(guide_data[patch_id].T[0]);
				guide_spaceinv[patch_id].setOrientations(guide_data[patch_id].T[0], binor, guide_data[patch_id].N[0]);
				guide_spaceinv[patch_id].inverse();
			}
		}
		patch_acc += nend;
	}
}

void hairMap::updateGuide()
{
	if(!has_guide || !guide_data || oguide.length() != num_guideobj) return;
	MStatus status;
	MPoint cen;
	MVector nor, tang;
	MIntArray vertlist;
	int patch_id, patch_acc =0;
	XYZ pcur, ppre;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++)
	{
		MFnMesh meshFn(oguide[iguide], &status);
		MItMeshPolygon faceIter(oguide[iguide], &status);
// find per-patch number of segment
		int nend = 0;
		int nconn;
		for(; !faceIter.isDone(); faceIter.next()) 
		{
			faceIter.numConnectedFaces (nconn);
			if(nconn==1) nend++;
		}
		nend /= 2;
		
		int nseg = meshFn.numPolygons()/nend;
		faceIter.reset();
		for(int i=0; !faceIter.isDone(); faceIter.next(), i++) 
		{
			patch_id = i/nseg + patch_acc;
			
			cen = faceIter.center (  MSpace::kObject);
			pcur = XYZ(cen.x, cen.y, cen.z);
			
			guide_data[patch_id].P[i%nseg] = pcur;
			
			faceIter.getNormal ( nor,  MSpace::kObject );
			guide_data[patch_id].N[i%nseg] = XYZ(nor.x, nor.y, nor.z);
			
			faceIter.getVertices (vertlist);
			
			meshFn.getFaceVertexTangent (i, vertlist[0], tang,  MSpace::kObject);
			tang = nor^tang;
			tang.normalize();
			guide_data[patch_id].T[i%nseg] = XYZ(tang.x, tang.y, tang.z);
			
			MPoint corner0, corner1;
			meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
			meshFn.getPoint (vertlist[2], corner1, MSpace::kObject );
			
			MVector dv = corner0 - cen + corner1 - cen;
			
			guide_data[patch_id].dispv[i%nseg] = XYZ(dv.x, dv.y, dv.z);
			
			XYZ side = guide_data[patch_id].dispv[i%nseg].cross(guide_data[patch_id].N[i%nseg]);
			side.normalize();
			guide_data[patch_id].T[i%nseg] = guide_data[patch_id].N[i%nseg].cross(side);
			guide_data[patch_id].T[i%nseg].normalize();
			
			if(i%nseg ==0)
			{
				guide_spaceinv[patch_id].setIdentity();
				XYZ binor = guide_data[patch_id].N[0].cross(guide_data[patch_id].T[0]);
				guide_spaceinv[patch_id].setOrientations(guide_data[patch_id].T[0], binor, guide_data[patch_id].N[0]);
				guide_spaceinv[patch_id].inverse();
			}
		}
		patch_acc += nend;
	}
}

void hairMap::bind()
{
	if(!guide_data || n_samp < 1) return;
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++)
	{
		pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
	}

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
			togd = pbuf[i] - guide_data[j].P[0];
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
	if(!guide_data) return;
	glBegin(GL_LINES);

	XYZ pp;
	for(int i=0; i<num_guide; i++) 
	{
		//glColor3f(guide_data[i].dsp_col.x, guide_data[i].dsp_col.y, guide_data[i].dsp_col.z);
		for(short j = 0; j< guide_data[i].num_seg; j++) 
		{
			XYZ pp = guide_data[i].P[j];
			glVertex3f(pp.x, pp.y, pp.z);
			pp += guide_data[i].N[j];
			glVertex3f(pp.x, pp.y, pp.z);
			pp = guide_data[i].P[j];
			glVertex3f(pp.x, pp.y, pp.z);
			pp += guide_data[i].T[j];
			glVertex3f(pp.x, pp.y, pp.z);
		}
	}
	
	glEnd();
}

int hairMap::save(const char* filename)
{
	ofstream outfile;
	outfile.open(filename, ios_base::out | ios_base::binary);
	if(!outfile.is_open()) 
	{
		MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		return 0;
	}
	outfile.write((char*)&num_guide,sizeof(unsigned));
	for(unsigned i = 0;i<num_guide;i++)
	{
		outfile.write((char*)&guide_data[i].num_seg,sizeof(guide_data[i].num_seg));
		//outfile.write((char*)&guide_data[i].dsp_col,sizeof(XYZ));
		outfile.write((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));	
	}
	outfile.write((char*)&sum_area, sizeof(float));
	outfile.write((char*)&n_tri, sizeof(unsigned));
	outfile.write((char*)pconnection, sizeof(int)*n_tri*3);
	outfile.write((char*)&n_vert, sizeof(unsigned));
	outfile.write((char*)parray, sizeof(XYZ)*n_vert);
	outfile.close();
	return 1;
}

int hairMap::saveStart(const char* filename)
{
	ofstream outfile;
	outfile.open(filename, ios_base::out | ios_base::binary);
	if(!outfile.is_open()) 
	{
		MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		return 0;
	}
	outfile.write((char*)&num_guide,sizeof(unsigned));
	for(unsigned i = 0;i<num_guide;i++)
	{
		outfile.write((char*)&guide_data[i].num_seg,sizeof(guide_data[i].num_seg));
		//outfile.write((char*)&guide_data[i].dsp_col,sizeof(XYZ));
		outfile.write((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));	
	}
	outfile.write((char*)&sum_area, sizeof(float));
	outfile.write((char*)&n_tri, sizeof(unsigned));
	outfile.write((char*)pconnection, sizeof(int)*n_tri*3);
	outfile.write((char*)&n_vert, sizeof(unsigned));
	outfile.write((char*)parray, sizeof(XYZ)*n_vert);
	outfile.write((char*)uarray, sizeof(float)*n_tri*3);
	outfile.write((char*)varray, sizeof(float)*n_tri*3);
	outfile.close();
	return 1;
}

int hairMap::load(const char* filename)
{
	ifstream infile;
	infile.open(filename, ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		return 0;
	}
	infile.read((char*)&num_guide,sizeof(unsigned));
	
	if(guide_data) delete[] guide_data;
    guide_data = new Dguide[num_guide];
	for(unsigned i = 0;i<num_guide;i++)
	{
		infile.read((char*)&guide_data[i].num_seg, sizeof(guide_data[i].num_seg));

		guide_data[i].P = new XYZ[guide_data[i].num_seg];
		guide_data[i].N = new XYZ[guide_data[i].num_seg];
		guide_data[i].T = new XYZ[guide_data[i].num_seg];
		guide_data[i].dispv = new XYZ[guide_data[i].num_seg];

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
	}
	infile.read((char*)&sum_area,sizeof(float));
	infile.read((char*)&n_tri,sizeof(unsigned));
	
	if(pconnection) delete[] pconnection;
	pconnection = new int[n_tri*3];
	infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	if(parray) delete[] parray;
	parray = new XYZ[n_vert];
	infile.read((char*)parray, sizeof(XYZ)*n_vert);
	
	infile.close();	
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		guide_spaceinv[i].setIdentity();
		XYZ binor = guide_data[i].N[0].cross(guide_data[i].T[0]);
		guide_spaceinv[i].setOrientations(guide_data[i].T[0], binor, guide_data[i].N[0]);
		guide_spaceinv[i].inverse();
	}
	return 1;
}

int hairMap::loadStart(const char* filename)
{
	ifstream infile;
	infile.open(filename, ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		return 0;
	}
	infile.read((char*)&num_guide,sizeof(unsigned));
	
	if(guide_data) delete[] guide_data;
    guide_data = new Dguide[num_guide];
	for(unsigned i = 0;i<num_guide;i++)
	{
		infile.read((char*)&guide_data[i].num_seg, sizeof(guide_data[i].num_seg));

		guide_data[i].P = new XYZ[guide_data[i].num_seg];
		guide_data[i].N = new XYZ[guide_data[i].num_seg];
		guide_data[i].T = new XYZ[guide_data[i].num_seg];
		guide_data[i].dispv = new XYZ[guide_data[i].num_seg];

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
	}
	infile.read((char*)&sum_area,sizeof(float));
	infile.read((char*)&n_tri,sizeof(unsigned));
	
	if(pconnection) delete[] pconnection;
	pconnection = new int[n_tri*3];
	infile.read((char*)pconnection, sizeof(int)*n_tri*3);
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	if(parray) delete[] parray;
	parray = new XYZ[n_vert];
	infile.read((char*)parray, sizeof(XYZ)*n_vert);
	
	if(uarray) delete[] uarray;
	uarray = new float[n_tri*3];
	infile.read((char*)uarray, sizeof(float)*n_tri*3);
	
	if(varray) delete[] varray;
	varray = new float[n_tri*3];
	infile.read((char*)varray, sizeof(float)*n_tri*3);
	
	infile.close();	
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		guide_spaceinv[i].setIdentity();
		XYZ binor = guide_data[i].N[0].cross(guide_data[i].T[0]);
		guide_spaceinv[i].setOrientations(guide_data[i].T[0], binor, guide_data[i].N[0]);
		guide_spaceinv[i].inverse();
	}
	return 1;
}
//~:
