#pragma once

#include <iostream>
#include<stdlib.h>
#include "E:\git_home\shared\zData.h"
using namespace std;

extern const int size ;
extern const int range;
extern XYZ array[];
extern XYZ gainarray[];
extern int sizegainarray;  
extern XYZ val;

class OCTreeNode
{
private:
	TreeNode *left;
	TreeNode *right;
public:
	int begin,end;
	float max,min;
	int flag;
	TreeNode():left(0),right(0),begin(0),end(0),max(0),min(0),flag(0){ }
	TreeNode(const int&item1,const int &item2 ):begin(item1),end(item2),max(0),min(0),flag(0),left(0),right(0){}
	friend class FOctree;
};

class FOctree
{
private:
	TreeNode *tree;
public:
	FOctree( ) { tree = NULL;}
    void FreeTreeNode(TreeNode *tree);
	TreeNode* GetTreeNode(int begin,int end, TreeNode *lptr , TreeNode *rptr );
	bool FOctreeEmpty() { return tree==NULL;}
    int Depth(TreeNode *t);
	int FOctreeDepth();
	void CreateFOctree(TreeNode *t,XYZ array[],int begin,int end);
	void SearchFOctree(TreeNode *t,XYZ val,int range );
	TreeNode* GetRootNode(){return tree;}
	void quick_sortX(XYZ array[],int first,int last);
	void quick_sortY(XYZ array[],int first,int last);
	void quick_sortZ(XYZ array[],int first,int last);
	void PrintFOctree(TreeNode *t);
};