/*
 *  VWing.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "VWing.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMeshData.h>
#include "FXMLVaneMap.h"
#include "../shared/zGlobal.h"
#include "../shared/zNoise.h"

VWing::VWing():has_nurbs(0),m_time(0),m_noise_period(4),m_noise_k(1),m_seed_offset(13) {}
VWing::~VWing() {}

void VWing::setNurbs(const MObject& obj)
{
	//MDagPath an;
	pnurbs = obj;
	
	has_nurbs = 1;
}

void VWing::draw() const
{
	if(!has_nurbs) return;
	
	MStatus status;
	
	int ncvu, ncvv, nc;
	MPointArray cvs;
	MVector nn, tnu, tnv;
	
		MFnNurbsSurface surff(pnurbs, &status);
		if(status)
		{
			ncvu = surff.numCVsInU();
			ncvv = surff.numCVsInV();
			surff.getCVs( cvs,  MSpace::kWorld);
			
			glBegin(GL_LINES);
			for(int i=0; i<ncvu; i++)
			{
				if(i==1 || i== ncvu-2) continue;
				
				nc = i*ncvv;

				glVertex3f(cvs[nc].x, cvs[nc].y, cvs[nc].z);
				nn = surff.normal ( (float)i/(ncvu-1), 0.0,  MSpace::kWorld );
				nn.normalize();
				if(m_is_reversed==1) nn *= -1;
				glVertex3f(cvs[nc].x+nn.x, cvs[nc].y+nn.y, cvs[nc].z+nn.z);
				
				nc = i*ncvv+ncvv-1;

				glVertex3f(cvs[nc].x, cvs[nc].y, cvs[nc].z);
				nn = surff.normal ( (float)i/(ncvu-1), 0.0,  MSpace::kWorld );
				nn.normalize();
				if(m_is_reversed==1) nn *= -1;
				glVertex3f(cvs[nc].x+nn.x, cvs[nc].y+nn.y, cvs[nc].z+nn.z);
				
				/*
				surff.getTangents ( (float)i/(ncvu-1), 1.0,  tnu,  tnv,  MSpace::kWorld );
				tnv.normalize();
				
				if(m_is_reversed!=1) nn *= -1;
				tnu = nn^tnv;
				tnu.normalize();
				
				tw = m_feather_twist0 + (m_feather_twist1 - m_feather_twist0)*(float)i/((float)ncvu-3);
				tw = -3.1415927f*tw/180.f;
				if(m_is_reversed==1) tw *= -1;
				
				nn = tnu*cos(tw) + nn*sin(tw);
				nn.normalize();
				*/
				//glVertex3f(cvs[nc].x, cvs[nc].y, cvs[nc].z);
				//glVertex3f(cvs[nc].x+nn.x, cvs[nc].y+nn.y, cvs[nc].z+nn.z);
				
			}
			glEnd();
		}
	
	
}

void VWing::getBBox(MPoint& corner1, MPoint& corner2)
{
	if(!has_nurbs) return;
	
	corner1 = MPoint(10e6, 10e6, 10e6);
	corner2 = MPoint(-10e6, -10e6, -10e6);
	
	MStatus status;
	MPointArray cvs;

		MFnNurbsSurface surff(pnurbs, &status);
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
		
	corner1 -= MVector(m_feather_width0, m_feather_width0, m_feather_width0);
	corner2 += MVector(m_feather_width0, m_feather_width0, m_feather_width0);
}

