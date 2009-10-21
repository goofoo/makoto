#include "OcTree.h"

#include "../shared/gBase.h"
#include "CubeRaster.h"
#include "../shared/QuickSort.h"

const float constantCoeff[9] = { 3.543211, 
								0.000605, 0.000152, -0.003217, 
								0.000083, -0.002813, -0.000021, -0.001049, 0.000144};

OcTree::OcTree():root(0),num_voxel(0), m_pGrid(0), num_grid(0),idBuf(0),sample_opacity(0.04f),m_hasHdr(0)
{
}

OcTree::~OcTree()
{
	release();
}

void OcTree::release()
{
	if(idBuf) delete[] idBuf;
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

void OcTree::orderGridData(unsigned* data, int n_data)
{
	unsigned* tmp = new unsigned[n_data];
	for(int i=0; i< n_data; i++) tmp[i] = data[i];
	for(int i=0; i< n_data; i++) data[i] = tmp[idBuf[i]];
	delete[] tmp;
}

void OcTree::construct(const XYZ& rootCenter, float rootSize, int maxLevel)
{
	idBuf = new unsigned[num_grid];
	for(unsigned i=0; i< num_grid; i++) idBuf[i] = i;
	
	max_level = maxLevel;

	root = new OCTNode();
	//root->parent = NULL;
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
// if surfel is smaller use surfel
	float nodearea = size*size*4;
	if(nodearea < node->area) node->area = nodearea;
	
	node->index = count;
/*	
	float noi0 = float(rand()%111)/111.f;
	float noi1 = float(rand()%91)/91.f;
	float noi2 = float(rand()%131)/131.f;
	node->col = XYZ(noi0, noi1, noi2);
*/
	count++;
	
	if(level == max_level) {
		node->isLeaf = 1;
		return;
	}

	level++;
	float halfsize = size/2;
	XYZ acen;
	
	node->child000 = node->child001 = node->child010 = node->child011 = node->child100 = node->child101 = node->child110 = node->child111 = NULL;
	int midx, yindex1, yindex2,zindex1,zindex2,zindex3,zindex4;	

	QuickSort::sortX(m_pGrid, idBuf, low,high);
	for( midx = low; (m_pGrid[midx].pos.x < center.x)&&(midx<=high);midx++ );

		QuickSort::sortY(m_pGrid, idBuf, low,midx-1);
		for( yindex1 = low; (m_pGrid[yindex1].pos.y < center.y)&&(yindex1<= midx-1);yindex1++ );

		QuickSort::sortY(m_pGrid, idBuf, midx,high);
		for( yindex2 = midx; (m_pGrid[yindex2].pos.y < center.y)&&(yindex2<=high);yindex2++ );
     
		QuickSort::sortZ(m_pGrid, idBuf, low,yindex1-1);
		for( zindex1 = low; (m_pGrid[zindex1].pos.z < center.z)&&(zindex1<=yindex1-1);zindex1++ );

		QuickSort::sortZ(m_pGrid, idBuf, yindex1, midx-1);
		for( zindex2 = yindex1; (m_pGrid[zindex2].pos.z < center.z)&&(zindex2<= midx-1);zindex2++ );

		QuickSort::sortZ(m_pGrid, idBuf, midx,yindex2-1);
		for( zindex3 = midx; (m_pGrid[zindex3].pos.z < center.z)&&(zindex3<=yindex2-1);zindex3++ );

		QuickSort::sortZ(m_pGrid, idBuf, yindex2,high);
		for( zindex4 = yindex2; (m_pGrid[zindex4].pos.z < center.z)&&(zindex4<=high);zindex4++ );
	
		if(low <= zindex1-1) {
			node->child000 = new OCTNode();//node->child000->parent = node;
			acen = center + XYZ(-halfsize, -halfsize, -halfsize);
			create(node->child000, low, zindex1-1,acen,halfsize,level, count);
		}
		

		if(zindex1 <= yindex1-1) {
			node->child001 = new OCTNode();//node->child001->parent = node;
			acen = center + XYZ(-halfsize, -halfsize, halfsize);
			create(node->child001, zindex1,yindex1-1,acen,halfsize,level, count);
		}
		

		if(yindex1<=zindex2-1) {
			node->child010 = new OCTNode();//node->child010->parent = node;
			acen = center + XYZ(-halfsize, halfsize, -halfsize);
			create(node->child010, yindex1,zindex2-1,acen,halfsize,level, count);
		}

		if(zindex2<=midx-1) {
			node->child011 = new OCTNode();//node->child011->parent = node;
			acen = center + XYZ(-halfsize, halfsize, halfsize);
			create(node->child011, zindex2, midx-1,acen,halfsize,level, count);
		}

		if(midx<=zindex3-1) {
			node->child100 = new OCTNode();//node->child100->parent = node;
			acen = center + XYZ(halfsize, -halfsize, -halfsize);
			create(node->child100, midx,zindex3-1,acen,halfsize,level, count);
		}

		if(zindex3<=yindex2-1) {
			node->child101 = new OCTNode();//node->child101->parent = node;
			acen = center + XYZ(halfsize, -halfsize, halfsize);
			create(node->child101, zindex3,yindex2-1,acen,halfsize,level, count);
		}

		if(yindex2<=zindex4-1) {
			node->child110 = new OCTNode();//node->child110->parent = node;
			acen = center + XYZ(halfsize, halfsize, -halfsize);
			create(node->child110, yindex2,zindex4-1,acen,halfsize,level, count);
		}

		if(zindex4<=high) {
			node->child111 = new OCTNode();//node->child111->parent = node;
			acen = center + XYZ(halfsize, halfsize, halfsize);
			create(node->child111, zindex4,high,acen,halfsize,level, count);
		}
}
/*
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
*/
//void OcTree::doOcclusion(SHB3COEFF* res) const
//{

//}
/*
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
*/
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
	file.write((char*)&node->col,sizeof(XYZ));
	file.write((char*)&node->dir,sizeof(XYZ));
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
		//root->parent = NULL;
		num_voxel = num_leaf = 0;
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
	file.read((char*)&node->col,sizeof(XYZ));
	file.read((char*)&node->dir,sizeof(XYZ));
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
	if(node->isLeaf) num_leaf++;
	
	if(node->child000) {
		node->child000 = new OCTNode();//node->child000->parent = node;
		load(file, node->child000);
	}
	else node->child000 = NULL;
	
	if(node->child001) {
		node->child001 = new OCTNode();//node->child001->parent = node;
		load(file, node->child001);
	}
	else node->child001 = NULL;
	
	if(node->child010) {
		node->child010 = new OCTNode();//node->child010->parent = node;
		load(file, node->child010);
	}
	else node->child010 = NULL;
	
	if(node->child011) {
		node->child011 = new OCTNode();//node->child011->parent = node;
		load(file, node->child011);
	}
	else node->child011 = NULL;
	
	if(node->child100) {
		node->child100 = new OCTNode();//node->child100->parent = node;
		load(file, node->child100);
	}
	else node->child100 = NULL;
	
	if(node->child101) {
		node->child101 = new OCTNode();//node->child101->parent = node;
		load(file, node->child101);
	}
	else node->child101 = NULL;
	
	if(node->child110) {
		node->child110 = new OCTNode();//node->child110->parent = node;
		load(file, node->child110);
	}
	else node->child110 = NULL;
	
	if(node->child111) {
		node->child111 = new OCTNode();//node->child111->parent = node;
		load(file, node->child111);
	}
	else node->child111 = NULL;
}

