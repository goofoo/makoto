#pragma once

#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../shared/zData.h"
#include "../sh_lighting/SphericalHarmonics.h"
using namespace std;

class sphericalHarmonics;

struct GRID2Draw
{
	int grid_id, detail;
	XYZ ibl;
	float r;
};

struct SLICE2Draw
{
	int draw_id;
	XYZ pos;
	float z;
};

struct GRIDNId
{
	RGRID grid;
	unsigned idx, gridId;
	int detail;
};

struct NamedSingle
{
	string name;
	float* data;
};

struct NamedThree
{
	string name;
	XYZ* data;
};

typedef vector<NamedSingle*>VoxSingleList;
typedef vector<NamedThree*>VoxThreeList;

struct OCTNode
{

	OCTNode():child000(0),child001(0),child010(0),child011(0),
		      child100(0),child101(0),child110(0),child111(0),isLeaf(0) {}

	unsigned low, high, index;
	XYZ center, mean, col, dir;
	float size, area;
	char isLeaf;
	
//OCTNode *parent;
	OCTNode *child000;
	OCTNode *child001;
	OCTNode *child010;
	OCTNode *child011;
	OCTNode *child100;
	OCTNode *child101;
	OCTNode *child110;
	OCTNode *child111;
};

class CubeRaster;

class OcTree
{
public:
	OcTree();
	~OcTree();
	
	void release();
	void release(OCTNode *node);
	
	void construct(const XYZ& rootCenter, float rootSize, int maxLevel);
	void create(OCTNode *node, int low, int high, const XYZ& center, const float size, short level, unsigned &count);
	
	void nearestGrid(const XYZ& to, float min, float& dist, XYZ& res);
	void nearestGrid(OCTNode *node, const XYZ& to, float min, float& dist, XYZ& res);
	
	void save(ofstream& file) const;
	void save(ofstream& file, OCTNode *node) const;
	
	char load(ifstream& file);
	void load(ifstream& file, OCTNode *node);
	char loadGrid(const char* filename);
	void setGrid(RGRID* data, int n_data);
	void orderGridData(unsigned* data, int n_data);
	void orderGridData(float* data, int n_data);
	
	unsigned getNumGrid() const {return num_grid;}
	unsigned getNumVoxel() const {return num_voxel;}
	unsigned getNumLeaf() const {return num_leaf;}
	short getMaxLevel() const {return max_level;}
	
	void drawCube();
	void drawSprite();
	void sortDraw();
	void drawCube(const OCTNode *node);
	void drawSprite(const OCTNode *node);
	void drawNeighbour(const OCTNode *node);
	void pushDrawList(const OCTNode *node, int& count, int& slice_count, GRID2Draw* res);
	
	void getRootCenterNSize(XYZ& center, float&size) const;
	float getMaxSearchDistance() const;

	void combineSurfel(const RGRID *data, const int low, const int high, XYZ& center, XYZ& color, XYZ& dir, float& area) const;
	
	void occlusionAccum(const XYZ& origin);
	void occlusionAccum(OCTNode *node, const XYZ& origin);
	
	void setSampleOpacity(const float& val) {sample_opacity = val/(float)num_grid*512;}
	void setDirs(XYZ& key, XYZ& view) {m_key = key; m_eye = view;}
	
	void setSH(sphericalHarmonics* _sh) {sh = _sh;}
	void setRaster(CubeRaster* _raster) {raster = _raster;}
	void setSHBuf(SHB3COEFF* _buf) {m_pSHBuf = _buf;}
	void setSHBuf1(SHB3COEFF* _buf) {m_pSHBuf1 = _buf;}
	
	void voxelOcclusionAccum();
	void voxelOcclusionAccum(OCTNode *node);
	
	void setHDRLighting(XYZ* coe);
	
	void LODGrid(GRIDNId* res, unsigned& n_data) const;
	void LODGrid(const OCTNode *node, unsigned& count, GRIDNId* res) const;
	void setProjection(MATRIX44F mat, float fov, int iperspective);
	void setSpriteSpace(MATRIX44F mat);
	
	void setGridId(unsigned* data) {t_grid_id = data;}
	void setGridOpacity(float* data) {t_grid_opacity = data;}
	unsigned* getIdBuf() const {return idBuf;}
	
	void setProgram(GLhandleARB obj) { program_object = obj; }
	
	void setHalfPortWidth(int &val) {fHalfPortWidth = val;}
	void setMeanRadius(const float& val) {fMeanRadius = val;}
	void setKeyScale(const float& val) { fKeyLight = val; }
	void setBackScale(const float& val) { fBackLight = val; }
	void drawAParticle(const XYZ& center, const float& radius, const int& detail);
	void reconstructColor(const unsigned& idx, XYZ& res);
	void drawASlice(const XYZ& center, const float& radius, const int& z);
	
private:
	
	OCTNode *root;
	short max_level;
	unsigned num_voxel, num_leaf;
	
	int num_grid;
	RGRID* m_pGrid;
	
	VoxSingleList dSingle;
	VoxThreeList dThree;
	
	sphericalHarmonics* sh;
	CubeRaster* raster;
	
	unsigned *idBuf;
	unsigned *t_grid_id;
	float* t_grid_opacity;
	float sample_opacity;
	
	SHB3COEFF* m_pSHBuf;
	SHB3COEFF* m_pSHBuf1;
	
	XYZ m_key, m_eye;
	char m_hasHdr;
	XYZ m_hdrCoe[SH_N_BASES];
	MATRIX44F f_cameraSpaceInv;
	float f_fieldOfView;
	char f_isPersp;
	float fMatSprite[16];
	XYZ fSpriteX, fSpriteY, fSpriteZ, fEye;
	GLhandleARB program_object;
	int fHalfPortWidth;
	float fMeanRadius, fKeyLight, fBackLight;
};