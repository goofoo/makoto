/*
 *  GLSLSprite.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/20/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLSLSprite.h"

#ifndef __APPLE__
#include "../shared/gExtension.h"
#endif

const char *particleVS = 
// point size in world space 
//"uniform float pointRadius;     \n"
// scale to calculate size in pixels \n
//"uniform float pointScale;"   
"uniform vec4 spriteSide;"
"uniform vec4 spriteUp;"

"uniform mat4 shadowMatrix;"

"void main()                                                 \n"
"{                                                           \n"
"	vec4 wpos = vec4(gl_Vertex.xyz, 1.0);                   \n"
"    gl_Position = gl_ModelViewMatrix * wpos;      \n"

//" gl_Position = gl_ProjectionMatrix * gl_Position;"

// calculate window-space point size                    
//"    vec4 eyeSpacePos = gl_ModelViewMatrix * wpos;           \n"
//"    float dist = length(eyeSpacePos.xyz);                   \n"
//"    gl_PointSize = 128.0/dist;       \n"

// velocity and densit

"    gl_TexCoord[0] = gl_MultiTexCoord0; "
//"    gl_TexCoord[1] = eyeSpacePos;                           \n"

// transform point in shadow, calc ndc
"    gl_TexCoord[2] = vec4(shadowMatrix * wpos);"
"    gl_TexCoord[2].x = (gl_TexCoord[2].x + 1.0)*0.5;"
"    gl_TexCoord[2].y = (gl_TexCoord[2].y + 1.0)*0.5;"

// world point for noise
"	gl_TexCoord[3] = wpos;"

// side and up of 
"	gl_TexCoord[4] = spriteSide;"
"	gl_TexCoord[5] = spriteUp;"
"}";

const char *particleGS =      
"uniform float pointRadius;  \n"
"void main()                                                    \n"
"{                                                              \n"
"	float radius = pointRadius;                                \n"

    // eye space                                               \n
"    vec3 pos = gl_PositionIn[0].xyz;                           \n"
//"    vec3 pos2 = gl_TexCoordIn[0][0].xyz;                       \n"
//"    vec3 motion = pos - pos2;                                  \n"
//"    vec3 dir = normalize(motion);                              \n"
//"    float len = length(motion);                                \n"
//"pos = gl_Vertex.xyz;"
"vec3 x = vec3(radius,0,0);"  
"vec3 y = vec3(0,radius,0);"

" vec3 vel = gl_TexCoordIn[0][0].xyz;"

"vec3 I = cross( gl_TexCoordIn[0][4].xyz, gl_TexCoordIn[0][5].xyz);"

" I = I*dot(vel, I);"

"vec3 Side = (gl_TexCoordIn[0][4].xyz + I) * radius;"  
"vec3 Up = (gl_TexCoordIn[0][5].xyz + I) * radius;"                         

"    gl_TexCoord[0] = vec4(0, 0, 0, 1);                     \n"

"    gl_TexCoord[1] = gl_TexCoordIn[0][0];"

// point in shadow
"    gl_TexCoord[2] = gl_TexCoordIn[0][2];"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz + Side + Up, 1);"

"    gl_Position = gl_ProjectionMatrix * vec4(pos + x + y, 1);  \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(0, 1, 0, 1);                     \n"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz + Side - Up, 1);"

"    gl_Position = gl_ProjectionMatrix * vec4(pos + x - y, 1);  \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(1, 0, 0, 1);                     \n"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz - Side + Up, 1);"

"    gl_Position = gl_ProjectionMatrix * vec4(pos - x + y, 1); \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(1, 1, 0, 1);                     \n"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz - Side - Up, 1);"

"    gl_Position = gl_ProjectionMatrix * vec4(pos - x - y, 1); \n"
"    EmitVertex();                                              \n"
"}";

const char *particleFS = 
//"uniform float pointRadius;                                         \n"
"uniform vec3 baseColor;"
"uniform vec3 lightColor;"
"uniform vec3 shadowColor;"
"uniform float scaleDensity;"
"uniform sampler2D ShadowMap;"

// noise set
"uniform sampler3D EarthNight;"
"uniform float Lacunarity;"
"uniform float Dimension;"
"uniform float Frequency;"
"uniform vec3 NoiseOrigin;"

"vec4 fractal_func(vec3 pcoord)"
"{"
"	float f=1.0;"
"	vec4 fractal = texture3D(EarthNight, pcoord).rgba;" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f, Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f, Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f, Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f, Dimension);" 
"return fractal;"
"}"

"void main()                                                        \n"
"{                                                                  \n"
"    vec3 N;                                                        \n"
"    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);    \n"
"    float r2 = dot(N.xy, N.xy);                                    \n"

   // kill pixels outside circle  
"    if (r2 > 1.0) discard;       \n"

"    float alpha = 1.0 - r2*r2;"
"alpha *= alpha*alpha;"
"	float density = gl_TexCoord[1].w * scaleDensity;"

"alpha *= density;"

"	vec4 fractal = fractal_func((gl_TexCoord[3].xyz + NoiseOrigin) * Frequency);"

"alpha = clamp(alpha * fractal.a, 0.0, 1.0);"

"	float inshd = texture2D(ShadowMap, gl_TexCoord[2].xy).r;"
"	inshd = exp(-inshd*4.0);"
"	vec3 cs = baseColor + lightColor * inshd + shadowColor * (1.0 - inshd);"

//"cs = fractal.xyz;"
"    gl_FragColor = vec4(cs*alpha , alpha);              \n"
"} ";

const char *particleShadowFS = 
"uniform float scaleDensity;"
"uniform float scaleShadow;                                         \n"
"void main()                                                        \n"
"{                                                                  \n"
"    vec3 N;                                                        \n"
"    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);    \n"
"    float r2 = dot(N.xy, N.xy);                                    \n"

   // kill pixels outside circle  
"    if (r2 > 1.0) discard;       \n"

"    float alpha = clamp((1.0 - r2*r2), 0.0, 1.0);"
"alpha *= alpha*alpha;"
"	float density = gl_TexCoord[1].w * scaleDensity;"

"alpha *= density;"

"alpha = clamp(alpha, 0.0, 1.0);"

"    gl_FragColor = vec4(vec3(0.01*scaleShadow)*alpha , alpha);              \n"
"} ";


GLSLSprite::GLSLSprite() 
{
	m_isValid = initProgram();
}

GLSLSprite::~GLSLSprite() 
{
	glDeleteProgram( program );
	glDeleteProgram( shadowProgram );
}

char GLSLSprite::initProgram()
{
	GLuint vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &particleVS, NULL);
	glCompileShader(vertex_shader);
	
	GLint vertexshader_compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, (GLint*)&vertexshader_compiled);
	if(!vertexshader_compiled) return 0;
	
	GLuint fragment_shader   = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &particleFS, NULL);
	glCompileShader(fragment_shader);
	
	GLint fragmentshader_compiled;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, (GLint*)&fragmentshader_compiled);
	if(!fragmentshader_compiled) return 0;
	
	program = glCreateProgram();
	
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
	GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER_EXT);
	glShaderSource(geomShader, 1, &particleGS, 0);
	glCompileShader(geomShader);
	
	GLint geomshader_compiled;
	glGetShaderiv(geomShader, GL_COMPILE_STATUS, (GLint*)&geomshader_compiled);
	if(!geomshader_compiled) return 0;
	
	
	glAttachShader(program, geomShader);
	
	glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT,  GL_POINTS);
	glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP); 
	glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, 4);
	
	glLinkProgram(program);
	
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked) return 0;
	
	GLuint shadow_fragment_shader   = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shadow_fragment_shader, 1, &particleShadowFS, NULL);
	glCompileShader(shadow_fragment_shader);
	
	GLint shadowfragmentshader_compiled;
	glGetShaderiv(shadow_fragment_shader, GL_COMPILE_STATUS, (GLint*)&shadowfragmentshader_compiled);
	if(!shadowfragmentshader_compiled) return 0;
	
	shadowProgram = glCreateProgram();
	glAttachShader(shadowProgram, vertex_shader);
	glAttachShader(shadowProgram, shadow_fragment_shader);
	
	glAttachShader(shadowProgram, geomShader);
	
	glProgramParameteriEXT(shadowProgram, GL_GEOMETRY_INPUT_TYPE_EXT,  GL_POINTS);
	glProgramParameteriEXT(shadowProgram, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP); 
	glProgramParameteriEXT(shadowProgram, GL_GEOMETRY_VERTICES_OUT_EXT, 4);
	
	glLinkProgram(shadowProgram);
	
	glGetProgramiv(shadowProgram, GL_LINK_STATUS, &linked);
	if(!linked) return 0;
	
	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, "scaleDensity"), 1.0);
	glUniform1f(glGetUniformLocation(program, "pointRadius"), 1.0);
	glUniform3f(glGetUniformLocation(program, "baseColor"), 1.0, 0.5, 0.0);
	glUniform3f(glGetUniformLocation(program, "lightColor"), 0.0, 1.0, 1.0);
	glUniform3f(glGetUniformLocation(program, "shadowColor"), 0.0, 0.0, 0.0);
	glUniform1f(glGetUniformLocation(program, "Lacunarity"), 2.0);
	glUniform1f(glGetUniformLocation(program, "Dimension"), 1.0);
	glUniform1f(glGetUniformLocation(program, "Frequency"), 1.0/256.0);
	glUniform1i(glGetUniformLocation(program, "ShadowMap"), 0);
	glUniform1i(glGetUniformLocation(program, "EarthNight"), 1);
	glUniform4f(glGetUniformLocation(program, "spriteSide"), 1.0, 0.0, 0.0, 1.0);
	glUniform4f(glGetUniformLocation(program, "spriteUp"), 0.0, 1.0, 0.0, 1.0);
	glUniform3f(glGetUniformLocation(program, "NoiseOrigin"), 0.0, 0.0, 0.0);
	
	glUseProgram(shadowProgram);
	glUniform1f(glGetUniformLocation(shadowProgram, "scaleDensity"), 1.0);
	glUniform1f(glGetUniformLocation(shadowProgram, "pointRadius"), 1.0);
	glUniform1f(glGetUniformLocation(shadowProgram, "scaleShadow"), 1.0);
	glUniform4f(glGetUniformLocation(shadowProgram, "spriteSide"), 1.0, 0.0, 0.0, 1.0);
	glUniform4f(glGetUniformLocation(shadowProgram, "spriteUp"), 0.0, 1.0, 0.0, 1.0);
	glUseProgram(0);
	
	return 1;
}

void GLSLSprite::enable()
{
	glUseProgram(program);
}

void GLSLSprite::setShadowMatrix(float* m)
{
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "shadowMatrix"), 1, 0, (float*)m);
	glUseProgram(0);
	
	glUseProgram(shadowProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "shadowMatrix"), 1, 0, (float*)m);
	glUseProgram(0);
}

void GLSLSprite::setShadowTex(GLuint tex)
{
	//glUseProgram(program);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	//glUseProgram(0);
}

void GLSLSprite::setNoiseTex(GLuint tex)
{
	//glUseProgram(program);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, tex);
	//glUseProgram(0);
}

void GLSLSprite::enableShadow()
{
	glUseProgram(shadowProgram);
}

void GLSLSprite::disable()
{
	glUseProgram(0);
}

void GLSLSprite::setParam(GLSLSpritePARAM& param)
{
	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, "scaleDensity"), param.density);
	glUniform1f(glGetUniformLocation(program, "pointRadius"), param.radius);
	glUniform3f(glGetUniformLocation(program, "baseColor"), param.base_r, param.base_g, param.base_b);
	glUniform3f(glGetUniformLocation(program, "lightColor"), param.light_r, param.light_g, param.light_b);
	glUniform3f(glGetUniformLocation(program, "shadowColor"), param.shadow_r, param.shadow_g, param.shadow_b);
	glUniform4f(glGetUniformLocation(program, "spriteSide"), param.side_x, param.side_y, param.side_z, 1.0);
	glUniform4f(glGetUniformLocation(program, "spriteUp"), param.up_x, param.up_y, param.up_z, 1.0);
	glUniform1f(glGetUniformLocation(program, "Lacunarity"), param.lacunarity);
	glUniform1f(glGetUniformLocation(program, "Dimension"), param.dimension);
	glUniform1f(glGetUniformLocation(program, "Frequency"), param.frequency/256.0);
	glUniform3f(glGetUniformLocation(program, "NoiseOrigin"), param.noise_x, param.noise_y, param.noise_z);
	//glUseProgram(0);
	
	glUseProgram(shadowProgram);
	glUniform1f(glGetUniformLocation(shadowProgram, "scaleDensity"), param.density);
	glUniform1f(glGetUniformLocation(shadowProgram, "scaleShadow"), param.shadow_scale);
	glUniform1f(glGetUniformLocation(shadowProgram, "pointRadius"), param.radius);
	//glUniform4f(glGetUniformLocation(shadowProgram, "spriteSide"), param.side_x, param.side_y, param.side_z, 1.0);
	//glUniform4f(glGetUniformLocation(shadowProgram, "spriteUp"), param.up_x, param.up_y, param.up_z, 1.0);
	glUseProgram(0);
}