void OcTree::draw()
{
	drawCube(root);
}

void OcTree::draw(const XYZ& viewdir)
{
	drawSurfel(root, viewdir);
	//glBegin(GL_LINES);
	//drawNeighbour(root);
	//glEnd();
}

void OcTree::drawNeighbour(const OCTNode *node)
{
	if(!node) return;
	if(node->isLeaf) {
		for(unsigned i= node->low; i<= node->high; i++) {
			glColor3f(1,0,0);
			glVertex3f(m_pGrid[i].pos.x, m_pGrid[i].pos.y, m_pGrid[i].pos.z);
			
			XYZ vneib(0,1,0);
			float dist = 16.1f;
			
			glColor3f(0,1,0);
			nearestGrid(m_pGrid[i].pos, 0.1, dist, vneib);
			glVertex3f(vneib.x, vneib.y, vneib.z);
		}
	}
	else {
		drawNeighbour(node->child000 );
		drawNeighbour(node->child001 );
		drawNeighbour(node->child010 );
		drawNeighbour(node->child011 );
		drawNeighbour(node->child100 );
		drawNeighbour(node->child101 );
		drawNeighbour(node->child110 );
		drawNeighbour(node->child111 );
	}
}

void OcTree::drawCube(const OCTNode *node)
{
	if(!node) return;
	XYZ cen = node->center;
	float size2 = node->size*2;
	
	XYZ pcam = cen;
	f_cameraSpaceInv.transform(pcam);
	
	if(pcam.z + size2 < 0) return;

	float depthz = pcam.z - node->size;
	if(depthz < 0.01) depthz = 0.01;
	
	float portWidth;
	if(f_isPersp) portWidth = depthz*f_fieldOfView;
	else portWidth = f_fieldOfView;
	
	if(pcam.x - size2 > portWidth) return;
	if(pcam.x + size2 < -portWidth) return;
	if(pcam.y - size2 > portWidth) return;
	if(pcam.y + size2 < -portWidth) return;
	
	int detail = node->size/portWidth*1024;
	
	if(detail < 8 || node->isLeaf) {
		if(m_pSHBuf) {
			if(m_hasHdr) {
				XYZ inc(0);
				for(int i = 0; i < SH_N_BASES; i++) {
					inc.x += m_pSHBuf[node->index].value[i].x*m_hdrCoe[i].x;
					inc.y += m_pSHBuf[node->index].value[i].y*m_hdrCoe[i].y;
					inc.z += m_pSHBuf[node->index].value[i].z*m_hdrCoe[i].z;
				}
				glColor3f(inc.x, inc.y, inc.z);
			}
			else {
				float ov  = 0;
				for(int i = 0; i < SH_N_BASES; i++) ov += m_pSHBuf[node->index].value[i].x*sh->constantCoeff[i].x;
				ov /= 3.14;
				glColor3f(ov, ov, ov);
			}
		}
		gBase::drawBox(cen, node->size);
		return;
	}
	
	drawCube(node->child000 );
	drawCube(node->child001 );
	drawCube(node->child010 );
	drawCube(node->child011 );
	drawCube(node->child100 );
	drawCube(node->child101 );
	drawCube(node->child110 );
	drawCube(node->child111 );
}

