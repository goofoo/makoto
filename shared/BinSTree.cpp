#include ".\BinSTree.h"
/*
1.define global variable :
  define input array's size（const int size = 1000）；
  declaration determine point（XYZ val）；
  define determine point  extent （const int range = 20.0）；
  define input array（XYZ array[size]）；
  define output array（XYZ gainarray[size]）;
  define output array's index（int sizegainarray = 0）；
  example：
  const int size = 500;
  const int range = 20;
  XYZ array[size];
  XYZ gainarray[size];
  int sizegainarray = 0;  
  XYZ val;
2.create BinSTree object ，example：BinSTree b；
3.create TreeNode object ，example：TreeNode *t = new TreeNode(0,size - 1);
4.create BST tree，example：b.CreateBinSTree(t,array,0,size - 1);
5.define determine point，example：val.x = 50.0;val.y = 50.0;val.z = 50.0;
6.search determine point's around，example：
  sizegainarray = 0;
  b.SearchBinSTree(t,val,range);
  （attention：every search ,use sizegainarray = 0）
7.重复5-6，搜索不同点。
*/

void BinSTree::FreeTreeNode(TreeNode *p)
{
	delete p;
}

TreeNode* BinSTree::GetTreeNode(int item1, int item2, TreeNode *lptr = 0, TreeNode *rptr = 0)
{
	TreeNode *p = new TreeNode(item1,item2);
	if(p == NULL) 
	{
		cerr<<"Memory allocation failure!\n";
		exit(1);
	} 
	return p;
}

int BinSTree::Depth(TreeNode *t)
{
	int depthLeft,depthRight;
	if(t == NULL)
		return -1;
	else
	{
		depthLeft = Depth(t->left);
		depthRight = Depth(t->right);
		if(depthLeft > depthRight) return depthLeft+1;
		else return depthRight+1;
	}	
}

int BinSTree::BinSTreeDepth()
{
	return Depth(tree);
}


void BinSTree::quick_sortX(XYZ array[],int first,int last)
{
	int low,high;float list_separator;
	XYZ temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].x;
	do
	{
		while(array[low].x<list_separator) low++;
		while(array[high].x>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	}while(low<=high);
	if(first<high)
		quick_sortX(array,first,high);
	if(low<last)
		quick_sortX(array,low,last);
}

void BinSTree::quick_sortY(XYZ array[],int first,int last)
{
	int low,high;float list_separator;
	XYZ temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].y;
	do
	{
		while(array[low].y<list_separator) low++;
		while(array[high].y>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	}while(low<=high);
	if(first<high)
		quick_sortY(array,first,high);
	if(low<last)
		quick_sortY(array,low,last);
}

void BinSTree::quick_sortZ(XYZ array[],int first,int last)
{
	int low,high;float list_separator;
	XYZ temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].z;
	do
	{
		while(array[low].z<list_separator) low++;
		while(array[high].z>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	}while(low<=high);
	if(first<high)
		quick_sortZ(array,first,high);
	if(low<last)
		quick_sortZ(array,low,last);
}


