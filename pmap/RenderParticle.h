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

class RenderParticle
{
public:
	RenderParticle();
	~RenderParticle();
	
	void render();
	
	void setNumParticle(long val) { m_num_particle = val; }
	void setVertex(float* vertex) { m_pVertex = vertex; }
	
private:
	long m_num_particle;
	float* m_pVertex;
};

#endif