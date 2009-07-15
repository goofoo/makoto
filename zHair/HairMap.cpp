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
#include "../shared/QuickSort.h"
#include <iostream>
#include <fstream>
using namespace std;

hairMap::hairMap():has_base(0),ddice(0),n_samp(0),has_guide(0),guide_data(0),bind_data(0),guide_spaceinv(0),pNSeg(0),
parray(0),pconnection(0),uarray(0),varray(0),
sum_area(0.f),mutant_scale(0.f),
draw_step(1),order(0),isInterpolate(0),nsegbuf(0)
{
	root_color.x = 1.f; root_color.y = root_color.z = 0.f;
	tip_color.y = 0.7f; tip_color.x = 0.f; tip_color.z = 0.2f;
	mutant_color.x = mutant_color.y = mutant_color.z = 0.f;
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
	if(pNSeg) delete[] pNSeg;
	if(nsegbuf) delete[] nsegbuf;
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
		
	float epsilon = sqrt(sum_area/n_tri/2/2);

	int estimate_ncell = n_tri*2*2;
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
	if(n_samp < 1 || !bind_data || !guide_data || !parray || !pNSeg) return;
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++) pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;

	int g_seed = 13;
	FNoise fnoi;
	
	float noi, keepx;
	MATRIX44F tspace, tspace1, tspace2;
	XYZ ppre, pcur, dv, ddv, cc, pobj, pt[3], pw[3], dv0, dv1, dv2;
	glBegin(GL_LINES);
	for(unsigned i=0; i<n_samp; i += draw_step)
	{
		noi  = fnoi.randfint( g_seed )*mutant_scale; g_seed++;
		XYZ croot = root_color + (mutant_color - root_color)*noi;
		XYZ ctip = tip_color + (mutant_color - tip_color)*noi;
		
		ppre = pbuf[i];
		
		pobj = ppre;
		pt[0] = pt[1] = pt[2] = ppre;
		guide_spaceinv[bind_data[i].idx[0]].transform(pobj);
		
		guide_spaceinv[bind_data[i].idx[0]].transform(pt[0]);
		guide_spaceinv[bind_data[i].idx[1]].transform(pt[1]);
		guide_spaceinv[bind_data[i].idx[2]].transform(pt[2]);
		
		//int num_seg = guide_data[bind_data[i].idx[0]].num_seg;
		unsigned num_seg = pNSeg[i];
		float dparam = 1.f/num_seg;
		XYZ dcolor = (ctip - croot)/(float)num_seg;
		float param;
		if(bind_data[i].wei[0] > .8f) {
			for(short j = 0; j< num_seg; j++) {
				param = dparam*j;
				
				//dv = guide_data[bind_data[i].idx[0]].dispv[j];
				guide_data[bind_data[i].idx[0]].getDvAtParam(dv, param, num_seg);
				guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
				
				cc = croot + dcolor * j;
				glColor3f(cc.x, cc.y, cc.z);
				glVertex3f(ppre.x, ppre.y, ppre.z);
				
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
				
				keepx = pt[0].x;
				pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
				pw[0] *= noi;
				pw[0].x = keepx;
				//guide_data[bind_data[i].idx[0]].space[j].transform(pw[0]);
				tspace.transform(pw[0]);
				
				pcur = pw[0];
				
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
				dv *= noi;
				pcur += dv;
				
				ddv = pcur - ppre;
				ddv.normalize();
				ddv *= dv.length();

				ppre += ddv;
				
				cc = croot + dcolor * (j+1);
				glColor3f(cc.x, cc.y, cc.z);
				glVertex3f(ppre.x, ppre.y, ppre.z);
			}
		}
		else {
			for(short j = 0; j< num_seg; j++) {
				param = dparam*j;
				
				guide_data[bind_data[i].idx[0]].getDvAtParam(dv0, param, num_seg);
				guide_data[bind_data[i].idx[1]].getDvAtParam(dv1, param, num_seg);
				guide_data[bind_data[i].idx[2]].getDvAtParam(dv2, param, num_seg);
				
				guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace, param);
				guide_data[bind_data[i].idx[1]].getSpaceAtParam(tspace1, param);
				guide_data[bind_data[i].idx[2]].getSpaceAtParam(tspace2, param);
				
				//dv = guide_data[bind_data[i].idx[0]].dispv[j]*bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].dispv[j]*bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].dispv[j]*bind_data[i].wei[2];
				dv = dv0 * bind_data[i].wei[0] + dv1 * bind_data[i].wei[1] + dv2 * bind_data[i].wei[2];
				dv.setLength(dv0.length());
				
				cc = croot + dcolor * j;
				glColor3f(cc.x, cc.y, cc.z);
				glVertex3f(ppre.x, ppre.y, ppre.z);
				
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
				
				keepx = pt[0].x;
				pw[0] = pt[0]*(1 - clumping*(j+1)/num_seg);
				pw[0] *= noi;
				pw[0].x = keepx;
				//guide_data[bind_data[i].idx[0]].space[j].transform(pw[0]);
				tspace.transform(pw[0]);
				
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
				keepx = pt[1].x;
				pw[1] = pt[1]*(1 - clumping*(j+1)/num_seg);
				pw[1] *= noi;
				pw[1].x = keepx;
				//guide_data[bind_data[i].idx[1]].space[j].transform(pw[1]);
				tspace1.transform(pw[1]);
				
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*kink; g_seed++;
				keepx = pt[2].x;
				pw[2] = pt[2]*(1 - clumping*(j+1)/num_seg);
				pw[2] *= noi;
				pw[2].x = keepx;
				//guide_data[bind_data[i].idx[2]].space[j].transform(pw[2]);
				tspace2.transform(pw[2]);
				
				pcur = pw[0]*bind_data[i].wei[0] + pw[1]*bind_data[i].wei[1] + pw[2]*bind_data[i].wei[2];
				
				noi = 1.f + (fnoi.randfint( g_seed )-0.5)*fuzz; g_seed++;
				dv *= noi;
				pcur += dv;
				
				ddv = pcur - ppre;
				ddv.normalize();
				ddv *= dv.length();

				ppre += ddv;
				
				cc = croot + dcolor * (j+1);
				glColor3f(cc.x, cc.y, cc.z);
				glVertex3f(ppre.x, ppre.y, ppre.z);
			}
		}
	}
	glEnd();
	
	delete[] pbuf;
}

