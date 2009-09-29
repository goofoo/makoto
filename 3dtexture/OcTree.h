#pragma once

#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "QuickSort.h"
using namespace std;

class sphericalHarmonics;

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
	
	OCTNode *parent;
	OCTNode *child000;
	OCTNode *child001;
	OCTNode *child010;
	OCTNode *child011;
	OCTNode *child100;
	OCTNode *child101;
	OCTNode *child110;
	OCTNode *child111;
};

class OcTree
{
public:
	OcTree();
	~OcTree();
	
	void release();
	void release(OCTNode *node);
	
	void construct();
	void create(OCTNode *node, PosAndId* data, int low, int high, const XYZ& center, const float size, short level, unsigned &count);
	
	void nearestGrid(const XYZ& to, float min, float max, float& dist);
	void nearestGrid(OCTNode *node, const XYZ& to, float min, float max, float& dist);
	
	void computePower(sphericalHarmonics* sh);
	void computePower(OCTNode *node);
	
	void doOcclusion(SHB3COEFF* res) const;
	
	void addSingle(const float *rawdata, const char* name, const PosAndId *index);
	void setSingle(float *res, OCTNode *node, const float *rawdata, const PosAndId *index);
	void addThree(const XYZ *rawdata, const char* name, const PosAndId *index);
	void setThree(XYZ *res, OCTNode *node, const XYZ *rawdata, const PosAndId *index);
	
	void save(ofstream& file) const;
	void save(ofstream& file, OCTNode *node) const;
	
	char load(ifstream& file);
	void load(ifstream& file, OCTNode *node);
	char loadGrid(const char* filename);
	void setGrid(RGRID* data, int n_data);
	
	unsigned getNumGrid() const {return num_grid;}
	unsigned getNumVoxel() const {return num_voxel;}
	short getMaxLevel() const {return max_level;}
	int hasColor() const;
	
	void draw();
	//void drawGrid();
	void drawCube(const OCTNode *node);
	void drawSurfel(const OCTNode *node);
	//void draw(const PerspectiveView *pview,XYZ facing,string drawType);
	//void drawCube(const OCTNode *node, const PerspectiveView *pview);
	//void getDrawList(const OCTNode *node, const PerspectiveView *pview,int &index,DataAndId* drawList);
	
	void getRootCenterNSize(XYZ& center, float&size) const;

	//void searchNearVoxel(OcTree* tree,const XYZ position,int & treeindex);
	//void searchNearVoxel(OCTNode *node,const XYZ position,int & treeindex);

	static void splitX(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitY(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitZ(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void getBBox(const PosAndId* data, const int num_data, XYZ& center, float& size);
	static char isInBox(const XYZ& data, const XYZ& center, float size);
	void combineSurfel(const PosAndId *data, const int low, const int high, XYZ& center, XYZ& color, XYZ& dir, float& area) const;
	
private:
	
	OCTNode *root;
	short max_level;
	unsigned num_voxel;
	
	int num_grid;
	RGRID* m_pGrid;
	
	VoxSingleList dSingle;
	VoxThreeList dThree;
	
	SHB3COEFF* m_pPower;
	sphericalHarmonics* sh;
	
	PosAndId *idBuf;
};