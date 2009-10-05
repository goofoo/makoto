#include "OcTree.h"

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

#include "../sh_lighting/SphericalHarmonics.h"
#include "gBase.h"
#include "CubeRaster.h"

OcTree::OcTree():root(0),num_voxel(0), m_pGrid(0), num_grid(0),m_pPower(0),idBuf(0),sample_opacity(0.04f)
{
}

OcTree::~OcTree()
{
	release();
}

void OcTree::release()
{
	if(idBuf) delete[] idBuf;
	if(m_pPower) delete[] m_pPower;
	if(root) release(root);
	for(unsigned i=0; i<dSingle.size(); i++) delete[] dSingle[i]->data;
	for(unsigned i=0; i<dThree.size(); i++) delete[] dThree[i]->data;
	dSingle.clear();
	dThree.clear();
}

void OcTree::release(OCTNode *node)
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

char OcTree::loadGrid(const char* filename)
{
	ifstream ffile;
	ffile.open(filename, ios::in | ios::binary | ios::ate);
	
	if(!ffile.is_open()) return 0;

	ifstream::pos_type size = ffile.tellg();
	
	num_grid = (int)size / sizeof(RGRID);
	
	m_pGrid = new RGRID[num_grid];
	
	ffile.seekg(0, ios::beg);
	
	ffile.read((char*)m_pGrid, sizeof(RGRID)*num_grid);
	
	ffile.close();
	return 1;
}

void OcTree::setGrid(RGRID* data, int n_data)
{
	num_grid = n_data;
	m_pGrid = data;
}

void OcTree::construct()
{
	idBuf = new unsigned[num_grid];
	float mean_r = 0;
	for(int i=0; i<num_grid; i++) {
		idBuf[i] = i;
		mean_r += m_pGrid[i].area;
	}
	mean_r /= num_grid;
	
	XYZ rootCenter;
	float rootSize;
    getBBox(m_pGrid, num_grid, rootCenter, rootSize);
	rootSize += mean_r;
	
	max_level = 0;
	
	float minboxsize = rootSize;
	while(minboxsize > mean_r*4.1) {
		max_level++;
		minboxsize /=2;
	}

	root = new OCTNode();
	root->parent = NULL;
	num_voxel = 0;
	create(root, 0, num_grid-1, rootCenter, rootSize, 0, num_voxel);
	
}