void hairMap::drawUV()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray || !pNSeg) return;
	glPointSize(3);
	glBegin(GL_POINTS);
	for(unsigned i=0; i<n_samp; i += draw_step) {
		XYZ cc = guide_data[bind_data[i].idx[0]].dsp_col * ddice[i].alpha + guide_data[bind_data[i].idx[1]].dsp_col * ddice[i].beta + guide_data[bind_data[i].idx[2]].dsp_col * ddice[i].gamma;
		glColor3f(cc.x, cc.y, cc.z);
		
		float u = guide_data[bind_data[i].idx[0]].u * ddice[i].alpha + guide_data[bind_data[i].idx[1]].u * ddice[i].beta + guide_data[bind_data[i].idx[2]].u * ddice[i].gamma;
		float v = guide_data[bind_data[i].idx[0]].v * ddice[i].alpha + guide_data[bind_data[i].idx[1]].v * ddice[i].beta + guide_data[bind_data[i].idx[2]].v * ddice[i].gamma;
		glVertex3f(u, v, 0);
		//glVertex3f(ddice[i].coords, ddice[i].coordt, 0);
	}
	glEnd();
}

void hairMap::initGuide()
{
	if(!has_guide) return;
	MStatus status;
	MFnMesh baseFn(obase, &status );
	if(!status) return;

	num_guideobj = oguide.length();
	num_guide = 0;
// find number of guider
	for(unsigned iguide=0; iguide<oguide.length(); iguide++)
	{
		MItMeshPolygon faceIter(oguide[iguide], &status);
// find per-mesh number of patch
		int nend = 0;
		int nconn;
		for(; !faceIter.isDone(); faceIter.next()) {
			faceIter.numConnectedFaces (nconn);
			if(nconn==1) nend++;
		}
		num_guide += nend/2;
	}
	
// store per-guider segment		
	if(nsegbuf) delete[] nsegbuf;
	nsegbuf = new int[num_guide];
	
	int ipatch = 0;
	int nconn;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++) {
		MItMeshPolygon faceIter(oguide[iguide], &status);

		char isstart = 0;
		int nseg = 0;
		for(; !faceIter.isDone(); faceIter.next()) {
			nseg++;
			faceIter.numConnectedFaces (nconn);
			if(nconn==1) {
				if(!isstart) isstart = 1;
				else {
					nsegbuf[ipatch] = nseg;
					nseg = 0;
					ipatch++;
					isstart = 0;
				}
			}
		}
	}
	
	if(guide_data) delete[] guide_data;
	guide_data = new Dguide[num_guide];
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];

	MPoint cen;
	MVector nor, tang;
	MIntArray vertlist;
	float r,g,b;
	int patch_id=-1;
	XYZ pcur, ppre, pcen;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++) {
		MFnMesh meshFn(oguide[iguide], &status);
		MItMeshPolygon faceIter(oguide[iguide], &status);

		int patchend = 0, patchstart = 0;
		for(int i=0; !faceIter.isDone(); faceIter.next(), i++) {
			if(i == patchend)
			{
				patch_id++;
				patchstart = patchend;
				patchend += nsegbuf[patch_id];
				
				guide_data[patch_id].num_seg = nsegbuf[patch_id];
				guide_data[patch_id].P = new XYZ[nsegbuf[patch_id]];
				guide_data[patch_id].N = new XYZ[nsegbuf[patch_id]];
				guide_data[patch_id].T = new XYZ[nsegbuf[patch_id]];
				guide_data[patch_id].dispv = new XYZ[nsegbuf[patch_id]];
				guide_data[patch_id].space = new MATRIX44F[nsegbuf[patch_id]];

				r = rand( )%31/31.f;
				g = rand( )%71/71.f;
				b = rand( )%11/11.f;
				guide_data[patch_id].dsp_col = XYZ(r,g,b);
			}
			
			cen = faceIter.center (  MSpace::kObject);
			pcur = XYZ(cen.x, cen.y, cen.z);
			pcen = pcur;
			
			int iseg = i - patchstart;
			guide_data[patch_id].P[iseg] = pcur;
			
			faceIter.getNormal ( nor,  MSpace::kObject );
			guide_data[patch_id].N[iseg] = XYZ(nor.x, nor.y, nor.z);
			
			faceIter.getVertices (vertlist);
			
			meshFn.getFaceVertexTangent (i, vertlist[0], tang,  MSpace::kObject);
			tang = nor^tang;
			tang.normalize();
			guide_data[patch_id].T[iseg] = XYZ(tang.x, tang.y, tang.z);
			
			MPoint corner0, corner1, dv;
			if(order == 0)
			{
				meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
				meshFn.getPoint (vertlist[2], corner1, MSpace::kObject );
				
				dv = corner0 - cen + corner1 - cen;
			}
			else
			{
				meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
				meshFn.getPoint (vertlist[0], corner1, MSpace::kObject );
				
				dv = cen - corner1 + cen - corner0;
			}
			
			guide_data[patch_id].dispv[iseg] = XYZ(dv.x, dv.y, dv.z);
			
			if(iseg==0) {
// set guider texcoord
				XYZ rr = pcur - guide_data[patch_id].dispv[iseg]/2;
				MPoint rootp(rr.x, rr.y, rr.z);
				float uv[2]; uv[0] = uv[1] = 0.f;
#ifndef USE_MAYA2007
				baseFn.getUVAtPoint ( rootp, uv);
#endif
				guide_data[patch_id].u = uv[0];
				guide_data[patch_id].v = uv[1];
			}
			
			XYZ side = guide_data[patch_id].dispv[iseg].cross(guide_data[patch_id].N[iseg]);
			side.normalize();
			guide_data[patch_id].T[iseg] = guide_data[patch_id].N[iseg].cross(side);
			guide_data[patch_id].T[iseg].normalize();
			XYZ binor = guide_data[patch_id].N[iseg].cross(guide_data[patch_id].T[iseg]);
			
			guide_data[patch_id].space[iseg].setIdentity();
			guide_data[patch_id].space[iseg].setOrientations(guide_data[patch_id].T[iseg], binor, guide_data[patch_id].N[iseg]);
			guide_data[patch_id].space[iseg].setTranslation(pcen);
			
			if(iseg ==0)
			{
				guide_spaceinv[patch_id] = guide_data[patch_id].space[0];
				guide_spaceinv[patch_id].inverse();
			}
		}
	}
}

