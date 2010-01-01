/*
 *  shader.h
 *  triangle
 *
 *  Created by jian zhang on 12/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SHADER_H
#define _SHADER_H

GLint initShaders();
void initTextures();
void drawTexQuad();

float downsample(int x, int y, float *data);
#endif