void VWing::save(const char* filename)
{
	if(!has_nurbs) return;
	
	FXMLVaneMap fmap;
	fmap.beginMap(filename);
	
	string data_path(filename);
	zGlobal::cutByLastDot(data_path);
	data_path.append(".vnd");
	
	fmap.dataBegin(data_path.c_str());
	
	MStatus status;
	
	int ncvu, ncvv, nc, nc1, k0, k1;
	MPointArray cvs;
	MVector nn, tnu, tnv, nhit, tohit, tokn0, tokn1, tokn, tnn;
	MPoint vx, cvx, cvx1, phit;
	float w, e, tw, twv, w1, e1, tw1, interpw, interpe, interptw, toknl;
	double hitu, hitv, distance, realu;
	bool wasExactHit;
	MStatus  noinserr;
	
	
	MFnNurbsSurface surff(pnurbs, &status);
	if(status)
	{
		ncvu = surff.numCVsInU();
		ncvv = surff.numCVsInV();
		surff.getCVs( cvs,  MSpace::kWorld);

		XYZ* vert = new XYZ[ncvv-2];
		XYZ* up = new XYZ[ncvv-2];
		XYZ* down = new XYZ[ncvv-2];
		XYZ* nor = new XYZ[ncvv-2];
			
		for(int i=0; i<ncvu-2; i++)
		{
			if(i==ncvu-3)
			{
				nc = (ncvu-1)*ncvv;
				
				w = m_feather_width1;
				e = m_feather_edge1;
				tw = m_feather_twist1;
				tw = -3.1415927f*tw/180.f;
				if(m_is_reversed==1) tw *= -1;
				
				for(int j=1; j<ncvv-1; j++)
				{
					
					k0 = nc+j;
					
					nn = surff.normal ( 1.0, 0.0,  MSpace::kWorld );
					nn.normalize();
					
					if(j!= ncvv-2) tnv = cvs[k0+1] - cvs[k0];
					else tnv = cvs[k0] - cvs[k0-1];
					tnv.normalize();
					
					tnu = nn^tnv;
					tnu.normalize();
					
					twv = tw*sqrt(((float)j-1)/(ncvv-2));
					tnu = tnu*cos(twv) + nn*sin(twv);
					tnu.normalize();
					
					vx = cvs[k0]-tnu*w*e;
					
					if(m_is_reversed==1)
					{
						if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w*e)
							{
								vx = phit - nhit*w/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w*e;
							}
						}

						up[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);

						vert[j-1] = XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);
						
						vx = cvs[k0]+tnu*w;
						// move trailing edge knot beneath wing surface if necessary
						if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w)
							{
								vx = phit + nhit*w/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w;
							}
						}
						

						down[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);
						
						tnn = tnv^tnu;
						tnn.normalize();
						nor[j-1] = XYZ(tnn.x, tnn.y, tnn.z);
						
						
					}
					else
					{
						// move leading edge knot above wing surface if necessary				
						if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w*e)
							{
								vx = phit + nhit*w/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w*e;
							}
						}
						up[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);

						vert[j-1] = XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);
						
						vx = cvs[k0]+tnu*w;
						// move trailing edge knot beneath wing surface if necessary
						if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w)
							{
								vx = phit - nhit*w/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w;
							}
						}
						
						down[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvs[k0].x, cvs[k0].y, cvs[k0].z);
						
						tnn = tnv^tnu;
						tnn.normalize();
						nor[j-1] = XYZ(tnn.x, tnn.y, tnn.z);
					}
				}
				fmap.vaneBegin(ncvv-2);
				fmap.addP(ncvv-2, vert);
				fmap.addVectorArray("up", ncvv-2, up);
				fmap.addVectorArray("down", ncvv-2, down);
				fmap.addVectorArray("surface_n", ncvv-2, nor);
				fmap.vaneEnd();
			}
			else
			{
				if(i==0) 
				{
					nc = 0;
					nc1 = 2*ncvv;
					realu = 0;
				}
				else  
				{
					nc = (i+1)*ncvv;
					nc1 = (i+2)*ncvv;
					realu = float(i)/(ncvu-3);
				}
	
				w = m_feather_width0 + (m_feather_width1 - m_feather_width0)*(float)i/((float)ncvu-3);
				w1 = m_feather_width0 + (m_feather_width1 - m_feather_width0)*((float)i+1)/((float)ncvu-3);
				e = m_feather_edge0 + (m_feather_edge1 - m_feather_edge0)*(float)i/((float)ncvu-3);
				e1 = m_feather_edge0 + (m_feather_edge1 - m_feather_edge0)*((float)i+1)/((float)ncvu-3);
				tw = m_feather_twist0 + (m_feather_twist1 - m_feather_twist0)*(float)i/((float)ncvu-3);
				tw = -3.1415927f*tw/180.f;
				if(m_is_reversed==1) tw *= -1;
				tw1 = m_feather_twist0 + (m_feather_twist1 - m_feather_twist0)*((float)i+1)/((float)ncvu-3);
				tw1 = -3.1415927f*tw1/180.f;
				if(m_is_reversed==1) tw1 *= -1;
				
				for(int k=0; k<m_n_step; k++)
				{
					float paramk = (float)k/m_n_step;
					for(int j=1; j<ncvv-1; j++)
					{
						nn = surff.normal ( realu, 0.0,  MSpace::kWorld );
						nn.normalize();
						
						k0 = nc+j;
						k1 = nc1+j;
						
						tokn0 = cvs[k0] - cvs[nc];
						tokn1 = cvs[k1] - cvs[nc1];
						tokn = tokn0 + (tokn1 - tokn0)*paramk;
						toknl = tokn0.length()*(1-paramk) + tokn1.length()*paramk;
						tokn.normalize();
						
						//cvx = cvs[k0] + (cvs[k1]-cvs[k0])*paramk;
						cvx = cvs[nc] + (cvs[nc1]-cvs[nc])*paramk;
						cvx += tokn*toknl;
						
						if(j!= ncvv-2) 
						{
							cvx1 = cvs[k0+1] + (cvs[k1+1]-cvs[k0+1])*paramk;
							tnv = cvx1 - (cvs[k0] + (cvs[k1]-cvs[k0])*paramk);
						}
						else
						{
							cvx1 = cvs[k0-1] + (cvs[k1-1]-cvs[k0-1])*paramk;
							tnv = cvs[k0] + (cvs[k1]-cvs[k0])*paramk - cvx1;
						}
						
						interpw = w + (w1-w)*paramk;
						interpe = e + (e1-e)*paramk;
						interptw = tw + (tw1-tw)*paramk;
						
						//surff.getTangents ( realu, (double)j/(ncvv-1),  tnu,  tnv,  MSpace::kWorld );
						
						tnv.normalize();
						
						
						tnu = nn^tnv;
						tnu.normalize();
						
						twv = interptw*sqrt(((float)j-1)/(ncvv-2));
						tnu = tnu*cos(twv) + nn*sin(twv);
						tnu.normalize();
						
						vx = cvx - tnu*interpw*interpe;
						
						if(m_is_reversed==1)
						{
							if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < w)
								{
									vx = phit - nhit*w/10;
									tohit = vx - cvs[k0];
									tohit.normalize();
									vx = cvx+tohit*interpw*interpe;
								}
							}
							up[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvx.x, cvx.y, cvx.z);

							vert[j-1] = XYZ(cvx.x, cvx.y, cvx.z);
							
							vx = cvx +tnu*interpw;
							// move trailing edge knot beneath wing surface if necessary
							if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < w)
								{
									vx = phit + nhit*interpw/10;
									tohit = vx - cvx;
									tohit.normalize();
									vx = cvx+tohit*interpw;
								}
							}
							
							down[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvx.x, cvx.y, cvx.z);
					
							tnn = tnv^tnu;
							tnn.normalize();
							nor[j-1] = XYZ(tnn.x, tnn.y, tnn.z);
						}
						else
						{
							// move leading edge knot above wing surface if necessary				
							if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < w)
								{
									vx = phit + nhit*interpw/10;
									tohit = vx - cvx;
									tohit.normalize();
									vx = cvx+tohit*interpw*interpe;
								}
							}
							up[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvx.x, cvx.y, cvx.z);

							vert[j-1] = XYZ(cvx.x, cvx.y, cvx.z);
							
							vx = cvx+tnu*w;
							// move trailing edge knot beneath wing surface if necessary
							if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < w)
								{
									vx = phit - nhit*interpw/10;
									tohit = vx - cvx;
									tohit.normalize();
									vx = cvx+tohit*interpw;
								}
							}
							
							down[j-1] = XYZ(vx.x, vx.y, vx.z) - XYZ(cvx.x, cvx.y, cvx.z);
					
							tnn = tnv^tnu;
							tnn.normalize();
							nor[j-1] = XYZ(tnn.x, tnn.y, tnn.z);
						}
					}
					fmap.vaneBegin(ncvv-2);
					fmap.addP(ncvv-2, vert);
					fmap.addVectorArray("up", ncvv-2, up);
					fmap.addVectorArray("down", ncvv-2, down);
					fmap.addVectorArray("surface_n", ncvv-2, nor);
					fmap.vaneEnd();
				}
			}
		}
		
		delete[] vert;
		delete[] up;
		delete[] down;
		delete[] nor;
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

