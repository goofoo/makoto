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
#include "Z3DTexture.h"

using namespace std;

Z3DTexture::Z3DTexture() : m_pTree(0), m_sh(0), m_pGrid(0) {}
Z3DTexture::~Z3DTexture() 
{
	for(unsigned i=0; i<attrib_sh.size(); i++) delete[] attrib_sh[i]->data;
	attrib_sh.clear();
	if(m_pTree) delete m_pTree;
	if(m_sh) delete m_sh;
	if(m_pGrid) delete[] m_pGrid;
}

void Z3DTexture::setGrid(RGRID* data, int n_data)
{
	if(m_pTree) delete m_pTree;
	m_pTree = new OcTree();
	m_pTree->setGrid(data, n_data);
	m_ngrd = n_data;
	m_pGrid = data;
}

char Z3DTexture::load(const char* filename)
{
	ifstream infile;
	infile.open(filename,ios_base::in | ios_base::binary );
	if(!infile.is_open()) return 0;
	
	infile.read((char*)&m_ngrd, sizeof(int));
	if(m_ngrd < 1) {
		infile.close();
		return 0;
	}
	
	m_pGrid = new RGRID[m_ngrd];
	infile.read((char*)m_pGrid, sizeof(RGRID)*m_ngrd);
	
	m_pTree = new OcTree();
	
	m_pTree->load(infile);
	
	infile.close();
	
	return 1;
}

void Z3DTexture::constructTree()
{
	m_pTree->construct();
}

void Z3DTexture::computePower()
{
	m_sh = new sphericalHarmonics();
	m_pTree->computePower(m_sh);
}

void Z3DTexture::draw() const
{
	if(m_pTree) m_pTree->draw();
}

void Z3DTexture::drawGrid() const
{
	glBegin(GL_POINTS);
	for(int i=0; i<m_ngrd; i++) glVertex3f(m_pGrid[i].pos.x, m_pGrid[i].pos.y, m_pGrid[i].pos.z);
	glEnd();
}

void Z3DTexture::doOcclusion()
{
	NamedSHCOEFF* attr = new NamedSHCOEFF();
	attr->name = "occlusion";
	attr->data = new SHB3COEFF[m_pTree->getNumVoxel()];
	m_pTree->doOcclusion(attr->data);
	attrib_sh.push_back(attr);
}

void Z3DTexture::save(const char* filename)
{
	ofstream outfile;
	
	outfile.open(filename, ios_base::out | ios_base::binary );
	if(!outfile.is_open()) return;
	
	m_ngrd = getNumGrid();
	outfile.write((char*)&m_ngrd,sizeof(int));
	outfile.write((char*)m_pGrid,sizeof(RGRID)*m_ngrd);
	
	m_pTree->save(outfile);
	
	outfile.close();
}

void Z3DTexture::getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const
{
	if(m_pTree) {
		XYZ center; float size;
		m_pTree->getRootCenterNSize(center, size);
		xmin = center.x -size;
		ymin = center.y -size;
		zmin = center.z -size;
		xmax = center.x +size;
		ymax = center.y +size;
		zmax = center.z +size;
	}
}