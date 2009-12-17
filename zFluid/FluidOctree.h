/*
 *  FluidOctree.h
 *
 *  Created by jian zhang on 12/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <list>

#include "../shared/zData.h"

#include <ImfHeader.h>
#include <ImfArray.h>
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfTiledOutputFile.h>
#include <ImfTiledInputFile.h>
#include <ImfChannelList.h>

#include <ImfBoxAttribute.h>
#include <ImfChannelListAttribute.h>
#include <ImfCompressionAttribute.h>
#include <ImfChromaticitiesAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfEnvmapAttribute.h>
#include <ImfDoubleAttribute.h>
#include <ImfIntAttribute.h>
#include <ImfLineOrderAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfOpaqueAttribute.h>
#include <ImfStringAttribute.h>
#include <ImfVecAttribute.h>

using namespace Imf;

#define INDIRECTIONPOOLWIDTH 1024
#define INDIRECTIONPOOLSIZE 1024*1024
#define DATAPOOLWIDTH 512
#define DATAPOOLSIZE 262144
#define MAXNNODE 262143

struct AParticle
{
	XYZ pos;
	float r;
};

class FluidSolver;


struct GPUTreeNode
{
	GPUTreeNode()
	{
		child = new GPUTreeNode *[8];
		for(int i=0; i < 8; i++) child[i] = 0;
	}
	
	short coordx, coordy;
	GPUTreeNode **child;
	
// data
	float density;
};

class FluidOctree
{
public:
	FluidOctree();
	~FluidOctree();
	
	void release();
	void releaseNode(GPUTreeNode *node);
#ifndef FLUIDTREEREADONLY	
	void create(FluidSolver *solver, short maxLevel);
	void subdivideNode(GPUTreeNode *node, int cx, int cy, int cz, int size, short level, float density);
	float addupDensityCells(int cx, int cy, int cz, int size);
	
	void dumpIndirection(const char *filename);
	void setIndirection(const GPUTreeNode *node, short level);
#endif	
	unsigned long getNumVoxel() const {return m_currentIndex-1;}
	unsigned long getNumLeaf() const {return m_currentLeafIndex-1;}
	float getMinVariation() const {return m_min_variation;}
	short getMaxLevel() const {return m_maxLevel;}
	//float filterBox(const XYZ& center, float size, const AParticle *particle);
	//char bInBox(const XYZ& center, float size, const AParticle *particle);
	
	char load(const char *filename);
	void getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const;
	void getBBox(float& rootX, float& rootY, float& rootZ, float& rootSize) const;
	void drawBox() const;
	void drawBoxNode(const XYZ& center, float size, int x, int y, short level) const;
	void drawCube() const;
	void drawCubeNode(const XYZ& center, float size, int x, int y, int level) const;
	
	short* getIndirection() const {return m_idr;}
	float* getData() const {return m_dt;}
	
	void drawBox(const XYZ& center, float size) const;
private:
	GPUTreeNode *m_root;
	short m_maxLevel;
	unsigned long m_currentIndex;
	unsigned long m_currentLeafIndex;
	XYZ m_rootCenter;
	float m_rootSize;
	
	float *m_dt;
	short *m_idr;
	float m_min_variation;
	FluidSolver *m_pSolver;
	int m_dimx, m_dimy, m_dimz;
};
