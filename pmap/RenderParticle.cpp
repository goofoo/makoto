/*
 *  RenderParticle.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderParticle.h"

RenderParticle::RenderParticle() : m_isInitialized(0) {}
RenderParticle::~RenderParticle() {uninitialize();}

void RenderParticle::initialize()
{
	glGenBuffers(1, &ibo);
	shader = new GLSLSprite();
	m_isInitialized = 1;
}

void RenderParticle::uninitialize()
{
	glDeleteBuffers(1, &ibo);
	delete shader;
}

void RenderParticle::sort()
{
	unsigned int size = m_num_particle * sizeof(unsigned int);
	unsigned int* idxbuffer = new unsigned int[m_num_particle];
	unsigned int i;
	for(i=0; i<m_num_particle; i++) idxbuffer[i]= i;
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idxbuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	delete[] idxbuffer;
}

void RenderParticle::drawPoints()
{
	//glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f,0.0f, 0.0f);
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glDepthMask( GL_FALSE );
	glDisable(GL_DEPTH_TEST);
	
	shader->enable();
	
	glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (float*)m_pVertex);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(4, GL_FLOAT, 0, (float*)m_pCoord);
				
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);
        glDrawElements(GL_POINTS, m_num_particle, GL_UNSIGNED_INT, 0 );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
		
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	shader->disable();
	
	glDepthMask( GL_TRUE );
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glPopAttrib();
}