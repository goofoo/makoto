#include "OcTree.h"
#include "zWorks.h"

#ifdef WIN32
# include <windows.h>
#endif
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#include "./glext.h"
#endif


OcTree::OcTree():root(0)
{
}

OcTree::~OcTree()
{
	if(root) DeleteTree();
}

void OcTree::construct(XYZ* data, const int num_data, const XYZ& center, const float size,short level)
{
	if(root) delete root; 
	root = new TreeNode();
	create(root, data, 0, num_data-1, center, size, level);
}

void OcTree::create(TreeNode *node, XYZ* data, int low, int high, const XYZ& center, const float size, short level)
{
//zDisplayFloat2(low, high);
	
	//if(high == low) if(!isInBox(data[low], center, size)) return;
	
	node->begin = low;
	node->end = high;
	node->center = center;
	node->size = size;
	node->isNull = 0;

	if(level == 0 ) 
	{
		node->isNull = 1;
		return;
	}
	level -= 1;
	float halfsize = size/2;
	XYZ acen;
	
	
	int xindex,yindex1,yindex2,zindex1,zindex2,zindex3,zindex4;	

		quick_sortX(data,low,high);
		for( xindex = low; (data[xindex].x < center.x)&&(xindex<=high);xindex++ );

		quick_sortY(data,low,xindex-1);
		for( yindex1 = low; (data[yindex1].y < center.y)&&(yindex1<=xindex-1);yindex1++ );

		quick_sortY(data,xindex,high);
		for( yindex2 = xindex; (data[yindex2].y < center.y)&&(yindex2<=high);yindex2++ );
       
		quick_sortZ(data,low,yindex1-1);
		for( zindex1 = low; (data[zindex1].z < center.z)&&(zindex1<=yindex1-1);zindex1++ );

		quick_sortZ(data,yindex1,xindex-1);
		for( zindex2 = yindex1; (data[zindex2].z < center.z)&&(zindex2<=xindex-1);zindex2++ );

		quick_sortZ(data,xindex,yindex2-1);
		for( zindex3 = xindex; (data[zindex3].z < center.z)&&(zindex3<=yindex2-1);zindex3++ );

		quick_sortZ(data,yindex2,high);
		for( zindex4 = yindex2; (data[zindex4].z < center.z)&&(zindex4<=high);zindex4++ );
	
		if(low <= zindex1-1)
		{
			node->child000 = new TreeNode();
			acen = center + XYZ(-halfsize, -halfsize, -halfsize);
			create(node->child000,data,low,zindex1-1,acen,halfsize,level);
		}
		else node->child000 = NULL;

		if(zindex1 <= yindex1-1)
		{
			node->child001 = new TreeNode();
			acen = center + XYZ(-halfsize, -halfsize, halfsize);
			create(node->child001,data,zindex1,yindex1-1,acen,halfsize,level);
		}
		else node->child001 = NULL;

		if(yindex1<=zindex2-1)
		{
			node->child010 = new TreeNode();
			acen = center + XYZ(-halfsize, halfsize, -halfsize);
			create(node->child010,data,yindex1,zindex2-1,acen,halfsize,level);
		}
		else node->child010 = NULL;

		if(zindex2<=xindex-1)
		{
			node->child011 = new TreeNode();
			acen = center + XYZ(-halfsize, halfsize, halfsize);
			create(node->child011,data,zindex2,xindex-1,acen,halfsize,level);
		}
		else node->child011 = NULL;

		if(xindex<=zindex3-1)
		{
			node->child100 = new TreeNode();
			acen = center + XYZ(halfsize, -halfsize, -halfsize);
			create(node->child100,data,xindex,zindex3-1,acen,halfsize,level);
		}
		else node->child100 = NULL;

		if(zindex3<=yindex2-1)
		{
			node->child101 = new TreeNode();
			acen = center + XYZ(halfsize, -halfsize, halfsize);
			create(node->child101,data,zindex3,yindex2-1,acen,halfsize,level);
		}
		else node->child101 = NULL;

		if(yindex2<=zindex4-1)
		{
			node->child110 = new TreeNode();
			acen = center + XYZ(halfsize, halfsize, -halfsize);
			create(node->child110,data,yindex2,zindex4-1,acen,halfsize,level);
		}
		else node->child110 = NULL;

		if(zindex4<=high)
		{
			node->child111 = new TreeNode();
			acen = center + XYZ(halfsize, halfsize, halfsize);
			create(node->child111,data,zindex4,high,acen,halfsize,level);
		}
		else node->child111 = NULL;

}

