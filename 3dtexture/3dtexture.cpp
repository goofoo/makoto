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

z3dtexture::z3dtexture() : m_pGrid(0), num_grid(0), m_pTree(0) {}
z3dtexture::~z3dtexture() 
{
	if(m_pGrid) delete[] m_pGrid;
}

char z3dtexture::loadGrid(const char* filename)
{
	ifstream ffile;
	ffile.open(filename, ios::in | ios::binary | ios::ate);
	
	if(!ffile.is_open()) return 0;

	ifstream::pos_type size = ffile.tellg();
	
	num_grid = (int)size / sizeof(RGRID);
	
	m_pGrid = new RGRID[num_grid];
	
	ffile.seekg(0, ios::beg);
	
	ffile.read((char*)m_pGrid, sizeof(RGRID)*num_grid);
	
	ffile.close();
	return 1;
}

void z3dtexture::constructTree()
{
	PosAndId *buf = new PosAndId[num_grid];
	for(int i=0; i<num_grid; i++) {
		buf[i].pos = m_pGrid[i].pos;
		buf[i].idx = i;
	}
	
	XYZ rootCenter;
	float rootSize;
    OcTree::getBBox(buf, num_grid, rootCenter, rootSize);
	
	short maxlevel = 8;
	
	if(m_pTree) delete m_pTree;
	m_pTree = new OcTree();
	m_pTree->construct(buf, num_grid, rootCenter, rootSize, maxlevel);
	
	XYZ* colbuf = new XYZ[num_grid];
	for(int i=0; i<num_grid; i++) colbuf[i] = XYZ(0.5+0.5*m_pGrid[i].nor.x, 0.5+0.5*m_pGrid[i].nor.y, 0.5+0.5*m_pGrid[i].nor.z);
	
	m_pTree->addThree(colbuf, "Cs", buf);
	
	delete[] buf;
	delete[] colbuf;
}

void z3dtexture::draw() const
{
	if(m_pTree) m_pTree->draw();
}

void z3dtexture::getGrid(int& idx, XYZ& pos, XYZ& nor, XYZ& col, float& r) const
{
	pos = m_pGrid[idx].pos;
	nor = m_pGrid[idx].nor;
	col = m_pGrid[idx].col;
	r = m_pGrid[idx].area;
}