void OcTree::drawSurfel(const OCTNode *node, const XYZ& viewdir)
{
	if(!node) return;
	XYZ cen = node->center;
	float size2 = node->size*2;
	
	XYZ pcam = cen;
	f_cameraSpaceInv.transform(pcam);
	
	if(pcam.z + size2 < 0) return;
	
	float depthz = pcam.z - node->size;
	if(depthz < 0.01) depthz = 0.01;
	
	float portWidth;
	if(f_isPersp) portWidth = depthz*f_fieldOfView;
	else portWidth = f_fieldOfView;
	
	if(pcam.x - size2 > portWidth) return;
	if(pcam.x + size2 < -portWidth) return;
	if(pcam.y - size2 > portWidth) return;
	if(pcam.y + size2 < -portWidth) return;
	
	// sum of grid and biggest one
	float sumarea =0;
	unsigned ibig = node->low;
	float fbig = m_pGrid[ibig].area;
	for(unsigned i= node->low; i<= node->high; i++) {
		sumarea += m_pGrid[i].area;
		if(m_pGrid[i].area > fbig) {
			ibig = i;
			fbig = m_pGrid[i].area;
		}
	}

	float r = sqrt(sumarea * 0.25);
	
	int detail = r/portWidth*1024;
	
	if(detail < 8) {
		if(m_pSHBuf) {
			if(m_hasHdr) {
				XYZ inc(0);
				for(int i = 0; i < SH_N_BASES; i++) {
					inc.x += m_pSHBuf[node->index].value[i].x*m_hdrCoe[i].x;
					inc.y += m_pSHBuf[node->index].value[i].y*m_hdrCoe[i].y;
					inc.z += m_pSHBuf[node->index].value[i].z*m_hdrCoe[i].z;
				}
				glColor3f(inc.x, inc.y, inc.z);
			}
			else {
				float ov  = 0;
				for(int i = 0; i < SH_N_BASES; i++) ov += m_pSHBuf[node->index].value[i].x*sh->constantCoeff[i].x;
				ov /= 3.14;
				glColor3f(ov, ov, ov);
			}
		}

		gBase::drawSplatAt(node->mean, viewdir, r);

		return;
	}

	if(node->isLeaf) {
		if(m_pSHBuf) {
			if(m_hasHdr) {
				XYZ inc(0);
				for(int i = 0; i < SH_N_BASES; i++) {
					inc.x += m_pSHBuf[node->index].value[i].x*m_hdrCoe[i].x;
					inc.y += m_pSHBuf[node->index].value[i].y*m_hdrCoe[i].y;
					inc.z += m_pSHBuf[node->index].value[i].z*m_hdrCoe[i].z;
				}
				glColor3f(inc.x, inc.y, inc.z);
			}
			else {
				float ov  = 0;
				for(int i = 0; i < SH_N_BASES; i++) ov += m_pSHBuf[node->index].value[i].x*sh->constantCoeff[i].x;
				ov /= 3.14;
				glColor3f(ov, ov, ov);
			}
		}
		for(unsigned i= node->low; i<= node->high; i++) {
			r = sqrt(m_pGrid[i].area * 0.25);
			gBase::drawSplatAt(m_pGrid[i].pos, viewdir, r);
			//glVertex3f(origin.x, origin.y, origin.z);
			//glVertex3f(m_pGrid[i].pos.x, m_pGrid[i].pos.y, m_pGrid[i].pos.z);
		}
	}
	else {
		drawSurfel(node->child000, viewdir);
		drawSurfel(node->child001, viewdir);
		drawSurfel(node->child010, viewdir);
		drawSurfel(node->child011, viewdir);
		drawSurfel(node->child100, viewdir);
		drawSurfel(node->child101, viewdir);
		drawSurfel(node->child110, viewdir);
		drawSurfel(node->child111, viewdir);
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

int OcTree::hasColor() const
{
	if(dThree.size()) 
		for(unsigned int i = 0;i<dThree.size();i++)
			if(dThree[i]->name=="color" || dThree[i]->name=="Cs")
				return i;
	return -1;
}
*/
void OcTree::getRootCenterNSize(XYZ& center, float&size) const
{
	if(root) {
		center = root->center;
		size = root->size;
	}
}
/*
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
*/
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
	dir/= float(high - low + 1);
	
	if(high == low) return;
	
	float maxdist=0, dist;
	XYZ tomean;
	for(int i=low; i<=high; i++) {
		tomean = data[i].pos - center;
		dist = tomean.length();
		if(dist > maxdist) maxdist = dist;
	}
	dist = maxdist * maxdist *4;
	area = dist + data[low].area;
}

void OcTree::nearestGrid(const XYZ& to, float min, float& dist, XYZ& res)
{
	nearestGrid(root, to, min, dist, res);
}

void OcTree::nearestGrid(OCTNode *node, const XYZ& to, float min, float& dist, XYZ& res)
{
	if(!node) return;
	
	float delta;
	delta = node->center.x - to.x;
	if(delta < 0) delta = -delta;
	delta -= node->size;
	if(delta > dist) return;
	
	delta = node->center.y - to.y;
	if(delta < 0) delta = -delta;
	delta -= node->size;
	if(delta > dist) return;
	
	delta = node->center.z - to.z;
	if(delta < 0) delta = -delta;
	delta -= node->size;
	if(delta > dist) return;
	
	if(node->isLeaf) {
		
		for(unsigned i= node->low; i<= node->high; i++) {
			XYZ disp = m_pGrid[i].pos - to;
			delta = disp.length();
			if(delta < dist && delta > min) {
				dist = delta;
				res = m_pGrid[i].pos;
			}
		}
	}
	else {
		nearestGrid(node->child000, to, min, dist, res);
		nearestGrid(node->child001, to, min, dist, res);
		nearestGrid(node->child010, to, min, dist, res);
		nearestGrid(node->child011, to, min, dist, res);
		nearestGrid(node->child100, to, min, dist, res);
		nearestGrid(node->child101, to, min, dist, res);
		nearestGrid(node->child110, to, min, dist, res);
		nearestGrid(node->child111, to, min, dist, res);
	}
}

void OcTree::occlusionAccum(const XYZ& origin)
{
	occlusionAccum(root, origin);
}

void OcTree::occlusionAccum(OCTNode *node, const XYZ& origin)
{
	if(!node) return;
	XYZ ray;
	float solidangle;
	ray = node->mean - origin;
	solidangle = node->area/(ray.lengthSquare() + 0.00001);
	if(solidangle < CUBERASTER_MAXANGLE) {
		raster->write(ray, sample_opacity*(node->high-node->low+1));
		return;
	}
	if(node->isLeaf) {
		for(unsigned i= node->low; i<= node->high; i++) {
			ray = m_pGrid[i].pos - origin;
			if(ray.lengthSquare() < node->area) return;
			
			solidangle = m_pGrid[i].area/(ray.lengthSquare() + 0.00001);
			
			int nsamp = 1+solidangle*CUBERASTER_MAXANGLEINV;
			nsamp *= nsamp;
			float r = sqrt(m_pGrid[i].area);
			XYZ vnoi;
			for(int j=0; j<nsamp; j++) {
				vnoi.x = float(random()%197)/197.f - 0.5;
				vnoi.y = float(random()%203)/203.f - 0.5;
				vnoi.z = float(random()%337)/337.f - 0.5;
				vnoi *= r;
				ray = m_pGrid[i].pos + vnoi - origin;
				if(ray.lengthSquare() < node->area) continue;
				raster->write(ray, sample_opacity/nsamp);
			}
		}
	}
	else {
		occlusionAccum(node->child000, origin);
		occlusionAccum(node->child001, origin);
		occlusionAccum(node->child010, origin);
		occlusionAccum(node->child011, origin);
		occlusionAccum(node->child100, origin);
		occlusionAccum(node->child101, origin);
		occlusionAccum(node->child110, origin);
		occlusionAccum(node->child111, origin);
	}
}

void OcTree::voxelOcclusionAccum()
{
	voxelOcclusionAccum(root);
}

void OcTree::voxelOcclusionAccum(OCTNode *node)
{
	if(!node) return;
	sh->zeroCoeff(m_pSHBuf[node->index].value);
	sh->zeroCoeff(m_pSHBuf1[node->index].value);
	if(node->isLeaf) {
		raster->clear();
		occlusionAccum(node->mean);
		
		float l, vl;
		for(unsigned int j=0; j<SH_N_SAMPLES; j++) {
			SHSample s = sh->getSample(j);
			vl = m_key.dot(s.vector);
			if(vl>0) {
				raster->readLight(s.vector, l);
				l *= vl;
				sh->projectASample(m_pSHBuf[node->index].value, j, l);
			}
			vl = m_eye.dot(s.vector);
			if(vl>0) {
				raster->readLight(s.vector, l);
				l *= vl;
				sh->projectASample(m_pSHBuf1[node->index].value, j, l);
			}
		}
	}
	else {
		voxelOcclusionAccum(node->child000);
		voxelOcclusionAccum(node->child001);
		voxelOcclusionAccum(node->child010);
		voxelOcclusionAccum(node->child011);
		voxelOcclusionAccum(node->child100);
		voxelOcclusionAccum(node->child101);
		voxelOcclusionAccum(node->child110);
		voxelOcclusionAccum(node->child111);
		
		int nchd = 0;
		if(node->child000) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child000->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child000->index].value);
		}
		if(node->child001) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child001->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child001->index].value);
		}
		if(node->child010) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child010->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child010->index].value);
		}
		if(node->child011) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child011->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child011->index].value);
		}
		if(node->child100) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child100->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child100->index].value);
		}
		if(node->child101) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child101->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child101->index].value);
		}
		if(node->child110) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child110->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child110->index].value);
		}
		if(node->child111) {
			nchd++;
			sh->addCoeff(m_pSHBuf[node->index].value, m_pSHBuf[node->child111->index].value);
			sh->addCoeff(m_pSHBuf1[node->index].value, m_pSHBuf1[node->child111->index].value);
		}
		
		if(nchd > 1) {
			for (int j = 0; j < SH_N_BASES; j++) {
				m_pSHBuf[node->index].value[j] /= float(nchd);
				m_pSHBuf1[node->index].value[j] /= float(nchd);
			}
		}
	}
}

