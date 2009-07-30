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


OcTree::OcTree():root(0),num_voxel(0)
{
}

OcTree::~OcTree()
{
	release();
}

void OcTree::release()
{
	if(root) release(root);
	for(unsigned i=0; i<dSingle.size(); i++) delete[] dSingle[i]->data;
	for(unsigned i=0; i<dThree.size(); i++) delete[] dThree[i]->data;
	dSingle.clear();
	dThree.clear();
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

void OcTree::construct(PosAndId* data, const int num_data, const XYZ& center, const float size,short level)
{
	release();
	if(num_data == 0) return;
	root = new TreeNode();
	max_level = level;
	num_voxel = 0;
	create(root, data, 0, num_data-1, center, size, 0, num_voxel);
}

void OcTree::create(TreeNode *node, PosAndId* data, int low, int high, const XYZ& center, const float size, short level, unsigned &count)
{
	node->low = low;
	node->high = high;
	node->center = center;
	node->size = size;
	getMean(data, low, high, node->mean);
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

void OcTree::addSingle(const float *rawdata, const char* name, const PosAndId *index)
{
	NamedSingle* attr = new NamedSingle();
	attr->name = name;
	attr->data = new float[num_voxel];
	
	setSingle(attr->data, root, rawdata, index);
	
	dSingle.push_back(attr);
}

void OcTree::setSingle(float *res, TreeNode *node, const float *rawdata, const PosAndId *index)
{
	if(!node) return;
	else {
		float mean = 0.f;
		
		for(unsigned i = node->low; i<=node->high; i++) mean += rawdata[index[i].idx];
		
		mean /= float(node->high - node->low + 1);
		
		res[node->index - 1] = mean;
		
	    setSingle(res, node->child000, rawdata, index);
		setSingle(res, node->child001, rawdata, index);
		setSingle(res, node->child010, rawdata, index);
		setSingle(res, node->child011, rawdata, index);
		setSingle(res, node->child100, rawdata, index);
		setSingle(res, node->child101, rawdata, index);
		setSingle(res, node->child110, rawdata, index);
		setSingle(res, node->child111, rawdata, index);
	}
}

void OcTree::addThree(const XYZ *rawdata, const char* name, const PosAndId *index)
{
	if(num_voxel == 0) return;
	NamedThree* attr = new NamedThree();
	attr->name = name;
	attr->data = new XYZ[num_voxel];
	setThree(attr->data, root, rawdata, index);
	dThree.push_back(attr);
}

void OcTree::setThree(XYZ *res, TreeNode *node, const XYZ *rawdata, const PosAndId *index)
{
	if(!node) return;
	else {
		XYZ mean(0.f);
		
		for(unsigned i = node->low; i<=node->high; i++) mean += rawdata[index[i].idx];
		
		mean /= float(node->high - node->low + 1);
		
		res[node->index - 1] = mean;
		
	    setThree(res, node->child000, rawdata, index);
		setThree(res, node->child001, rawdata, index);
		setThree(res, node->child010, rawdata, index);
		setThree(res, node->child011, rawdata, index);
		setThree(res, node->child100, rawdata, index);
		setThree(res, node->child101, rawdata, index);
		setThree(res, node->child110, rawdata, index);
		setThree(res, node->child111, rawdata, index);
	}
}

void OcTree::save(const char *filename) const
{
	ofstream outfile;
	
	outfile.open(filename, ios_base::out | ios_base::binary );
	if(!outfile.is_open()) return;
	
	//outfile.write((char*)&sum,sizeof(sum));
	outfile.write((char*)&num_voxel,sizeof(unsigned));
	save(outfile, root);
	
	int datatype = 4;
	int datalen;
	for(unsigned i=0; i<dSingle.size(); i++) {
		outfile.write((char*)&datatype, 4);
		datalen = dSingle[i]->name.size();
		outfile.write((char*)&datalen, 4);
		outfile.write((char*)dSingle[i]->name.c_str(), datalen);
		outfile.write((char*)dSingle[i]->data, sizeof(float)*num_voxel);
	}
	datatype = 12;
	for(unsigned i=0; i<dThree.size(); i++) {
		outfile.write((char*)&datatype, 4);
		datalen = dThree[i]->name.size();
		outfile.write((char*)&datalen,4);
		outfile.write((char*)dThree[i]->name.c_str(), datalen);
		outfile.write((char*)dThree[i]->data, sizeof(XYZ)*num_voxel);
	}

	outfile.close();
}

void OcTree::save(ofstream& file, TreeNode *node) const
{
	if(!node) return;
	else
	{
		file.write((char*)&node->index,sizeof(int));
		file.write((char*)&node->low,sizeof(unsigned));
		file.write((char*)&node->high,sizeof(unsigned));
		file.write((char*)&node->center,sizeof(XYZ));
		file.write((char*)&node->mean,sizeof(XYZ));
		file.write((char*)&node->size,sizeof(float));
		file.write((char*)&node->child000,sizeof(node->child000));
	    file.write((char*)&node->child001,sizeof(node->child001));
	    file.write((char*)&node->child010,sizeof(node->child010));
	    file.write((char*)&node->child011,sizeof(node->child011));
	    file.write((char*)&node->child100,sizeof(node->child100));
	    file.write((char*)&node->child101,sizeof(node->child101));
	    file.write((char*)&node->child110,sizeof(node->child110));
	    file.write((char*)&node->child111,sizeof(node->child111));
		
		save(file, node->child000);
		save(file, node->child001);
		save(file, node->child010);
		save(file, node->child011);
		save(file, node->child100);
		save(file, node->child101);
		save(file, node->child110);
		save(file, node->child111);
	}
}

int OcTree::load(const char*filename)
{
	ifstream infile;
	infile.open(filename,ios_base::in | ios_base::binary );
	if(!infile.is_open()) return 0;
	
	infile.read((char*)&num_voxel, sizeof(unsigned));
	if(num_voxel>0) {   
		root = new TreeNode();
		load(infile, root);
		
		int datatype;
		int datalen;
		
		infile.read((char*)&datatype, 4);
		while(!infile.eof()) {
			if(datatype == 12) {;
				infile.read((char*)&datalen, 4);
				char *cn = new char[datalen];
				infile.read((char*)cn, datalen);
				NamedThree* attr = new NamedThree();
				for(int i = 0;i<datalen;i++)
					attr->name.append(1,cn[i]);
				attr->data = new XYZ[num_voxel];
				infile.read((char*)attr->data, 12*num_voxel);
				dThree.push_back(attr);
			}
			if(datatype == 4){
			    infile.read((char*)&datalen, 4);
				char *cn = new char[datalen];
                infile.read((char*)cn, datalen);
				NamedSingle* attr = new NamedSingle();
				for(int i = 0;i<datalen;i++)
					attr->name.append(1,cn[i]);
				attr->data = new float[num_voxel];
				infile.read((char*)attr->data, 4*num_voxel);
				dSingle.push_back(attr);
			}
			infile.read((char*)&datatype, 4);
		}
	}
	
	infile.close();
	return 1;
}

void OcTree::load(ifstream& file, TreeNode *node)
{   
	if(node == NULL) return;
	
	file.read((char*)&node->index,sizeof(int));
	file.read((char*)&node->low,sizeof(unsigned));
	file.read((char*)&node->high,sizeof(unsigned));
	file.read((char*)&node->center,sizeof(XYZ));
	file.read((char*)&node->mean,sizeof(XYZ));
	file.read((char*)&node->size,sizeof(float));
	file.read((char*)&node->child000,sizeof(node->child000));
	file.read((char*)&node->child001,sizeof(node->child001));
	file.read((char*)&node->child010,sizeof(node->child010));
	file.read((char*)&node->child011,sizeof(node->child011));
	file.read((char*)&node->child100,sizeof(node->child100));
	file.read((char*)&node->child101,sizeof(node->child101));
	file.read((char*)&node->child110,sizeof(node->child110));
	file.read((char*)&node->child111,sizeof(node->child111));
	
	if(node->child000) {
		node->child000 = new TreeNode();
		load(file, node->child000);
	}
	else node->child000 = NULL;
	
	if(node->child001) {
		node->child001 = new TreeNode();
		load(file, node->child001);
	}
	else node->child001 = NULL;
	
	if(node->child010) {
		node->child010 = new TreeNode();
		load(file, node->child010);
	}
	else node->child010 = NULL;
	
	if(node->child011) {
		node->child011 = new TreeNode();
		load(file, node->child011);
	}
	else node->child011 = NULL;
	
	if(node->child100) {
		node->child100 = new TreeNode();
		load(file, node->child100);
	}
	else node->child100 = NULL;
	
	if(node->child101) {
		node->child101 = new TreeNode();
		load(file, node->child101);
	}
	else node->child101 = NULL;
	
	if(node->child110) {
		node->child110 = new TreeNode();
		load(file, node->child110);
	}
	else node->child110 = NULL;
	
	if(node->child111) {
		node->child111 = new TreeNode();
		load(file, node->child111);
	}
	else node->child111 = NULL;
}

void OcTree::draw(const particleView *pview)
{
	if(root) draw(root, pview);
}

void OcTree::draw(const TreeNode *node, const particleView *pview)
{
	if(!node) return;
	if(!pview->needSplit(node->size,node->center) || (!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111)) {
			XYZ cen = node->center;
			float size = node->size;
			
			int i = hasColor();
			if(i>=0)
				glColor3f(dThree[i]->data[node->index -1].x, dThree[i]->data[node->index -1].y, dThree[i]->data[node->index -1].z);
			else 
				glColor3f(1,0,0);
			
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
		draw(node->child000,pview);
		draw(node->child001,pview);
		draw(node->child010,pview);
		draw(node->child011,pview);
		draw(node->child100,pview);
		draw(node->child101,pview);
		draw(node->child110,pview);
		draw(node->child111,pview);
	}
}

int OcTree::hasColor() const
{
	if(dThree.size()) 
		for(unsigned int i = 0;i<dThree.size();i++)
			if(dThree[i]->name=="color")
				return i;
	return -1;
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

void OcTree::getMean(const PosAndId *data, const int low, const int high, XYZ& center)
{
	center.x = center.y = center.z = 0.f;
	
	for(int i=low; i<=high; i++) center += data[i].pos;
	
	center /= float(high - low + 1);
}
/*
void OcTree::saveColor(const char*filename,XYZ *color,PosAndId *buf,unsigned sum)
{
	outfile.open(filename,ios_base::out | ios_base::binary | ios_base::app);
	if(!outfile.is_open())
		return;
	if(sum>0)
		saveColor(root,color,buf);

	outfile.close();
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

void OcTree::saveVelocity(const char*filename,XYZ *velocity,PosAndId *buf,unsigned sum)
{
	outfile.open(filename,ios_base::out | ios_base::binary | ios_base::app);
	if(!outfile.is_open())
		return;
	if(sum>0)
		saveVelocity(root,velocity,buf);

	outfile.close();
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


void OcTree::loadColor(const char*filename,XYZ* &voxelcolor)
{   
	if(num_voxel == 0)
		return;
	voxelcolor = new XYZ[num_voxel];
	infile.open(filename,ios_base::in | ios_base::binary );
	if(!infile.is_open())
		return;
	infile.seekg(72*num_voxel+8,ios_base::beg);
	for(unsigned int i = 0;i < num_voxel;i++)
		infile.read((char*)&voxelcolor[i],sizeof(XYZ));
	infile.close();
}

void OcTree::loadVelocity(const char*filename,XYZ* &voxelvelocity)
{
	if(num_voxel == 0)
		return;
	voxelvelocity = new XYZ[num_voxel];
	infile.open(filename,ios_base::in | ios_base::binary );
	if(!infile.is_open())
		return;
	infile.seekg(84*num_voxel+8,ios_base::beg);
	for(unsigned int i = 0;i < num_voxel;i++)
		infile.read((char*)&voxelvelocity[i],sizeof(XYZ));
	infile.close();
}
*/

void OcTree::searchNearVoxel(OcTree* tree,const XYZ position,int & treeindex)
{
	if(root == NULL) return;
	searchNearVoxel(root,position,treeindex);
}

void OcTree::searchNearVoxel(TreeNode *node,const XYZ position,int & treeindex)
{
	if(!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111)
	{
		treeindex = node->index ;
		return;}
	if(position.x <= node->center.x){
		if(position.y <= node->center.y)
			if(position.z <= node->center.z)
				searchNearVoxel(node->child000,position,treeindex);
			else 
				searchNearVoxel(node->child001,position,treeindex);
		else if(position.y > node->center.y)
			if(position.z <= node->center.z)
				searchNearVoxel(node->child010,position,treeindex);
			else
				searchNearVoxel(node->child011,position,treeindex);
	}
	else{
		if(position.y <= node->center.y)
			if(position.z <= node->center.z)
				searchNearVoxel(node->child100,position,treeindex);
			else 
				searchNearVoxel(node->child101,position,treeindex);
		else if(position.y > node->center.y)
			if(position.z <= node->center.z)
				searchNearVoxel(node->child110,position,treeindex);
			else
				searchNearVoxel(node->child111,position,treeindex);
	}
}
//:~