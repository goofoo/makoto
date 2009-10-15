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

typedef vector<NamedSHCOEFF*>TRIDTexSHList;

class Z3DTexture
{
public:
	Z3DTexture();
	~Z3DTexture();
	
	void setGrid(RGRID* data, int n_data);
	void orderGridData(unsigned* data, int n_data);
	char load(const char* filename);
	void constructTree(const XYZ& rootCenter, float rootSize, int maxLevel);

	void draw() const;
	void draw(const XYZ& viewdir) const;
	
	int getNumGrid() const { if(m_pTree) return m_pTree->getNumGrid(); else return -1; }
	int getNumVoxel() const {if(m_pTree) return m_pTree->getNumVoxel(); else return -1;}
	int getNumLeaf() const {if(m_pTree) return m_pTree->getNumLeaf(); else return -1;}
	short getMaxLevel() const {if(m_pTree) return m_pTree->getMaxLevel(); else return -1;}
	void getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const;
	
	void distanceToNeighbour(float min, float max);
	void occlusionVolume(float opacity, XYZ& key_dir, XYZ& view_dir);
	
	void save(const char* filename);
	
	XYZ testRaster(const XYZ& ori);
	
	void setDraw(const char* name);
	char hasAttrib(const char* name) const;
	const char* fisrtattrib() const {return attrib_sh[0]->name.c_str();}
	void setHDRLighting(XYZ* coe);
	void getLODGrid(GRIDNId* data, unsigned& n_data) const;
	void getGridById(unsigned idx, RGRID& data) const;
	void setProjection(MATRIX44F cameraspace, float fov, int iperspective);
	SHB3COEFF* getNamedSH(const char* name);
private:
	sphericalHarmonics* m_sh;
	OcTree* m_pTree;
	TRIDTexSHList attrib_sh;
	RGRID* m_pGrid;
	unsigned* m_pId;
	CubeRaster* raster;
};