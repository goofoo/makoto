#include ".\OcTree.h"

void OcTree::FreeTreeNode(TreeNode *p)
{
	delete p;
}

TreeNode* OcTree::GetTreeNode(int item1, int item2 )
{
	TreeNode *p = new TreeNode(item1,item2);
	if(p == NULL) 
	{
		cerr<<"Memory allocation failure!\n";
		exit(1);
	} 
	return p;
}

int OcTree::Depth(TreeNode *t)
{	
	return 0;
}

int OcTree::OcTreeDepth()
{
	return Depth(tree);
}


void OcTree::quick_sortX(XYZ array[],int first,int last)
{
	if(first<last){
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
		quick_sortX(array,low,last); }
}

void OcTree::quick_sortY(XYZ array[],int first,int last)
{
	if(first<last){
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
		quick_sortY(array,low,last);}
}

void OcTree::quick_sortZ(XYZ array[],int first,int last)
{
	if(first<last){
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
		quick_sortZ(array,low,last);}
}


void OcTree::CreateOcTree( TreeNode *t,XYZ array[],int begin,int end,XYZ center,float side,int level)
{//cout<<level<<endl;
	if( sizegainarray == 0 )
	{   
		int xindex,yindex1,yindex2,zindex1,zindex2,zindex3,zindex4;
		tree = t;
		XYZ center = GetCenter( array,begin,end );
		float side = GetSideLength( array,begin,end );
		//cout <<center.x<<"  "<<center.y<<"  "<<center.z<<"  "<<side<<endl;
		GetChildren(center,side);
		
        if( level == 0 )
			return;
		
		else
		{
		level--;

		quick_sortX(array,begin,end);
		for( xindex = begin; (array[xindex].x < center.x)&&(xindex<=end);xindex++ );

		quick_sortY(array,begin,xindex-1);
		for( yindex1 = begin; (array[yindex1].y < center.y)&&(yindex1<=xindex-1);yindex1++ );

		quick_sortY(array,xindex,end);
		for( yindex2 = xindex; (array[yindex2].y < center.y)&&(yindex2<=end);yindex2++ );
       
		quick_sortZ(array,begin,yindex1-1);
		for( zindex1 = begin; (array[zindex1].z < center.z)&&(zindex1<=yindex1-1);zindex1++ );

		quick_sortZ(array,yindex1,xindex-1);
		for( zindex2 = yindex1; (array[zindex2].z < center.z)&&(zindex2<=xindex-1);zindex2++ );

		quick_sortZ(array,xindex,yindex2-1);
		for( zindex3 = xindex; (array[zindex3].z < center.z)&&(zindex3<=yindex2-1);zindex3++ );

		quick_sortZ(array,yindex2,end);
		for( zindex4 = yindex2; (array[zindex4].z < center.z)&&(zindex4<=end);zindex4++ );

        XYZ center1,center2,center3,center4,center5,center6,center7,center8;
		center1.x = center.x - side/2;center1.y = center.y - side/2;center1.z = center.z - side/2;
		center2.x = center.x - side/2;center2.y = center.y - side/2;center2.z = center.z + side/2;
		center3.x = center.x - side/2;center3.y = center.y + side/2;center3.z = center.z - side/2;
		center4.x = center.x - side/2;center4.y = center.y + side/2;center4.z = center.z + side/2;
		center5.x = center.x + side/2;center5.y = center.y - side/2;center5.z = center.z - side/2;
		center6.x = center.x + side/2;center6.y = center.y - side/2;center6.z = center.z + side/2;
		center7.x = center.x + side/2;center7.y = center.y + side/2;center7.z = center.z - side/2;
		center8.x = center.x + side/2;center8.y = center.y + side/2;center8.z = center.z + side/2;
        
		if(begin <= zindex1-1)
		{
			if((begin == zindex1-1)||level==0)
			{
				GetChildren(center1,side/2);
				//level = 0;
			}
			t->firstChildren = GetTreeNode(begin,zindex1-1);
			CreateOcTree(t->firstChildren,array,begin,zindex1-1,center1,side/2,level);
		}
		else t->firstChildren = NULL;

		if(zindex1 <= yindex1-1)
		{
			if((zindex1 == yindex1-1)||level==0)
			{
				GetChildren(center2,side/2);
				//level = 0;
			}
			t->secondChildren = GetTreeNode(zindex1,yindex1-1);
		    CreateOcTree(t->secondChildren,array,zindex1,yindex1-1,center2,side/2,level);
		}
		else t->secondChildren = NULL;
		if(yindex1<=zindex2-1)
		{
			if(yindex1==zindex2-1||level==0)
			{
				GetChildren(center3,side/2);
				//level = 0;
			}
			t->thirdChildren = GetTreeNode(yindex1,zindex2-1);
		    CreateOcTree(t->thirdChildren,array,yindex1,zindex2-1,center3,side/2,level);
		}
		else t->thirdChildren = NULL;
		if(zindex2<=xindex-1)
		{
			if((zindex2 == xindex-1)||level==0)
			{
				GetChildren(center4,side/2);
				//level = 0;
			}
			t->fourthChildren = GetTreeNode(zindex2,xindex-1);
		    CreateOcTree(t->fourthChildren,array,zindex2,xindex-1,center4,side/2,level);
		}
		else t->fourthChildren = NULL;
		if(xindex<=zindex3-1)
		{
			if((xindex==zindex3-1)||level==0)
			{
				GetChildren(center5,side/2);
				//level = 0;
			}
			t->fifthChildren = GetTreeNode(xindex,zindex3-1);
		    CreateOcTree(t->fifthChildren,array,xindex,zindex3-1,center5,side/2,level);
		}
		else t->fifthChildren = NULL;
		if(zindex3<=yindex2-1)
		{
			if((zindex3 == yindex2-1)||level==0)
			{
				GetChildren(center6,side/2);
				//level = 0;
			}
			t->sixthChildren = GetTreeNode(zindex3,yindex2-1);
		    CreateOcTree(t->sixthChildren,array,zindex3,yindex2-1,center6,side/2,level);
		}
		else t->sixthChildren = NULL;
		if(yindex2<=zindex4-1)
		{
			if((yindex2==zindex4-1)||level==0)
			{
				GetChildren(center7,side/2);
				//level = 0;
			}
			t->seventhChildren = GetTreeNode(yindex2,zindex4-1);
		    CreateOcTree(t->seventhChildren,array,yindex2,zindex4-1,center7,side/2,level);
		}
		else t->seventhChildren = NULL;
		if(zindex4<=end)
		{
			if((zindex4==end)||level==0)
			{
				GetChildren(center8,side/2);
				//level = 0;
			}
			t->eighthChildren = GetTreeNode(zindex4,end);
			CreateOcTree(t->eighthChildren,array,zindex4,end,center8,side/2,level);
		}
		else t->eighthChildren = NULL;
		}
	}

	else if( (end - begin)>0&&level>0 )
	{   
		level--;
		int xindex,yindex1,yindex2,zindex1,zindex2,zindex3,zindex4;	

		quick_sortX(array,begin,end);
		for( xindex = begin; (array[xindex].x < center.x)&&(xindex<=end);xindex++ );

		quick_sortY(array,begin,xindex-1);
		for( yindex1 = begin; (array[yindex1].y < center.y)&&(yindex1<=xindex-1);yindex1++ );

		quick_sortY(array,xindex,end);
		for( yindex2 = xindex; (array[yindex2].y < center.y)&&(yindex2<=end);yindex2++ );
       
		quick_sortZ(array,begin,yindex1-1);
		for( zindex1 = begin; (array[zindex1].z < center.z)&&(zindex1<=yindex1-1);zindex1++ );

		quick_sortZ(array,yindex1,xindex-1);
		for( zindex2 = yindex1; (array[zindex2].z < center.z)&&(zindex2<=xindex-1);zindex2++ );

		quick_sortZ(array,xindex,yindex2-1);
		for( zindex3 = xindex; (array[zindex3].z < center.z)&&(zindex3<=yindex2-1);zindex3++ );

		quick_sortZ(array,yindex2,end);
		for( zindex4 = yindex2; (array[zindex4].z < center.z)&&(zindex4<=end);zindex4++ );
		
		XYZ center1,center2,center3,center4,center5,center6,center7,center8;
		center1.x = center.x - side/2;center1.y = center.y - side/2;center1.z = center.z - side/2;
		center2.x = center.x - side/2;center2.y = center.y - side/2;center2.z = center.z + side/2;
		center3.x = center.x - side/2;center3.y = center.y + side/2;center3.z = center.z - side/2;
		center4.x = center.x - side/2;center4.y = center.y + side/2;center4.z = center.z + side/2;
		center5.x = center.x + side/2;center5.y = center.y - side/2;center5.z = center.z - side/2;
		center6.x = center.x + side/2;center6.y = center.y - side/2;center6.z = center.z + side/2;
		center7.x = center.x + side/2;center7.y = center.y + side/2;center7.z = center.z - side/2;
		center8.x = center.x + side/2;center8.y = center.y + side/2;center8.z = center.z + side/2;
	
		if(begin <= (zindex1-1))
		{
			if((begin == zindex1-1)||level==0)
			{
				GetChildren(center1,side/2);
				//level = 0;
			}
			t->firstChildren = GetTreeNode(begin,zindex1-1);
			CreateOcTree(t->firstChildren,array,begin,zindex1-1,center1,side/2,level);
		}
		else t->firstChildren = NULL;

		if(zindex1 <= (yindex1-1))
		{
			if((zindex1 == yindex1-1)||level==0)
			{
				GetChildren(center2,side/2);
				//level = 0;
			}
			t->secondChildren = GetTreeNode(zindex1,yindex1-1);
		    CreateOcTree(t->secondChildren,array,zindex1,yindex1-1,center2,side/2,level);
		}
		else t->secondChildren = NULL;

		if(yindex1<=(zindex2-1))
		{
			if(yindex1==zindex2-1||level==0)
			{
				GetChildren(center3,side/2);
				//level = 0;
			}
			t->thirdChildren = GetTreeNode(yindex1,zindex2-1);
		    CreateOcTree(t->thirdChildren,array,yindex1,zindex2-1,center3,side/2,level);
		}
		else t->thirdChildren = NULL;

		if(zindex2<=(xindex-1))
		{
			if((zindex2 == xindex-1)||level==0)
			{
				GetChildren(center4,side/2);
				//level = 0;
			}
			t->fourthChildren = GetTreeNode(zindex2,xindex-1);
		    CreateOcTree(t->fourthChildren,array,zindex2,xindex-1,center4,side/2,level);
		}
		else t->fourthChildren = NULL;

		if(xindex<=(zindex3-1))
		{
			if((xindex==zindex3-1)||level==0)
			{
				GetChildren(center5,side/2);
				//level = 0;
			}
			t->fifthChildren = GetTreeNode(xindex,zindex3-1);
		    CreateOcTree(t->fifthChildren,array,xindex,zindex3-1,center5,side/2,level);
		}
		else t->fifthChildren = NULL;

		if(zindex3<=(yindex2-1))
		{
			if((zindex3 == yindex2-1)||level==0)
			{
				GetChildren(center6,side/2);
				//level = 0;
			}
			t->sixthChildren = GetTreeNode(zindex3,yindex2-1);
		    CreateOcTree(t->sixthChildren,array,zindex3,yindex2-1,center6,side/2,level);
		}
		else t->sixthChildren = NULL;

		if(yindex2<=(zindex4-1))
		{
			if((yindex2==zindex4-1)||level==0)
			{
				GetChildren(center7,side/2);
				//level = 0;
			}
			t->seventhChildren = GetTreeNode(yindex2,zindex4-1);
		    CreateOcTree(t->seventhChildren,array,yindex2,zindex4-1,center7,side/2,level);
		}
		else t->seventhChildren = NULL;

		if(zindex4<=end)
		{
			if((zindex4==end)||level==0)
			{
				GetChildren(center8,side/2);
				//level = 0;
			}
			t->eighthChildren = GetTreeNode(zindex4,end);
			CreateOcTree(t->eighthChildren,array,zindex4,end,center8,side/2,level);
		}
		else t->eighthChildren = NULL;
	}

	else
	{
		t->firstChildren = NULL;
		t->secondChildren = NULL;
		t->thirdChildren = NULL;
		t->fourthChildren = NULL;
		t->fifthChildren = NULL;
		t->sixthChildren = NULL;
		t->seventhChildren = NULL;
		t->eighthChildren = NULL;
	}
}

