#pragma once

#include <iostream>
#include <stdlib.h>
#include "zData.h"
using namespace std;

struct TreeNode
{

	TreeNode():child000(0),child001(0),child010(0),child011(0),
		      child100(0),child101(0),child110(0),child111(0),isNull(1) {}
	
	
	int begin,end;
	XYZ center;
	float size;
	char isNull;

	
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
	
	void construct(XYZ* data, const int num_data, const XYZ& center, const float size);
	void draw();
	void create(TreeNode *node, XYZ* data, int low, int high, const XYZ& center, const float size, const short level);
	void draw(const TreeNode *node);

	static void quick_sortX(XYZ array[],int first,int last);
	static void quick_sortY(XYZ array[],int first,int last);
	static void quick_sortZ(XYZ array[],int first,int last);
	static void splitX(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitY(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void splitZ(const XYZ *data, const int low, const int high, const float center, int& cutat);
	static void getBBox(const XYZ *data, const int num_data, XYZ& center, float& size);
	
private:
	TreeNode *root;
};