void OcTree::search( XYZ position,float area,XYZ* data,XYZ* &areadata,int count)
{
	if(root) search(root,position,area,data,areadata,count);
}

void OcTree::search(TreeNode *node,XYZ position,float area,XYZ* data,XYZ* &areadata,int count)
{
	if(!node) return;
	if( node->isNull )
		for(int i = node->begin;i<=node->end;i++)
		{
			if(acount<count && ((data[i].x - position.x)*(data[i].x - position.x)+(data[i].y - position.y)*(data[i].y - position.y)+(data[i].z - position.z)*(data[i].z - position.z))<=area*area)
			{		
				areadata[acount] = data[i];
				//cout<<acount<<"  "<<node->begin<<"  "<<node->end<<"  "<<i<<"  "<<areadata[acount].x<<"  "<<areadata[acount].y<<"  "<<areadata[acount].z<<endl;
				acount++;
			}
		}
	//cout<<node->center.x<<"  "<<position.x<<"  "<<area<<"  "<<node->size<<endl;
	
	if((position.x + area<=node->center.x)&&(position.x + area>=node->center.x-node->size))
	{
		if((position.y + area<=node->center.y)&&(position.y + area>=node->center.y-node->size))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child000,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child001,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child001,position,area,data,areadata,count);
			}
			else return;
		}
		else if((position.y - area>node->center.y)&&(position.y - area<=node->center.y+node->size))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child010,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child011,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child010,position,area,data,areadata,count);
				search(node->child011,position,area,data,areadata,count);
			}
			else return;
		}
		else if((position.y + area>node->center.y)&&(position.y - area<node->center.y))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child010,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child001,position,area,data,areadata,count);
				search(node->child011,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child001,position,area,data,areadata,count);
				search(node->child010,position,area,data,areadata,count);
				search(node->child011,position,area,data,areadata,count);
			}
			else return;
		}
		else return;
	}
	else if((position.x - area>node->center.x)&&(position.x - area<=node->center.x+node->size))
	{
		if((position.y + area<=node->center.y)&&(position.y + area>=node->center.y-node->size))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child100,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child101,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child100,position,area,data,areadata,count);
				search(node->child101,position,area,data,areadata,count);
			}
			else return;
		}
		else if((position.y - area>node->center.y)&&(position.y - area<=node->center.y+node->size))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child110,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child111,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child110,position,area,data,areadata,count);
				search(node->child111,position,area,data,areadata,count);
			}
			else return;
		}
		else if((position.y + area>node->center.y)&&(position.y - area<node->center.y))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child100,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child101,position,area,data,areadata,count);
				search(node->child111,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child100,position,area,data,areadata,count);
				search(node->child101,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
				search(node->child111,position,area,data,areadata,count);
			}
			else return;
		}
		else return;
	}
	else if((position.x + area>node->center.x)&&(position.x - area<node->center.x))
	{
		if((position.y + area<=node->center.y)&&(position.y + area>=node->center.y-node->size))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child100,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child001,position,area,data,areadata,count);
				search(node->child101,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child001,position,area,data,areadata,count);
				search(node->child100,position,area,data,areadata,count);
				search(node->child101,position,area,data,areadata,count);
			}
			else return;
		}
		else if((position.y - area>node->center.y)&&(position.y - area<=node->center.y+node->size))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child010,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child011,position,area,data,areadata,count);
				search(node->child111,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child010,position,area,data,areadata,count);
				search(node->child011,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
				search(node->child111,position,area,data,areadata,count);
			}
			else return;
		}
		else if((position.y + area>node->center.y)&&(position.y - area<node->center.y))
		{
			if((position.z + area<=node->center.z)&&(position.z + area>=node->center.z-node->size))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child100,position,area,data,areadata,count);
				search(node->child010,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
			}
			else if((position.z - area>node->center.z)&&(position.z - area<=node->center.z+node->size))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child100,position,area,data,areadata,count);
				search(node->child010,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
			}
			else if((position.z + area>node->center.z)&&(position.z - area<node->center.z))
			{
				search(node->child000,position,area,data,areadata,count);
				search(node->child001,position,area,data,areadata,count);
				search(node->child010,position,area,data,areadata,count);
				search(node->child011,position,area,data,areadata,count);
				search(node->child100,position,area,data,areadata,count);
				search(node->child101,position,area,data,areadata,count);
				search(node->child110,position,area,data,areadata,count);
				search(node->child111,position,area,data,areadata,count);
			}
			else return;
		}
		else return;
	}
	else return;

