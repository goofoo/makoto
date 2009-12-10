/*
 *  GPUOctree.h
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

struct GPUTreeNode
{
	GPUTreeNode()
	{
		child = new GPUTreeNode *[8];
		for(int i=0; i < 8; i++) child[i] = 0;
	}
	
	unsigned short coordx, coordy;
	GPUTreeNode **child;
	
// data
	float density;
};

class GPUOctree
{
public:
	GPUOctree();
	~GPUOctree();
	
	void release();
	void releaseNode(GPUTreeNode *node);
	
	void create(const XYZ& rootCenter, float rootSize, short maxLevel, const std::list<AParticle *>& particles);
	void subdivideNode(GPUTreeNode *node, const XYZ& center, float size, short level, const std::list<AParticle *>& particles, float density);
	
	void dumpIndirection(const char *filename);
	void setIndirection(const GPUTreeNode *node);
	
	unsigned long getNumVoxel() const {return m_currentIndex-1;}
	short getMaxLevel() const {return m_maxLevel;}
	float filterBox(const XYZ& center, float size, const AParticle *particle);
	char bInBox(const XYZ& center, float size, const AParticle *particle);
	
	char load(const char *filename);
	void getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const;
	void drawBox() const;
	void drawBoxNode(const XYZ& center, float size, int x, int y) const;
	void drawCube() const;
	void drawCubeNode(const XYZ& center, float size, int x, int y) const;
private:
	GPUTreeNode *m_root;
	short m_maxLevel;
	unsigned long m_currentIndex;
	XYZ m_rootCenter;
	float m_rootSize;
	
	half *m_idr_r;
	half *m_idr_g;
	half *m_idr_b;
	half *m_idr_a;
	
	half *m_dt_r;
	half *m_dt_g;
	half *m_dt_b;
	half *m_dt_a;
};
