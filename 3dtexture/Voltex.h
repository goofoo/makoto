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
	
	char load(const char* filename);
	
	void setDraw(const char* name);
	void setHDRLighting(XYZ* coe);
	void setProjection(MATRIX44F cameraspace, float fov, int iperspective);
	
	void drawCube() const;
	void drawSprite() const;
	
	void getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const;
	
	char isDiagnosed() const { return fHasDiagnosis; }
	void diagnose(string& log);
private:
	Z3DTexture* db;
	char fHasDiagnosis, fHasExtensions;
};