void OcTree::setHDRLighting(XYZ* coe)
{
	m_hasHdr = 1;
	for(int i = 0; i < SH_N_BASES; i++) m_hdrCoe[i] = coe[i];
}

void OcTree::LODGrid(GRIDNId* res, unsigned& n_data) const
{
	n_data = 0;
	LODGrid(root, n_data, res);
}

void OcTree::LODGrid(const OCTNode *node, unsigned& count, GRIDNId* res) const
{
	if(!node) return;
	XYZ cen = node->center;
	float size2 = node->size*2;
	
	XYZ pcam = cen;
	f_cameraSpaceInv.transform(pcam);
	
	if(pcam.z + size2 < 0) return;
	
	float depthz = pcam.z - node->size;
	if(depthz < 0.01) depthz = 0.01;
	
	float portWidth;
	if(f_isPersp) portWidth = depthz*f_fieldOfView;
	else portWidth = f_fieldOfView;
	
	if(pcam.x - size2 > portWidth) return;
	if(pcam.x + size2 < -portWidth) return;
	if(pcam.y - size2 > portWidth) return;
	if(pcam.y + size2 < -portWidth) return;
	
	// sum of grid and biggest one
	float sumarea =0;
	unsigned ibig = node->low;
	float fbig = m_pGrid[ibig].area;
	for(unsigned i= node->low; i<= node->high; i++) {
		sumarea += m_pGrid[i].area;
		if(m_pGrid[i].area > fbig) {
			ibig = i;
			fbig = m_pGrid[i].area;
		}
	}
	
	float r = sqrt(sumarea * 0.25);

	int detail = r/portWidth*1024;
	
	if(detail < 8) {
		res[count].grid.pos = node->mean;
		res[count].grid.nor = node->dir;
		res[count].grid.col = node->col;
		res[count].grid.area = sumarea;
		res[count].idx = node->index;
		res[count].detail = detail;
		res[count].gridId = t_grid_id[ibig];
		count++;
		return;
	}
	
	if(node->isLeaf) {
		for(unsigned i= node->low; i<= node->high; i++) {
			res[count].grid = m_pGrid[i];
			res[count].idx = node->index;
			//res[count].detail = detail;
			res[count].detail = sqrt(m_pGrid[i].area * 0.25)/portWidth*1024;
			res[count].gridId = t_grid_id[i];
			count++;
		}
	}
	else {
		LODGrid(node->child000, count, res);
		LODGrid(node->child001, count, res);
		LODGrid(node->child010, count, res);
		LODGrid(node->child011, count, res);
		LODGrid(node->child100, count, res);
		LODGrid(node->child101, count, res);
		LODGrid(node->child110, count, res);
		LODGrid(node->child111, count, res);
	}
}

void OcTree::setProjection(MATRIX44F mat, float fov, int iperspective)
{
	f_cameraSpaceInv = mat;
	f_fieldOfView = fov;
	if(iperspective == 1) {
		f_isPersp = 1;
		f_fieldOfView = tan(fov*0.5*PI/180);
	}
	else {
		f_isPersp = 0;
		f_fieldOfView = fov/2;
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