/*	
	if((acount<count)&&(node->isNull == 1 || node->end - node->begin < 8))
	{
		for(int i = node->begin;i<node->end;i++)
		{
			if(((data[i].x - position.x)*(data[i].x - position.x)+(data[i].y - position.y)*(data[i].y - position.y)+(data[i].z - position.z)*(data[i].z - position.z))<=area*area)
			{		
				areadata[acount] = data[i];
				cout<<acount<<"  "<<node->begin<<"  "<<node->end<<"  "<<i<<"  "<<areadata[acount].x<<"  "<<areadata[acount].y<<"  "<<areadata[acount].z<<endl;
				acount++;
			}
		}
	}*/
}

void OcTree::draw()
{
	if(root) draw(root);
}

void OcTree::draw(const TreeNode *node)
{
	if(!node) return;
	if(!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111) {
		
		if(node->isNull) {
			XYZ cen = node->center;
			float size = node->size;
			
			glVertex3f(cen.x - size, cen.y - size, cen.z - size);
			glVertex3f(cen.x + size, cen.y - size, cen.z - size);
			glVertex3f(cen.x - size, cen.y + size, cen.z - size);
			glVertex3f(cen.x + size, cen.y + size, cen.z - size);
			glVertex3f(cen.x - size, cen.y - size, cen.z + size);
			glVertex3f(cen.x + size, cen.y - size, cen.z + size);
			glVertex3f(cen.x - size, cen.y + size, cen.z + size);
			glVertex3f(cen.x + size, cen.y + size, cen.z + size);
			
			glVertex3f(cen.x - size, cen.y - size, cen.z - size);
			glVertex3f(cen.x - size, cen.y - size, cen.z + size);
			glVertex3f(cen.x - size, cen.y + size, cen.z - size);
			glVertex3f(cen.x - size, cen.y + size, cen.z + size);
			glVertex3f(cen.x + size, cen.y - size, cen.z - size);
			glVertex3f(cen.x + size, cen.y - size, cen.z + size);
			glVertex3f(cen.x + size, cen.y + size, cen.z - size);
			glVertex3f(cen.x + size, cen.y + size, cen.z + size);
			
			glVertex3f(cen.x - size, cen.y - size, cen.z - size);
			glVertex3f(cen.x - size, cen.y + size, cen.z - size);
			glVertex3f(cen.x - size, cen.y - size, cen.z + size);
			glVertex3f(cen.x - size, cen.y + size, cen.z + size);
			glVertex3f(cen.x + size, cen.y - size, cen.z - size);
			glVertex3f(cen.x + size, cen.y + size, cen.z - size);
			glVertex3f(cen.x + size, cen.y - size, cen.z + size);
			glVertex3f(cen.x + size, cen.y + size, cen.z + size);
		}
	}
	else {
		draw(node->child000);
		draw(node->child001);
		draw(node->child010);
		draw(node->child011);
		draw(node->child100);
		draw(node->child101);
		draw(node->child110);
		draw(node->child111);
	}
}