void BinSTree::CreateBinSTree( TreeNode *t,XYZ array[],int begin,int end)
{
	if(tree == NULL )
	{
		int i ;
		quick_sortX(array,begin,end);
		tree = t;
		t->min = array[begin].x;
		t->max = array[end].x;
		t->flag = -1;
		for( i = begin; array[i].x <(array[begin].x+array[end].x)/2;i++ );
		t->left = GetTreeNode(begin,i-1);
		t->right = GetTreeNode(i,end);
		CreateBinSTree(t->left,array,begin,i-1);
		CreateBinSTree(t->right,array,i,end);
	}
	else 
	if((end - begin)>=6)
	{
		quick_sortX(array,begin,end);
		float Xarea = array[end].x - array[begin].x;
		quick_sortY(array,begin,end);
		float Yarea = array[end].y - array[begin].y;
		quick_sortZ(array,begin,end);
		float Zarea = array[end].z - array[begin].z;
		if(Xarea>=Yarea&&Xarea>=Zarea)
		{
			int i;
		    quick_sortX(array,begin,end);
		    t->min = array[begin].x;
		    t->max = array[end].x;
			t->flag = 1;
			for( i = begin;array[i].x <(array[begin].x+array[end].x)/2;i++ );
			t->left = GetTreeNode(begin,i-1);
		    t->right = GetTreeNode(i,end);
		    CreateBinSTree(t->left,array,begin,i-1);
		    CreateBinSTree(t->right,array,i,end);
		}
		else if((Yarea>=Xarea&&Yarea>Zarea)||(Yarea>Xarea&&Yarea>=Zarea))
		{
			int i;
		    quick_sortY(array,begin,end);
		    t->min = array[begin].y;
		    t->max = array[end].y;
			t->flag = 2;
			for( i = begin;array[i].y <(array[begin].y+array[end].y)/2;i++ );
			t->left = GetTreeNode(begin,i-1);
		    t->right = GetTreeNode(i,end);
		    CreateBinSTree(t->left,array,begin,i-1);
		    CreateBinSTree(t->right,array,i,end);
		}
		else if((Zarea>Xarea&&Zarea>=Yarea)||(Zarea>=Xarea&&Zarea>Yarea))
		{
			int i;
		    quick_sortZ(array,begin,end);
		    t->min = array[begin].z;
		    t->max = array[end].z;
			t->flag = 3;
			for( i = begin;array[i].z <(array[begin].z+array[end].z)/2;i++ );
			t->left = GetTreeNode(begin,i-1);
		    t->right = GetTreeNode(i,end);
		    CreateBinSTree(t->left,array,begin,i-1);
		    CreateBinSTree(t->right,array,i,end);	
		}
	}
	else
	{
		t->flag = 0;
		t->left = NULL;
	    t->right = NULL;
	}	
}

void BinSTree::SearchBinSTree( TreeNode *t,XYZ val,int range)
{
	float middle;
	if(t==NULL){
		t = tree;
	}
	if(t->flag == -1)
	{
		middle = (t->max + t->min)/2;
		if((t->min<=val.x+range)&&(val.x-range<=t->max))
		{
			if( -range<= val.x - middle && val.x-middle <= range )
			{
				SearchBinSTree( t->left,val,range );
				SearchBinSTree( t->right,val,range );
			}
			else if(  val.x - middle > range )
				SearchBinSTree( t->right,val,range );
			else
				SearchBinSTree( t->left,val,range );
		}
		else return;
	}
	else if(t->flag == 1)
	{
		middle = (t->max + t->min)/2;
		if((t->min<=val.x+range)&&(val.x-range<=t->max))
		{
			if( -range<= val.x - middle && val.x-middle <= range )
			{
				SearchBinSTree( t->left,val,range );
				SearchBinSTree( t->right,val,range );
			}
			else if(  val.x - middle > range )
				SearchBinSTree( t->right,val,range );
			else
				SearchBinSTree( t->left,val,range );
		}
		else return;
	}
	else if(t->flag == 2)
	{
		middle = (t->max + t->min)/2;
		if((t->min<=val.y+range)&&(val.y-range<=t->max))
		{
			if( -range<= val.y - middle && val.y-middle <= range )
			{
				SearchBinSTree( t->left,val,range );
				SearchBinSTree( t->right,val,range );
			}
			else if(  val.y - middle > range )
				SearchBinSTree( t->right,val,range);
			else
				SearchBinSTree( t->left,val,range);
		}
		else return;
	}
	else if(t->flag == 3)
	{
		middle = (t->max + t->min)/2;
		if((t->min<=val.z+range)&&(val.z-range<=t->max))
		{
			if( -range<= val.z - middle && val.z-middle <= range )
			{
				SearchBinSTree( t->left,val,range );
				SearchBinSTree( t->right,val,range );
			}
			else if(  val.z - middle > range )
				SearchBinSTree( t->right,val,range );
			else
				SearchBinSTree( t->left,val,range );
		}
		else return;
	}
	else if(t->flag == 0)
	{
		for(int i = t->begin; i<=t->end; i++ )
			if(((array[i].x - val.x)*(array[i].x - val.x)+(array[i].y - val.y)*(array[i].y - val.y)+(array[i].z - val.z)*(array[i].z - val.z))<=range*range)
				gainarray[sizegainarray++] = array[i];	
		return;
	}
}

void BinSTree::PrintBinSTree(TreeNode *t)
{
	if(t!=NULL)
	{
		cout<<"begin:"<<t->begin<<" end:"<<t->end<<endl;
		PrintBinSTree(t->left);
		PrintBinSTree(t->right);
	}
}