//
//  SimpleNoise.m
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "SimpleNoise.h"
/*

*/
@implementation SimpleNoise
- (id) init
{
	[super init];
	
	poolw = poolh = 128;
	glInited = 0;
	
	vert_source =

@"uniform float Frequency;"

"varying vec3  TexCoord;"

"void main(void)"
"{"
"    TexCoord        = (gl_MultiTexCoord0.xyz + vec3(-31.7, -19.1, 23.7))*Frequency;"
"    gl_Position     = ftransform();"
"}";

	frag_source =
@"uniform sampler2D WhiteNoise;"
"uniform float Lacunarity;"
"uniform float Dimension;"

"varying vec3  TexCoord;"

"float fractal_func(vec2 pcoord)"
"{"
"	float f=1.0;"
"	float fractal = texture2D(WhiteNoise, pcoord).r;" 

"	f*= Lacunarity;"

"	fractal += (texture2D(WhiteNoise, pcoord*f).r- 0.5 )/pow(f, Dimension);" 


"	f*= Lacunarity;"

"	fractal += (texture2D(WhiteNoise, pcoord*f).r- 0.5 )/pow(f, Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture2D(WhiteNoise, pcoord*f).r- 0.5 )/pow(f, Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture2D(WhiteNoise, pcoord*f).r- 0.5 )/pow(f, Dimension);" 
"return fractal;"
"}"

"void main (void)"
"{" 
"	float s = fractal_func(TexCoord.xy);"
"    gl_FragColor = vec4 (s, s, s,  1.0);"
"}";

	FloatAttr *lacunarity = [[FloatAttr alloc] init];
	lacunarity.modelName =@"Lacunarity";
	lacunarity.val = 2.0;
	lacunarity.min = 1.0;
	lacunarity.max = 4.0;
	FloatAttr *dimension = [[FloatAttr alloc] init];
	dimension.modelName =@"Dimension";
	dimension.val = 1.0;
	dimension.min = 0.0;
	dimension.max = 3.0;
	FloatAttr *freq = [[FloatAttr alloc] init];
	freq.modelName =@"Frequency";
	freq.val = 1.0;
	freq.min = 0.1;
	freq.max = 4.0;
	
	float_attr_array = [NSArray arrayWithObjects:
	lacunarity,
	dimension,
	freq,
	nil];
	
	[float_attr_array retain];

	return self;
}

- (NSString *) name
{
	return @"SimpleNoise";
}

- (void) draw
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texname);
	glUseProgram(program);
	[self updateUniformFloat];
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
	glUseProgram(0);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, poolw, poolh, 0, GL_LUMINANCE, GL_FLOAT, texels);
	
	free(texels);
	
	[self initShaders];
	[self uniformParam];
	
	glInited = 1;
}

- (void)uniformParam
{
/* Setup uniforms */
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "WhiteNoise"), 0);
		glUniform1f(glGetUniformLocation(program, "Lacunarity"), 2.0);
		glUniform1f(glGetUniformLocation(program, "Dimension"), 1.0);
		glUniform1f(glGetUniformLocation(program, "Frequency"), 32.0);
		glUseProgram(0);
}
@end
