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
#include "../shared/QuickSort.h"

struct SLICEAttrib
{
	XYZ pos;
	float depth, z, r;
	int gridId, brickId;
};

Z3DTexture::Z3DTexture() : m_pTree(0), m_sh(0), m_pGrid(0),m_pId(0),m_pOpacity(0),m_pAge(0)
{
	raster = new CubeRaster();
	m_sh = new sphericalHarmonics();
}

Z3DTexture::~Z3DTexture() 
{
	for(unsigned i=0; i<attrib_sh.size(); i++) delete[] attrib_sh[i]->data;
	attrib_sh.clear();
	if(m_pTree) delete m_pTree;
	if(m_sh) delete m_sh;
	if(m_pGrid) delete[] m_pGrid;
	if(m_pId) delete[] m_pId;
	if(m_pOpacity) delete[] m_pOpacity;
	if(m_pAge) delete[] m_pAge;
	delete raster;
}

void Z3DTexture::setGrid(RGRID* data, int n_data)
{
	m_pGrid = data;
	if(m_pTree) delete m_pTree;
	m_pTree = new OcTree();
	m_pTree->setGrid(data, n_data);
}

void Z3DTexture::setGridIdData(unsigned* data, int n_data)
{
	m_pId = data;
	m_pTree->orderGridData(m_pId, n_data);
}

void Z3DTexture::setGridOpacityData(float* data, int n_data)
{
	m_pOpacity = data;
	m_pTree->orderGridData(m_pOpacity, n_data);
}

void Z3DTexture::setGridAgeData(float* data, int n_data)
{
	m_pAge = data;
	m_pTree->orderGridData(m_pAge, n_data);
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
	m_pId = new unsigned[ngrid];
	infile.read((char*)m_pId,4*ngrid);
	
	m_pOpacity = new float[ngrid];
	infile.read((char*)m_pOpacity,4*ngrid);
	
	m_pAge = new float[ngrid];
	infile.read((char*)m_pAge,4*ngrid);
	
	m_pTree = new OcTree();
	m_pTree->setGrid(m_pGrid, ngrid);
	m_pTree->load(infile);
	
// recover voxle arributes
	int datatype;
	int namelength;
	
	infile.read((char*)&datatype, 4);
	while(!infile.eof()) {
		if(datatype == 108) {
			infile.read((char*)&namelength, 4);
			char *attrname = new char[32];
			infile.read((char*)attrname, namelength);
			attrname[namelength] = '\0';
			
			NamedSHCOEFF* attr = new NamedSHCOEFF();
			
			attr->name = attrname;
			attr->data = new SHB3COEFF[m_pTree->getNumVoxel()];
			
			infile.read((char*)attr->data, 108*m_pTree->getNumVoxel());
			attrib_sh.push_back(attr);
		}
		infile.read((char*)&datatype, 4);
	}

	infile.close();
	
	m_pTree->setGridId(m_pId);
	m_pTree->setGridOpacity(m_pOpacity);
	
	return 1;
}

void Z3DTexture::constructTree(const XYZ& rootCenter, float rootSize, int maxLevel)
{
	m_pTree->construct(rootCenter, rootSize, maxLevel);
}

void Z3DTexture::drawCube() const
{
	if(m_pTree) m_pTree->drawCube();
}

void Z3DTexture::drawSprite() const
{
	if(m_pTree) m_pTree->sortDraw();
}

void Z3DTexture::save(const char* filename)
{
	ofstream outfile;
	
	outfile.open(filename, ios_base::out | ios_base::binary );
	if(!outfile.is_open()) return;
	
	int ngrid = m_pTree->getNumGrid();
	outfile.write((char*)&ngrid,sizeof(int));
	outfile.write((char*)m_pGrid,sizeof(RGRID)*ngrid);
	outfile.write((char*)m_pId,4*ngrid);
	outfile.write((char*)m_pOpacity,4*ngrid);
	outfile.write((char*)m_pAge,4*ngrid);
	m_pTree->save(outfile);
	
// record voxel arributes	
	int datatype = 108, namelength;
	for(unsigned i=0; i < attrib_sh.size(); i++) {
		outfile.write((char*)&datatype, 4);
		namelength = attrib_sh[i]->name.size();
		outfile.write((char*)&namelength, 4);
		outfile.write((char*)attrib_sh[i]->name.c_str(), namelength);
		outfile.write((char*)attrib_sh[i]->data, 108*m_pTree->getNumVoxel());
	}
	
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
		XYZ vneib;
		m_pTree->nearestGrid(to, min, dist, vneib);
		m_pGrid[i].area = dist*dist*4;
	}
}

