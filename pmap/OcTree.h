#ifndef _OcTree
#define _OcTree

#include <iostream>
#include<stdlib.h>
#include "E:\git_home\shared\zData.h"
using namespace std;






class TreeNode
{
private:
	TreeNode *firstChildren;
	TreeNode *secondChildren;
	TreeNode *thirdChildren;
	TreeNode *fourthChildren;
	TreeNode *fifthChildren;
	TreeNode *sixthChildren;
	TreeNode *seventhChildren;
	TreeNode *eighthChildren;
public:
	int begin,end;
	XYZ center;
	float side;
	TreeNode():firstChildren(0),secondChildren(0),thirdChildren(0),fourthChildren(0),
		       fifthChildren(0),sixthChildren(0),seventhChildren(0),eighthChildren(0),
			   side(0.0),center(),begin(0),end(0){ }
	TreeNode(const int&item1,const int &item2 ):begin(item1),end(item2),center(),side(0.0),
		                                        firstChildren(0),secondChildren(0),thirdChildren(0),fourthChildren(0),
		                                        fifthChildren(0),sixthChildren(0),seventhChildren(0),eighthChildren(0){}
	friend class OcTree;
};

class OcTree
{
private:
	TreeNode *tree;
public:
	OcTree( ) { tree = NULL;}
    void FreeTreeNode(TreeNode *tree);
	TreeNode* GetTreeNode(int begin,int end );
	bool OcTreeEmpty() { return tree==NULL;}
    int Depth(TreeNode *t);
	int OcTreeDepth();
	void CreateOcTree(TreeNode *t,XYZ array[],int begin,int end ,XYZ center,float sidelength,int level);
	void SearchOcTree(TreeNode *t,XYZ val,int range );
	TreeNode* GetRootNode(){return tree;}
	void quick_sortX(XYZ array[],int first,int last);
	void quick_sortY(XYZ array[],int first,int last);
	void quick_sortZ(XYZ array[],int first,int last);
	XYZ GetCenter(XYZ array[],int first,int last);
	void GetChildren(TreeNode *t,int level);
	float GetSideLength(XYZ array[],int first,int last);
	void PrintOcTree(TreeNode *t);
	//XYZ *array;
    XYZ *gainarray;
	int sizegainarray; 
};

#endif