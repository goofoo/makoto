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
#include "../shared/zData.h"

RenderParticle::RenderParticle() : m_isInitialized(0),
m_image_fbo(0),
m_image_tex(0),
m_shadow_fbo(0),
m_shadow_tex(0)
{}

RenderParticle::~RenderParticle() {uninitialize();}

void RenderParticle::initialize()
{
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &tbo);
	shader = new GLSLSprite();
	
	glGenFramebuffersEXT(1, &m_shadow_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_shadow_fbo);
	
	glGenTextures(1, &m_shadow_tex);
	glBindTexture(GL_TEXTURE_2D, m_shadow_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_shadow_tex, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	m_isInitialized = 1;
}

void RenderParticle::uninitialize()
{
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &tbo);
	delete shader;
	if(m_shadow_fbo) glDeleteFramebuffersEXT(1, &m_shadow_fbo);
	if(m_shadow_tex) glDeleteTextures(1, &m_shadow_tex);
	if(m_image_fbo) glDeleteFramebuffersEXT(1, &m_image_fbo);
	if(m_image_tex) glDeleteTextures(1, &m_image_tex);
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
	
// calc sort vec
	XYZ dirLight(m_light_vec_x, m_light_vec_y, m_light_vec_z);
	dirLight.normalize();
	
	XYZ dirView(m_view_x, m_view_y, m_view_z);
	dirView.normalize();
	
// check invert
	m_invert = 0;
	if(dirView.dot(dirLight) < 0.f) {
		dirView *= -1.0f;
		m_invert = 1;
	}
	
	XYZ vec_sort = dirView + dirLight;
	vec_sort.normalize();
	
	float *key = new float[m_num_particle];
	for(i=0; i<m_num_particle; i++) {
// calc depth 
		key[i]= (m_pVertex[i*4] -m_eye_x) * vec_sort.x + (m_pVertex[i*4+1] -m_eye_y) * vec_sort.y + (m_pVertex[i*4+2] -m_eye_z) * vec_sort.z;
	}
	
	QuickSortIdx::sort(key, idxbuffer, 0, m_num_particle-1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idxbuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	delete[] idxbuffer;
	delete[] key;
}

void RenderParticle::render()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
	
	glViewport(0,0,m_image_width, m_image_height);
	glShadeModel(GL_SMOOTH);
	glColor4f(1,1,1,1);
	glClearColor(0.0f, 0.0f,0.0f, 0.0f);
	
	if(m_invert) glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	else glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

	glEnable(GL_BLEND);
	glDepthMask( GL_FALSE );
	glDisable(GL_DEPTH_TEST);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
	
	shader->enable();
	
	drawPoints();
	
	shader->disable();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_shadow_fbo);
	
	glViewport(0,0, 512, 512);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_light_size, m_light_size, -m_light_size, m_light_size, 1.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
		gluLookAt(m_light_x,m_light_y,m_light_z,
				  m_light_x + m_light_vec_x, m_light_y + m_light_vec_y, m_light_z + m_light_vec_z,
				  0, 1, 0);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
	shader->enableShadow();
	
	drawPoints();
	
	shader->disable();
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void RenderParticle::compose()
{
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
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

}

void RenderParticle::showShadow()
{
	glDisable(GL_BLEND);
	glViewport(0,0, 256, 256);
	
	glBindTexture(GL_TEXTURE_2D, m_shadow_tex);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	
	glDepthMask( GL_TRUE );
	
	glEnable(GL_DEPTH_TEST);
}

void RenderParticle::drawPoints()
{

	
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