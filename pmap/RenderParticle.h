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
	void render();
	void compose();
	void showShadow();
	void drawPoints(int start, int count);
	
	void setNumParticle(long val) { m_num_particle = val; }
	void setVertex(float* vertex);
	void setCoord(float* coord);
	
	void setViewVector(float x, float y, float z) {m_view_x=x; m_view_y=y; m_view_z=z;}
	void setEyePoint(float x, float y, float z) {m_eye_x=x; m_eye_y=y; m_eye_z=z;}
	void setUpVector(float x, float y, float z) {m_up_x=x; m_up_y=y; m_up_z=z;}
	void setLightPoint(float x, float y, float z) {m_light_x=x; m_light_y=y; m_light_z=z;}
	void setLightVector(float x, float y, float z) {m_light_vec_x=x - m_light_x; m_light_vec_y=y - m_light_y; m_light_vec_z=z - m_light_z;}
	void setLightSize(float val) {m_light_size = val;}
	void setImageDim(int w, int h);
	void setFrustum(float l, float r, float b, float t, float n, float f);
	
	void getLightPoint(float& x, float& y, float& z) const {x = m_light_x; y = m_light_y; z = m_light_z;}
	
	void setPrimProjection();
	void setShadowProjection();
	
private:
	long m_num_particle;
	float* m_pVertex;
	GLuint ibo;
	GLuint vbo;
	GLuint tbo;
	GLuint m_image_fbo;
	GLuint m_image_tex;
	GLuint m_shadow_fbo;
	GLuint m_shadow_tex;
	char m_isInitialized;
	
	GLSLSprite* shader;
	
	float m_view_x, m_view_y, m_view_z;
	float m_eye_x, m_eye_y, m_eye_z;
	float m_up_x, m_up_y, m_up_z;
	int m_image_width, m_image_height;
	float m_light_x, m_light_y, m_light_z;
	float m_light_vec_x, m_light_vec_y, m_light_vec_z;
	float m_light_size;
	char m_invert;
	float m_left, m_right, m_bottom, m_top, m_near, m_far;
};

#endif