void OcTree::getBBox(const XYZ *data, const int num_data, XYZ& center, float& size)
{
	XYZ bbmin(10000000);
	XYZ bbmax(-10000000);
	
	for(int i=0; i<num_data; i++) {
		if(data[i].x < bbmin.x) bbmin.x = data[i].x;
		if(data[i].y < bbmin.y) bbmin.y = data[i].y;
		if(data[i].z < bbmin.z) bbmin.z = data[i].z;
		if(data[i].x > bbmax.x) bbmax.x = data[i].x;
		if(data[i].y > bbmax.y) bbmax.y = data[i].y;
		if(data[i].z > bbmax.z) bbmax.z = data[i].z;
	}
	
	center = (bbmin + bbmax)/2;
	
	size = bbmax.x - center.x;
	float ts = bbmax.y - center.y;
	if(ts > size) size = ts;
	ts = bbmax.z - center.z;
	if(ts > size) size = ts;
}

void OcTree::splitX(const XYZ *data, const int low, const int high, const float center, int& cutat)
{
	if(data[low].x > center) {
		cutat = low;
		return;
	}
	
	if(data[high].x <= center) {
		cutat = high;
		return;
	}
	
	cutat = (low + high)/2;
	if(cutat == low) return;
	
	if(data[cutat].x <= center) {
		if(data[cutat+1].x > center) return;
	
		splitX(data, cutat+1, high, center, cutat);
	}
	else splitX(data, low, cutat, center, cutat);
}

void OcTree::splitY(const XYZ *data, const int low, const int high, const float center, int& cutat)
{
	if(data[low].y > center) {
		cutat = low;
		return;
	}
	
	if(data[high].y <= center) {
		cutat = high;
		return;
	}
	
	cutat = (low + high)/2;
	if(cutat == low) return;
	
	if(data[cutat].y <= center) {
		if(data[cutat+1].y > center) return;
	
		splitY(data, cutat+1, high, center, cutat);
	}
	else splitY(data, low, cutat, center, cutat);
}

void OcTree::splitZ(const XYZ *data, const int low, const int high, const float center, int& cutat)
{
	if(data[low].z > center) {
		cutat = low;
		return;
	}
	
	if(data[high].z <= center) {
		cutat = high;
		return;
	}
	
	cutat = (low + high)/2;
	if(cutat == low) return;
	
	if(data[cutat].z <= center) {
		if(data[cutat+1].z > center) return;
	
		splitZ(data, cutat+1, high, center, cutat);
	}
	else splitZ(data, low, cutat, center, cutat);
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

char OcTree::isInBox(const XYZ& data, const XYZ& center, float size)
{
	if(data.x < center.x - size - 0.0001 || data.x > center.x + size + 0.0001) return 0;
	if(data.y < center.y - size - 0.0001 || data.y > center.y + size + 0.0001) return 0;
	if(data.z < center.z - size - 0.0001 || data.z > center.z + size + 0.0001) return 0;
	
	return 1;
}

void OcTree::DeleteTree()
{
	if(root==NULL)	return;
	DeleteTree(root);
}

void OcTree::DeleteTree(TreeNode *root)
{
	if(root->child000!=NULL)
		DeleteTree(root->child000);
	if(root->child001!=NULL)
		DeleteTree(root->child001);
	if(root->child010!=NULL)
		DeleteTree(root->child010);
	if(root->child011!=NULL)
		DeleteTree(root->child011);
	if(root->child100!=NULL)
		DeleteTree(root->child100);
	if(root->child101!=NULL)
		DeleteTree(root->child101);
	if(root->child110!=NULL)
		DeleteTree(root->child110);
	if(root->child111!=NULL)
		DeleteTree(root->child111);
	free(root);
}

//:~