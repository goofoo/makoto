/*
 *  3dtexture.h
 *  octreeTest
 *
 *  Created by jian zhang on 9/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../shared/zData.h"
#include "OcTree.h"
#include "../sh_lighting/SphericalHarmonics.h"

struct NamedSHCOEFF
{
	string name;
	SHB3COEFF* data;
};

typedef vector<NamedSHCOEFF*>TriDTexSHList;

class Z3DTexture
{
public:
	Z3DTexture();
	~Z3DTexture();
	
	void setGrid(RGRID* data, int n_data);
	char load(const char* filename);
	void constructTree();
	void computePower();
	void draw() const;
	void drawGrid() const;
	
	int getNumGrid() const { return m_nGrid; }
	int getNumVoxel() const {return m_pTree->getNumVoxel(); }
	short getMaxLevel() const {return m_pTree->getMaxLevel();}
	void getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const;
	
	void doOcclusion();
	
	void save(const char* filename);
private:
	sphericalHarmonics* m_sh;
	OcTree* m_pTree;
	TriDTexSHList attrib_sh;
	int m_nGrid;
	RGRID* m_pGrid;
};