MObject VWing::createShaft() const
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
	
	int ncvu, ncvv, nc, k0, acc=0, acc1=0;
	MPointArray cvs;
	MVector nn, tnu, tnv;
	MPoint vx;
	float w, dw;
	
	
		MFnNurbsSurface surff(pnurbs, &stat);
		if(stat)
		{
			ncvu = surff.numCVsInU();
			ncvv = surff.numCVsInV();
			surff.getCVs( cvs,  MSpace::kWorld);
			
			numVertex += ncvu*ncvv*4;
			numPolygon += ncvu*(ncvv-1)*4;
						
			for(int i=0; i<ncvu; i++)
			{
				nc = i*ncvv;
				w = m_shaft_width0 + (m_shaft_width1 - m_shaft_width0)*(float)i/(float)ncvu;

			for(int j=0; j<ncvv; j++)
			{
				
				k0 = nc+j;
				
				nn = surff.normal ( 0.5, (double)j/ncvv,  MSpace::kWorld );
				
				surff.getTangents ( 0.5, (double)j/ncvv,  tnu,  tnv,  MSpace::kWorld );
				
				tnu.normalize();
				nn.normalize();
				
				dw = 1.5f - (float)j/(ncvv-1)*(float)j/(ncvv-1);
				
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
				
				vArray.append((float)j/(ncvv-1));
				vArray.append((float)j/(ncvv-1));
				vArray.append((float)j/(ncvv-1));
				vArray.append((float)j/(ncvv-1));
				vArray.append((float)j/(ncvv-1));
			}
			
			for(int j=0; j<ncvv-1; j++)
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

MObject VWing::createFeather()
{
	MStatus stat;
	MFnMeshData dataCreator;
	MObject outMeshData = dataCreator.create(&stat);
	
	int numVertex = 0;
	int numPolygon = 0;
	MPointArray vertexArray;
	MFloatArray uArray, vArray;
	MIntArray polygonCounts, polygonConnects, polygonUVs;
	
	int ncvu, ncvv, nc, nc1, k0, k1, acc=0;
	MPointArray cvs;
	MVector nn, tnu, tnv, nhit, tohit, tokn0, tokn1, tokn;
	MPoint vx, cvx, cvx1, phit;
	float w, e, tw, twv, w1, e1, tw1, interpw, interpe, interptw, toknl, tip, tips, fractnoi;
	double hitu, hitv, distance, realu;
	bool wasExactHit;
	MStatus  noinserr;
	
	
	
	fnoi = m_time - int(m_time);
	
	
	MFnNurbsSurface surff(pnurbs, &stat);
	if(stat)
	{
		ncvu = surff.numCVsInU();
		ncvv = surff.numCVsInV();
		surff.getCVs( cvs,  MSpace::kWorld);
		
		for(int i=0; i<cvs.length(); i++) 
		{
			cvs[i] = surff.closestPoint (cvs[i], &hitu, &hitv, true, m_feather_width1/100,  MSpace::kObject,  &stat );
		}
		
		numVertex += (ncvv-2)*3*((ncvu-2-1)*m_n_step+1);
		numPolygon += (ncvv-2-1)*2*((ncvu-2-1)*m_n_step+1);
					
		for(int i=0; i<ncvu-2; i++)
		{
			if(i==ncvu-3) 
			{
				nc = (ncvu-2)*ncvv;
				
				w = m_feather_width1;
				e = m_feather_edge1;
				tw = m_feather_twist1;
				tw = -3.1415927f*tw/180.f;
				if(m_is_reversed==1) tw *= -1;
				
				nn = surff.normal ( 1.0, 0.1,  MSpace::kWorld );
				nn.normalize();

				g_seed = m_seed_offset  + int(m_time)*3 + i*7;
				vnoi0 = m_noise_k*(randfint( g_seed )-0.5);
				g_seed1 = m_seed_offset  + (int(m_time)+1)*3 + i*7;
				vnoi1 = m_noise_k*(randfint( g_seed1 )-0.5);
				vnoi = vnoi1*fnoi + vnoi0*(1-fnoi);
				
				for(int j=1; j<ncvv-1; j++)
				{
					tips = tip = 1.0f;
					if(j==ncvv-2)
					{
						tip = 0.29f;
						tips = 0;
						e=1;
					}
					k0 = nc+j;
					
					//surff.getTangents ( 1.0, (double)j/(ncvv-1),  tnu,  tnv,  MSpace::kWorld );
					if(j!= ncvv-2) tnv = cvs[k0+1] - cvs[k0];
					else tnv = cvs[k0] - cvs[k0-1];

					tnv.normalize();
					
					
					tnu = nn^tnv;
					tnu.normalize();
					
					twv = tw*sqrt(((float)j-1)/(ncvv-2));
					tnu = tnu*cos(twv) + nn*sin(twv);
					tnu.normalize();
					
					vx = cvs[k0]-tnu*w*e*tips+tnv*w*e*tip;
					
					if(m_is_reversed==1)
					{
						if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w*e*8)
							{
								vx = phit - nhit*w*tip/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w*e*tip;
							}
						}
						vertexArray.append(vx);
						
						vx = cvs[k0];
						vertexArray.append(vx);
						
						vx = cvs[k0]+tnu*w*tips+tnv*w*tip;
						// move trailing edge knot beneath wing surface if necessary
						if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w*8)
							{
								vx = phit + nhit*w*tip/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w*tip;
							}
						}
						
						vertexArray.append(vx);
					}
					else
					{
						// move leading edge knot above wing surface if necessary				
						if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w*e*8)
							{
								vx = phit + nhit*w*tip/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w*e*tip;
							}
						}
						vertexArray.append(vx);
						
						vx = cvs[k0];
						vertexArray.append(vx);
						
						vx = cvs[k0]+tnu*w*tips+tnv*w*tip;
						// move trailing edge knot beneath wing surface if necessary
						if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
						{
							nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
							if(nhit*nn>0.1 && distance < w*8)
							{
								vx = phit - nhit*w*tip/10;
								tohit = vx - cvs[k0];
								tohit.normalize();
								vx = cvs[k0]+tohit*w*tip;
							}
						}
						
						vertexArray.append(vx);
					}
					
					int ivex = vertexArray.length() -1;
					fractnoi = ((float)j-1)/(ncvv-2);
					fractnoi *= fractnoi;
					fractnoi *= fractnoi;
					fractnoi *= fractnoi;
					vertexArray[ivex] += nn*vnoi*fractnoi*0.75;
					vertexArray[ivex-1] += nn*vnoi*fractnoi;
					vertexArray[ivex-2] += nn*vnoi*fractnoi*0.75;
					
					uArray.append(1.0);
					vArray.append(((float)j-1)/(ncvv-3));
					
					uArray.append(0.5);
					vArray.append(((float)j-1)/(ncvv-3));
					
					uArray.append(0.0);
					vArray.append(((float)j-1)/(ncvv-3));
				}
			
				for(int j=0; j<ncvv-3; j++)
				{
					polygonCounts.append(4);
					polygonCounts.append(4);
					
					if(m_is_reversed==1)
						{
							polygonConnects.append(j*3+acc);
							polygonConnects.append(j*3+1+acc);
							polygonConnects.append((j+1)*3+1+acc);
							polygonConnects.append((j+1)*3+acc);

							polygonConnects.append(j*3+1+acc);
							polygonConnects.append(j*3+2+acc);
							polygonConnects.append((j+1)*3+2+acc);
							polygonConnects.append((j+1)*3+1+acc);
							
							polygonUVs.append(j*3+acc);
							polygonUVs.append(j*3+1+acc);
							polygonUVs.append((j+1)*3+1+acc);
							polygonUVs.append((j+1)*3+acc);
							
							polygonUVs.append(j*3+1+acc);
							polygonUVs.append(j*3+2+acc);
							polygonUVs.append((j+1)*3+2+acc);
							polygonUVs.append((j+1)*3+1+acc);
							
						}
						else
						{
							polygonConnects.append(j*3+acc);
							polygonConnects.append((j+1)*3+acc);
							polygonConnects.append((j+1)*3+1+acc);
							polygonConnects.append(j*3+1+acc);
							
							polygonConnects.append(j*3+1+acc);
							polygonConnects.append((j+1)*3+1+acc);
							polygonConnects.append((j+1)*3+2+acc);
							polygonConnects.append(j*3+2+acc);
							
							polygonUVs.append(j*3+acc);
							polygonUVs.append((j+1)*3+acc);
							polygonUVs.append((j+1)*3+1+acc);
							polygonUVs.append(j*3+1+acc);
							
							polygonUVs.append(j*3+1+acc);
							polygonUVs.append((j+1)*3+1+acc);
							polygonUVs.append((j+1)*3+2+acc);
							polygonUVs.append(j*3+2+acc);
						}
				}
				acc = vertexArray.length();
			}
			else
			{
				if(i==0) 
				{
					nc = ncvv;
					nc1 = 2*ncvv;
					realu = 0;
				}
				else  
				{
					nc = (i+1)*ncvv;
					nc1 = (i+2)*ncvv;
					realu = float(i)/(ncvu-3);
				}
				
				w = m_feather_width0 + (m_feather_width1 - m_feather_width0)*(float)i/((float)ncvu-3);
				w1 = m_feather_width0 + (m_feather_width1 - m_feather_width0)*((float)i+1)/((float)ncvu-3);
				e = m_feather_edge0 + (m_feather_edge1 - m_feather_edge0)*(float)i/((float)ncvu-3);
				e1 = m_feather_edge0 + (m_feather_edge1 - m_feather_edge0)*((float)i+1)/((float)ncvu-3);
				tw = m_feather_twist0 + (m_feather_twist1 - m_feather_twist0)*(float)i/((float)ncvu-3);
				tw = -3.1415927f*tw/180.f;
				if(m_is_reversed==1) tw *= -1;
				tw1 = m_feather_twist0 + (m_feather_twist1 - m_feather_twist0)*((float)i+1)/((float)ncvu-3);
				tw1 = -3.1415927f*tw1/180.f;
				if(m_is_reversed==1) tw1 *= -1;
				
				for(int k=0; k<m_n_step; k++)
				{
					g_seed = m_seed_offset  + int(m_time)*3 + i*7 + k*11;
					vnoi0 = m_noise_k*(randfint( g_seed )-0.5);
					g_seed1 = m_seed_offset  + (int(m_time)+1)*3 + i*7 + k*11;
					vnoi1 = m_noise_k*(randfint( g_seed1 )-0.5);
					vnoi = vnoi1*fnoi + vnoi0*(1-fnoi);
					
					float paramk = (float)k/m_n_step;
					for(int j=1; j<ncvv-1; j++)
					{
						tips = tip = 1.0f;
						if(j==ncvv-2) 
						{
							tip = 0.29f;
							tips = 0;
						}
						
						nn = surff.normal ( realu+paramk/(ncvu-2-1), 0.1,  MSpace::kWorld );
						nn.normalize();
						
						k0 = nc+j;
						k1 = nc1+j;
						
						tokn0 = cvs[k0] - cvs[nc];
						tokn1 = cvs[k1] - cvs[nc1];
						tokn = tokn0 + (tokn1 - tokn0)*paramk;
						toknl = tokn0.length()*(1-paramk) + tokn1.length()*paramk;
						tokn.normalize();

						cvx = cvs[nc] + (cvs[nc1]-cvs[nc])*paramk;
						cvx += tokn*toknl;
						
						if(j!= ncvv-2) 
						{
							cvx1 = cvs[k0+1] + (cvs[k1+1]-cvs[k0+1])*paramk;
							tnv = cvx1 - (cvs[k0] + (cvs[k1]-cvs[k0])*paramk);
						}
						else
						{
							cvx1 = cvs[k0-1] + (cvs[k1-1]-cvs[k0-1])*paramk;
							tnv = cvs[k0] + (cvs[k1]-cvs[k0])*paramk - cvx1;
						}
						
						interpw = w + (w1-w)*paramk;
						interpe = e + (e1-e)*paramk;
						interptw = tw + (tw1-tw)*paramk;
						
						//surff.getTangents ( realu, (double)j/(ncvv-1),  tnu,  tnv,  MSpace::kWorld );
						
						tnv.normalize();

						tnu = nn^tnv;
						tnu.normalize();
						
						twv = interptw*sqrt(((float)j-1)/(ncvv-2));
						tnu = tnu*cos(twv) + nn*sin(twv);
						tnu.normalize();
						
						if(j==ncvv-2) interpe = 1;
						
						vx = cvx - tnu*interpw*interpe*tips+tnv*interpw*interpe*tip;
						
						if(m_is_reversed==1)
						{
							if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < interpw*interpe*8)
								{
									vx = phit - nhit*interpw*tip/10;
									tohit = vx - cvs[k0];
									tohit.normalize();
									vx = cvx+tohit*interpw*interpe*tip*1.414;
								}
							}
							vertexArray.append(vx);
							
							vx = cvx;
							vertexArray.append(vx);
							
							vx = cvx +tnu*interpw*tips+tnv*interpw*tip;
							// move trailing edge knot beneath wing surface if necessary
							if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < interpw*8)
								{
									vx = phit + nhit*interpw*tip/10;
									tohit = vx - cvx;
									tohit.normalize();
									vx = cvx+tohit*interpw*tip*1.414;
								}
							}
							
							vertexArray.append(vx);
						}
						else
						{
							// move leading edge knot above wing surface if necessary				
							if(surff.intersect ( vx, nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < w)
								{
									vx = phit + nhit*interpw*tip/10;
									tohit = vx - cvx;
									tohit.normalize();
									vx = cvx+tohit*interpw*interpe*tip*1.414;
								}
							}
							vertexArray.append(vx);
							
							vx = cvx;
							vertexArray.append(vx);
							
							vx = cvx+tnu*interpw*tips+tnv*interpw*tip;
							// move trailing edge knot beneath wing surface if necessary
							if(surff.intersect ( vx, -nn, hitu, hitv,  phit, w/100,  MSpace::kObject , 1, &distance, 0, &wasExactHit,  &noinserr ))
							{
								nhit = surff.normal ( hitu, hitv,  MSpace::kWorld );
								if(nhit*nn>0.1 && distance < w)
								{
									vx = phit - nhit*interpw*tip/10;
									tohit = vx - cvx;
									tohit.normalize();
									vx = cvx+tohit*interpw*tip*1.414;
								}
							}
							
							vertexArray.append(vx);
						}
						
						int ivex = vertexArray.length() -1;
						fractnoi = ((float)j-1)/(ncvv-2);
						fractnoi *= fractnoi;
						fractnoi *= fractnoi;
						fractnoi *= fractnoi;
						vertexArray[ivex] += nn*vnoi*fractnoi*0.75;
						vertexArray[ivex-1] += nn*vnoi*fractnoi;
						vertexArray[ivex-2] += nn*vnoi*fractnoi*0.75;	
						
						uArray.append(1.0);
						vArray.append(((float)j-1)/(ncvv-3));
						
						uArray.append(0.5);
						vArray.append(((float)j-1)/(ncvv-3));
						
						uArray.append(0.0);
						vArray.append(((float)j-1)/(ncvv-3));
					}
				
					for(int j=0; j<ncvv-3; j++)
					{
						polygonCounts.append(4);
						polygonCounts.append(4);
						
						if(m_is_reversed==1)
						{
							polygonConnects.append(j*3+acc);
							polygonConnects.append(j*3+1+acc);
							polygonConnects.append((j+1)*3+1+acc);
							polygonConnects.append((j+1)*3+acc);

							polygonConnects.append(j*3+1+acc);
							polygonConnects.append(j*3+2+acc);
							polygonConnects.append((j+1)*3+2+acc);
							polygonConnects.append((j+1)*3+1+acc);
							
							polygonUVs.append(j*3+acc);
							polygonUVs.append(j*3+1+acc);
							polygonUVs.append((j+1)*3+1+acc);
							polygonUVs.append((j+1)*3+acc);
							
							polygonUVs.append(j*3+1+acc);
							polygonUVs.append(j*3+2+acc);
							polygonUVs.append((j+1)*3+2+acc);
							polygonUVs.append((j+1)*3+1+acc);
							
						}
						else
						{
							polygonConnects.append(j*3+acc);
							polygonConnects.append((j+1)*3+acc);
							polygonConnects.append((j+1)*3+1+acc);
							polygonConnects.append(j*3+1+acc);
							
							polygonConnects.append(j*3+1+acc);
							polygonConnects.append((j+1)*3+1+acc);
							polygonConnects.append((j+1)*3+2+acc);
							polygonConnects.append(j*3+2+acc);
							
							polygonUVs.append(j*3+acc);
							polygonUVs.append((j+1)*3+acc);
							polygonUVs.append((j+1)*3+1+acc);
							polygonUVs.append(j*3+1+acc);
							
							polygonUVs.append(j*3+1+acc);
							polygonUVs.append((j+1)*3+1+acc);
							polygonUVs.append((j+1)*3+2+acc);
							polygonUVs.append(j*3+2+acc);
						}
					}
					acc = vertexArray.length();
				}
			}
		}
	}
	
	
	MFnMesh meshFn;
	meshFn.create( numVertex, numPolygon, vertexArray, polygonCounts, polygonConnects, outMeshData, &stat );
	
	meshFn.setUVs ( uArray, vArray );
	meshFn.assignUVs ( polygonCounts, polygonUVs );
	
	return outMeshData;
}

void VWing::getTexcoordOffset(MDoubleArray& texcoord) const
{
	MStatus stat;
	MFnNurbsSurface surff(pnurbs, &stat);
	
	if(!stat) return;
	
	int ncvu, ncvv;
	
	ncvu = surff.numCVsInU();
	ncvv = surff.numCVsInV();
	
	texcoord.clear();
	float fract;
	//int numVertex = (ncvv-2)*3*((ncvu-2-1)*m_n_step+1);
	//texcoord.setLength( numVertex );
	
	for(int i=0; i<ncvu-2; i++)
	{
		if(i==ncvu-3) for(int j=1; j<ncvv-1; j++) 
		{
			texcoord.append(1.0);
			texcoord.append(1.0);
			texcoord.append(1.0);
		}
		else
		{
			for(int k=0; k<m_n_step; k++)
			{
				fract = float(i*m_n_step+k)/((ncvu-3)*m_n_step);
				for(int j=1; j<ncvv-1; j++)
				{
					texcoord.append(fract );
					texcoord.append(fract );
					texcoord.append(fract );
				}
			}
		}
	}
}
/*


void VWing::init()
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

void VWing::update()
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


void VWing::drawVelocity() const
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
