/*
 *  VVane.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "VVane.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMeshData.h>
#include "FXMLVaneMap.h"
#include "../shared/zGlobal.h"

VVane::VVane():has_nurbs(0) {}
VVane::~VVane() {}

void VVane::setNurbs(const MObjectArray& obj)
{
	//MDagPath an;
	pnurbs.clear();
	for(int i=0; i<obj.length(); i++)
	{
		//zWorks::getTypedPath(MFn::kNurbsSurface , obj[i], an);MGlobal::displayInfo(an.fullPathName());
		pnurbs.append(obj[i]);
	}
	
	has_nurbs = 1;
}

void VVane::draw() const
{
	if(!has_nurbs) return;
	
	MStatus status;
	
	int ncvu, ncvv, nc;
	MPointArray cvs;
	MVector nn;
	float w;
	
	glBegin(GL_LINES);
	for(int i=0; i<pnurbs.length(); i++)
	{
		MFnNurbsSurface surff(pnurbs[i], &status);
		if(status)
		{
			ncvu = surff.numCVsInU();
			ncvv = surff.numCVsInV();
			surff.getCVs( cvs,  MSpace::kWorld);
			
			nc = (ncvu/2)*ncvv;
			nn = cvs[(ncvu-1)*ncvv] - cvs[0];
			w = nn.length()/3;
			/*
			
			
			for(int j=0; j<ncvv-3; j++)
			{
				if(j==0) 
				{
					k0 = nc+0;
					k1 = nc+2;
				}
				else if(j==ncvv-4)
				{
					k0 = nc+ncvv-3;
					k1 = nc+ncvv-1;
				}
				else
				{
					k0 = nc+j+1;
					k1 = nc+j+2;
				}
				glVertex3f(cvs[k0].x, cvs[k0].y, cvs[k0].z);
				glVertex3f(cvs[k1].x, cvs[k1].y, cvs[k1].z);
			}
			*/
			nn = surff.normal ( 0.0, 0.0,  MSpace::kWorld );
			nn.normalize();
		
			glVertex3f(cvs[0].x, cvs[0].y, cvs[0].z);
			glVertex3f(cvs[0].x+nn.x*w, cvs[0].y+nn.y*w, cvs[0].z+nn.z*w);
			
			
			
			glVertex3f(cvs[0].x, cvs[0].y, cvs[0].z);
			glVertex3f(cvs[nc].x+nn.x*w, cvs[nc].y+nn.y*w, cvs[nc].z+nn.z*w);
			
			glVertex3f(cvs[nc].x+nn.x*w, cvs[nc].y+nn.y*w, cvs[nc].z+nn.z*w);
			nc = (ncvu-1)*ncvv;
			glVertex3f(cvs[nc].x, cvs[nc].y, cvs[nc].z);
		}
	}
	glEnd();
}

void VVane::getBBox(MPoint& corner1, MPoint& corner2)
{
	if(!has_nurbs) return;
	
	corner1 = MPoint(10e6, 10e6, 10e6);
	corner2 = MPoint(-10e6, -10e6, -10e6);
	
	MStatus status;
	MPointArray cvs;
	for(int i=0; i<pnurbs.length(); i++)
	{
		MFnNurbsSurface surff(pnurbs[i], &status);
		if(status)
		{
			surff.getCVs( cvs,  MSpace::kWorld);
			for(int j=0; j<cvs.length(); j++)
			{
				if( cvs[j].x < corner1.x ) corner1.x = cvs[j].x;
				if( cvs[j].y < corner1.y ) corner1.y = cvs[j].y;
				if( cvs[j].z < corner1.z ) corner1.z = cvs[j].z;
				if( cvs[j].x > corner2.x ) corner2.x = cvs[j].x;
				if( cvs[j].y > corner2.y ) corner2.y = cvs[j].y;
				if( cvs[j].z > corner2.z ) corner2.z = cvs[j].z;
			}
		}
	}
}

