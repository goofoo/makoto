//
//  WhiteNoise.m
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "WhiteNoise.h"


@implementation WhiteNoise
- (id) init
{
	[super init];
	modelName = @"WhiteNoise";
	poolw = poolh = 128;
	
	glInited = 0;

	return self;
}

- (NSString *) name
{
	return @"WhiteNoise";
}

- (void) draw
{
	glBindTexture(GL_TEXTURE_2D, texname);
	glBegin(GL_QUADS);
	glMultiTexCoord3f(GL_TEXTURE0, 0, 0, 0);
	glVertex3f(-1, -1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 1, 0, 0);
	glVertex3f(1, -1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 1, 1, 0);
	glVertex3f(1, 1, 0);
	glMultiTexCoord3f(GL_TEXTURE0, 0, 1, 0);
	glVertex3f(-1, 1, 0);
	glEnd();
}

- (void) initGL
{
	float *texels = malloc(poolw*poolh*sizeof(float));
	int u, v;
	
	for(v=0; v<poolh; v++) {
		for(u=0; u<poolw; u++) {
			texels[v*poolw+u] = (float)(random()%1931)/1931.f;
		}
	}
	
	glGenTextures(1, &texname);	
	glBindTexture(GL_TEXTURE_2D, texname);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, poolw, poolh, 0, GL_LUMINANCE, GL_FLOAT, texels);
	
	free(texels);
	
	glInited = 1;
}

@end
