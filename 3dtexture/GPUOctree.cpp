/*
 *  GPUOctree.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GPUOctree.h"
#include "../shared/gBase.h"

GPUOctree::GPUOctree() : m_root(0),
m_idr_r(0),
m_idr_g(0),
m_idr_b(0),
m_idr_a(0),
m_dt_r(0),
m_dt_g(0),
m_dt_b(0),
m_dt_a(0)
{}

GPUOctree::~GPUOctree() 
{
	release();
}

void GPUOctree::release()
{
	if(m_root) releaseNode(m_root);
	if(m_idr_r) delete[] m_idr_r;
	if(m_idr_g) delete[] m_idr_g;
	if(m_idr_b) delete[] m_idr_b;
	if(m_idr_a) delete[] m_idr_a;
	if(m_dt_r) delete[] m_dt_r;
	if(m_dt_g) delete[] m_dt_g;
	if(m_dt_b) delete[] m_dt_b;
	if(m_dt_a) delete[] m_dt_a;
}

void GPUOctree::releaseNode(GPUTreeNode *node)
{
	if(!node) return;
	
	for(int i=0; i < 8; i++) releaseNode( node->child[i] );
			
	delete[] node->child;
	delete node;
}

void GPUOctree::create(const XYZ& rootCenter, float rootSize, short maxLevel, const std::list<AParticle *>& particles)
{
	m_maxLevel = maxLevel;
	m_rootCenter = rootCenter;
	m_rootSize = rootSize;
	m_root = new GPUTreeNode();
	m_currentIndex = 0;
	
	float den = 0;
	for(std::list<AParticle *>::const_iterator it = particles.begin(); it != particles.end(); it++) {
// store data
		den += filterBox(rootCenter, rootSize, *it);
	}
	subdivideNode(m_root, rootCenter, rootSize, 0, particles, den);
}

void GPUOctree::subdivideNode(GPUTreeNode *node, const XYZ& center, float size, short level, const std::list<AParticle *>& particles, float density)
{
	float halfSize = size/2;
	XYZ halfCenter;
	level++;
	
	node->coordy = m_currentIndex/DATAPOOLWIDTH;
	node->coordx = m_currentIndex%DATAPOOLWIDTH;
	node->density = density;
	
	
						
	
	m_currentIndex++;
	
	if(level == m_maxLevel || m_currentIndex > MAXNNODE) return;

	
	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				
				if(i==0) halfCenter.x = center.x - halfSize;
				else halfCenter.x = center.x + halfSize;
				
				if(j==0) halfCenter.y = center.y - halfSize;
				else halfCenter.y = center.y + halfSize;
				
				if(k==0) halfCenter.z = center.z - halfSize;
				else halfCenter.z = center.z + halfSize;
				
				std::list<AParticle *> divParticles;
				float den = 0;
				for(std::list<AParticle *>::const_iterator it = particles.begin(); it != particles.end(); it++) {
					float aden = filterBox(halfCenter, halfSize, *it);
					if( aden > 0.f) {
						den += aden;
						divParticles.push_back(*it);
					}
				}
				
				if(!divParticles.empty()) {
					node->child[k*4+j*2+i] = new GPUTreeNode();
					subdivideNode(node->child[k*4+j*2+i], halfCenter, halfSize, level, divParticles, den);
					
					divParticles.clear();
				}
			}
		}
	}
}

void GPUOctree::dumpIndirection(const char *filename)
{
	m_idr_r = new half[INDIRECTIONPOOLSIZE];
	m_idr_g = new half[INDIRECTIONPOOLSIZE];
	m_idr_b = new half[INDIRECTIONPOOLSIZE];
	m_idr_a = new half[INDIRECTIONPOOLSIZE];
	m_dt_r = new half[DATAPOOLSIZE];
	m_dt_g = new half[DATAPOOLSIZE];
	m_dt_b = new half[DATAPOOLSIZE];
	m_dt_a = new half[DATAPOOLSIZE];
	
	setIndirection(m_root);
// save indirection	
	Header idrheader (INDIRECTIONPOOLWIDTH, INDIRECTIONPOOLWIDTH); 
	idrheader.insert ("root_size", FloatAttribute (m_rootSize));
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
                                   (char *) m_idr_r,            // base   // 8 
                                   sizeof (*m_idr_r) * 1,       // xStride// 9 
                                   sizeof (*m_idr_r) * INDIRECTIONPOOLWIDTH));
        idrframeBuffer.insert ("G",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) m_idr_g,            // base   // 8 
                                   sizeof (*m_idr_g) * 1,       // xStride// 9 
                                   sizeof (*m_idr_g) * INDIRECTIONPOOLWIDTH));
		 idrframeBuffer.insert ("B",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) m_idr_b,            // base   // 8 
                                   sizeof (*m_idr_b) * 1,       // xStride// 9 
                                   sizeof (*m_idr_b) * INDIRECTIONPOOLWIDTH));
		 idrframeBuffer.insert ("A",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) m_idr_a,            // base   // 8 
                                   sizeof (*m_idr_a) * 1,       // xStride// 9 
                                   sizeof (*m_idr_a) * INDIRECTIONPOOLWIDTH));
       
        idrfile.setFrameBuffer (idrframeBuffer);                                // 16 
        idrfile.writePixels (INDIRECTIONPOOLWIDTH); 
// save data
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
                                   (char *) m_dt_r,            // base   // 8 
                                   sizeof (*m_dt_r) * 1,       // xStride// 9 
                                   sizeof (*m_dt_r) * DATAPOOLWIDTH));
        dtframeBuffer.insert ("G",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) m_dt_g,            // base   // 8 
                                   sizeof (*m_dt_g) * 1,       // xStride// 9 
                                   sizeof (*m_dt_g) * DATAPOOLWIDTH));
		 dtframeBuffer.insert ("B",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) m_dt_b,            // base   // 8 
                                   sizeof (*m_dt_b) * 1,       // xStride// 9 
                                   sizeof (*m_dt_b) * DATAPOOLWIDTH));
		 dtframeBuffer.insert ("A",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) m_dt_a,            // base   // 8 
                                   sizeof (*m_dt_a) * 1,       // xStride// 9 
                                   sizeof (*m_dt_a) * DATAPOOLWIDTH));
       
        dtfile.setFrameBuffer (dtframeBuffer);                                // 16 
        dtfile.writePixels (DATAPOOLWIDTH); 
}

void GPUOctree::setIndirection(const GPUTreeNode *node)
{
	if(!node) return;
	
	unsigned short pools = node->coordx * 2;
	unsigned short poolt = node->coordy * 2;
	
// set data for current node	
	m_dt_r[node->coordy * DATAPOOLWIDTH + node->coordx] = node->density;
	m_dt_g[node->coordy * DATAPOOLWIDTH + node->coordx] = 0.f;
	m_dt_b[node->coordy * DATAPOOLWIDTH + node->coordx] = 0.f;
	m_dt_a[node->coordy * DATAPOOLWIDTH + node->coordx] = 0.f;
	
	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				unsigned long poolloc = (poolt + j)*INDIRECTIONPOOLWIDTH + pools + i;
				if(k==0) {
					
					if(node->child[k*4+j*2+i]) {
						m_idr_r[poolloc] = float(node->child[k*4+j*2+i]->coordx);
						m_idr_g[poolloc] = float(node->child[k*4+j*2+i]->coordy);
					}
					else { // empty
						m_idr_r[poolloc] = m_idr_g[poolloc] = -1;
					}
				}
				else {
					if(node->child[k*4+j*2+i]) {
						m_idr_b[poolloc] = float(node->child[k*4+j*2+i]->coordx);
						m_idr_a[poolloc] = float(node->child[k*4+j*2+i]->coordy);
					}
					else { // empty
						m_idr_b[poolloc] = m_idr_a[poolloc] = -1;
					}
				}
				
				setIndirection(node->child[k*4+j*2+i]);
					
			}
		}
	}
}

float GPUOctree::filterBox(const XYZ& center, float size, const AParticle *particle)
{
	float diffx = particle->pos.x - center.x;
	if(diffx < 0) diffx = -diffx;
	diffx -= size;
	if(diffx > particle->r) return 0.f;
	
	float diffy = particle->pos.y - center.y;
	if(diffy < 0) diffy = -diffy;
	diffy -= size;
	if(diffy > particle->r) return 0.f;
	
	float diffz = particle->pos.z - center.z;
	if(diffz < 0) diffz = -diffz;
	diffz -= size;
	if(diffz > particle->r) return 0.f;
	
	if(diffx < 0) diffx = 1.f;
	else diffx = 1.f - diffx/particle->r;
	
	if(diffy < 0) diffy = 1.f;
	else diffy = 1.f - diffy/particle->r;
	
	if(diffz < 0) diffz = 1.f;
	else diffz = 1.f - diffz/particle->r;
	
	return (diffx*diffy*diffz);
}

char GPUOctree::bInBox(const XYZ& center, float size, const AParticle *particle)
{
	float diffx = particle->pos.x - center.x;
	if(diffx < 0) diffx = -diffx;
	diffx -= size;
	if(diffx > particle->r) return 0;
	
	float diffy = particle->pos.y - center.y;
	if(diffy < 0) diffy = -diffy;
	diffy -= size;
	if(diffy > particle->r) return 0;
	
	float diffz = particle->pos.z - center.z;
	if(diffz < 0) diffz = -diffz;
	diffz -= size;
	if(diffz > particle->r) return 0;
	
	return 1;
}

//#include <maya/MGlobal.h>
char GPUOctree::load(const char *filename)
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
		
		const V3fAttribute *centerattr = idrfile.header().findTypedAttribute <V3fAttribute> ("root_center");
		Imath::V3f center = centerattr->value();
		
		m_rootCenter.x = center[0];
		m_rootCenter.y = center[1];
		m_rootCenter.z = center[2];
		
		Imath::Box2i dw = idrfile.header().dataWindow();
		
		int width  = dw.max.x - dw.min.x + 1;
		int height = dw.max.y - dw.min.y + 1; 
		
		m_idr_r = new half[width*height];
		m_idr_g = new half[width*height];
		m_idr_b = new half[width*height];
		m_idr_a = new half[width*height];
	
		FrameBuffer idrframeBuffer; 
		idrframeBuffer.insert ("R",                                  
							Slice (HALF,                         
								   (char *) m_idr_r, 
								   sizeof (*m_idr_r) * 1,    
								   sizeof (*m_idr_r) * (width),
								   1, 1,                          
								   0.0));
		idrframeBuffer.insert ("G",                                  
							Slice (HALF,                         
								   (char *) m_idr_g, 
								   sizeof (*m_idr_g) * 1,    
								   sizeof (*m_idr_g) * (width),
								   1, 1,                          
								   0.0));
		idrframeBuffer.insert ("B",                                  
							Slice (HALF,                         
								   (char *) m_idr_b, 
								   sizeof (*m_idr_b) * 1,    
								   sizeof (*m_idr_b) * (width),
								   1, 1,                          
								   0.0));
		idrframeBuffer.insert ("A",                                  
							Slice (HALF,                         
								   (char *) m_idr_a, 
								   sizeof (*m_idr_a) * 1,    
								   sizeof (*m_idr_a) * (width),
								   1, 1,                          
								   0.0));
								   
		idrfile.setFrameBuffer (idrframeBuffer); 
		idrfile.readPixels (dw.min.y, dw.max.y); 
// read data
		std::string dtname = filename;
		dtname += ".exr";
        InputFile dtfile(dtname.c_str());
		
		dw = dtfile.header().dataWindow();
		
		width  = dw.max.x - dw.min.x + 1;
		height = dw.max.y - dw.min.y + 1; 
		
		m_dt_r = new half[DATAPOOLSIZE];
		m_dt_g = new half[DATAPOOLSIZE];
		m_dt_b = new half[DATAPOOLSIZE];
		m_dt_a = new half[DATAPOOLSIZE];
	
		FrameBuffer dtframeBuffer; 
		dtframeBuffer.insert ("R",                                  
							Slice (HALF,                         
								   (char *) m_dt_r, 
								   sizeof (*m_dt_r) * 1,    
								   sizeof (*m_dt_r) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
		dtframeBuffer.insert ("G",                                  
							Slice (HALF,                         
								   (char *) m_dt_g, 
								   sizeof (*m_dt_g) * 1,    
								   sizeof (*m_dt_g) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
		dtframeBuffer.insert ("B",                                  
							Slice (HALF,                         
								   (char *) m_dt_b, 
								   sizeof (*m_dt_b) * 1,    
								   sizeof (*m_dt_b) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
		dtframeBuffer.insert ("A",                                  
							Slice (HALF,                         
								   (char *) m_dt_a, 
								   sizeof (*m_dt_a) * 1,    
								   sizeof (*m_dt_a) * (DATAPOOLWIDTH),
								   1, 1,                          
								   0.0));
								   
		dtfile.setFrameBuffer (dtframeBuffer); 
		dtfile.readPixels (dw.min.y, dw.max.y);
    } 
    catch (const std::exception &exc) 
    { 
		return 0;
	}
	
	//MGlobal::displayInfo(MString("dens first; ")+m_dt_r[0]);
	return 1;
}

void GPUOctree::getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const
{
	xmin = m_rootCenter.x - m_rootSize;
	xmax = m_rootCenter.x + m_rootSize;
	ymin = m_rootCenter.y - m_rootSize;
	ymax = m_rootCenter.y + m_rootSize;
	zmin = m_rootCenter.z - m_rootSize;
	zmax = m_rootCenter.z + m_rootSize;
}

void GPUOctree::drawBox() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	drawBoxNode(m_rootCenter, m_rootSize, 0, 0);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GPUOctree::drawBoxNode(const XYZ& center, float size, int x, int y) const
{
	gBase::drawBox(center, size);
	unsigned short pools = x * 2;
	unsigned short poolt = y * 2;
	
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
					x = m_idr_r[poolloc];
					y = m_idr_g[poolloc];
					
					if(x >=0) drawBoxNode(halfCenter, halfSize, x, y);			
				}
				else {
					x = m_idr_b[poolloc];
					y = m_idr_a[poolloc];
					
					if(x >=0) drawBoxNode(halfCenter, halfSize, x, y);
				}
			}
		}
	}
}

void GPUOctree::drawCube() const
{
	glBegin(GL_QUADS);
	drawCubeNode(m_rootCenter, m_rootSize, 0, 0);
	glEnd();
}

void GPUOctree::drawCubeNode(const XYZ& center, float size, int x, int y) const
{
// get density
	float den = m_dt_r[y * DATAPOOLWIDTH + x];
	den /= 8;
	
	unsigned short pools = x * 2;
	unsigned short poolt = y * 2;
	
	float halfSize = size/2;
	XYZ halfCenter;
	
	int bleaf = 1;
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
					x = m_idr_r[poolloc];
					y = m_idr_g[poolloc];
					
					if(x >=0) {
						drawCubeNode(halfCenter, halfSize, x, y);	
						bleaf = 0;
					}
				}
				else {
					x = m_idr_b[poolloc];
					y = m_idr_a[poolloc];
					
					if(x >=0) {
						drawCubeNode(halfCenter, halfSize, x, y);
						bleaf = 0;
					}
				}
			}
		}
	}
	
	if(bleaf == 1) {
		glColor3f(den, den, den);
		gBase::drawBox(center, size/2);
	}
}