void VVane::save(const char* filename)
{
	if(!has_nurbs) return;
	
	FXMLVaneMap fmap;
	fmap.beginMap(filename);
	
	string data_path(filename);
	zGlobal::cutByLastDot(data_path);
	data_path.append(".vnd");
	
	fmap.dataBegin(data_path.c_str());
	
	MStatus status;
	
	int ncvu, ncvv, nc, k0, k1, k2;
	MPointArray cvs;
	MVector tn;
	
	for(int i=0; i<pnurbs.length(); i++)
	{
		MFnNurbsSurface surff(pnurbs[i], &status);
		if(status)
		{
			ncvu = surff.numCVsInU();
			ncvv = surff.numCVsInV();
			surff.getCVs( cvs,  MSpace::kWorld);
			
			nc = ncvu/2*ncvv;
			
			XYZ* vert = new XYZ[ncvv-2];
			XYZ* up = new XYZ[ncvv-2];
			XYZ* down = new XYZ[ncvv-2];
			XYZ* nn = new XYZ[ncvv-2];
			for(int j=0; j<ncvv-2; j++)
			{
				if(j==0)
				{
					k0 = nc+j;
					k1 = j;
					k2 = (ncvu-1)*ncvv + j;
				}
				else if(j==ncvv-3)
				{
					k0 = nc+j+2;
					k1 = j+2;
					k2 = (ncvu-1)*ncvv + j+2;
				}
				else
				{
					k0 = nc+j+1;
					k1 = j+1;
					k2 = (ncvu-1)*ncvv + j+1;
				}
					
				vert[j] = XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);
				up[j] = XYZ(cvs[k1].x, cvs[k1].y, cvs[k1].z) - vert[j];
				down[j] = XYZ(cvs[k2].x, cvs[k2].y, cvs[k2].z) - vert[j];
				
				tn = surff.normal ( 0.5, (double)j/(ncvv-1),  MSpace::kWorld );
			tn.normalize();
				
				nn[j] = XYZ(tn.x, tn.y, tn.z);
			}
			
			fmap.vaneBegin(ncvv-2);
				fmap.addP(ncvv-2, vert);
				fmap.addVectorArray("up", ncvv-2, up);
				fmap.addVectorArray("down", ncvv-2, down);
				fmap.addVectorArray("surface_n", ncvv-2, nn);
			fmap.vaneEnd();
			
			delete[] vert;
			delete[] up;
			delete[] down;
			delete[] nn;
		}
	}
	
	MPoint corner1, corner2;
	
	getBBox(corner1, corner2);
	
	fmap.addBBox(corner1.x, corner1.y, corner1.z, corner2.x, corner2.y, corner2.z);
		
	fmap.dataEnd();
	
	fmap.endMap(filename);
	
	char info[256];
	sprintf(info, "hairBase writes %s", filename);
	MGlobal::displayInfo ( info );

}

