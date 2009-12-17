/*
 *  FluidOctree.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "FluidOctree.h"
#include "../shared/gBase.h"
#include "fluidSolver.h"
#include <maya/MGlobal.h>

FluidOctree::FluidOctree() : m_root(0),
m_idr(0),
m_dt(0)
{}

FluidOctree::~FluidOctree() 
{
	release();
}

void FluidOctree::release()
{
	if(m_root) releaseNode(m_root);
	if(m_dt) delete[] m_dt;
	if(m_idr) delete[] m_idr;
}

void FluidOctree::releaseNode(GPUTreeNode *node)
{
	if(!node) return;
	
	for(int i=0; i < 8; i++) releaseNode( node->child[i] );
			
	delete[] node->child;
	delete node;
}
#ifndef FLUIDTREEREADONLY
void FluidOctree::create(FluidSolver *solver, short maxLevel)
//void FluidOctree::create(const XYZ& rootCenter, float rootSize, short maxLevel, const std::list<AParticle *>& particles)
{
	m_maxLevel = maxLevel;
	m_pSolver = solver;
	
	int w, h, d;
	m_pSolver->getDimension(w, h, d);
	
	m_dimx = w;
	m_dimy = h;
	m_dimz = d;
	
	int cx = w/2;
	int cy = h/2;
	int cz = d/2;
	
	int maxdim = w;
	if(h > maxdim) maxdim = h;
	if(d > maxdim) maxdim = d;
	
	int size = maxdim/2;
	
	m_pSolver->getOrigin(m_rootCenter.x, m_rootCenter.y, m_rootCenter.z);
	float unit = m_pSolver->getGridSize();
	m_rootSize = unit * size;
	
	m_rootCenter.x += unit * cx;
	m_rootCenter.y += unit * cy; 
	m_rootCenter.z += unit * cz;
	
	m_root = new GPUTreeNode();
	m_currentIndex = 0;
	m_currentLeafIndex = 0;
	float density = addupDensityCells( cx, cy, cz, size);
	subdivideNode(m_root, cx, cy, cz, size, 0, density);
}

void FluidOctree::subdivideNode(GPUTreeNode *node, int cx, int cy, int cz, int size, short level, float density)
{	
	node->density = density;
	//MGlobal::displayInfo(MString("level ")+ level +"sum "+node->density);
	if(level < m_maxLevel) {
		node->coordy = m_currentIndex/DATAPOOLWIDTH;
		node->coordx = m_currentIndex%DATAPOOLWIDTH;
		m_currentIndex++;
	}
	else {//node->density = float(rand()%313)/313.f;
		node->coordy = m_currentLeafIndex/DATAPOOLWIDTH;
		node->coordx = m_currentLeafIndex%DATAPOOLWIDTH;
		//node->coordy = rand()%DATAPOOLWIDTH;
		//node->coordx = rand()%DATAPOOLWIDTH;
		m_currentLeafIndex++;
		return;
	}

	level++;
	
	int halfSize = size/2; //if(halfSize < 1) MGlobal::displayInfo(MString("level ")+ level +MString("size ")+ halfSize);
	int childcx, childcy, childcz;

	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				if(halfSize >0) {
					if(i==0) childcx = cx - halfSize;
					else childcx = cx + halfSize;
					
					if(j==0) childcy = cy - halfSize;
					else childcy = cy + halfSize;
					
					if(k==0) childcz = cz - halfSize;
					else childcz = cz + halfSize;
				}
				else {
					if(i==0) childcx = cx - 1;
					else childcx = cx;
					
					if(j==0) childcy = cy - 1;
					else childcy = cy;
					
					if(k==0) childcz = cz - 1;
					else childcz = cz;
				}
				
				float sum = addupDensityCells( childcx, childcy, childcz, halfSize);

				if(sum > 0.004f) {
					node->child[k*4+j*2+i] = new GPUTreeNode();
					
					subdivideNode(node->child[k*4+j*2+i], childcx, childcy, childcz, halfSize, level, sum);
				}					
			}
		}
	}
}

float FluidOctree::addupDensityCells(int cx, int cy, int cz, int size)
{
	// find dim bound
	int xmin = cx - size;
	int xmax = cx + size;
	int ymin = cy - size;
	int ymax = cy + size;
	int zmin = cz - size;
	int zmax = cz + size;

// discard outside box	
	if(xmin >= m_dimx || xmax < 0) return 0.f;
	if(ymin >= m_dimy || ymax < 0) return 0.f;
	if(zmin >= m_dimz || zmax < 0) return 0.f;
	
// validate box
	if(xmin < 0) xmin = 0;
	if(xmax > m_dimx) xmax = m_dimx;
	if(ymin < 0) ymin = 0;
	if(ymax > m_dimy) ymax = m_dimy;
	if(zmin < 0) zmin = 0;
	if(zmax > m_dimz) zmax = m_dimz;
	
// add up density
	float sum = 0;
	
	if(size < 1) sum = m_pSolver->getVoxelDensity(xmin, ymin, zmin);
	else {
		for (int k=zmin;k<zmax;k++) {
			for (int j=ymin;j<ymax;j++) {
				for (int i=xmin;i<xmax;i++) {
					sum += m_pSolver->getVoxelDensity(i, j, k);
				}
			}
		}
	}
	
	return sum;
}

void FluidOctree::dumpIndirection(const char *filename)
{
	m_idr = new short[INDIRECTIONPOOLSIZE*4];
	m_dt = new float[DATAPOOLSIZE*4];

	setIndirection(m_root, 0);
	
	half *idr_r = new half[INDIRECTIONPOOLSIZE];
	half *idr_g = new half[INDIRECTIONPOOLSIZE];
	half *idr_b = new half[INDIRECTIONPOOLSIZE];
	half *idr_a = new half[INDIRECTIONPOOLSIZE];
	
	for(long i=0; i<INDIRECTIONPOOLSIZE; i++) {
		idr_r[i] = (half)m_idr[i*4];
		idr_g[i] = (half)m_idr[i*4+1];
		idr_b[i] = (half)m_idr[i*4+2];
		idr_a[i] = (half)m_idr[i*4+3];
	}
	
// save indirection	
	Header idrheader (INDIRECTIONPOOLWIDTH, INDIRECTIONPOOLWIDTH); 
	idrheader.insert ("root_size", FloatAttribute (m_rootSize));
	idrheader.insert ("max_level", FloatAttribute (m_maxLevel));
	idrheader.insert ("root_center", V3fAttribute (Imath::V3f(m_rootCenter.x, m_rootCenter.y, m_rootCenter.z))); 

		idrheader.channels().insert ("R", Channel (HALF));
		idrheader.channels().insert ("G", Channel (HALF));                                   // 1 
        idrheader.channels().insert ("B", Channel (HALF));
		idrheader.channels().insert ("A", Channel (HALF));                   // 2  
    
	std::string idrname = filename;
	idrname += ".idr";
        OutputFile idrfile (idrname.c_str(), idrheader);                               // 4 
        FrameBuffer idrframeBuffer;
		 idrframeBuffer.insert ("R",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_r,            // base   // 8 
                                   sizeof (*idr_r) * 1,       // xStride// 9 
                                   sizeof (*idr_r) * INDIRECTIONPOOLWIDTH));
        idrframeBuffer.insert ("G",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_g,            // base   // 8 
                                   sizeof (*idr_g) * 1,       // xStride// 9 
                                   sizeof (*idr_g) * INDIRECTIONPOOLWIDTH));
		 idrframeBuffer.insert ("B",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_b,            // base   // 8 
                                   sizeof (*idr_b) * 1,       // xStride// 9 
                                   sizeof (*idr_b) * INDIRECTIONPOOLWIDTH));
		 idrframeBuffer.insert ("A",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_a,            // base   // 8 
                                   sizeof (*idr_a) * 1,       // xStride// 9 
                                   sizeof (*idr_a) * INDIRECTIONPOOLWIDTH));
       
        idrfile.setFrameBuffer (idrframeBuffer);                                // 16 
        idrfile.writePixels (INDIRECTIONPOOLWIDTH); 
        
        delete[] idr_r;
	delete[] idr_g;
	delete[] idr_b;
	delete[] idr_a;
// save data

	half *dt_r = new half[DATAPOOLSIZE];
	half *dt_g = new half[DATAPOOLSIZE];
	half *dt_b = new half[DATAPOOLSIZE];
	half *dt_a = new half[DATAPOOLSIZE];
	
	for(long i=0; i<DATAPOOLSIZE; i++) {
		dt_r[i] = (half)m_dt[i*4];
		dt_g[i] = (half)m_dt[i*4+1];
		dt_b[i] = (half)m_dt[i*4+2];
		dt_a[i] = (half)m_dt[i*4+3];
	}
	
	Header dtheader (DATAPOOLWIDTH, DATAPOOLWIDTH); 
		dtheader.channels().insert ("R", Channel (HALF));
		dtheader.channels().insert ("G", Channel (HALF));                                   // 1 
        dtheader.channels().insert ("B", Channel (HALF));
		dtheader.channels().insert ("A", Channel (HALF));                   // 2  
    
	std::string dtname = filename;
	dtname += ".exr";
        OutputFile dtfile (dtname.c_str(), dtheader);                               // 4 
        FrameBuffer dtframeBuffer;
		 dtframeBuffer.insert ("R",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) dt_r,            // base   // 8 
                                   sizeof (*dt_r) * 1,       // xStride// 9 
                                   sizeof (*dt_r) * DATAPOOLWIDTH));
        dtframeBuffer.insert ("G",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) dt_g,            // base   // 8 
                                   sizeof (*dt_g) * 1,       // xStride// 9 
                                   sizeof (*dt_g) * DATAPOOLWIDTH));
		 dtframeBuffer.insert ("B",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) dt_b,            // base   // 8 
                                   sizeof (*dt_b) * 1,       // xStride// 9 
                                   sizeof (*dt_b) * DATAPOOLWIDTH));
		 dtframeBuffer.insert ("A",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) dt_a,            // base   // 8 
                                   sizeof (*dt_a) * 1,       // xStride// 9 
                                   sizeof (*dt_a) * DATAPOOLWIDTH));
       
        dtfile.setFrameBuffer (dtframeBuffer);                                // 16 
        dtfile.writePixels (DATAPOOLWIDTH); 
		
	delete[] dt_r;
	delete[] dt_g;
	delete[] dt_b;
	delete[] dt_a;
}

void FluidOctree::setIndirection(const GPUTreeNode *node, short level)
{
	if(!node) return;
	
// set data for leaf node
	if(level == m_maxLevel) {
		long dataloc = node->coordy * DATAPOOLWIDTH + node->coordx;
	m_dt[dataloc*4] = node->density;
	m_dt[dataloc*4+1] = 0.f;
	m_dt[dataloc*4+2] = 0.f;
	m_dt[dataloc*4+3] = 0.f;
	
	return;
	}
	
	int pools = node->coordx * 2;
	int poolt = node->coordy * 2;
	
	level++;
	
	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				unsigned long poolloc = (poolt + j)*INDIRECTIONPOOLWIDTH + pools + i;
				if(k==0) {
					
					if(node->child[k*4+j*2+i]) {
						m_idr[poolloc*4] = node->child[k*4+j*2+i]->coordx;
						m_idr[poolloc*4+1] = node->child[k*4+j*2+i]->coordy;
					}
					else { // empty
						m_idr[poolloc*4] = m_idr[poolloc*4+1] = -1;
					}
				}
				else {
					if(node->child[k*4+j*2+i]) {
						m_idr[poolloc*4+2] = node->child[k*4+j*2+i]->coordx;
						m_idr[poolloc*4+3] = node->child[k*4+j*2+i]->coordy;
					}
					else { // empty
						m_idr[poolloc*4+2] = m_idr[poolloc*4+3] = -1;
					}
				}
				
				setIndirection(node->child[k*4+j*2+i], level);
					
			}
		}
	}
}
#endif
/*
float FluidOctree::filterBox(const XYZ& center, float size, const AParticle *particle)
{	
	float H = particle->r;
	float diffx = particle->pos.x - center.x;
	if(diffx < 0.0f) diffx = -diffx;
	diffx = diffx - size;
	if(diffx >= H) return 0.f;
	
	float diffy = particle->pos.y - center.y;
	if(diffy < 0.0f) diffy = -diffy;
	diffy = diffy - size;
	if(diffy >= H) return 0.f;
	
	float diffz = particle->pos.z - center.z;
	if(diffz < 0.0f) diffz = -diffz;
	diffz = diffz - size;
	if(diffz >= H) return 0.f;
	

	if(diffx < 0) diffx = 1.f;
	else diffx = 1.f - diffx/H;
	
	
	if(diffy < 0) diffy = 1.f;
	else diffy = 1.f - diffy/H;
	
	
	if(diffz < 0) diffz = 1.f;
	else diffz = 1.f - diffz/H;
	
	return (diffx*diffy*diffz);
}

char FluidOctree::bInBox(const XYZ& center, float size, const AParticle *particle)
{
	float diffx = particle->pos.x - center.x;
	if(diffx < -size || diffx >= size) return 0;
	
	float diffy = particle->pos.y - center.y;
	if(diffy < -size || diffy >= size) return 0;
	
	float diffz = particle->pos.z - center.z;
	if(diffz < -size || diffz >= size) return 0;
	
	return 1;
}
*/

