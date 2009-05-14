/*
 *  DFeather.cpp
 *  featherTest
 *
 *  Created by zhang on 08-12-18.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DFeather.h"
#include "FBend.h"
//#include "../shared/gBase.h"

//static float sf[10] = {1.0f, 1.0f, 0.99f, 0.98f, 0.96f, 0.9f, 0.7f, 0.4f, 0.06f, 0.05f};

DFeather::DFeather():m_wh(0.125f) {}
DFeather::~DFeather() {}

void DFeather::init(const DHair* data, const CoeRec* rt, int n_data)
{
	n_hair = n_data;
	hairs = new DHair[n_data];
	coeffs = new CoeRec[n_data];
	for(int i=0; i<n_data; i++)
	{
		hairs[i] = data[i];
		coeffs[i] = rt[i];
	}
}

void DFeather::init(const DHair* data, int n_data)
{
	n_hair = n_data;
	hairs = new DHair[n_data];
	for(int i=0; i<n_data; i++) hairs[i] = data[i];
}
/*
void DFeather::draw() const
{
	MATRIX44F hair_space;
	FBend fb;
	XYZ vt, vc;
	XYZ p0, p, pw;
	
	glBegin(GL_LINES);
	for(int i=0; i<n_hair; i++)
	{
		hair_space.setIdentity();
		hair_space.setOrientations(hairs[i].binormal, hairs[i].up, hairs[i].tangent);
		hair_space.setTranslation(hairs[i].root);
		
		fb.setParam(hairs[i].scale, hairs[i].wind.x, hairs[i].wind.y, 8);
		
		p0 = XYZ(0,0,0);
		for(int j=0; j<=8; j++)
		{
			pw = p0;
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
			
			p = XYZ(0,0, j/8.f*hairs[i].scale);
			
			fb.add(p);
			
			pw = p;
			hair_space.transform(pw);
			glVertex3f(pw.x, pw.y, pw.z);
			p0 = p;
		}
	}
	glEnd();
}

void DFeather::drawQuad() const
{
	MATRIX44F hair_space;
	FBend fb;
	XYZ vt, vc;
	XYZ p0, p, pw, n0, n, nw;
	float wid0;

	glBegin(GL_QUADS);
	for(int i=0; i<n_hair; i++)
	{
		hair_space.setIdentity();
		hair_space.setOrientations(hairs[i].binormal, hairs[i].up, hairs[i].tangent);
		hair_space.setTranslation(hairs[i].root);
		
		fb.setParam(hairs[i].scale, hairs[i].wind.x, hairs[i].wind.y, 8);
		
		p0 = XYZ(0,0,0);
		n0 = XYZ(0,1,0);
		wid0 = sf[0];
		for(int j=0; j<=8; j++)
		{
			pw = p0;
			hair_space.transform(pw);
			nw = n0;
			hair_space.transformAsNormal(nw);
			glNormal3f(nw.x, nw.y, nw.z);
			
			vt = pw - hairs[i].binormal*m_wh*hairs[i].scale*wid0;
			glVertex3f(vt.x, vt.y, vt.z);
			vt = pw + hairs[i].binormal*m_wh*hairs[i].scale*wid0;
			glVertex3f(vt.x, vt.y, vt.z);
			
			p = XYZ(0,0, j/8.f*hairs[i].scale);
			
			fb.add(p, n);
			
			nw = n;
			hair_space.transformAsNormal(nw);
			glNormal3f(nw.x, nw.y, nw.z);
			
			pw = p;
			hair_space.transform(pw);
			
			vt = pw + hairs[i].binormal*m_wh*hairs[i].scale*sf[j];
			glVertex3f(vt.x, vt.y, vt.z);
			vt = pw - hairs[i].binormal*m_wh*hairs[i].scale*sf[j];
			glVertex3f(vt.x, vt.y, vt.z);
			
			p0 = p;
			n0 = n;
			wid0 = sf[j];
		}
	}
	glEnd();
}
*/
void DFeather::defineP(const int& i, XYZ* cv, const int& nu, const int& nv) const
{
	MATRIX44F hair_space;
	FBend fb;
	XYZ vt;
	XYZ p, pw;
	float w, dw, param;
	int ndiv = nv-2-1;

	hair_space.setIdentity();
	hair_space.setOrientations(hairs[i].binormal, hairs[i].up, hairs[i].tangent);
	hair_space.setTranslation(hairs[i].root);
		
	fb.setParam(hairs[i].scale, hairs[i].wind.x, hairs[i].wind.y, ndiv);
	
	w = m_wh*hairs[i].scale;
	dw = w*2/(nu-1);
		
		for(int iv=0; iv<nv; iv++)
		{
			param = iv - 1;
			
			if(iv==0) param = 0.f;
			else if(iv==1) param = 0.333333f;
			else if(iv==nv-2) param = nv - 3.333333f;
			else if(iv==nv-1) param = nv - 3;
			 
			p = XYZ(0,0, param/(float)ndiv*hairs[i].scale);
			
			fb.add(p);
			
			pw = p;
			hair_space.transform(pw);
			
			vt = pw - hairs[i].binormal*w;
			
			for(int iu=0; iu<nu; iu++)
			{
				cv[iv*nu+iu] = vt + hairs[i].binormal*dw*iu;
			}
			
		}

}

void DFeather::getAParam(int idx, MATRIX44F& space, float& length, XYZ& wind, XYZ& binorm, float& curl, float& s, float& t, CoeRec& coeff) const
{
	space.setIdentity();
	space.setOrientations(hairs[idx].binormal, hairs[idx].up, hairs[idx].tangent);
	space.setTranslation(hairs[idx].root);
	
	length = hairs[idx].scale;
	wind = hairs[idx].wind;
	binorm = hairs[idx].binormal;
	curl = hairs[idx].curl;
	s = hairs[idx].s;
	t = hairs[idx].t;
	coeff = coeffs[idx];
}

void DFeather::getAParam(int idx, MATRIX44F& space, float& length, XYZ& wind, XYZ& binorm, float& curl, float& s, float& t) const
{
	space.setIdentity();
	space.setOrientations(hairs[idx].binormal, hairs[idx].up, hairs[idx].tangent);
	space.setTranslation(hairs[idx].root);
	
	length = hairs[idx].scale;
	wind = hairs[idx].wind;
	binorm = hairs[idx].binormal;
	curl = hairs[idx].curl;
	s = hairs[idx].s;
	t = hairs[idx].t;
}
//:~