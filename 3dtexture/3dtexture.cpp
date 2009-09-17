/*
 *  3dtexture.cpp
 *  octreeTest
 *
 *  Created by jian zhang on 9/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <stdlib.h>
#include <fstream>
#include "3dtexture.h"

using namespace std;

z3dtexture::z3dtexture() : m_pTree(0), m_sh(0) {}
z3dtexture::~z3dtexture() 
{
	for(unsigned i=0; i<attrib_sh.size(); i++) delete[] attrib_sh[i]->data;
	attrib_sh.clear();
	if(m_pTree) delete m_pTree;
	if(m_sh) delete m_sh;
}

char z3dtexture::loadGrid(const char* filename)
{
	if(m_pTree) delete m_pTree;
	m_pTree = new OcTree();
	return m_pTree->loadGrid(filename);
}

void z3dtexture::constructTree()
{
	m_pTree->construct();
}

void z3dtexture::computePower()
{
	m_sh = new sphericalHarmonics();
	m_pTree->computePower(m_sh);
}

void z3dtexture::draw() const
{
	if(m_pTree) m_pTree->draw();
}

void z3dtexture::drawGrid() const
{
	if(m_pTree) m_pTree->drawGrid();
}

void z3dtexture::doOcclusion()
{
	NamedSHCOEFF* attr = new NamedSHCOEFF();
	attr->name = "occlusion";
	attr->data = new SHB3COEFF[m_pTree->getNumVoxel()];
	m_pTree->doOcclusion(attr->data);
	attrib_sh.push_back(attr);
}