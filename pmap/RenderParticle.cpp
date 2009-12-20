/*
 *  RenderParticle.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderParticle.h"
#include "../shared/QuickSortIdx.h"

RenderParticle::RenderParticle() : m_isInitialized(0),
m_image_fbo(0),
m_image_tex(0)
{}

RenderParticle::~RenderParticle() {uninitialize();}

void RenderParticle::initialize()
{
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &tbo);
	shader = new GLSLSprite();
	m_isInitialized = 1;
}

void RenderParticle::uninitialize()
{
	glDeleteBuffers(1, &ibo);
	delete shader;
}

void RenderParticle::setVertex(float* vertex) 
{ 
	m_pVertex = vertex;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	unsigned int size = m_num_particle * sizeof(float) * 4;
        glBufferData(GL_ARRAY_BUFFER, size, vertex, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void RenderParticle::setCoord(float* coord) 
{ 
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	unsigned int size = m_num_particle * sizeof(float) * 4;
        glBufferData(GL_ARRAY_BUFFER, size, coord, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void RenderParticle::sort()
{
	unsigned int size = m_num_particle * sizeof(unsigned int);
	unsigned int* idxbuffer = new unsigned int[m_num_particle];
	unsigned int i;
	for(i=0; i<m_num_particle; i++) idxbuffer[i]= i;
	
	float *key = new float[m_num_particle];
	for(i=0; i<m_num_particle; i++) {
// calc depth 
		key[i]= (m_pVertex[i*4] -m_eye_x) * m_view_x + (m_pVertex[i*4+1] -m_eye_y) * m_view_y + (m_pVertex[i*4+2] -m_eye_z) * m_view_z;
	}
	
	QuickSortIdx::sort(key, idxbuffer, 0, m_num_particle-1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idxbuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	delete[] idxbuffer;
	delete[] key;
}

void RenderParticle::drawPoints()
{
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glViewport(0,0,m_image_width, m_image_height);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f,0.0f, 0.0f);
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	//
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
	glEnable(GL_BLEND);
	glDepthMask( GL_FALSE );
	glDisable(GL_DEPTH_TEST);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
	
	shader->enable();
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
		
		glBindBufferARB(GL_ARRAY_BUFFER, tbo);
		glClientActiveTexture(GL_TEXTURE0);
				glTexCoordPointer(4, GL_FLOAT, 0, 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
				
				
				
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);
        glDrawElements(GL_POINTS, m_num_particle, GL_UNSIGNED_INT, 0 );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		
		
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	 glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	shader->disable();
	
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glViewport(0,0, m_image_width*2, m_image_height*2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-1, 1, -1, 1, 0.1, 10.0);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
		gluLookAt(0,0,1,
				  0,0,-1,
				  0, 1, 0);
	
	glBindTexture(GL_TEXTURE_2D, m_image_tex);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	
	glDepthMask( GL_TRUE );
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
	glPopAttrib();
}

void RenderParticle::setImageDim(int w, int h) 
{
	m_image_width = w/2; m_image_height = h/2;
	
	if(m_image_fbo) glDeleteFramebuffersEXT(1, &m_image_fbo);
	if(m_image_tex) glDeleteTextures(1, &m_image_tex);
	
	glGenFramebuffersEXT(1, &m_image_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
	
	glGenTextures(1, &m_image_tex);
	glBindTexture(GL_TEXTURE_2D, m_image_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, m_image_width, m_image_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_image_tex, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}