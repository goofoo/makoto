#pragma once

#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "QuickSort.h"
#include "view.h"
using namespace std;

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

struct TreeNode
{

	TreeNode():child000(0),child001(0),child010(0),child011(0),
		      child100(0),child101(0),child110(0),child111(0)/*,isNull(1)*/ {}

	unsigned low, high;
	XYZ center,mean;
	float size;
	unsigned index;
	
	TreeNode *child000;
	TreeNode *child001;
	TreeNode *child010;
	TreeNode *child011;
	TreeNode *child100;
	TreeNode *child101;
	TreeNode *child110;
	TreeNode *child111;
};

class OcTree
{
public:
	OcTree();
	~OcTree();
	
	void release();
	void release(TreeNode *node);
	
	void construct(PosAndId* data, const int num_data, const XYZ& center, const float size,short level);
	void create(TreeNode *node, PosAndId* data, int low, int high, const XYZ& center, const float size, short level, unsigned &count);
	void addSingle(const float *rawdata, const char* name, const PosAndId *index);
	void setSingle(float *res, TreeNode *node, const float *rawdata, const PosAndId *index);
	void addThree(const XYZ *rawdata, const char* name, const PosAndId *index);
	void setThree(XYZ *res, TreeNode *node, const XYZ *rawdata, const PosAndId *index);
	
	void save(const char *filename) const;
	void save(ofstream& file, TreeNode *node) const;
	
	int load(const char *filename);
	void load(ifstream& file, TreeNode *node);
	
	unsigned getNumVoxel() const {return num_voxel;}
	int hasColor() const;
	
	void draw(const particleView *pview);
	void draw(const TreeNode *node, const particleView *pview);
	
	void getRootCenterNSize(XYZ& center, float&size) const;

	
	
	
	//void saveColor(const char*filename,XYZ *color,PosAndId *buf,unsigned sum);
    //void saveColor(TreeNode *node,XYZ *color,PosAndId *buf);
	
	//void saveVelocity(const char*filename,XYZ *velocity,PosAndId *buf,unsigned sum);
	//void saveVelocity(TreeNode *node,XYZ *velocity,PosAndId *buf);
	
	
	
	//void loadColor(const char*filename,XYZ* &voxelcolor);
	//void loadVelocity(const char*filename,XYZ* &voxelvelocity);
	void searchNearVoxel(OcTree* tree,const XYZ position,int & treeindex);
	void searchNearVoxel(TreeNode *node,const XYZ position,int & treeindex);

	static void splitX(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitY(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitZ(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void getBBox(const PosAndId* data, const int num_data, XYZ& center, float& size);
	static char isInBox(const XYZ& data, const XYZ& center, float size);
	static void getMean(const PosAndId *data, const int low, const int high, XYZ& center);
private:
	
	TreeNode *root;
	short max_level;
	unsigned num_voxel;
	
	
	VoxSingleList dSingle;
	VoxThreeList dThree;
};