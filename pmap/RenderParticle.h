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
	void setVertex(float* vertex) { m_pVertex = vertex; }
	void setCoord(float* coord) { m_pCoord = coord; }
	
private:
	long m_num_particle;
	float* m_pVertex;
	float* m_pCoord;
	GLuint ibo;
	char m_isInitialized;
};

#endif