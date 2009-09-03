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

struct NamedSHCOEFF
{
	string name;
	SHCOEFF* data;
};

typedef vector<NamedSHCOEFF*>TriDTexSHList;

class z3dtexture
{
public:
	z3dtexture();
	~z3dtexture();
	
	char loadGrid(const char* filename);
	void constructTree();
	void draw() const;
	
	int getNumGrid() const { return num_grid; }
	int getNumVoxel() const {return m_pTree->getNumVoxel(); }
	void getGrid(int& idx, XYZ& pos, XYZ& nor, XYZ& col, float& r) const;
	
	void doOcclusion();
private:
	int num_grid;
	RGRID* m_pGrid;
	OcTree* m_pTree;
	TriDTexSHList attrib_sh;
};