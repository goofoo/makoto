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

class GLSLSprite
{
public:
	GLSLSprite();
	~GLSLSprite();
	
	char initProgram();
	void enable();
	void setShadowMatrix(float* m);
	void setShadowTex(GLuint tex);
	void enableShadow();
	void disable();
	
private:
	GLuint program;
	GLuint shadowProgram;
	char m_isValid;
};
#endif