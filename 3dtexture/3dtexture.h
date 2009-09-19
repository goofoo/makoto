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
#include "../shared/OcTree.h"
#include "../sh_lighting/SphericalHarmonics.h"

struct NamedSHCOEFF
{
	string name;
	SHB3COEFF* data;
};

typedef vector<NamedSHCOEFF*>TriDTexSHList;

class z3dtexture
{
public:
	z3dtexture();
	~z3dtexture();
	
	char loadGrid(const char* filename);
	void constructTree();
	void computePower();
	void draw() const;
	void drawGrid() const;
	
	int getNumGrid() const { return m_pTree->getNumGrid(); }
	int getNumVoxel() const {return m_pTree->getNumVoxel(); }
	short getMaxLevel() const {return m_pTree->getMaxLevel();}
	
	void doOcclusion();
private:
	sphericalHarmonics* m_sh;
	OcTree* m_pTree;
	TriDTexSHList attrib_sh;
};