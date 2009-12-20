/*
 *  GLSLSprite.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/20/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLSLSprite.h"

const char *particleVS = 
// point size in world space 
//"uniform float pointRadius;     \n"
// scale to calculate size in pixels \n
//"uniform float pointScale;"   
"uniform vec4 eyePos;                                        \n"
"void main()                                                 \n"
"{                                                           \n"
"	vec4 wpos = vec4(gl_Vertex.xyz, 1.0);                   \n"
"    gl_Position = gl_ModelViewMatrix * wpos;      \n"

//" gl_Position = gl_ProjectionMatrix * gl_Position;"

// calculate window-space point size                    
"    vec4 eyeSpacePos = gl_ModelViewMatrix * wpos;           \n"
//"    float dist = length(eyeSpacePos.xyz);                   \n"
//"    gl_PointSize = 128.0/dist;       \n"

"    gl_TexCoord[0] = gl_MultiTexCoord0; // sprite texcoord  \n"
"    gl_TexCoord[1] = eyeSpacePos;                           \n"
"}";

const char *particleGS =      
"uniform float pointRadius;  \n"
"void main()                                                    \n"
"{                                                              \n"
"	float radius = 2.0;                                \n"

    // eye space                                               \n
"    vec3 pos = gl_PositionIn[0].xyz;                           \n"
//"    vec3 pos2 = gl_TexCoordIn[0][0].xyz;                       \n"
//"    vec3 motion = pos - pos2;                                  \n"
//"    vec3 dir = normalize(motion);                              \n"
//"    float len = length(motion);                                \n"
//"pos = gl_Vertex.xyz;"
"vec3 x = vec3(radius,0,0);"  
"vec3 y = vec3(0,radius,0);"                        

"    gl_TexCoord[0] = vec4(0, 0, 0, 1);                     \n"

"    gl_TexCoord[1] = gl_TexCoordIn[0][0];"

"    gl_Position = gl_ProjectionMatrix * vec4(pos + x + y, 1);  \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(0, 1, 0, 1);                     \n"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos + x - y, 1);  \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(1, 0, 0, 1);                     \n"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos - x + y, 1); \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(1, 1, 0, 1);                     \n"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos - x - y, 1); \n"
"    EmitVertex();                                              \n"
"}";

const char *particleFS = 
"uniform float pointRadius;                                         \n"
"void main()                                                        \n"
"{                                                                  \n"
"    vec3 N;                                                        \n"
"    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);    \n"
"    float r2 = dot(N.xy, N.xy);                                    \n"

   // kill pixels outside circle  
"    if (r2 > 1.0) discard;       \n"

"    float alpha = clamp((1.0 - r2*r2), 0.0, 1.0);"
"alpha *= alpha;"
"	float density = gl_TexCoord[1].w * 2.0;"

"alpha *= density;"

"	vec3 cvel = normalize(gl_TexCoord[1].xyz);"
"	cvel = cvel*0.5 + vec3(0.5);"

"    gl_FragColor = vec4(cvel*alpha , alpha);              \n"
"} ";


GLSLSprite::GLSLSprite() 
{
	m_isValid = initProgram();
}

GLSLSprite::~GLSLSprite() 
{
	glDeleteProgram( program );
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
	
	return 1;
}

void GLSLSprite::enable()
{
	glUseProgram(program);
}

void GLSLSprite::disable()
{
	glUseProgram(0);
}
