/*
 *  GLSLSprite.h
 *  pmap
 *
 *  Created by jian zhang on 12/20/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLSL_SPRITE_H
#define _GLSL_SPRITE_H

#include "../shared/gBase.h"

struct GLSLSpritePARAM
{
	float density, shadow_scale, radius;
	float base_r, base_g, base_b;
	float light_r, light_g, light_b;
	float shadow_r, shadow_g, shadow_b;
	float side_x, side_y, side_z;
	float up_x, up_y, up_z;
	float lacunarity, dimension, frequency;
	float noise_x, noise_y, noise_z;
};

class GLSLSprite
{
public:
	GLSLSprite();
	~GLSLSprite();
	
	char initProgram();
	void enable();
	void setShadowMatrix(float* m);
	void setShadowTex(GLuint tex);
	void setNoiseTex(GLuint tex);
	void enableShadow();
	void disable();
	void setParam(GLSLSpritePARAM& param);
	
private:
	GLuint program;
	GLuint shadowProgram;
	char m_isValid;
};
#endif