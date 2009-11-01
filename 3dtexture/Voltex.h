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
	
	char isDiagnosed() const { return fHasDiagnosis; }
	void diagnose(string& log);
private:
	char fHasDiagnosis, fHasExtensions;
};
