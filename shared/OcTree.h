#pragma once

#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "QuickSort.h"
using namespace std;

struct TreeNode
{

	TreeNode():child000(0),child001(0),child010(0),child011(0),
		      child100(0),child101(0),child110(0),child111(0)/*,isNull(1)*/ {}

	unsigned low, high;
	XYZ center,mean;
	float size;
	int index;
	
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
	unsigned getNumVoxel() const {return num_voxel;}
	void draw(const char*filename);
	void create(TreeNode *node, PosAndId* data, int low, int high, const XYZ& center, const float size, short level, unsigned &count);
	void search(XYZ position,float area,XYZ* data,XYZ* &areadata, int count);
	void search(TreeNode *node,XYZ position,float area,XYZ* data,XYZ* &areadata, int count);
	void draw(const TreeNode *node,const char*filename,unsigned numVoxel);
	void getRootCenterNSize(XYZ& center, float&size) const;

	void saveFile(const char*filename,OcTree* tree,unsigned sum,XYZ *color);
	void saveTree(TreeNode *node);
    void saveColor(TreeNode *node,XYZ *color);
	void loadFile(const char*filename,OcTree* tree);
	void loadTree(TreeNode *node);

	static void splitX(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitY(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitZ(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void getBBox(const PosAndId* data, const int num_data, XYZ& center, float& size);
	static char isInBox(const XYZ& data, const XYZ& center, float size);
	static void getMean(const PosAndId *data, const int low, const int high, XYZ& center);
	//static void getColorMean(const PosAndId *data, const int low, const int high, XYZ& color);
	//int acount;
private:
	ifstream infile;
	TreeNode *root;
	short max_level;
	unsigned num_voxel;
	ofstream outfile;
};