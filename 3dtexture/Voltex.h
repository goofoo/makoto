/*
 *  Voltex.h
 *  pmap
 *
 *  Created by jian zhang on 10/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include "Z3DTexture.h"

class Voltex
{
public:
	Voltex();
	~Voltex();
	
	void start(Z3DTexture* db) const;
	void end() const;
	
	char isDiagnosed() const { return fHasDiagnosis; }
	void diagnose(string& log);
	
	void setKNoise(float& val) {fKNoise = val;}
	void setKDiffuse(float& val) {fKDiffuse = val;}
	void setLightPos(float& x, float& y, float& z) {fLightPos.x = x; fLightPos.y = y; fLightPos.z = z;}
private:
	char fHasDiagnosis, fHasExtensions;
	float fKNoise, fKDiffuse;
	XYZ fLightPos;
};