char FluidOctree::load(const char *filename)
{
	release();
	try 
    { 
// read indirection
		std::string idrname = filename;
		idrname += ".idr";
        InputFile idrfile(idrname.c_str()); 
		const FloatAttribute *sizeattr = idrfile.header().findTypedAttribute <FloatAttribute> ("root_size");
		m_rootSize = sizeattr->value();
		
		const FloatAttribute *levelattr = idrfile.header().findTypedAttribute <FloatAttribute> ("max_level");
		m_maxLevel = (int)levelattr->value();
		
		const V3fAttribute *centerattr = idrfile.header().findTypedAttribute <V3fAttribute> ("root_center");
		Imath::V3f center = centerattr->value();
		
		m_rootCenter.x = center[0];
		m_rootCenter.y = center[1];
		m_rootCenter.z = center[2];
		
		Imath::Box2i dw = idrfile.header().dataWindow();
		
		int width  = dw.max.x - dw.min.x + 1;
		int height = dw.max.y - dw.min.y + 1; 
		
		half *idr_r = new half[width*height];
		half *idr_g = new half[width*height];
		half *idr_b = new half[width*height];
		half *idr_a = new half[width*height];
	
		FrameBuffer idrframeBuffer; 
		idrframeBuffer.insert ("R",                                  
							Slice (HALF,                         
								   (char *) idr_r, 
								   sizeof (*idr_r) * 1,    
								   sizeof (*idr_r) * (width),
								   1, 1,                          
								   0.0));
		idrframeBuffer.insert ("G",                                  
							Slice (HALF,                         
								   (char *) idr_g, 
								   sizeof (*idr_g) * 1,    
								   sizeof (*idr_g) * (width),
								   1, 1,                          
								   0.0));
		idrframeBuffer.insert ("B",                                  
							Slice (HALF,                         
								   (char *) idr_b, 
								   sizeof (*idr_b) * 1,    
								   sizeof (*idr_b) * (width),
								   1, 1,                          
								   0.0));
		idrframeBuffer.insert ("A",                                  
							Slice (HALF,                         
								   (char *) idr_a, 
								   sizeof (*idr_a) * 1,    
								   sizeof (*idr_a) * (width),
								   1, 1,                          
								   0.0));
								   
		idrfile.setFrameBuffer (idrframeBuffer); 
		idrfile.readPixels (dw.min.y, dw.max.y);
		
		m_idr = new short[INDIRECTIONPOOLSIZE*4];

		for(int i=0; i<INDIRECTIONPOOLSIZE; i++) {
			m_idr[i*4] = idr_r[i];
			m_idr[i*4+1] = idr_g[i];
			m_idr[i*4+2] = idr_b[i];
			m_idr[i*4+3] = idr_a[i];
		}
		
		
		
		delete[] idr_r;
		delete[] idr_g;
		delete[] idr_b;
		delete[] idr_a;
// read data

		std::string dtname = filename;
		dtname += ".exr";
        InputFile dtfile(dtname.c_str());
		
		dw = dtfile.header().dataWindow();
		
		width  = dw.max.x - dw.min.x + 1;
		height = dw.max.y - dw.min.y + 1; 
		
		half *dt_r = new half[DATAPOOLSIZE];
		half *dt_g = new half[DATAPOOLSIZE];
		half *dt_b = new half[DATAPOOLSIZE];
		half *dt_a = new half[DATAPOOLSIZE];
	
		FrameBuffer dtframeBuffer; 
		dtframeBuffer.insert ("R",                                  
							Slice (HALF,                         
								   (char *) dt_r, 
								   sizeof (*dt_r) * 1,    
								   sizeof (*dt_r) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
		dtframeBuffer.insert ("G",                                  
							Slice (HALF,                         
								   (char *) dt_g, 
								   sizeof (*dt_g) * 1,    
								   sizeof (*dt_g) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
		dtframeBuffer.insert ("B",                                  
							Slice (HALF,                         
								   (char *) dt_b, 
								   sizeof (*dt_b) * 1,    
								   sizeof (*dt_b) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
		dtframeBuffer.insert ("A",                                  
							Slice (HALF,                         
								   (char *) dt_a, 
								   sizeof (*dt_a) * 1,    
								   sizeof (*dt_a) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
								   
		dtfile.setFrameBuffer (dtframeBuffer); 
		dtfile.readPixels (dw.min.y, dw.max.y);
		
		m_dt = new float[DATAPOOLSIZE*4];

		for(int i=0; i<DATAPOOLSIZE; i++) {
			m_dt[i*4] = dt_r[i];
			m_dt[i*4+1] = dt_g[i];
			m_dt[i*4+2] = dt_b[i];
			m_dt[i*4+3] = dt_a[i];
		}
		
		delete[] dt_r;
		delete[] dt_g;
		delete[] dt_b;
		delete[] dt_a;
    } 
    catch (const std::exception &exc) 
    { 
		return 0;
	}
	
	//MGlobal::displayInfo(MString("dens first; ")+m_dt_r[0]);
	return 1;
}

void FluidOctree::getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const
{
	xmin = m_rootCenter.x - m_rootSize;
	xmax = m_rootCenter.x + m_rootSize;
	ymin = m_rootCenter.y - m_rootSize;
	ymax = m_rootCenter.y + m_rootSize;
	zmin = m_rootCenter.z - m_rootSize;
	zmax = m_rootCenter.z + m_rootSize;
}

void FluidOctree::getBBox(float& rootX, float& rootY, float& rootZ, float& rootSize) const 
{
	rootX = m_rootCenter.x;
	rootY = m_rootCenter.y;
	rootZ = m_rootCenter.z;
	rootSize = m_rootSize;
}

void FluidOctree::drawBox() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	drawBoxNode(m_rootCenter, m_rootSize, 0, 0, 0);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void FluidOctree::drawBoxNode(const XYZ& center, float size, int x, int y, short level) const
{
	drawBox(center, size);
	
	if(level == m_maxLevel) return;
			
	int pools = x * 2;
	int poolt = y * 2;
	
	float halfSize = size/2;
	XYZ halfCenter;
	
	level++;
	
	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				unsigned long poolloc = (poolt + j)*INDIRECTIONPOOLWIDTH + pools + i;
				
				if(i==0) halfCenter.x = center.x - halfSize;
				else halfCenter.x = center.x + halfSize;
				
				if(j==0) halfCenter.y = center.y - halfSize;
				else halfCenter.y = center.y + halfSize;
				
				if(k==0) halfCenter.z = center.z - halfSize;
				else halfCenter.z = center.z + halfSize;
				
				if(k==0) {
					x = m_idr[poolloc*4];
					y = m_idr[poolloc*4+1];
					
					if(x >=0) drawBoxNode(halfCenter, halfSize, x, y, level);			
				}
				else {
					x = m_idr[poolloc*4+2];
					y = m_idr[poolloc*4+3];
					
					if(x >=0) drawBoxNode(halfCenter, halfSize, x, y, level);
				}
			}
		}
	}
}

void FluidOctree::drawCube() const
{
	glBegin(GL_QUADS);
	drawCubeNode(m_rootCenter, m_rootSize, 0, 0, 0);
	glEnd();
}

void FluidOctree::drawCubeNode(const XYZ& center, float size, int x, int y, int level) const
{
	if(level== m_maxLevel) {
		// get density
		float den = m_dt[(y * DATAPOOLWIDTH + x)*4];
		glColor3f(den, den, den);
		drawBox(center, size/2);
		return;
	}
	
	level++;
	
	int pools = x * 2;
	int poolt = y * 2;
	
	float halfSize = size/2;
	XYZ halfCenter;
	
	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				unsigned long poolloc = (poolt + j)*INDIRECTIONPOOLWIDTH + pools + i;
				
				if(i==0) halfCenter.x = center.x - halfSize;
				else halfCenter.x = center.x + halfSize;
				
				if(j==0) halfCenter.y = center.y - halfSize;
				else halfCenter.y = center.y + halfSize;
				
				if(k==0) halfCenter.z = center.z - halfSize;
				else halfCenter.z = center.z + halfSize;
				
				if(k==0) {
					x = m_idr[poolloc*4];
					y = m_idr[poolloc*4+1];
					
					if(x >=0) {
						drawCubeNode(halfCenter, halfSize, x, y, level);	
						
					}
				}
				else {
					x = m_idr[poolloc*4+2];
					y = m_idr[poolloc*4+3];
					
					if(x >=0) {
						drawCubeNode(halfCenter, halfSize, x, y, level);
						
					}
				}
				
				
			}
		}
	}
	
}

void FluidOctree::drawBox(const XYZ& center, float size) const
{
	glVertex3f(center.x - size, center.y - size, center.z - size);
	glVertex3f(center.x + size, center.y - size, center.z - size);
	glVertex3f(center.x + size, center.y + size, center.z - size);
	glVertex3f(center.x - size, center.y + size, center.z - size);
	
	glVertex3f(center.x + size, center.y - size, center.z - size);
	glVertex3f(center.x + size, center.y - size, center.z + size);
	glVertex3f(center.x + size, center.y + size, center.z + size);
	glVertex3f(center.x + size, center.y + size, center.z - size);
	
	glVertex3f(center.x + size, center.y - size, center.z + size);
	glVertex3f(center.x - size, center.y - size, center.z + size);
	glVertex3f(center.x - size, center.y + size, center.z + size);
	glVertex3f(center.x + size, center.y + size, center.z + size);
	
	glVertex3f(center.x - size, center.y - size, center.z - size);
	glVertex3f(center.x - size, center.y - size, center.z + size);
	glVertex3f(center.x - size, center.y + size, center.z + size);
	glVertex3f(center.x - size, center.y + size, center.z - size);
	
	glVertex3f(center.x - size, center.y - size, center.z - size);
	glVertex3f(center.x + size, center.y - size, center.z - size);
	glVertex3f(center.x + size, center.y - size, center.z + size);
	glVertex3f(center.x - size, center.y - size, center.z + size);
	
	glVertex3f(center.x - size, center.y + size, center.z - size);
	glVertex3f(center.x + size, center.y + size, center.z - size);
	glVertex3f(center.x + size, center.y + size, center.z + size);
	glVertex3f(center.x - size, center.y + size, center.z + size);	
}