void hairMap::updateGuide()
{
	if(!has_guide || !guide_data || oguide.length() != num_guideobj || !nsegbuf) return;
	MStatus status;
	MPoint cen;
	MVector nor, tang;
	MIntArray vertlist;
	int patch_id = -1;
	XYZ pcur, ppre, pcen;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++)
	{
		MFnMesh meshFn(oguide[iguide], &status);
		MItMeshPolygon faceIter(oguide[iguide], &status);
		
		int patchend = 0, patchstart;
		for(int i=0; !faceIter.isDone(); faceIter.next(), i++) {
			if(faceIter.index() == patchend)
			{
				patch_id++;
				patchstart = patchend;
				patchend += nsegbuf[patch_id];
			}
			
			cen = faceIter.center (  MSpace::kObject);
			pcur = XYZ(cen.x, cen.y, cen.z);
			pcen = pcur;
			
			int iseg = i - patchstart;
			guide_data[patch_id].P[iseg] = pcur;
			
			faceIter.getNormal ( nor,  MSpace::kObject );
			guide_data[patch_id].N[iseg] = XYZ(nor.x, nor.y, nor.z);
			
			faceIter.getVertices (vertlist);
			
			meshFn.getFaceVertexTangent (i, vertlist[0], tang,  MSpace::kObject);
			tang = nor^tang;
			tang.normalize();
			guide_data[patch_id].T[iseg] = XYZ(tang.x, tang.y, tang.z);
			
			MPoint corner0, corner1, dv;
			if(order == 0)
			{
				meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
				meshFn.getPoint (vertlist[2], corner1, MSpace::kObject );
				
				dv = corner0 - cen + corner1 - cen;
			}
			else
			{
				meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
				meshFn.getPoint (vertlist[0], corner1, MSpace::kObject );
				
				dv = cen - corner1 + cen -corner0;
			}
			
			guide_data[patch_id].dispv[iseg] = XYZ(dv.x, dv.y, dv.z);
			
			XYZ side = guide_data[patch_id].dispv[iseg].cross(guide_data[patch_id].N[iseg]);
			side.normalize();
			guide_data[patch_id].T[iseg] = guide_data[patch_id].N[iseg].cross(side);
			guide_data[patch_id].T[iseg].normalize();
			XYZ binor = guide_data[patch_id].N[iseg].cross(guide_data[patch_id].T[iseg]);
			
			guide_data[patch_id].space[iseg].setIdentity();
			guide_data[patch_id].space[iseg].setOrientations(guide_data[patch_id].T[iseg], binor, guide_data[patch_id].N[iseg]);
			guide_data[patch_id].space[iseg].setTranslation(pcen);
			
			if(iseg ==0)
			{
				guide_spaceinv[patch_id] = guide_data[patch_id].space[0];
				guide_spaceinv[patch_id].inverse();
			}
		}
	}
}

