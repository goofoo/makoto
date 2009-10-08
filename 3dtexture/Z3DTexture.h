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
#include "CubeRaster.h"
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
	void drawGrid(const XYZ& viewdir) const;
	
	int getNumGrid() const { if(m_pTree) return m_pTree->getNumGrid(); else return -1; }
	int getNumVoxel() const {if(m_pTree) return m_pTree->getNumVoxel(); else return -1;}
	short getMaxLevel() const {if(m_pTree) return m_pTree->getMaxLevel(); else return -1;}
	void getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const;
	
	void distanceToNeighbour(float min, float max);
	void doOcclusion();
	
	void save(const char* filename);
	
	void testRaster(const XYZ& ori);
private:
	sphericalHarmonics* m_sh;
	OcTree* m_pTree;
	TriDTexSHList attrib_sh;
	RGRID* m_pGrid;
	CubeRaster* raster;
};