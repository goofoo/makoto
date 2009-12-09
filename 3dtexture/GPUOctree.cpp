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
m_idr_a(0)
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
	subdivideNode(m_root, rootCenter, rootSize, 0, particles);
}

void GPUOctree::subdivideNode(GPUTreeNode *node, const XYZ& center, float size, short level, const std::list<AParticle *>& particles)
{
	float halfSize = size/2;
	XYZ halfCenter;
	level++;
	
	node->coordy = m_currentIndex/DATAPOOLWIDTH;
	node->coordx = m_currentIndex%DATAPOOLWIDTH;
	
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
				
				for(std::list<AParticle *>::const_iterator it = particles.begin(); it != particles.end(); it++) {
					if(isInBox(halfCenter, halfSize, *it)) divParticles.push_back(*it);
				}
				
				if(!divParticles.empty()) {
					node->child[k*4+j*2+i] = new GPUTreeNode();
					subdivideNode(node->child[k*4+j*2+i], halfCenter, halfSize, level, divParticles);
					
					divParticles.clear();
				}
			}
		}
	}
}

void GPUOctree::dumpIndirection(const char *filename) const
{
	half *r = new half[INDIRECTIONPOOLSIZE];
	half *g = new half[INDIRECTIONPOOLSIZE];
	half *b = new half[INDIRECTIONPOOLSIZE];
	half *a = new half[INDIRECTIONPOOLSIZE];
	
	setIndirection(m_root, r, g, b, a);
	
	Header header (INDIRECTIONPOOLWIDTH, INDIRECTIONPOOLWIDTH); 
	header.insert ("root_size", FloatAttribute (m_rootSize));
	header.insert ("root_center", V3fAttribute (Imath::V3f(m_rootCenter.x, m_rootCenter.y, m_rootCenter.z))); 

		header.channels().insert ("R", Channel (HALF));
		header.channels().insert ("G", Channel (HALF));                                   // 1 
        header.channels().insert ("B", Channel (HALF));
		header.channels().insert ("A", Channel (HALF));                   // 2  
    
	
        OutputFile file (filename, header);                               // 4 
        FrameBuffer frameBuffer;
		 frameBuffer.insert ("R",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) r,            // base   // 8 
                                   sizeof (*r) * 1,       // xStride// 9 
                                   sizeof (*r) * INDIRECTIONPOOLWIDTH));
        frameBuffer.insert ("G",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) g,            // base   // 8 
                                   sizeof (*g) * 1,       // xStride// 9 
                                   sizeof (*g) * INDIRECTIONPOOLWIDTH));
		 frameBuffer.insert ("B",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) b,            // base   // 8 
                                   sizeof (*b) * 1,       // xStride// 9 
                                   sizeof (*b) * INDIRECTIONPOOLWIDTH));
		 frameBuffer.insert ("A",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) a,            // base   // 8 
                                   sizeof (*a) * 1,       // xStride// 9 
                                   sizeof (*a) * INDIRECTIONPOOLWIDTH));
       
        file.setFrameBuffer (frameBuffer);                                // 16 
        file.writePixels (INDIRECTIONPOOLWIDTH);  
		
	delete[] r;
	delete[] g;
	delete[] b;
	delete[] a;
}

void GPUOctree::setIndirection(const GPUTreeNode *node,  half *r, half *g, half *b, half *a) const
{
	if(!node) return;
	
	unsigned short pools = node->coordx * 2;
	unsigned short poolt = node->coordy * 2;
	
	for (int k=0;k<2;k++) {
		for (int j=0;j<2;j++) {
			for (int i=0;i<2;i++) {
				unsigned long poolloc = (poolt + j)*INDIRECTIONPOOLWIDTH + pools + i;
				if(k==0) {
					
					if(node->child[k*4+j*2+i]) {
						r[poolloc] = float(node->child[k*4+j*2+i]->coordx);
						g[poolloc] = float(node->child[k*4+j*2+i]->coordy);
					}
					else { // empty
						r[poolloc] = g[poolloc] = -1;
					}
				}
				else {
					if(node->child[k*4+j*2+i]) {
						b[poolloc] = float(node->child[k*4+j*2+i]->coordx);
						a[poolloc] = float(node->child[k*4+j*2+i]->coordy);
					}
					else { // empty
						b[poolloc] = a[poolloc] = -1;
					}
				}
				
				setIndirection(node->child[k*4+j*2+i], r, g, b, a);
					
			}
		}
	}
}

char GPUOctree::isInBox(const XYZ& center, float size, const AParticle *particle)
{
	float diff = particle->pos.x - center.x;
	if(diff < 0) diff = -diff;
	if(diff - size > particle->r) return 0;
	
	diff = particle->pos.y - center.y;
	if(diff < 0) diff = -diff;
	if(diff - size > particle->r) return 0;
	
	diff = particle->pos.z - center.z;
	if(diff < 0) diff = -diff;
	if(diff - size > particle->r) return 0;
	
	return 1;
}

char GPUOctree::load(const char *filename)
{
	release();
	try 
    { 
        InputFile file(filename); 
		const FloatAttribute *sizeattr = file.header().findTypedAttribute <FloatAttribute> ("root_size");
		m_rootSize = sizeattr->value();
		
		const V3fAttribute *centerattr = file.header().findTypedAttribute <V3fAttribute> ("root_center");
		Imath::V3f center = centerattr->value();
		
		m_rootCenter.x = center[0];
		m_rootCenter.y = center[1];
		m_rootCenter.z = center[2];
		
		Imath::Box2i dw = file.header().dataWindow();
		
		int width  = dw.max.x - dw.min.x + 1;
		int height = dw.max.y - dw.min.y + 1; 
		
		m_idr_r = new half[width*height];
		m_idr_g = new half[width*height];
		m_idr_b = new half[width*height];
		m_idr_a = new half[width*height];
	
		FrameBuffer frameBuffer; 
		frameBuffer.insert ("R",                                  
							Slice (HALF,                         
								   (char *) m_idr_r, 
								   sizeof (*m_idr_r) * 1,    
								   sizeof (*m_idr_r) * (width),
								   1, 1,                          
								   0.0));
		frameBuffer.insert ("G",                                  
							Slice (HALF,                         
								   (char *) m_idr_g, 
								   sizeof (*m_idr_g) * 1,    
								   sizeof (*m_idr_g) * (width),
								   1, 1,                          
								   0.0));
		frameBuffer.insert ("B",                                  
							Slice (HALF,                         
								   (char *) m_idr_b, 
								   sizeof (*m_idr_b) * 1,    
								   sizeof (*m_idr_b) * (width),
								   1, 1,                          
								   0.0));
		frameBuffer.insert ("A",                                  
							Slice (HALF,                         
								   (char *) m_idr_a, 
								   sizeof (*m_idr_a) * 1,    
								   sizeof (*m_idr_a) * (width),
								   1, 1,                          
								   0.0));
								   
		file.setFrameBuffer (frameBuffer); 
		file.readPixels (dw.min.y, dw.max.y); 
    } 
    catch (const std::exception &exc) 
    { 
		return 0;
	}
	
	
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
	drawBoxNode(m_rootCenter, m_rootSize, 0, 0);
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