void hairMap::bind()
{
	if(!guide_data || n_samp < 1) return;
	
	if(bind_data) delete[] bind_data;
	bind_data = new triangle_bind_info[n_samp];
	
	if(pNSeg) delete[] pNSeg;
	pNSeg = new unsigned[n_samp];
	
	for(unsigned i=0; i<n_samp; i++)
	{
// finder 3 three nearest guides
		XY from(ddice[i].coords, ddice[i].coordt);
		ValueAndId* idx = new ValueAndId[num_guide];
		for(unsigned j=0; j<num_guide; j++)
		{
			idx[j].idx = j;
			
			XY to(guide_data[j].u, guide_data[j].v);
			idx[j].val = from.distantTo(to);
		}
		
		QuickSort::sort(idx, 0, num_guide-1);
		
		if(isInterpolate==1) {
			XY corner[3];
			
			for(unsigned hdl=0; hdl<3; hdl++) {
			
				bind_data[i].idx[0] = idx[0].idx;
				bind_data[i].idx[1+hdl] = idx[1+hdl].idx;
				bind_data[i].idx[2+hdl] = idx[2+hdl].idx;
		
				corner[0].x = guide_data[idx[0].idx].u;
				corner[1].x = guide_data[idx[1+hdl].idx].u;
				corner[2].x = guide_data[idx[2+hdl].idx].u;
				corner[0].y = guide_data[idx[0].idx].v;
				corner[1].y = guide_data[idx[1+hdl].idx].v;
				corner[2].y = guide_data[idx[2+hdl].idx].v;
				
				if( BindTriangle::set(corner, from, bind_data[i]) ) hdl = 4;
			}
		}
		else {
			bind_data[i].idx[0] = idx[0].idx;
			bind_data[i].idx[1] = idx[1].idx;
			bind_data[i].idx[2] = idx[2].idx;
			bind_data[i].wei[0] = 1.f;
			bind_data[i].wei[1] = bind_data[i].wei[2] = 0.f;
		}
		//zDisplayFloat3(bind_data[i].wei[0], bind_data[i].wei[1], bind_data[i].wei[2]);
		
		pNSeg[i] = guide_data[idx[0].idx].num_seg * bind_data[i].wei[0] + guide_data[idx[1].idx].num_seg * bind_data[i].wei[1] + guide_data[idx[2].idx].num_seg * bind_data[i].wei[2];
		
		delete[] idx;
	}
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

void hairMap::drawGuideUV()
{
	if(!guide_data) return;
	glPointSize(2.f);
	glBegin(GL_LINES);
	
	for(int i=0; i<num_guide; i++) {
		XYZ cc = guide_data[i].dsp_col;
		glColor3f(cc.x, cc.y, cc.z);
		
		glVertex3f(guide_data[i].u-0.015, guide_data[i].v-0.015, 0);
		glVertex3f(guide_data[i].u+0.015, guide_data[i].v-0.015, 0);
		
		glVertex3f(guide_data[i].u+0.015, guide_data[i].v-0.015, 0);
		glVertex3f(guide_data[i].u+0.015, guide_data[i].v+0.015, 0);
		
		glVertex3f(guide_data[i].u+0.015, guide_data[i].v+0.015, 0);
		glVertex3f(guide_data[i].u-0.015, guide_data[i].v+0.015, 0);
		
		glVertex3f(guide_data[i].u-0.015, guide_data[i].v+0.015, 0);
		glVertex3f(guide_data[i].u-0.015, guide_data[i].v-0.015, 0);
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
		outfile.write((char*)&guide_data[i].u, sizeof(float));
		outfile.write((char*)&guide_data[i].v, sizeof(float));	
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
		outfile.write((char*)&guide_data[i].u, sizeof(float));
		outfile.write((char*)&guide_data[i].v, sizeof(float));
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
	m_cachename = filename;
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
		guide_data[i].space = new MATRIX44F[guide_data[i].num_seg];

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)&guide_data[i].u, sizeof(float));
		infile.read((char*)&guide_data[i].v, sizeof(float));
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
	/*
	for(unsigned i = 0;i<num_guide;i++)
	{
		guide_spaceinv[i].setIdentity();
		XYZ binor = guide_data[i].N[0].cross(guide_data[i].T[0]);
		guide_spaceinv[i].setOrientations(guide_data[i].T[0], binor, guide_data[i].N[0]);
		guide_spaceinv[i].inverse();
	}
	*/
	for(unsigned i = 0;i<num_guide;i++)
	{
		for(unsigned j=0; j<guide_data[i].num_seg; j++)
		{
			guide_data[i].space[j].setIdentity();
			XYZ binor = guide_data[i].N[j].cross(guide_data[i].T[j]);
			guide_data[i].space[j].setOrientations(guide_data[i].T[j], binor, guide_data[i].N[j]);
			guide_data[i].space[j].setTranslation(guide_data[i].P[j]);
		}
		guide_spaceinv[i] = guide_data[i].space[0];
		guide_spaceinv[i].inverse();
	}
// calculate bbox	
	bbox_low = XYZ(10e6, 10e6, 10e6);
	bbox_high = XYZ(-10e6, -10e6, -10e6);
	for(unsigned i = 0;i<num_guide;i++)
	{
		for(unsigned j = 0;j<guide_data[i].num_seg;j++)
		{
			if(guide_data[i].P[j].x < bbox_low.x) bbox_low.x = guide_data[i].P[j].x;
			if(guide_data[i].P[j].y < bbox_low.y) bbox_low.y = guide_data[i].P[j].y;
			if(guide_data[i].P[j].x < bbox_low.z) bbox_low.z = guide_data[i].P[j].z;
			if(guide_data[i].P[j].x > bbox_high.x) bbox_high.x = guide_data[i].P[j].x;
			if(guide_data[i].P[j].y > bbox_high.y) bbox_high.y = guide_data[i].P[j].y;
			if(guide_data[i].P[j].x > bbox_high.z) bbox_high.z = guide_data[i].P[j].z;
		}
	}
	XYZ bboxcen = (bbox_low + bbox_high)/2;
	bbox_low -= bboxcen - bbox_low;
	bbox_high -= bboxcen - bbox_high;
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
		guide_data[i].space = new MATRIX44F[guide_data[i].num_seg];

		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)&guide_data[i].u, sizeof(float));
		infile.read((char*)&guide_data[i].v, sizeof(float));
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
		for(unsigned j=0; j<guide_data[i].num_seg; j++)
		{
			guide_data[i].space[j].setIdentity();
			XYZ binor = guide_data[i].N[j].cross(guide_data[i].T[j]);
			guide_data[i].space[j].setOrientations(guide_data[i].T[j], binor, guide_data[i].N[j]);
			guide_data[i].space[j].setTranslation(guide_data[i].P[j]);
		}
		guide_spaceinv[i] = guide_data[i].space[0];
		guide_spaceinv[i].inverse();
	}
	return 1;
}
//~:
