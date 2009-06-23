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
	if(root) delete root;
}

void OcTree::construct(XYZ* data, const int num_data, const XYZ& center, const float size)
{
	if(root) delete root; 
	root = new TreeNode();
	create(root, data, 0, num_data-1, center, size, 0);
}

void OcTree::create(TreeNode *node, XYZ* data, int low, int high, const XYZ& center, const float size, const short level)
{
//zDisplayFloat2(low, high);
	
	if(high == low) if(!isInBox(data[low], center, size)) return;
	
	node->center = center;
	node->size = size;
	node->isNull = 0;
	
	if(level == 6) return;
	
	float halfsize = size/2;
	XYZ acen;
	short alevel = level + 1;
	
	if(high - low < 8) {
		int has000 = 0;
		int has001 = 0;
		int has010 = 0;
		int has011 = 0;
		int has100 = 0;
		int has101 = 0;
		int has110 = 0;
		int has111 = 0;
		for(int i=low; i<=high; i++) {
			if(isInBox(data[i], center, size)) {
				if(data[i].x <= center.x) {
					if(data[i].y <= center.y) {
						if(data[i].z <= center.z) has000 = 1;
						else has001 = 1;
					}
					else {
						if(data[i].z <= center.z) has010 = 1;
						else has011 = 1;
					}
				}
				else {
					if(data[i].y <= center.y) {
						if(data[i].z <= center.z) has100 = 1;
						else has101 = 1;
					}
					else {
						if(data[i].z <= center.z) has110 = 1;
						else has111 = 1;
					}
				}
			}/*
			if(data[i].x > center.x -size && data[i].x < center.x +size) {
				if(data[i].x <= center.x) {
					if(data[i].y > center.y -size && data[i].y < center.y +size) {
						if(data[i].y <= center.y) {
							if(data[i].z > center.z -size && data[i].z < center.z +size) {
								if(data[i].z <= center.z) has000 = 1;
								else has001 = 1;
							}
						}
						else {
							if(data[i].z > center.z -size && data[i].z < center.z +size) {
								if(data[i].z <= center.z) has010 = 1;
								else has011 = 1;
							}
						}
					}
				}
				else {
					if(data[i].y > center.y -size && data[i].y < center.y +size) {
						if(data[i].y <= center.y) {
							if(data[i].z > center.z -size && data[i].z < center.z +size) {
								if(data[i].z <= center.z) has100 = 1;
								else has101 = 1;
							}
						}
						else {
							if(data[i].z > center.z -size && data[i].z < center.z +size) {
								if(data[i].z <= center.z) has110 = 1;
								else has111 = 1;
							}
						}
					}
				}
			}*/
		}
		
		if(has000 == 0 && has001 == 0 && has010 == 0 && has011 == 0 && has100 == 0 && has101 == 0 && has110 == 0 && has111 == 0) {
			node->isNull = 1;
			return;
		}
		
		
		if(has000 == 1){
			acen = center + XYZ(-halfsize, -halfsize, -halfsize);
			node->child000 = new TreeNode();
			create(node->child000, data, low, high, acen, halfsize, alevel);
		}
		
		if(has001 == 1){
			acen = center + XYZ(-halfsize, -halfsize, halfsize);
			node->child001 = new TreeNode();
			create(node->child001, data, low, high, acen, halfsize, alevel);
		}
		if(has010 == 1){
			acen = center + XYZ(-halfsize, halfsize, -halfsize);
			node->child010 = new TreeNode();
			create(node->child010, data, low, high, acen, halfsize, alevel);
		}
		
		if(has011 == 1){
			acen = center + XYZ(-halfsize, halfsize, halfsize);
			node->child011 = new TreeNode();
			create(node->child011, data, low, high, acen, halfsize, alevel);
		}
		if(has100 == 1){
			acen = center + XYZ(halfsize, -halfsize, -halfsize);
			node->child100 = new TreeNode();
			create(node->child100, data, low, high, acen, halfsize, alevel);
		}
		
		if(has101 == 1){
			acen = center + XYZ(halfsize, -halfsize, halfsize);
			node->child101 = new TreeNode();
			create(node->child101, data, low, high, acen, halfsize, alevel);
		}
		if(has110 == 1){
			acen = center + XYZ(halfsize, halfsize, -halfsize);
			node->child110 = new TreeNode();
			create(node->child110, data, low, high, acen, halfsize, alevel);
		}
		
		if(has111 == 1){
			acen = center + XYZ(halfsize, halfsize, halfsize);
			node->child111 = new TreeNode();
			create(node->child111, data, low, high, acen, halfsize, alevel);
		}

	} 
	
	else {
		quick_sortX(data, low, high);
		
		int i_x, i_y, i_z;
		splitX(data, low, high, center.x, i_x);
		
		if(low <= i_x) {
			quick_sortY(data, low, i_x);
			splitY(data, low, i_x, center.y, i_y);
			if(low <= i_y) {
				quick_sortZ(data, low, i_y);
				splitZ(data, low, i_y, center.z, i_z);
				if(low <= i_z) {
					acen = center + XYZ(-halfsize, -halfsize, -halfsize);
					node->child000 = new TreeNode();
					create(node->child000, data, low, i_z, acen, halfsize, alevel);
				}
				if(i_z <= i_y) {
					acen = center + XYZ(-halfsize, -halfsize, halfsize);
					node->child001 = new TreeNode();
					create(node->child001, data, i_z, i_y, acen, halfsize, alevel);
				}
			}
			if(i_y <= i_x) {
				quick_sortZ(data, i_y, i_x);
				splitZ(data, i_y, i_x, center.z, i_z);
				if(i_y <= i_z) {
					acen = center + XYZ(-halfsize, halfsize, -halfsize);
					node->child010 = new TreeNode();
					create(node->child010, data, i_y, i_z, acen, halfsize, alevel);
				}
				if(i_z <= i_x) {
					acen = center + XYZ(-halfsize, halfsize, halfsize);
					node->child011 = new TreeNode();
					create(node->child011, data, i_z, i_x, acen, halfsize, alevel);
				}
			}
		}
		if(i_x <= high) {
			quick_sortY(data, i_x, high);
			splitY(data, i_x, high, center.y, i_y);
			if(i_x <= i_y) {
				quick_sortZ(data, i_x, i_y);
				splitZ(data, i_x, i_y, center.z, i_z);
				if(i_x <= i_z) {
					acen = center + XYZ(halfsize, -halfsize, -halfsize);
					node->child100 = new TreeNode();
					create(node->child100, data, i_x, i_z, acen, halfsize, alevel);
				}
				if(i_z <= i_y) {
					acen = center + XYZ(halfsize, -halfsize, halfsize);
					node->child101 = new TreeNode();
					create(node->child101, data, i_z, i_y, acen, halfsize, alevel);
				}
			}
			if(i_y <= high) {
				quick_sortZ(data, i_y, high);
				splitZ(data, i_y, high, center.z, i_z);
				if(i_y <= i_z) {
					acen = center + XYZ(halfsize, halfsize, -halfsize);
					node->child110 = new TreeNode();
					create(node->child110, data, i_y, i_z, acen, halfsize, alevel);
				}
				if(i_z <= high) {
					acen = center + XYZ(halfsize, halfsize, halfsize);
					node->child111 = new TreeNode();
					create(node->child111, data, i_z, high, acen, halfsize, alevel);
				}
			}
		}
	}
}

void OcTree::draw()
{
	if(root) draw(root);
}

void OcTree::draw(const TreeNode *node)
{
	if(!node) return;
	if(!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111) {
		
		if(!node->isNull) {
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
	if(data.x < center.x - size || data.x > center.x + size) return 0;
	if(data.y < center.y - size || data.y > center.y + size) return 0;
	if(data.z < center.z - size || data.z > center.z + size) return 0;
	
	return 1;
}
//:~