void Z3DTexture::occlusionVolume(float opacity, XYZ& key_dir, XYZ& view_dir)
{
	if(!m_pTree) return;
	raster->clear();
	m_pTree->setSampleOpacity(opacity);
	m_pTree->setDirs(key_dir, view_dir);
	
	NamedSHCOEFF* attr = new NamedSHCOEFF();
	attr->name = "key_lighting";
	attr->data = new SHB3COEFF[m_pTree->getNumVoxel()];
	
	NamedSHCOEFF* attr1 = new NamedSHCOEFF();
	attr1->name = "back_lighting";
	attr1->data = new SHB3COEFF[m_pTree->getNumVoxel()];
	
	m_pTree->setSH(m_sh);
	m_pTree->setRaster(raster);
	m_pTree->setSHBuf(attr->data);
	m_pTree->setSHBuf1(attr1->data);
	
	m_pTree->voxelOcclusionAccum();
	
	attrib_sh.push_back(attr);
	attrib_sh.push_back(attr1);
}

XYZ Z3DTexture::testRaster(const XYZ& ori)
{
	if(!m_pTree) return XYZ(0);
	raster->clear();
	m_pTree->setSampleOpacity(0.05f);
	
	m_pTree->setRaster(raster);
	m_pTree->occlusionAccum(ori);
	raster->draw();
	XYZ coe[SH_N_BASES];
	m_sh->zeroCoeff(coe);
	float l, vl;
	XYZ vn(0,1,0);
	for(unsigned int j=0; j<SH_N_SAMPLES; j++) {
		SHSample s = m_sh->getSample(j);
		vl = vn.dot(s.vector);
		if(vl>0) {
			raster->readLight(s.vector, l);
			l *= vl;
			m_sh->projectASample(coe, j, l);
		}
	}
	m_sh->reconstructAndDraw(coe);
	
	return m_sh->integrate(m_sh->constantCoeff, coe);
}

char Z3DTexture::hasAttrib(const char* name) const
{
	for(unsigned i=0; i < attrib_sh.size(); i++) {
		if(attrib_sh[i]->name == name) return 1;
	}
	return 0;
}

void Z3DTexture::setDraw()
{
	m_pTree->setSH(m_sh);
	m_pTree->setSHBuf(NULL);
	m_pTree->setSHBuf1(NULL);
	for(unsigned i=0; i < attrib_sh.size(); i++) {
		if(attrib_sh[i]->name == "key_lighting") m_pTree->setSHBuf(attrib_sh[i]->data);
		if(attrib_sh[i]->name == "back_lighting") m_pTree->setSHBuf1(attrib_sh[i]->data);
	}
}

void Z3DTexture::setHDRLighting(XYZ* coe)
{
	if(!m_pTree) return;
	m_pTree->setHDRLighting(coe);
}

void Z3DTexture::getLODGrid(GRIDNId* data, unsigned& n_data) const
{
	if(!m_pTree) return;
	m_pTree->LODGrid(data, n_data);
}

void Z3DTexture::getGridById(unsigned idx, RGRID& data) const
{
	data = m_pGrid[idx];
}

void Z3DTexture::setProjection(MATRIX44F cameraspace, float fov, int iperspective)
{
	if(!m_pTree) return;
	MATRIX44F cameraspaceinv = cameraspace;
	cameraspaceinv.inverse();
	m_pTree->setProjection(cameraspaceinv, fov, iperspective);
	m_pTree->setSpriteSpace(cameraspace);
}

void Z3DTexture::setPortWidth(int& val)
{
	val /= 2;
	m_pTree->setHalfPortWidth(val);
}

SHB3COEFF* Z3DTexture::getNamedSH(const char* name)
{
	for(unsigned i=0; i < attrib_sh.size(); i++) {
		if(attrib_sh[i]->name == name) {
			return attrib_sh[i]->data;
		}
	}
	return 0;
}