void OcTree::create(OCTNode *node, int low, int high, const XYZ& center, const float size, short level, unsigned &count)
{
	node->low = low;
	node->high = high;
	node->center = center;
	node->size = size;
	combineSurfel(m_pGrid, low, high, node->mean, node->col, node->dir, node->area);
	node->index = count;
	count++;
	
	if(level == max_level) {
		node->isLeaf = 1;
		return;
	}

	level++;
	float halfsize = size/2;
	XYZ acen;
	
	
	int xindex,yindex1,yindex2,zindex1,zindex2,zindex3,zindex4;	

		QuickSort::sortX(m_pGrid, idBuf, low,high);
		for( xindex = low; (m_pGrid[xindex].pos.x < center.x)&&(xindex<=high);xindex++ );

		QuickSort::sortY(m_pGrid, idBuf, low,xindex-1);
		for( yindex1 = low; (m_pGrid[yindex1].pos.y < center.y)&&(yindex1<=xindex-1);yindex1++ );

		QuickSort::sortY(m_pGrid, idBuf, xindex,high);
		for( yindex2 = xindex; (m_pGrid[yindex2].pos.y < center.y)&&(yindex2<=high);yindex2++ );
       
		QuickSort::sortZ(m_pGrid, idBuf, low,yindex1-1);
		for( zindex1 = low; (m_pGrid[zindex1].pos.z < center.z)&&(zindex1<=yindex1-1);zindex1++ );

		QuickSort::sortZ(m_pGrid, idBuf, yindex1,xindex-1);
		for( zindex2 = yindex1; (m_pGrid[zindex2].pos.z < center.z)&&(zindex2<=xindex-1);zindex2++ );

		QuickSort::sortZ(m_pGrid, idBuf, xindex,yindex2-1);
		for( zindex3 = xindex; (m_pGrid[zindex3].pos.z < center.z)&&(zindex3<=yindex2-1);zindex3++ );

		QuickSort::sortZ(m_pGrid, idBuf, yindex2,high);
		for( zindex4 = yindex2; (m_pGrid[zindex4].pos.z < center.z)&&(zindex4<=high);zindex4++ );
	
		if(low <= zindex1-1)
		{
			node->child000 = new OCTNode(); node->child000->parent = node;
			acen = center + XYZ(-halfsize, -halfsize, -halfsize);
			create(node->child000, low,zindex1-1,acen,halfsize,level, count);
		}
		else node->child000 = NULL;

		if(zindex1 <= yindex1-1)
		{
			node->child001 = new OCTNode(); node->child001->parent = node;
			acen = center + XYZ(-halfsize, -halfsize, halfsize);
			create(node->child001, zindex1,yindex1-1,acen,halfsize,level, count);
		}
		else node->child001 = NULL;

		if(yindex1<=zindex2-1)
		{
			node->child010 = new OCTNode(); node->child010->parent = node;
			acen = center + XYZ(-halfsize, halfsize, -halfsize);
			create(node->child010, yindex1,zindex2-1,acen,halfsize,level, count);
		}
		else node->child010 = NULL;

		if(zindex2<=xindex-1)
		{
			node->child011 = new OCTNode(); node->child011->parent = node;
			acen = center + XYZ(-halfsize, halfsize, halfsize);
			create(node->child011, zindex2,xindex-1,acen,halfsize,level, count);
		}
		else node->child011 = NULL;

		if(xindex<=zindex3-1)
		{
			node->child100 = new OCTNode(); node->child100->parent = node;
			acen = center + XYZ(halfsize, -halfsize, -halfsize);
			create(node->child100, xindex,zindex3-1,acen,halfsize,level, count);
		}
		else node->child100 = NULL;

		if(zindex3<=yindex2-1)
		{
			node->child101 = new OCTNode(); node->child101->parent = node;
			acen = center + XYZ(halfsize, -halfsize, halfsize);
			create(node->child101, zindex3,yindex2-1,acen,halfsize,level, count);
		}
		else node->child101 = NULL;

		if(yindex2<=zindex4-1)
		{
			node->child110 = new OCTNode(); node->child110->parent = node;
			acen = center + XYZ(halfsize, halfsize, -halfsize);
			create(node->child110, yindex2,zindex4-1,acen,halfsize,level, count);
		}
		else node->child110 = NULL;

		if(zindex4<=high)
		{
			node->child111 = new OCTNode(); node->child111->parent = node;
			acen = center + XYZ(halfsize, halfsize, halfsize);
			create(node->child111, zindex4,high,acen,halfsize,level, count);
		}
		else node->child111 = NULL;

}

void OcTree::computePower(sphericalHarmonics* _sh)
{
	m_pPower = new SHB3COEFF[num_voxel];
	sh = _sh;
	computePower(root);
}

void OcTree::computePower(OCTNode *node)
{
	if(!node) return;
	sh->zeroCoeff(m_pPower[node->index].value);
	if(node->isLeaf) {
		float H;
		XYZ one(1.f);
		for(unsigned j=0; j<SH_N_SAMPLES; j++) {
			SHSample s = sh->getSample(j);
			for(unsigned i=node->low; i<= node->high; i++) {
				H = m_pGrid[i].nor.dot(s.vector)*m_pGrid[i].area;
				if(H>0) sh->projectASample(m_pPower[node->index].value, j, one*H);
			}
		}
	}
	else {
		computePower(node->child000);
		computePower(node->child001);
		computePower(node->child010);
		computePower(node->child011);
		computePower(node->child100);
		computePower(node->child101);
		computePower(node->child110);
		computePower(node->child111);
		
		if(node->child000) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child000->index].value);
		if(node->child001) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child001->index].value);
		if(node->child010) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child010->index].value);
		if(node->child011) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child011->index].value);
		if(node->child100) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child100->index].value);
		if(node->child101) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child101->index].value);
		if(node->child110) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child110->index].value);
		if(node->child111) sh->addCoeff(m_pPower[node->index].value, m_pPower[node->child111->index].value);
	}
}

void OcTree::doOcclusion(SHB3COEFF* res) const
{

}

void OcTree::addSingle(const float *rawdata, const char* name, const PosAndId *index)
{
	NamedSingle* attr = new NamedSingle();
	attr->name = name;
	attr->data = new float[num_voxel];
	
	setSingle(attr->data, root, rawdata, index);
	
	dSingle.push_back(attr);
}

