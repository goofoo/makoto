/*
 *  DFeather.h
 *  featherTest
 *
 *  Created by zhang on 08-12-18.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _DFEATHER_H
#define _DFEATHER_H
#include "FDice.h"

class DFeather
{
public:
	DFeather();
	~DFeather();
	
	void init(const DHair* data, const CoeRec* rt, int n_data);
	void init(const DHair* data, int n_data);
	
	//void draw() const;
	//void drawQuad() const;
	
	void setWidth(float wh) {m_wh = wh/2;}
	int getNumHair() const {return n_hair;}
	void defineP(const int& index, XYZ* cv, const int& nu, const int& nv) const;
	void getAParam(int idx, MATRIX44F& space, float& length, XYZ& wind, XYZ& binorm, float& curl, float& s, float& t, CoeRec& coeff) const;
	void getAParam(int idx, MATRIX44F& space, float& length, XYZ& wind, XYZ& binorm, float& curl, float& s, float& t) const;
private:
	DHair* hairs;
	CoeRec* coeffs;
	int n_hair;
	float m_wh;
};
#endif
