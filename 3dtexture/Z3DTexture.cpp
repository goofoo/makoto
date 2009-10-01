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

Z3DTexture::Z3DTexture() : m_pTree(0), m_sh(0), m_pGrid(0) 
{
	raster = new CubeRaster();
}

Z3DTexture::~Z3DTexture() 
{
	for(unsigned i=0; i<attrib_sh.size(); i++) delete[] attrib_sh[i]->data;
	attrib_sh.clear();
	if(m_pTree) delete m_pTree;
	if(m_sh) delete m_sh;
	if(m_pGrid) delete[] m_pGrid;
	delete raster;
}

void Z3DTexture::setGrid(RGRID* data, int n_data)
{
	m_pGrid = data;
	if(m_pTree) delete m_pTree;
	m_pTree = new OcTree();
	m_pTree->setGrid(data, n_data);
}

char Z3DTexture::load(const char* filename)
{
	ifstream infile;
	infile.open(filename,ios_base::in | ios_base::binary );
	if(!infile.is_open()) return 0;
	
	int ngrid = -1;
	infile.read((char*)&ngrid, sizeof(int));
	if(ngrid < 1) {
		infile.close();
		return 0;
	}
	
	m_pGrid = new RGRID[ngrid];
	infile.read((char*)m_pGrid, sizeof(RGRID)*ngrid);
	
	m_pTree = new OcTree();
	m_pTree->setGrid(m_pGrid, ngrid);
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

void Z3DTexture::drawGrid(const XYZ& viewdir) const
{
	if(!m_pTree) return;
	int ngrid = m_pTree->getNumGrid();
	for(int i=0; i<ngrid; i++) {
		glColor3f(m_pGrid[i].col.x, m_pGrid[i].col.y, m_pGrid[i].col.z);
		float r = sqrt(m_pGrid[i].area/4);
		gBase::drawSplatAt(m_pGrid[i].pos, viewdir, r);
	}
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
	
	int ngrid = m_pTree->getNumGrid();
	outfile.write((char*)&ngrid,sizeof(int));
	outfile.write((char*)m_pGrid,sizeof(RGRID)*ngrid);
	
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

void Z3DTexture::distanceToNeighbour(float min, float max)
{
	if(!m_pTree) return;
	int ngrid = m_pTree->getNumGrid();
	for(int i=0; i<ngrid; i++) {

		XYZ to = m_pGrid[i].pos;
		float dist = max;
		float max2 = max*2;
		m_pTree->nearestGrid(to, min, max2, dist);
		m_pGrid[i].area = dist*dist*4;
	}
}

void Z3DTexture::testRaster(const XYZ& ori)
{
	if(!m_pTree) return;
	raster->clear();
	glBegin(GL_LINES);
	m_pTree->occlusion(ori, raster);
	glEnd();
	raster->draw();
}