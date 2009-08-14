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
#include "../shared/zFnEXR.h"
#include "../shared/zGlobal.h"
#include <iostream>
#include <fstream>
using namespace std;

hairMap::hairMap():has_base(0),ddice(0),n_samp(0),has_guide(0),guide_data(0),bind_data(0),guide_spaceinv(0),pNSeg(0),
parray(0),pframe1(0),pconnection(0),uarray(0),varray(0),
sum_area(0.f),mutant_scale(0.f),
draw_step(1),nsegbuf(0),m_offset(1.f),m_bald(0.f),pDensmap(0),is_nil(1)
{
	root_color.x = 1.f; root_color.y = root_color.z = 0.f;
	tip_color.y = 0.7f; tip_color.x = 0.f; tip_color.z = 0.2f;
	mutant_color.x = mutant_color.y = mutant_color.z = 0.f;
	m_densityname = "nil";
}
hairMap::~hairMap() 
{
	if(ddice) delete[] ddice;
	if(guide_data) {
		for(unsigned i=0; i<num_guide; i++) guide_data[i].release();
		delete[] guide_data;
	}
	if(bind_data) delete[] bind_data;
	if(guide_spaceinv) delete[] guide_spaceinv;
	if(parray) delete[] parray;
	if(pframe1) delete[] pframe1;
	if(pconnection) delete[] pconnection;
	if(uarray) delete[] uarray;
	if(varray) delete[] varray;
	if(pNSeg) delete[] pNSeg;
	if(nsegbuf) delete[] nsegbuf;
	if(pDensmap) delete[] pDensmap;
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
	MVector vn;
	for(unsigned i=0; i<n_vert; i++) {
		meshFn.getVertexNormal (i, vn, MSpace::kObject);
		parray[i] = XYZ(p_vert[i].x+vn.x*m_offset, p_vert[i].y+vn.y*m_offset, p_vert[i].z+vn.z*m_offset);
	}
	
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

int hairMap::pushFaceVertice()
{
	if(!pconnection || !parray) return 0;
	
	n_samp = n_tri*3;
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[n_samp];
	
	int a, b, c;
	for(unsigned i=0; i<n_tri; i++) {
		a = pconnection[i*3];
		b = pconnection[i*3+1];
		c = pconnection[i*3+2];
		
		ddice[i*3].alpha = 1.f;
		ddice[i*3].beta = 0.f;
		ddice[i*3].gamma = 0.f;
		ddice[i*3].id0 = a;
		ddice[i*3].id1 = b;
		ddice[i*3].id2 = c;
		ddice[i*3].coords = uarray[i*3];
		ddice[i*3].coordt = varray[i*3];
		
		ddice[i*3+1].alpha = 1.f;
		ddice[i*3+1].beta = 0.f;
		ddice[i*3+1].gamma = 0.f;
		ddice[i*3+1].id0 = b;
		ddice[i*3+1].id1 = c;
		ddice[i*3+1].id2 = a;
		ddice[i*3+1].coords = uarray[i*3+1];
		ddice[i*3+1].coordt = varray[i*3+1];
		
		ddice[i*3+2].alpha = 1.f;
		ddice[i*3+2].beta = 0.f;
		ddice[i*3+2].gamma = 0.f;
		ddice[i*3+2].id0 = c;
		ddice[i*3+2].id1 = a;
		ddice[i*3+2].id2 = b;
		ddice[i*3+2].coords = uarray[i*3+2];
		ddice[i*3+2].coordt = varray[i*3+2];
	}
	
	return n_samp;
}

void hairMap::draw()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray || !pNSeg) return;
	
	int g_seed = 13;
	FNoise fnoi;
	float noi;
	
	char* isoverbald = new char[n_samp];
	for(unsigned i=0; i<n_samp; i++) {
		noi = fnoi.randfint( g_seed ); g_seed++;
		if(pDensmap) muliplyDensityMap(noi, ddice[i].coords, ddice[i].coordt);
		if(noi > m_bald) isoverbald[i] = 1;
		else isoverbald[i] = 0;
	}
	
	XYZ* pbuf = new XYZ[n_samp];
	for(unsigned i=0; i<n_samp; i++) pbuf[i] = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
	
	float keepx;
	MATRIX44F tspace, tspace1, tspace2;
	XYZ ppre, pcur, dv, ddv, cc, pobj, pt[3], pw[3], dv0, dv1, dv2;
	glBegin(GL_LINES);
	for(unsigned i=0; i<n_samp; i += draw_step)
	{
		if(isoverbald[i]) {
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
			if(bind_data[i].wei[0] > .91f) {
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
	}
	glEnd();
	
	delete[] pbuf;
	delete[] isoverbald;
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
	
	if(guide_data) {
		for(unsigned i=0; i<num_guide; i++) guide_data[i].release();
		delete[] guide_data;
	}
	
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
	
	guide_data = new Dguide[num_guide];
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];

	MPoint cen;
	MVector nor;
	MIntArray vertlist;
	float r,g,b;
	int patch_id=-1;
	XYZ pcur, pcen;
	for(unsigned iguide=0; iguide<oguide.length(); iguide++) {
		MFnMesh meshFn(oguide[iguide], &status);
		MItMeshPolygon faceIter(oguide[iguide], &status);

		int patchend = 0, patchstart = 0;
		for(int i=0; !faceIter.isDone(); faceIter.next(), i++) {
			if(i == patchend) {
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
			//guide_data[patch_id].P[iseg] = pcur;
			
			faceIter.getNormal ( nor,  MSpace::kObject );
			guide_data[patch_id].N[iseg] = XYZ(nor.x, nor.y, nor.z);
			
			faceIter.getVertices (vertlist);
			/*
			meshFn.getFaceVertexTangent (i, vertlist[0], tang,  MSpace::kObject);
			tang = nor^tang;
			tang.normalize();
			guide_data[patch_id].T[iseg] = XYZ(tang.x, tang.y, tang.z);
			*/
			MPoint corner0, corner1, corner2, corner3;
			/*meshFn.getPoint (vertlist[0], corner0, MSpace::kObject );
			meshFn.getPoint (vertlist[1], corner1, MSpace::kObject );
			meshFn.getPoint (vertlist[2], corner2, MSpace::kObject );
			meshFn.getPoint (vertlist[3], corner3, MSpace::kObject );*/
			MVector dv;// = (corner2 + corner3)/2 - (corner0 + corner1)/2;
				
			//if(order == 0)
			//{
			meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
			meshFn.getPoint (vertlist[2], corner1, MSpace::kObject );
			meshFn.getPoint (vertlist[0], corner2, MSpace::kObject );
			meshFn.getPoint (vertlist[3], corner3, MSpace::kObject );
				
			dv = corner0 - cen + corner1 - cen;
			dv += cen - corner2 + cen - corner3;
			dv /= 2.0;
			//}
			//else
			//{
			//	meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
			//	meshFn.getPoint (vertlist[0], corner1, MSpace::kObject );
				
			//	dv = cen - corner1 + cen - corner0;
			//}
			
			guide_data[patch_id].dispv[iseg] = XYZ(dv.x, dv.y, dv.z);
			//dv.normalize();
			//guide_data[patch_id].T[iseg] = XYZ(dv.x, dv.y, dv.z);
			
			MPoint pr = cen - dv/2;
			guide_data[patch_id].P[iseg] = XYZ(pr.x, pr.y, pr.z);
			
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
				
				meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
				meshFn.getPoint (vertlist[0], corner1, MSpace::kObject );
				
				MVector dwidth = corner1 - corner0;
				guide_data[patch_id].radius = dwidth.length();
				
				MPoint pom;
				int iface;
				baseFn.getClosestPoint(pr, pom, MSpace::kObject, &iface);
				guide_data[patch_id].faceId = iface;
				//baseFn.getPointAtUV (iface, pom, uv, MSpace::kObject);
				guide_data[patch_id].P[iseg] = XYZ(pom.x, pom.y, pom.z);
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
	
	//for(unsigned i=0; i<num_guide; i++) guide_data[i].root = guide_data[i].P[0] - guide_data[i].dispv[0]/2;
// at least radius should be distance to closest guider in 3D
	for(unsigned i=0; i<num_guide; i++) {
		XYZ pto = guide_data[i].P[0];
		float mindist = 10e6;
		for(unsigned j=0; j<num_guide; j++) {
			if(j != i) {
				XYZ d2p = guide_data[j].P[0] - pto;
				float adist = d2p.length();
				if(adist < mindist) mindist = adist;
			}
		}
		if(guide_data[i].radius < mindist) guide_data[i].radius = mindist;
	}
}

void hairMap::updateGuide()
{
	if(!has_guide || !guide_data || oguide.length() != num_guideobj || !nsegbuf) return;
	
	MStatus status;
	MFnMesh baseFn(obase, &status );
	
	MPoint cen;
	MVector nor;
	MIntArray vertlist;
	int patch_id = -1;
	XYZ pcur, pcen;
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
			//guide_data[patch_id].P[iseg] = pcur;
			
			faceIter.getNormal ( nor,  MSpace::kObject );
			guide_data[patch_id].N[iseg] = XYZ(nor.x, nor.y, nor.z);
			
			faceIter.getVertices (vertlist);
			/*
			meshFn.getFaceVertexTangent (i, vertlist[0], tang,  MSpace::kObject);
			tang = nor^tang;
			tang.normalize();
			guide_data[patch_id].T[iseg] = XYZ(tang.x, tang.y, tang.z);
			*/
			
			MPoint corner0, corner1, corner2, corner3;
			/*meshFn.getPoint (vertlist[0], corner0, MSpace::kObject );
			meshFn.getPoint (vertlist[1], corner1, MSpace::kObject );
			meshFn.getPoint (vertlist[2], corner2, MSpace::kObject );
			meshFn.getPoint (vertlist[3], corner3, MSpace::kObject );*/
			MVector dv;// = (corner2 + corner3)/2 - (corner0 + corner1)/2;
			
			//if(order == 0)
			//{
				meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
				meshFn.getPoint (vertlist[2], corner1, MSpace::kObject );
				meshFn.getPoint (vertlist[0], corner2, MSpace::kObject );
				meshFn.getPoint (vertlist[3], corner3, MSpace::kObject );
				
				dv = corner0 - cen + corner1 - cen;
				dv += cen - corner2 + cen - corner3;
				dv /= 2.0;
			//}
			//else
			//{
			//	meshFn.getPoint (vertlist[1], corner0, MSpace::kObject );
			//	meshFn.getPoint (vertlist[0], corner1, MSpace::kObject );
				
			//	dv = cen - corner1 + cen -corner0;
			//}
			
			guide_data[patch_id].dispv[iseg] = XYZ(dv.x, dv.y, dv.z);
			MPoint pr = cen - dv/2;
			guide_data[patch_id].P[iseg] = XYZ(pr.x, pr.y, pr.z);
#ifndef USE_MAYA2007			
			if(iseg==0) {
				MPoint pom;
				int iface = guide_data[patch_id].faceId;
				float uv[2];
				uv[0] = guide_data[patch_id].u;
				uv[1] = guide_data[patch_id].v;
				MString smap("map1");
				baseFn.getPointAtUV (iface, pom, uv, MSpace::kObject, &smap, 1.0);
				guide_data[patch_id].P[iseg] = XYZ(pom.x, pom.y, pom.z);
			}
#endif			
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
// restrict st between 0 - 1
		if(ddice[i].coords < 0) ddice[i].coords = 0;
		if(ddice[i].coords > 1) ddice[i].coords = 1;
		if(ddice[i].coordt < 0) ddice[i].coordt = 0;
		if(ddice[i].coordt > 1) ddice[i].coordt = 1;
		
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
// find closest guider valid in 3D
		unsigned validid = 0;
		XYZ pto = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;
		XYZ dto = pto - guide_data[idx[validid].idx].P[0];
		float dist2valid = dto.length();
		
		while(dist2valid > guide_data[idx[validid].idx].radius*2 && validid<num_guide-1) {
			validid++;
			dto = pto - guide_data[idx[validid].idx].P[0];
			dist2valid = dto.length();
		}
		
		bind_data[i].wei[0] = 1.f;
		bind_data[i].wei[1] = bind_data[i].wei[2] = 0.f;
		
		bind_data[i].idx[0] = bind_data[i].idx[1] = bind_data[i].idx[2] = idx[validid].idx;
		
		if(validid < num_guide-6) {
			XY corner[3]; XYZ pw[3]; float dist[3];

			for(unsigned hdl=0; hdl<3; hdl++) {
			
				bind_data[i].idx[0] = idx[validid].idx;
				bind_data[i].idx[1] = idx[validid+1+hdl].idx;
				bind_data[i].idx[2] = idx[validid+2+hdl].idx;
		
				corner[0].x = guide_data[idx[validid].idx].u;
				corner[1].x = guide_data[idx[validid+1+hdl].idx].u;
				corner[2].x = guide_data[idx[validid+2+hdl].idx].u;
				corner[0].y = guide_data[idx[validid].idx].v;
				corner[1].y = guide_data[idx[validid+1+hdl].idx].v;
				corner[2].y = guide_data[idx[validid+2+hdl].idx].v;
				
				pw[0] = guide_data[idx[validid].idx].P[0];
				pw[1] = guide_data[idx[validid+1+hdl].idx].P[0];
				pw[2] = guide_data[idx[validid+2+hdl].idx].P[0];
				
				dist[0] = guide_data[idx[validid].idx].radius;
				dist[1] = guide_data[idx[validid+1+hdl].idx].radius;
				dist[2] = guide_data[idx[validid+2+hdl].idx].radius;
				
				if( BindTriangle::set(corner, from, pw, pto, dist, bind_data[i]) ) hdl = 4;
				else bind_data[i].idx[0] = bind_data[i].idx[1] = bind_data[i].idx[2] = idx[validid].idx;
			}
		}
		//zDisplayFloat3(bind_data[i].wei[0], bind_data[i].wei[1], bind_data[i].wei[2]);
		
		pNSeg[i] = guide_data[bind_data[i].idx[0]].num_seg;// * bind_data[i].wei[0] + guide_data[bind_data[i].idx[1]].num_seg * bind_data[i].wei[1] + guide_data[bind_data[i].idx[2]].num_seg * bind_data[i].wei[2];
		
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
			float ss = guide_data[i].radius;
			glVertex3f(pp.x, pp.y, pp.z);
			
			if(j==0) pp += guide_data[i].N[j]*ss;
			else pp += guide_data[i].N[j]*ss*0.25f;
			
			glVertex3f(pp.x, pp.y, pp.z);
			pp = guide_data[i].P[j];
			glVertex3f(pp.x, pp.y, pp.z);
			
			//if(j==0) 
			pp += guide_data[i].dispv[j];
			//else pp += guide_data[i].T[j]*ss*0.25f;
			
			glVertex3f(pp.x, pp.y, pp.z);
			
			if(j==0) {
				pp = guide_data[i].P[j];
				glVertex3f(pp.x, pp.y, pp.z);
				pp = guide_data[i].P[j+1];
				glVertex3f(pp.x, pp.y, pp.z);
			}
			
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
		outfile.write((char*)&guide_data[i].num_seg,sizeof(short));
		outfile.write((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)&guide_data[i].u, sizeof(float));
		outfile.write((char*)&guide_data[i].v, sizeof(float));
		outfile.write((char*)&guide_data[i].radius, sizeof(float));
	}
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
		outfile.write((char*)&guide_data[i].num_seg,sizeof(short));
		outfile.write((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		outfile.write((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));	
		outfile.write((char*)&guide_data[i].u, sizeof(float));
		outfile.write((char*)&guide_data[i].v, sizeof(float));
		outfile.write((char*)&guide_data[i].radius, sizeof(float));
	}
	outfile.write((char*)&sum_area, sizeof(float));
	outfile.write((char*)&n_tri, sizeof(unsigned));
	outfile.write((char*)pconnection, sizeof(int)*n_tri*3);
	outfile.write((char*)&n_vert, sizeof(unsigned));
	outfile.write((char*)parray, sizeof(XYZ)*n_vert);
	outfile.write((char*)uarray, sizeof(float)*n_tri*3);
	outfile.write((char*)varray, sizeof(float)*n_tri*3);
	outfile.write((char*)&n_samp,sizeof(int));
	outfile.write((char*)bind_data,sizeof(triangle_bind_info)*n_samp);
	outfile.write((char*)pNSeg,sizeof(unsigned)*n_samp);
	outfile.write((char*)ddice,sizeof(DiceParam)*n_samp);
	outfile.close();
	return 1;
}

int hairMap::load(const char* filename)
{
	is_nil = 1;
	ifstream infile;
	infile.open(filename, ios_base::in | ios_base::binary);
	if(!infile.is_open()) {
		MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		return 0;
	}
	m_cachename = filename;
	infile.read((char*)&num_guide,sizeof(unsigned));

	for(unsigned i = 0;i<num_guide;i++) {
		infile.read((char*)&guide_data[i].num_seg, sizeof(short));
		infile.read((char*)guide_data[i].P, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].N, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].T, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)guide_data[i].dispv, guide_data[i].num_seg*sizeof(XYZ));
		infile.read((char*)&guide_data[i].u, sizeof(float));
		infile.read((char*)&guide_data[i].v, sizeof(float));
		infile.read((char*)&guide_data[i].radius, sizeof(float));
	}
	
	infile.read((char*)&n_vert, sizeof(unsigned));
	
	infile.read((char*)parray, sizeof(XYZ)*n_vert);
	
	infile.close();
	
	if(pframe1) delete[] pframe1;
	pframe1 = new XYZ[n_vert];
	
	for(unsigned i=0; i<n_vert; i++) pframe1[i] = parray[i];
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];
	for(unsigned i = 0;i<num_guide;i++) {
		for(unsigned j=0; j<guide_data[i].num_seg; j++) {
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
	
	for(unsigned i = 0;i<num_guide;i++) {
		for(unsigned j = 0;j<guide_data[i].num_seg;j++) {
			if(guide_data[i].P[j].x < bbox_low.x) bbox_low.x = guide_data[i].P[j].x;
			if(guide_data[i].P[j].y < bbox_low.y) bbox_low.y = guide_data[i].P[j].y;
			if(guide_data[i].P[j].z < bbox_low.z) bbox_low.z = guide_data[i].P[j].z;
			if(guide_data[i].P[j].x > bbox_high.x) bbox_high.x = guide_data[i].P[j].x;
			if(guide_data[i].P[j].y > bbox_high.y) bbox_high.y = guide_data[i].P[j].y;
			if(guide_data[i].P[j].z > bbox_high.z) bbox_high.z = guide_data[i].P[j].z;
		}
	}
	
	XYZ bboxcen = (bbox_low + bbox_high)/2;
	bbox_low -= (bboxcen - bbox_low)/10;
	bbox_high -= (bboxcen - bbox_high)/10;

	is_nil = 0;
	return 1;
}

int hairMap::loadStart(const char* filename)
{
	is_nil = 1;
	if(guide_data) {
		for(unsigned i=0; i<num_guide; i++) guide_data[i].release();
		delete[] guide_data;
	}
	
	ifstream infile;
	infile.open(filename, ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		MGlobal::displayWarning(MString("Cannot open file: ")+filename);
		return 0;
	}
	infile.read((char*)&num_guide,sizeof(unsigned));
	
    guide_data = new Dguide[num_guide];
	for(unsigned i = 0;i<num_guide;i++)
	{
		infile.read((char*)&guide_data[i].num_seg, sizeof(short));

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
		infile.read((char*)&guide_data[i].radius, sizeof(float));
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
	
	infile.read((char*)&n_samp,sizeof(int));
	
	if(bind_data) delete[] bind_data;
	bind_data = new triangle_bind_info[n_samp];
	
	if(pNSeg) delete[] pNSeg;
	pNSeg = new unsigned[n_samp];
	
	if(ddice) delete[] ddice;
	ddice = new DiceParam[n_samp];
	
	infile.read((char*)bind_data,sizeof(triangle_bind_info)*n_samp);
	infile.read((char*)pNSeg,sizeof(unsigned)*n_samp);
	infile.read((char*)ddice,sizeof(DiceParam)*n_samp);
	
	infile.close();	
	
	if(guide_spaceinv) delete[] guide_spaceinv;
	guide_spaceinv = new MATRIX44F[num_guide];
	
	for(unsigned i = 0;i<num_guide;i++) {
		for(unsigned j=0; j<guide_data[i].num_seg; j++) {
			guide_data[i].space[j].setIdentity();
			XYZ binor = guide_data[i].N[j].cross(guide_data[i].T[j]);
			guide_data[i].space[j].setOrientations(guide_data[i].T[j], binor, guide_data[i].N[j]);
			guide_data[i].space[j].setTranslation(guide_data[i].P[j]);
		}
		guide_spaceinv[i] = guide_data[i].space[0];
		guide_spaceinv[i].inverse();
	}
	
	is_nil = 0;
	return 1;
}

int hairMap::loadNext()
{
	std::string filename = m_cachename;
	int frm = zGlobal::getFrameNumber(filename);
	frm++;
	zGlobal::setFrameNumberAndExtension(filename, frm, "hair");
	ifstream infile;
	infile.open(filename.c_str(), ios_base::in | ios_base::binary);
	if(!infile.is_open()) 
	{
		cout<<"Cannot open file: "<<filename<<endl;
		return 0;
	}
	
	int offs = sizeof(unsigned);
	
	for(unsigned i = 0;i<num_guide;i++)
	{
		offs += sizeof(short);
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(XYZ)*guide_data[i].num_seg;
		offs += sizeof(float)*3;
	}
	
	offs += sizeof(unsigned);
	
	infile.seekg(offs, ios::beg);
	if(pframe1) delete[] pframe1;
	pframe1 = new XYZ[n_vert];
	infile.read((char*)pframe1, sizeof(XYZ)*n_vert);
	
	infile.close();
	
	return 1;
}

void hairMap::drawBind()
{
	if(n_samp < 1 || !bind_data || !guide_data || !parray || !pNSeg) return;
	XYZ p0, p1;
	float eta;
	glBegin(GL_LINES);
	for(unsigned i=0; i<n_samp; i += draw_step) {
		p0 = parray[ddice[i].id0]*ddice[i].alpha + parray[ddice[i].id1]*ddice[i].beta + parray[ddice[i].id2]*ddice[i].gamma;

		p1 = guide_data[bind_data[i].idx[0]].P[0];
		eta = bind_data[i].wei[0];
		glColor3f(eta,eta,eta);
		glVertex3f(p0.x, p0.y, p0.z);
		glColor3f(eta, 0, 0);
		glVertex3f(p1.x, p1.y, p1.z);
		
		eta = bind_data[i].wei[1];
		if(eta>0) {
			p1 = guide_data[bind_data[i].idx[1]].P[0];
			
			glColor3f(eta,eta,eta);
			glVertex3f(p0.x, p0.y, p0.z);
			glColor3f(eta, 0, 0);
			glVertex3f(p1.x, p1.y, p1.z);
		}
		
		eta = bind_data[i].wei[2];
		if(eta > 0) {
			p1 = guide_data[bind_data[i].idx[2]].P[0];
			
			glColor3f(eta,eta,eta);
			glVertex3f(p0.x, p0.y, p0.z);
			glColor3f(eta, 0, 0);
			glVertex3f(p1.x, p1.y, p1.z);
		}
		
	}
	glEnd();
}

void hairMap::createSnow(double& time, MObject& meshData) const
{
	MPointArray vertexArray;
	MIntArray polygonCounts;
	MIntArray polygonConnects;
	MFloatArray uarray, varray;
	
	int g_seed;
	FNoise fnoi;
	float noi, size;
	float kvel = time - (int)time;
	
	int acc = 0;
	XYZ a[3], b[3], c[3], d[3], dv, ma, mb, mc, md; 
	MATRIX44F tspace[3];
	for(unsigned i=0; i<n_samp; i++) {
		noi  = fnoi.randfint( g_seed ); g_seed++;
		if(noi<m_snow_rate) {
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			
			guide_data[bind_data[i].idx[0]].getSpaceAtParam(tspace[0], noi*0.5);
			guide_data[bind_data[i].idx[1]].getSpaceAtParam(tspace[1], noi*0.5);
			guide_data[bind_data[i].idx[2]].getSpaceAtParam(tspace[2], noi*0.5);

			a[0] = a[1] = a[2] = XYZ(0.f);
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi -= .5f;
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			size = (1.f+(noi-0.5)*0.5)*m_snow_size;
			
			b[0] = b[1] = b[2] = XYZ(noi* size,0, size);
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi -= .5f;
			c[0] = c[1] = c[2] = XYZ(noi* size, size,0);
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			size = (1.f+(noi-0.5)*0.5)*m_snow_size;
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi -= .5f;
			d[0] = d[1] = d[2] = XYZ(noi* size, size, size);
			
			tspace[0].transform(a[0]);
			tspace[0].transform(b[0]);
			tspace[0].transform(c[0]);
			tspace[0].transform(d[0]);
			tspace[1].transform(a[1]);
			tspace[1].transform(b[1]);
			tspace[1].transform(c[1]);
			tspace[1].transform(d[1]);
			tspace[2].transform(a[2]);
			tspace[2].transform(b[2]);
			tspace[2].transform(c[2]);
			tspace[2].transform(d[2]);
			
			noi = fnoi.randfint( g_seed ); g_seed++;
			
			size = (1.f - noi)*fnoi.randfint( g_seed ); g_seed++;
			
			ma = a[0]*noi;
			ma += a[1]*size;
			ma += a[2]*(1.f-noi-size);
			
			mb = b[0]*noi;
			mb += b[1]*size;
			mb += b[2]*(1.f-noi-size);
			
			mc = c[0]*noi;
			mc += c[1]*size;
			mc += c[2]*(1.f-noi-size);
			
			md = d[0]*noi;
			md += d[1]*size;
			md += d[2]*(1.f-noi-size);
			
			if(kvel>0) {
				dv = (pframe1[ddice[i].id0] - parray[ddice[i].id0])*kvel;
				ma += dv;
				mb += dv;
				mc += dv;
				md += dv;
			}
			
			vertexArray.append(MPoint(ma.x, ma.y, ma.z));
			vertexArray.append(MPoint(mb.x, mb.y, mb.z));
			vertexArray.append(MPoint(mc.x, mc.y, mc.z));
			vertexArray.append(MPoint(md.x, md.y, md.z));
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi /= 4;
			uarray.append(noi);
			uarray.append(1.0-noi);
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi /= 4;
			uarray.append(noi);
			uarray.append(1.0-noi);
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi /= 4;
			varray.append(1.0-noi);
			varray.append(1.0-noi);
			noi  = fnoi.randfint( g_seed ); g_seed++;
			noi /= 4;
			varray.append(noi);
			varray.append(noi);
			
			polygonCounts.append(3);
			polygonCounts.append(3);
			
			noi  = fnoi.randfint( g_seed ); g_seed++;
			if(noi>.5f) {
				polygonConnects.append(acc);
				polygonConnects.append(acc+2);
				polygonConnects.append(acc+1);
				
				polygonConnects.append(acc+3);
				polygonConnects.append(acc+1);
				polygonConnects.append(acc+2);
			}
			else {
				polygonConnects.append(acc+1);
				polygonConnects.append(acc);
				polygonConnects.append(acc+3);
				
				polygonConnects.append(acc+2);
				polygonConnects.append(acc+3);
				polygonConnects.append(acc);
			}
			
			acc += 4;
		}

	}
		
	MFnMesh meshFn;
	meshFn.create(vertexArray.length(), polygonCounts.length(), vertexArray, polygonCounts, polygonConnects, meshData );
	meshFn.setUVs ( uarray, varray );
	meshFn.assignUVs ( polygonCounts, polygonConnects );
}

void hairMap::setDensityMap(const char* filename)
{
	densmap_w =-1, densmap_h = -1;
	if(pDensmap) {
		delete[] pDensmap;
		pDensmap = 0;
		m_densityname = "nil";
	}
	try 
	{
		ZFnEXR::checkFile(filename, &densmap_w, &densmap_h);
	}
	catch (const std::exception &exc) 
    { 
		MGlobal::displayWarning(exc.what()); 
	}
	if(densmap_w > 16 && densmap_h > 16) {
		pDensmap = new float[densmap_w*densmap_h];
		ZFnEXR::readR(filename, densmap_w, densmap_h, pDensmap);
		m_densityname = filename;
	}
}

void hairMap::muliplyDensityMap(float& val, float& s, float& t) const
{
	int it = (densmap_h-1)*(1.f-t);
	int is = (densmap_w-1)*s;
	val *= pDensmap[it*densmap_w + is];
}

void hairMap::drawBBox()
{
	glBegin(GL_LINES);
		glVertex3f(bbox_low.x, bbox_low.y, bbox_low.z);
		glVertex3f(bbox_high.x, bbox_low.y, bbox_low.z);
		glVertex3f(bbox_low.x, bbox_high.y, bbox_low.z);
		glVertex3f(bbox_high.x, bbox_high.y, bbox_low.z);
		glVertex3f(bbox_low.x, bbox_low.y, bbox_high.z);
		glVertex3f(bbox_high.x, bbox_low.y, bbox_high.z);
		glVertex3f(bbox_low.x, bbox_high.y, bbox_high.z);
		glVertex3f(bbox_high.x, bbox_high.y, bbox_high.z);
		
		glVertex3f(bbox_low.x, bbox_low.y, bbox_low.z);
		glVertex3f(bbox_low.x, bbox_high.y, bbox_low.z);
		glVertex3f(bbox_high.x, bbox_low.y, bbox_low.z);
		glVertex3f(bbox_high.x, bbox_high.y, bbox_low.z);
		glVertex3f(bbox_low.x, bbox_low.y, bbox_high.z);
		glVertex3f(bbox_low.x, bbox_high.y, bbox_high.z);
		glVertex3f(bbox_high.x, bbox_low.y, bbox_high.z);
		glVertex3f(bbox_high.x, bbox_high.y, bbox_high.z);
		
		glVertex3f(bbox_low.x, bbox_low.y, bbox_low.z);
		glVertex3f(bbox_low.x, bbox_low.y, bbox_high.z);
		glVertex3f(bbox_high.x, bbox_low.y, bbox_low.z);
		glVertex3f(bbox_high.x, bbox_low.y, bbox_high.z);
		glVertex3f(bbox_low.x, bbox_high.y, bbox_low.z);
		glVertex3f(bbox_low.x, bbox_high.y, bbox_high.z);
		glVertex3f(bbox_high.x, bbox_high.y, bbox_low.z);
		glVertex3f(bbox_high.x, bbox_high.y, bbox_high.z);
	glEnd();
}
//~:
