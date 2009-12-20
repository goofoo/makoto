/*
 *  RenderParticle.h
 *  pmap
 *
 *  Created by jian zhang on 12/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RENDER_PARTICLE_H
#define _RENDER_PARTICLE_H

#include "../shared/gBase.h"
#include "GLSLSprite.h"

class RenderParticle
{
public:
	RenderParticle();
	~RenderParticle();
	
	void initialize();
	void uninitialize();
	
	char isInitialized() const { return m_isInitialized; }
	
	void sort();
	void drawPoints();
	
	void setNumParticle(long val) { m_num_particle = val; }
	void setVertex(float* vertex);
	void setCoord(float* coord);
	
	void setViewVector(float x, float y, float z) {m_view_x=x; m_view_y=y; m_view_z=z;}
	void setEyePoint(float x, float y, float z) {m_eye_x=x; m_eye_y=y; m_eye_z=z;}
	void setImageDim(int w, int h);
	
private:
	long m_num_particle;
	float* m_pVertex;
	//float* m_pCoord;
	GLuint ibo;
	GLuint vbo;
	GLuint tbo;
	GLuint m_image_fbo;
	//GLuint m_depthBuffer;
	GLuint m_image_tex;
	char m_isInitialized;
	
	GLSLSprite* shader;
	
	float m_view_x, m_view_y, m_view_z;
	float m_eye_x, m_eye_y, m_eye_z;
	int m_image_width, m_image_height;
};

#endif