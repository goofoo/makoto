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
	release();
}

void OcTree::construct(PosAndId* data, const int num_data, const XYZ& center, const float size,short level)
{
	release();
	root = new TreeNode();
	max_level = level;
	create(root, data, 0, num_data-1, center, size, 0);
}

void OcTree::create(TreeNode *node, PosAndId* data, int low, int high, const XYZ& center, const float size, short level)
{
//zDisplayFloat2(low, high);
	
	//if(high == low) if(!isInBox(data[low], center, size)) return; 
	node->low = low;
	node->high = high;
	node->center = center;
	node->size = size;
	getMean(data, low, high, node->mean);
	//node->isNull = 0;

	if(level == max_level ) {
		//node->isNull = 1;
		return;
	}
	level++;
	float halfsize = size/2;
	XYZ acen;
	
	
	int xindex,yindex1,yindex2,zindex1,zindex2,zindex3,zindex4;	

		QuickSort::sortX(data,low,high);
		for( xindex = low; (data[xindex].pos.x < center.x)&&(xindex<=high);xindex++ );

		QuickSort::sortY(data,low,xindex-1);
		for( yindex1 = low; (data[yindex1].pos.y < center.y)&&(yindex1<=xindex-1);yindex1++ );

		QuickSort::sortY(data,xindex,high);
		for( yindex2 = xindex; (data[yindex2].pos.y < center.y)&&(yindex2<=high);yindex2++ );
       
		QuickSort::sortZ(data,low,yindex1-1);
		for( zindex1 = low; (data[zindex1].pos.z < center.z)&&(zindex1<=yindex1-1);zindex1++ );

		QuickSort::sortZ(data,yindex1,xindex-1);
		for( zindex2 = yindex1; (data[zindex2].pos.z < center.z)&&(zindex2<=xindex-1);zindex2++ );

		QuickSort::sortZ(data,xindex,yindex2-1);
		for( zindex3 = xindex; (data[zindex3].pos.z < center.z)&&(zindex3<=yindex2-1);zindex3++ );

		QuickSort::sortZ(data,yindex2,high);
		for( zindex4 = yindex2; (data[zindex4].pos.z < center.z)&&(zindex4<=high);zindex4++ );
	
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
{/*
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
*/
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
		
		//if(node->isNull) {
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
		//}
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

void OcTree::getRootCenterNSize(XYZ& center, float&size) const
{
	if(root) {
		center = root->center;
		size = root->size;
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

char OcTree::isInBox(const XYZ& data, const XYZ& center, float size)
{
	if(data.x < center.x - size - 0.0001 || data.x > center.x + size + 0.0001) return 0;
	if(data.y < center.y - size - 0.0001 || data.y > center.y + size + 0.0001) return 0;
	if(data.z < center.z - size - 0.0001 || data.z > center.z + size + 0.0001) return 0;
	
	return 1;
}

void OcTree::release()
{
	if(root) release(root);
}

void OcTree::release(TreeNode *node)
{
	if(!node) return;
	
	if(node->child000!=NULL)
		release(node->child000);
	if(node->child001!=NULL)
		release(node->child001);
	if(node->child010!=NULL)
		release(node->child010);
	if(node->child011!=NULL)
		release(node->child011);
	if(node->child100!=NULL)
		release(node->child100);
	if(node->child101!=NULL)
		release(node->child101);
	if(node->child110!=NULL)
		release(node->child110);
	if(node->child111!=NULL)
		release(node->child111);

	delete node;
}

void OcTree::getMean(const PosAndId *data, const int low, const int high, XYZ& center)
{
	center.x = center.y = center.z = 0.f;
	
	for(int i=low; i<=high; i++) center += data[i].pos;
	
	center /= float(high - low + 1);
}
//:~