MObject VVane::createShaft()
{
	MStatus stat;
	MFnMeshData dataCreator;
	MObject outMeshData = dataCreator.create(&stat);
	
	int numVertex = 0;
	int numPolygon = 0;
	MPointArray vertexArray;
	MFloatArray uArray, vArray;
	//vertexArray.append(MPoint(0,0,0));
	//vertexArray.append(MPoint(1,0,0));
	//vertexArray.append(MPoint(1,1,0));
	
	MIntArray polygonCounts;
	//polygonCounts.append(3);
	
	MIntArray polygonConnects, polygonUVs;
	//polygonConnects.append(0);
	//polygonConnects.append(1);
	//polygonConnects.append(2);
	
	int ncvu, ncvv, nc, k0, k1, acc=0, acc1=0;
	MPointArray cvs;
	MVector nn, tnu, tnv;
	MPoint vx;
	float w, dw;
	
	for(int i=0; i<pnurbs.length(); i++)
	{
		MFnNurbsSurface surff(pnurbs[i], &stat);
		if(stat)
		{
			ncvu = surff.numCVsInU();
			ncvv = surff.numCVsInV();
			surff.getCVs( cvs,  MSpace::kWorld);
			
			numVertex += (ncvv-2)*4;
			numPolygon += ((ncvv-2)-1)*4;
			
			nc = ncvu/2*ncvv;
			
			nn = cvs[(ncvu-1)*ncvv+ncvv/2] - cvs[ncvv/2];
			w = nn.length()/43;

			for(int j=0; j<ncvv-2; j++)
			{
				if(j==0)
				{
					k0 = nc+j;
					k1 = j;
				}
				else if(j==ncvv-3)
				{
					k0 = nc+j+2;
					k1 = j+2;
					
				}
				else
				{
					k0 = nc+j+1;
					k1 = j+1;
				}
				
				nn = surff.normal ( 0.5, (double)j/ncvv,  MSpace::kWorld );
				
				surff.getTangents ( 0.5, (double)j/ncvv,  tnu,  tnv,  MSpace::kWorld );
				
				tnu.normalize();
				nn.normalize();
				
				dw = 1.5f - (float)j/(ncvv-3)*(float)j/(ncvv-3);
				
				vx = cvs[k0]+tnu*w*dw;
				vertexArray.append(vx);
				uArray.append(0);
				
				vx = cvs[k0]+nn*w*dw;
				vertexArray.append(vx);
				uArray.append(0.25);
				
				vx = cvs[k0]-tnu*w*dw;
				vertexArray.append(vx);
				uArray.append(0.5);
				
				vx = cvs[k0]-nn*w*dw;
				vertexArray.append(vx);
				uArray.append(0.75);
				
				uArray.append(1.0);
				
				vArray.append((float)j/(ncvv-3));
				vArray.append((float)j/(ncvv-3));
				vArray.append((float)j/(ncvv-3));
				vArray.append((float)j/(ncvv-3));
				vArray.append((float)j/(ncvv-3));
			}
			
			for(int j=0; j<ncvv-3; j++)
			{
				polygonCounts.append(4);
				polygonCounts.append(4);
				polygonCounts.append(4);
				polygonCounts.append(4);
				
				polygonConnects.append(j*4+acc);
				polygonConnects.append((j+1)*4+acc);
				polygonConnects.append((j+1)*4+1+acc);
				polygonConnects.append(j*4+1+acc);
				
				polygonConnects.append(j*4+1+acc);
				polygonConnects.append((j+1)*4+1+acc);
				polygonConnects.append((j+1)*4+2+acc);
				polygonConnects.append(j*4+2+acc);
				
				polygonConnects.append(j*4+2+acc);
				polygonConnects.append((j+1)*4+2+acc);
				polygonConnects.append((j+1)*4+3+acc);
				polygonConnects.append(j*4+3+acc);
				
				polygonConnects.append(j*4+3+acc);
				polygonConnects.append((j+1)*4+3+acc);
				polygonConnects.append((j+1)*4+acc);
				polygonConnects.append(j*4+acc);
				
				polygonUVs.append(j*5+acc1);
				polygonUVs.append((j+1)*5+acc1);
				polygonUVs.append((j+1)*5+1+acc1);
				polygonUVs.append(j*5+1+acc1);
				
				polygonUVs.append(j*5+1+acc1);
				polygonUVs.append((j+1)*5+1+acc1);
				polygonUVs.append((j+1)*5+2+acc1);
				polygonUVs.append(j*5+2+acc1);
				
				polygonUVs.append(j*5+2+acc1);
				polygonUVs.append((j+1)*5+2+acc1);
				polygonUVs.append((j+1)*5+3+acc1);
				polygonUVs.append(j*5+3+acc1);
				
				polygonUVs.append(j*5+3+acc1);
				polygonUVs.append((j+1)*5+3+acc1);
				polygonUVs.append((j+1)*5+4+acc1);
				polygonUVs.append(j*5+4+acc1);
			}
			acc = vertexArray.length();
			acc1 = uArray.length();
		}
		
		
	}
	
	MFnMesh meshFn;
	meshFn.create( numVertex, numPolygon, vertexArray, polygonCounts, polygonConnects, outMeshData, &stat );
		
	meshFn.setUVs ( uArray, vArray );
	meshFn.assignUVs ( polygonCounts, polygonUVs );
	
	return outMeshData;
}
/*


void VVane::init()
{
	m_p_buf.clear();
	m_v_buf.clear();
	
	MStatus status;
	MItMeshVertex vertIter(pgrow, MObject::kNullObj, &status);
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

void VVane::update()
{
	MStatus status;
	MItMeshVertex vertIter(pgrow, MObject::kNullObj, &status);
	MPoint cv;
	for( int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		cv = vertIter.position(MSpace::kWorld);
		
		m_v_buf[i] = m_p_buf[i] - cv;
		
		m_p_buf[i] = cv;
	}
}


void VVane::drawVelocity() const
{
if(!has_mesh) return;
	MStatus status;
	MFnMesh meshFn(pgrow, &status );
if(m_v_buf.length() != meshFn.numVertices()) return;
	MItMeshVertex vertIter(pgrow, MObject::kNullObj, &status);
	
	MPoint S;
	MVector N, tang, ttang, binormal, dir, hair_up;
	MColor Cscale, Cerect, Crotate, Cstiff;
	float rot;
	MATRIX44F hair_space, hair_space_inv;
	XYZ dP;
	FBend fb;
	
	MString setScale("fb_scale");
	MString setErect("fb_erect");
	MString setRotate("fb_rotate");
	MString setStiff("fb_stiffness");
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
		
		S = S+dir*Cscale.r*m_scale;
		
		hair_up = dir^binormal;
		
		hair_space.setIdentity();
		hair_space.setOrientations(XYZ(binormal.x, binormal.y, binormal.z), XYZ(hair_up.x, hair_up.y, hair_up.z), XYZ(dir.x, dir.y, dir.z));
		
		S = vertIter.position(MSpace::kWorld);
		
		hair_space.setTranslation(XYZ(S.x, S.y, S.z));
		hair_space_inv = hair_space;
		hair_space_inv.inverse();
		
		dP = XYZ(m_v_buf[i].x, m_v_buf[i].y, m_v_buf[i].z);
		hair_space_inv.transformAsNormal(dP);
		
		if(dP.y<0) dP.y = 0;
		dP *= m_wind/Cstiff.r;
		
		fb.setParam(Cscale.r*m_scale, dP.x, dP.y, 8);
		
		XYZ p0(0,0,0), p, pw;
	
		for(int j=0; j<=8; j++)
		{
			pw = p0;
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
			
			p = XYZ(0,0, Cscale.r*m_scale*j/8);
			
			fb.add(p);
			
			pw = p;
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
			p0 = p;
		}
		
	}
	glEnd();
}


*/
//~: