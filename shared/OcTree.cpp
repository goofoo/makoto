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
	num_voxel = 0;
	create(root, data, 0, num_data-1, center, size, 0, num_voxel);
}

void OcTree::create(TreeNode *node, PosAndId* data, int low, int high, const XYZ& center, const float size, short level, unsigned &count)
{
//zDisplayFloat2(low, high);
	
	//if(high == low) if(!isInBox(data[low], center, size)) return; 
	node->low = low;
	node->high = high;
	node->center = center;
	node->size = size;
	getMean(data, low, high, node->mean);
	//getColorMean(data, low, high, node->colorMean);
	//node->isNull = 0;
	count++;
	node->index = count;
	if(level == max_level ) return;

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
			create(node->child000,data,low,zindex1-1,acen,halfsize,level, count);
		}
		else node->child000 = NULL;

		if(zindex1 <= yindex1-1)
		{
			node->child001 = new TreeNode();
			acen = center + XYZ(-halfsize, -halfsize, halfsize);
			create(node->child001,data,zindex1,yindex1-1,acen,halfsize,level, count);
		}
		else node->child001 = NULL;

		if(yindex1<=zindex2-1)
		{
			node->child010 = new TreeNode();
			acen = center + XYZ(-halfsize, halfsize, -halfsize);
			create(node->child010,data,yindex1,zindex2-1,acen,halfsize,level, count);
		}
		else node->child010 = NULL;

		if(zindex2<=xindex-1)
		{
			node->child011 = new TreeNode();
			acen = center + XYZ(-halfsize, halfsize, halfsize);
			create(node->child011,data,zindex2,xindex-1,acen,halfsize,level, count);
		}
		else node->child011 = NULL;

		if(xindex<=zindex3-1)
		{
			node->child100 = new TreeNode();
			acen = center + XYZ(halfsize, -halfsize, -halfsize);
			create(node->child100,data,xindex,zindex3-1,acen,halfsize,level, count);
		}
		else node->child100 = NULL;

		if(zindex3<=yindex2-1)
		{
			node->child101 = new TreeNode();
			acen = center + XYZ(halfsize, -halfsize, halfsize);
			create(node->child101,data,zindex3,yindex2-1,acen,halfsize,level, count);
		}
		else node->child101 = NULL;

		if(yindex2<=zindex4-1)
		{
			node->child110 = new TreeNode();
			acen = center + XYZ(halfsize, halfsize, -halfsize);
			create(node->child110,data,yindex2,zindex4-1,acen,halfsize,level, count);
		}
		else node->child110 = NULL;

		if(zindex4<=high)
		{
			node->child111 = new TreeNode();
			acen = center + XYZ(halfsize, halfsize, halfsize);
			create(node->child111,data,zindex4,high,acen,halfsize,level, count);
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

void OcTree::draw(const char*filename)
{
	infile.open(filename,ios_base::out | ios_base::binary );
	if(!infile.is_open())
		return;
	unsigned sum,numVoxel;
	short level;
	infile.read((char*)&level,sizeof(short));
	infile.read((char*)&sum,sizeof(unsigned));
	infile.read((char*)&numVoxel,sizeof(unsigned));
	max_level = level;
	if(root) draw(root,filename,numVoxel,0);
	infile.close();
}

void OcTree::draw(const TreeNode *node,const char*filename,unsigned numVoxel,short level)
{
	if(!node) return;
	if((!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111)||max_level == level) {
		//if(node->isNull) {
			XYZ cen = node->center;
			XYZ color;
			float size = node->size;
			infile.seekg(72*numVoxel+10+(node->index -1)*12,ios_base::beg);
			infile.read((char*)&color,sizeof(XYZ));
			glColor3f(color.x,color.y,color.z);
			/*
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
			*/

			glVertex3f(cen.x - 0.8*size, cen.y - 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y - 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y + 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y + 0.8*size, cen.z - 0.8*size);

			glVertex3f(cen.x + 0.8*size, cen.y - 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y - 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y + 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y + 0.8*size, cen.z - 0.8*size);
			
			glVertex3f(cen.x + 0.8*size, cen.y - 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y - 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y + 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y + 0.8*size, cen.z + 0.8*size);

			glVertex3f(cen.x - 0.8*size, cen.y - 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y - 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y + 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y + 0.8*size, cen.z - 0.8*size);
			
			glVertex3f(cen.x - 0.8*size, cen.y - 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y - 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y - 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y - 0.8*size, cen.z + 0.8*size);

			glVertex3f(cen.x - 0.8*size, cen.y + 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y + 0.8*size, cen.z - 0.8*size);
			glVertex3f(cen.x + 0.8*size, cen.y + 0.8*size, cen.z + 0.8*size);
			glVertex3f(cen.x - 0.8*size, cen.y + 0.8*size, cen.z + 0.8*size);
		
	}
	else {
		level++;
		draw(node->child000,filename,numVoxel,level);
		draw(node->child001,filename,numVoxel,level);
		draw(node->child010,filename,numVoxel,level);
		draw(node->child011,filename,numVoxel,level);
		draw(node->child100,filename,numVoxel,level);
		draw(node->child101,filename,numVoxel,level);
		draw(node->child110,filename,numVoxel,level);
		draw(node->child111,filename,numVoxel,level);
	}
}

void OcTree::getRootCenterNSize(XYZ& center, float&size) const
{
	if(root) {
		center = root->center;
		size = root->size;
	}
}

void OcTree::getBBox(const PosAndId* data, const int num_data, XYZ& center, float& size)
{
	XYZ bbmin(10000000);
	XYZ bbmax(-10000000);
	
	for(int i=0; i<num_data; i++) {
		if(data[i].pos.x < bbmin.x) bbmin.x = data[i].pos.x;
		if(data[i].pos.y < bbmin.y) bbmin.y = data[i].pos.y;
		if(data[i].pos.z < bbmin.z) bbmin.z = data[i].pos.z;
		if(data[i].pos.x > bbmax.x) bbmax.x = data[i].pos.x;
		if(data[i].pos.y > bbmax.y) bbmax.y = data[i].pos.y;
		if(data[i].pos.z > bbmax.z) bbmax.z = data[i].pos.z;
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


void OcTree::saveFile(const char*filename,OcTree* tree,unsigned sum,XYZ *color,PosAndId *buf,XYZ *velocity,const short level)
{
	outfile.open(filename,ios_base::out | ios_base::binary );
	if(!outfile.is_open())
		return ;
	outfile.write((char*)&level,sizeof(short));
    outfile.write((char*)&sum,sizeof(unsigned));
	unsigned numVoxel = tree->getNumVoxel();
	outfile.write((char*)&numVoxel,sizeof(unsigned));
	if(sum>0)
	{
		saveTree(root);
		saveColor(root,color,buf);
		saveVelocity(root,velocity,buf);
	}
	outfile.close();
}

void OcTree::saveTree(TreeNode *node)
{
	if(!node) return;
	else
	{
		outfile.write((char*)&node->index,sizeof(int));
		outfile.write((char*)&node->low,sizeof(unsigned));
		outfile.write((char*)&node->high,sizeof(unsigned));
		outfile.write((char*)&node->center,sizeof(XYZ));
		outfile.write((char*)&node->mean,sizeof(XYZ));
		outfile.write((char*)&node->size,sizeof(float));
		outfile.write((char*)&node->child000,sizeof(node->child000));
	    outfile.write((char*)&node->child001,sizeof(node->child001));
	    outfile.write((char*)&node->child010,sizeof(node->child010));
	    outfile.write((char*)&node->child011,sizeof(node->child011));
	    outfile.write((char*)&node->child100,sizeof(node->child100));
	    outfile.write((char*)&node->child101,sizeof(node->child101));
	    outfile.write((char*)&node->child110,sizeof(node->child110));
	    outfile.write((char*)&node->child111,sizeof(node->child111));
		saveTree(node->child000);
		saveTree(node->child001);
		saveTree(node->child010);
		saveTree(node->child011);
		saveTree(node->child100);
		saveTree(node->child101);
		saveTree(node->child110);
		saveTree(node->child111);
	}
}

void OcTree::saveColor(TreeNode *node,XYZ *color,PosAndId *buf)
{
	if(!node) return;
	else
	{
		XYZ colorMean = 0;
		for(unsigned int i = node->low;i<=node->high;i++)
			colorMean += color[buf[i].idx];
		colorMean /= float(node->high - node->low + 1);
		outfile.write((char*)&colorMean,sizeof(XYZ));
	    saveColor(node->child000,color,buf);
		saveColor(node->child001,color,buf);
		saveColor(node->child010,color,buf);
		saveColor(node->child011,color,buf);
		saveColor(node->child100,color,buf);
		saveColor(node->child101,color,buf);
		saveColor(node->child110,color,buf);
		saveColor(node->child111,color,buf);
	}
}

void OcTree::saveVelocity(TreeNode *node,XYZ *velocity,PosAndId *buf)
{
	if(!node) return;
	else
	{
		XYZ velocityMean = 0;
		for(unsigned int i = node->low;i<=node->high;i++)
			velocityMean += velocity[buf[i].idx];
		velocityMean /= float(node->high - node->low + 1);
		outfile.write((char*)&velocityMean,sizeof(XYZ));
	    saveColor(node->child000,velocity,buf);
		saveColor(node->child001,velocity,buf);
		saveColor(node->child010,velocity,buf);
		saveColor(node->child011,velocity,buf);
		saveColor(node->child100,velocity,buf);
		saveColor(node->child101,velocity,buf);
		saveColor(node->child110,velocity,buf);
		saveColor(node->child111,velocity,buf);
	}
}

void OcTree::loadFile(const char*filename,OcTree* tree)
{
	infile.open(filename,ios_base::out | ios_base::binary );
	if(!infile.is_open())
		return;
	unsigned sum,numVoxel;
	short level;
	infile.read((char*)&level,sizeof(short));
	infile.read((char*)&sum,sizeof(unsigned));
	infile.read((char*)&numVoxel,sizeof(unsigned));
	if(sum>0)
	{   
		root = new TreeNode();
		loadTree(root);
	}
	infile.close();
}
void OcTree::loadTree(TreeNode *node)
{   
	if(node == NULL) return;
	infile.read((char*)&node->index,sizeof(int));
	infile.read((char*)&node->low,sizeof(unsigned));
	infile.read((char*)&node->high,sizeof(unsigned));
	infile.read((char*)&node->center,sizeof(XYZ));
	infile.read((char*)&node->mean,sizeof(XYZ));
	infile.read((char*)&node->size,sizeof(float));
	infile.read((char*)&node->child000,sizeof(node->child000));
	infile.read((char*)&node->child001,sizeof(node->child001));
	infile.read((char*)&node->child010,sizeof(node->child010));
	infile.read((char*)&node->child011,sizeof(node->child011));
	infile.read((char*)&node->child100,sizeof(node->child100));
	infile.read((char*)&node->child101,sizeof(node->child101));
	infile.read((char*)&node->child110,sizeof(node->child110));
	infile.read((char*)&node->child111,sizeof(node->child111));
	if(node->child000)
	{
		node->child000 = new TreeNode();
		loadTree(node->child000);
	}
	else node->child000 = NULL;
	if(node->child001)
	{
		node->child001 = new TreeNode();
		loadTree(node->child001);
	}
	else node->child001 = NULL;
	if(node->child010)
	{
		node->child010 = new TreeNode();
		loadTree(node->child010);
	}
	else node->child010 = NULL;
	if(node->child011)
	{
		node->child011 = new TreeNode();
		loadTree(node->child011);
	}
	else node->child011 = NULL;
	if(node->child100)
	{
		node->child100 = new TreeNode();
		loadTree(node->child100);
	}
	else node->child100 = NULL;
	if(node->child101)
	{
		node->child101 = new TreeNode();
		loadTree(node->child101);
	}
	else node->child101 = NULL;
	if(node->child110)
	{
		node->child110 = new TreeNode();
		loadTree(node->child110);
	}
	else node->child110 = NULL;
	if(node->child111)
	{
		node->child111 = new TreeNode();
		loadTree(node->child111);
	}
	else node->child111 = NULL;
}
//:~