void OcTree::GetChildren(XYZ center,float side)
{
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;	
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;

	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;		
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;		
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;

	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x + side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z - side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y - side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;
	gainarray[sizegainarray].x = center.x - side;gainarray[sizegainarray].y = center.y + side;gainarray[sizegainarray].z = center.z + side;sizegainarray++;

}

void OcTree::SearchOcTree( TreeNode *t,XYZ val,int range)
{
}


XYZ OcTree::GetCenter( XYZ array[],int first,int last )
{
	XYZ center;
	quick_sortX(array,first,last);
	center.x = abs((array[last].x) + (array[first].x))/2;
	quick_sortY(array,first,last);
	center.y = abs((array[last].y) + (array[first].y))/2;
	quick_sortZ(array,first,last);
	center.z = abs((array[last].z) + (array[first].z))/2;

	return center;

}

float OcTree::GetSideLength( XYZ array[],int first,int last )
{
	quick_sortX(array,first,last);
	float Xarea = (array[last].x - array[first].x)/2;
	quick_sortY(array,first,last);
	float Yarea = (array[last].y - array[first].y)/2;
	quick_sortZ(array,first,last);
	float Zarea = (array[last].z - array[first].z)/2;
	if(Xarea>=Yarea&&Xarea>=Zarea)
		return Xarea;
	else if((Yarea>=Xarea&&Yarea>Zarea)||(Yarea>Xarea&&Yarea>=Zarea))
		return Yarea;
	else 
		return Zarea;

}
void OcTree::PrintOcTree(TreeNode *t)
{
	if(t!=NULL)
	{  
		cout<<"begin:"<<t->begin<<" end:"<<t->end<<endl;
		PrintOcTree(t->firstChildren);
		PrintOcTree(t->secondChildren);
		PrintOcTree(t->thirdChildren);
		PrintOcTree(t->fourthChildren);
		PrintOcTree(t->fifthChildren);
		PrintOcTree(t->sixthChildren);
		PrintOcTree(t->seventhChildren);
		PrintOcTree(t->eighthChildren);
	}
}