void OcTree::setSingle(float *res, OCTNode *node, const float *rawdata, const PosAndId *index)
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

void OcTree::setThree(XYZ *res, OCTNode *node, const XYZ *rawdata, const PosAndId *index)
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

void OcTree::save(ofstream& file) const
{
	file.write((char*)&num_voxel,sizeof(unsigned));
	save(file, root);
	/*
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
*/
	
}

void OcTree::save(ofstream& file, OCTNode *node) const
{
	if(!node) return;
	file.write((char*)&node->isLeaf,sizeof(char));
	file.write((char*)&node->index,sizeof(int));
	file.write((char*)&node->low,sizeof(unsigned));
	file.write((char*)&node->high,sizeof(unsigned));
	file.write((char*)&node->center,sizeof(XYZ));
	file.write((char*)&node->mean,sizeof(XYZ));
	file.write((char*)&node->size,sizeof(float));
	file.write((char*)&node->area,sizeof(float));
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

char OcTree::load(ifstream& file)
{
	file.read((char*)&num_voxel, sizeof(unsigned));
	if(num_voxel>0) {   
		root = new OCTNode();
		root->parent = NULL;
		num_voxel = 0;
		load(file, root);
		/*
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
		*/
	}

	return 1;
}

void OcTree::load(ifstream& file, OCTNode *node)
{   
	num_voxel++;
	file.read((char*)&node->isLeaf,sizeof(char));
	file.read((char*)&node->index,sizeof(int));
	file.read((char*)&node->low,sizeof(unsigned));
	file.read((char*)&node->high,sizeof(unsigned));
	file.read((char*)&node->center,sizeof(XYZ));
	file.read((char*)&node->mean,sizeof(XYZ));
	file.read((char*)&node->size,sizeof(float));
	file.read((char*)&node->area,sizeof(float));
	file.read((char*)&node->child000,sizeof(node->child000));
	file.read((char*)&node->child001,sizeof(node->child001));
	file.read((char*)&node->child010,sizeof(node->child010));
	file.read((char*)&node->child011,sizeof(node->child011));
	file.read((char*)&node->child100,sizeof(node->child100));
	file.read((char*)&node->child101,sizeof(node->child101));
	file.read((char*)&node->child110,sizeof(node->child110));
	file.read((char*)&node->child111,sizeof(node->child111));
	
	if(node->child000) {
		node->child000 = new OCTNode(); node->child000->parent = node;
		load(file, node->child000);
	}
	else node->child000 = NULL;
	
	if(node->child001) {
		node->child001 = new OCTNode(); node->child001->parent = node;
		load(file, node->child001);
	}
	else node->child001 = NULL;
	
	if(node->child010) {
		node->child010 = new OCTNode(); node->child010->parent = node;
		load(file, node->child010);
	}
	else node->child010 = NULL;
	
	if(node->child011) {
		node->child011 = new OCTNode(); node->child011->parent = node;
		load(file, node->child011);
	}
	else node->child011 = NULL;
	
	if(node->child100) {
		node->child100 = new OCTNode(); node->child100->parent = node;
		load(file, node->child100);
	}
	else node->child100 = NULL;
	
	if(node->child101) {
		node->child101 = new OCTNode(); node->child101->parent = node;
		load(file, node->child101);
	}
	else node->child101 = NULL;
	
	if(node->child110) {
		node->child110 = new OCTNode(); node->child110->parent = node;
		load(file, node->child110);
	}
	else node->child110 = NULL;
	
	if(node->child111) {
		node->child111 = new OCTNode(); node->child111->parent = node;
		load(file, node->child111);
	}
	else node->child111 = NULL;
}

void OcTree::draw()
{
	drawSurfel(root);
}

void OcTree::drawCube(const OCTNode *node)
{
	if(!node) return;
	if(node->isLeaf) {
		XYZ cen = node->center;
		float size = node->size;
		//glColor3f(node->col.x, node->col.y, node->col.z);
	
		gBase::drawBox(cen, size);
	}
	else {
		drawCube(node->child000 );
		drawCube(node->child001 );
		drawCube(node->child010 );
		drawCube(node->child011 );
		drawCube(node->child100 );
		drawCube(node->child101 );
		drawCube(node->child110 );
		drawCube(node->child111 );
	}
}

void OcTree::drawSurfel(const OCTNode *node)
{
	if(!node) return;
	float r = sqrt(node->area/3.14);
			//gBase::drawSplatAt(node->mean, node->col, r);
	if(node->isLeaf) {
		//glColor3f(node->col.x, node->col.y, node->col.z);
		//float r = sqrt(node->area/3.14);
		//gBase::drawSplatAt(node->mean, node->dir, r);
		for(unsigned i= node->low; i<= node->high; i++) {
			r = sqrt(m_pGrid[i].area/3.14);
			gBase::drawSplatAt(m_pGrid[i].pos, m_pGrid[i].col, r);
			//glVertex3f(origin.x, origin.y, origin.z);
			//glVertex3f(m_pGrid[i].pos.x, m_pGrid[i].pos.y, m_pGrid[i].pos.z);
		}
	}
	else {
		drawSurfel(node->child000 );
		drawSurfel(node->child001 );
		drawSurfel(node->child010 );
		drawSurfel(node->child011 );
		drawSurfel(node->child100 );
		drawSurfel(node->child101 );
		drawSurfel(node->child110 );
		drawSurfel(node->child111 );
	}
}
/*
void OcTree::draw(const PerspectiveView *pview,XYZ facing,string drawType)
{
	if(root) 
	{
		if(drawType == "cube")
			drawCube(root, pview);
		if(drawType == "disk")
		{
			int index = 0;
			DataAndId* drawList = new DataAndId[num_voxel];
			getDrawList(root, pview,index,drawList);
			QuickSort::sortDeep(drawList,0,index-1);
			int co = hasColor();
			for(int i = 0;i<index-1;i++)
			{
				if(co>=0)
					glColor4f(dThree[co]->data[drawList[i].idx].x, dThree[co]->data[drawList[i].idx].y, dThree[co]->data[drawList[i].idx].z, 0.2f);
				else 
				glColor4f(0.05f, 0.6f, 0.2f, 0.2f);
				gBase::drawSplatAt(drawList[i].pos,facing,drawList[i].size);
			}
			delete[] drawList;
		}
	}
}

void OcTree::getDrawList(const OCTNode *node, const PerspectiveView *pview,int &index,DataAndId* drawList)
{
	if(!node) return;
	if(!pview->needSplit(node->size,node->center) || (!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111)) 
	{
		
		XYZ cen = node->center;
		//float size = node->size;
		drawList[index].pos = cen;
		drawList[index].idx = node->index;
		drawList[index].val =pview->deep(cen);
		drawList[index].size = node->size;

		index ++;        
		//glColor4f(0.05f, 0.6f, 0.2f, 0.2f);
		//gBase::drawSplatAt(cen,facing,size);
	}
	else {
		getDrawList(node->child000,pview,index,drawList);
		getDrawList(node->child001,pview,index,drawList);
		getDrawList(node->child010,pview,index,drawList);
		getDrawList(node->child011,pview,index,drawList);
		getDrawList(node->child100,pview,index,drawList);
		getDrawList(node->child101,pview,index,drawList);
		getDrawList(node->child110,pview,index,drawList);
		getDrawList(node->child111,pview,index,drawList);
	}
}

void OcTree::drawCube(const OCTNode *node, const PerspectiveView *pview)
{
	if(!node) return;
	int i = hasColor();
	if(!pview->needSplit(node->size,node->center) || (!node->child000 && !node->child001 && !node->child010 && !node->child011 && !node->child100 && !node->child101 && !node->child110 && !node->child111)) {
			XYZ cen = node->center;
			float size = node->size;
			glBegin(GL_QUADS);
			if(i>=0)
				glColor4f(dThree[i]->data[node->index -1].x, dThree[i]->data[node->index -1].y, dThree[i]->data[node->index -1].z,0.2f);
			else 
				glColor4f(0.05f, 0.6f, 0.2f, 0.2f);
	
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
			glEnd();	
	}
	else {
		drawCube(node->child000,pview);
		drawCube(node->child001,pview);
		drawCube(node->child010,pview);
		drawCube(node->child011,pview);
		drawCube(node->child100,pview);
		drawCube(node->child101,pview);
		drawCube(node->child110,pview);
		drawCube(node->child111,pview);
	}
}
*/
int OcTree::hasColor() const
{
	if(dThree.size()) 
		for(unsigned int i = 0;i<dThree.size();i++)
			if(dThree[i]->name=="color" || dThree[i]->name=="Cs")
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

void OcTree::getBBox(const RGRID* data, const int num_data, XYZ& center, float& size)
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

void OcTree::combineSurfel(const RGRID *data, const int low, const int high, XYZ& center, XYZ& color, XYZ& dir, float& area) const
{
	center.x = center.y = center.z = 0.f;
	color.x = color.y = color.z = 0.f;
	dir.x = dir.y = dir.z = 0.f;
	area = 0.f;
	
	for(int i=low; i<=high; i++) {
		center += data[i].pos;
		color += data[i].col;
		dir += data[i].nor;
		area += data[i].area;
	}
	
	center /= float(high - low + 1);
	color /= float(high - low + 1);
	dir.normalize();
	
	if(high == low) return;
	
	float maxdist=0, dist;
	XYZ tomean;
	for(int i=low; i<=high; i++) {
		tomean = data[i].pos - center;
		dist = tomean.length();
		if(dist > maxdist) maxdist = dist;
	}
	dist = maxdist * maxdist *4;
	if(dist> area) area = dist;
}

void OcTree::nearestGrid(const XYZ& to, float min, float max, float& dist)
{
	nearestGrid(root, to, min, max, dist);
}

void OcTree::nearestGrid(OCTNode *node, const XYZ& to, float min, float max, float& dist)
{
	if(!node) return;
	
	float delta;
	delta = node->center.x - to.x;
	if(delta < 0) delta *= -1;
	delta -= node->size;
	if(delta > max) return;
	
	delta = node->center.y - to.y;
	if(delta < 0) delta *= -1;
	delta -= node->size;
	if(delta > max) return;
	
	delta = node->center.z - to.z;
	if(delta < 0) delta *= -1;
	delta -= node->size;
	if(delta > max) return;
	
	if(node->isLeaf) {
		
		for(unsigned i= node->low; i<= node->high; i++) {
			XYZ disp = m_pGrid[i].pos - to;
			delta = disp.length();
			if(delta < dist && delta > min) dist = delta;
		}
	}
	else {
		nearestGrid(node->child000, to, min, max, dist );
		nearestGrid(node->child001, to, min, max, dist);
		nearestGrid(node->child010, to, min, max, dist);
		nearestGrid(node->child011, to, min, max, dist);
		nearestGrid(node->child100, to, min, max, dist);
		nearestGrid(node->child101, to, min, max, dist);
		nearestGrid(node->child110, to, min, max, dist);
		nearestGrid(node->child111, to, min, max, dist);
	}
}

void OcTree::occlusionAccum(const XYZ& origin, CubeRaster* raster)
{
	occlusionAccum(root, origin, raster);
}

void OcTree::occlusionAccum(OCTNode *node, const XYZ& origin, CubeRaster* raster)
{
	if(!node) return;
	XYZ ray;
	if(node->isLeaf) {
		for(unsigned i= node->low; i<= node->high; i++) {
			ray = m_pGrid[i].pos - origin;
			raster->write(ray, sample_opacity);
		}
	}
	else {
		ray = node->mean - origin;
		float distance = ray.length() + 0.0001;
		float solidangle = node->area/distance/distance;
		if(solidangle < 0.09) {
			raster->write(ray, sample_opacity);
			return;
		}
		
		occlusionAccum(node->child000, origin, raster);
		occlusionAccum(node->child001, origin, raster);
		occlusionAccum(node->child010, origin, raster);
		occlusionAccum(node->child011, origin, raster);
		occlusionAccum(node->child100, origin, raster);
		occlusionAccum(node->child101, origin, raster);
		occlusionAccum(node->child110, origin, raster);
		occlusionAccum(node->child111, origin, raster);
	}
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

void OcTree::saveColor(OCTNode *node,XYZ *color,PosAndId *buf)
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

void OcTree::saveVelocity(OCTNode *node,XYZ *velocity,PosAndId *buf)
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


void OcTree::searchNearVoxel(OcTree* tree,const XYZ position,int & treeindex)
{
	if(root == NULL) return;
	searchNearVoxel(root,position,treeindex);
}

void OcTree::searchNearVoxel(OCTNode *node,const XYZ position,int & treeindex)
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

void OcTree::drawGrid()
{
	float r;
	for(unsigned i=0; i<num_grid; i++) {
		r = sqrt(m_pGrid[i].area/3.14);
		gBase::drawSplatAt(m_pGrid[i].pos, m_pGrid[i].nor, r);
	}
}
*/
//:~