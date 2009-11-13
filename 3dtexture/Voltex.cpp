/*
 *  Voltex.cpp
 *  pmap
 *
 *  Created by jian zhang on 10/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Voltex.h"
#include "../shared/FNoise.h"

typedef struct glExtensionEntry {
    char* name;
    GLfloat promoted;
    GLboolean supported;
} glExtensionEntry;

glExtensionEntry entriesNeeded[] = {
{"GL_EXT_framebuffer_object",   0.0, 0},
{"GL_ARB_texture_cube_map",     1.3, 0},
{"GL_ARB_shader_objects",       2.0, 0},
{"GL_ARB_shading_language_100", 2.0, 0},
{"GL_ARB_fragment_shader",      2.0, 0},
{"GL_ARB_vertex_buffer_object",      2.0, 0},
{"GL_ARB_multitexture",      1.3, 0},
{"GL_ARB_multisample",      1.3, 0},
{"GL_ARB_vertex_program",      2.0, 0},
{"GL_ARB_fragment_program",      2.0, 0},
{"GL_ARB_texture_rectangle",      0.0, 0},
};

GLhandleARB vertex_shader, fragment_shader, program_object;

const char *vert_source =
"uniform vec3 LightPosition;"
"uniform mat4 objspace;"
//"uniform float VFreq;"

"varying vec4 CParticle;"
"varying vec2 ViewCoord;"
"varying vec3  TexCoord;"
"varying vec3  TexCoordNoise;"
"varying vec3  LightVec;"
"varying vec3 CIBL;"
"varying float OScale;"

"void main(void)"
"{"
"	CParticle = gl_Color;"
"	ViewCoord = gl_MultiTexCoord0.xy;"
"	vec3 ecPos      = vec3 (gl_ModelViewMatrix * gl_Vertex);"
"	LightVec   = normalize(LightPosition - ecPos);"
"	vec4 pw = vec4(gl_MultiTexCoord0.xyz, 0.0);"
"	TexCoordNoise = vec3(objspace * pw);"
"    TexCoord        =  TexCoordNoise;"
"    TexCoordNoise  = TexCoordNoise/64.0*gl_MultiTexCoord2.w + gl_MultiTexCoord2.xyz;"
"	CIBL = gl_MultiTexCoord1.xyz;"
"	OScale = gl_MultiTexCoord1.w;"
"    gl_Position     = ftransform();"
"}";

const char *frag_source =
"uniform sampler3D EarthNight;"
"uniform float KNoise;"
"uniform float KDiffuse;"
"uniform float Lacunarity;"
"uniform float Dimension;"
"uniform float GScale;"
"uniform vec3 CCloud;"

"varying vec3 CIBL;"
"varying vec4 CParticle;"
"varying vec2 ViewCoord;"
"varying vec3  TexCoord;"
"varying vec3  TexCoordNoise;"
"varying vec3  LightVec;"
"varying float OScale;"

"vec3 fractal_func(vec3 pcoord)"
"{"
"	float f = 1.0;"
"	vec3 fractal = texture3D(EarthNight, pcoord).rgb;" 

"	f *= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgb-vec3(0.5))/pow(f,3.0-Dimension);" 

"	f *= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgb-vec3(0.5))/pow(f,3.0-Dimension);" 
 
"	f *= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgb-vec3(0.5))/pow(f,3.0-Dimension);" 

//"	f *= Lacunarity;"

//"	mag = pow(f,3.0-Dimension);"
//"	fractal += (texture3D(EarthNight, pcoord*f).rgb-vec3(0.5))/mag;" 
 
"	return fractal;"
"}"

"float density_func(vec3 coord)"
"{"
"	float freq= 1.0;"
"	float turbulence = abs(texture3D(EarthNight, coord).a - 0.5);"
"	coord *= 2.0;"
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord).a - 0.5)/freq;"
"	coord *= 2.0; " 
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord).a - 0.5)/freq;"
"	coord *= 2.0; " 
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord).a - 0.5)/freq;"
"	coord *= 2.0; " 
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord).a - 0.5)/freq;"

"return turbulence;"
"}"

"void main (void)"
"{" 
"	float edgexy = 1.0-smoothstep(0.25, 0.5, length(ViewCoord.xy));"

"	vec3 fractal = fractal_func(TexCoordNoise);"

"	float dens = density_func(TexCoordNoise);"

"	vec3 nn = normalize(TexCoord + (fractal.xyz - vec3(0.5))*KNoise);"
"	float NdotL           = (dot(LightVec, nn) + 1.0) * 0.5;"

"	float dist = length(TexCoord + (fractal.xyz - vec3(0.5))*1.5*KNoise);"

"    gl_FragColor = vec4 (CCloud * CParticle.rgb + CIBL * (1.0 + smoothstep(0.6, 1.0, NdotL)*KDiffuse), CParticle.a*GScale*OScale*dens*edgexy*(1.0 - smoothstep(0.6, 1.0, dist/0.5)));"
//"    gl_FragColor = vec4 (CParticle, 0.05);"
"}";

static GLuint noisepool;

static FNoise noise;

Voltex::Voltex() : fHasDiagnosis(0), fHasExtensions(0), 
fKNoise(0.5), fKDiffuse(0.5), fLacunarity(1.25), fDimension(0.0)
{
	fLightPos.x = fLightPos.y = fLightPos.z = 100.f;
}

Voltex::~Voltex() 
{
	if (program_object) glDeleteObjectARB(program_object);
}

void Voltex::start(Z3DTexture* db) const
{
	glBindTexture(GL_TEXTURE_3D, noisepool);
	glUseProgramObjectARB(program_object);	
// set global values
	glUniform1fARB(glGetUniformLocationARB(program_object, "KNoise"), fKNoise);
	glUniform3fARB(glGetUniformLocationARB(program_object, "LightPosition"), fLightPos.x, fLightPos.y, fLightPos.z);
	glUniform1fARB(glGetUniformLocationARB(program_object, "KDiffuse"), fKDiffuse);
	glUniform1fARB(glGetUniformLocationARB(program_object, "GScale"), fDensity);
	glUniform3fARB(glGetUniformLocationARB(program_object, "CCloud"), fCCloud.x, fCCloud.y, fCCloud.z);
	glUniform1fARB(glGetUniformLocationARB(program_object, "Lacunarity"), fLacunarity);
	glUniform1fARB(glGetUniformLocationARB(program_object, "Dimension"), fDimension);
	
	db->setProgram(program_object);
}

void Voltex::end() const
{
	glUseProgramObjectARB(NULL);
}


char Voltex::diagnose(string& log)
{
	float core_version;
	sscanf((char *)glGetString(GL_VERSION), "%f", &core_version);
	char sbuf[64];
	sprintf(sbuf, "%s version %s\n", (char *)glGetString(GL_RENDERER), (char *)glGetString(GL_VERSION));
	log = sbuf;
	
	const GLubyte * strExt = glGetString (GL_EXTENSIONS);
	int j = sizeof(entriesNeeded)/sizeof(glExtensionEntry);

	int supported = 1;
	for (int i = 0; i < j; i++) {
		entriesNeeded[i].supported = gluCheckExtension((GLubyte*)entriesNeeded[i].name, strExt) |
		(entriesNeeded[i].promoted && (core_version >= entriesNeeded[i].promoted));
		sprintf(sbuf, "%-32s %d\n", entriesNeeded[i].name, entriesNeeded[i].supported);
		log += sbuf;
		supported &= entriesNeeded[i].supported;
	}
	
	if(core_version < 1.4) {
		sprintf(sbuf, "OpenGL version too low, this thing may not work correctly!\n");
		log += sbuf;
	}
	
	if(supported != 1) return 0;

		fHasExtensions = 1;
		
// init textures
		glGenTextures(1, &noisepool);	
		glBindTexture(GL_TEXTURE_3D, noisepool);	
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
#define WIDTH	128
#define HEIGHT	128
#define DEPTH	128
		
		int g_seed;
		float *texels = new float[WIDTH*HEIGHT*DEPTH*4];
		int u, v, w;
		for(w=0; w<DEPTH; w++) {
			for(v=0; v<HEIGHT; v++) {
				for(u=0; u<WIDTH; u++) {
					g_seed = (w*(WIDTH * HEIGHT)+v*WIDTH+u)*4;
					texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4] = noise.randfint( g_seed );
					g_seed++;
					texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4+1] = noise.randfint( g_seed );
					g_seed++;
					texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4+2] = noise.randfint( g_seed );
					g_seed++;
					texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4+3] = noise.randfint( g_seed );
				}
			}
		}
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_FLOAT, texels);
		delete[] texels;

// init shaders
		GLint vertex_compiled, fragment_compiled;
		GLint linked;
		
		/* Delete any existing program object */
		if (program_object) {
			glDeleteObjectARB(program_object);
			program_object = NULL;
		}
		vertex_shader   = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		glShaderSourceARB(vertex_shader, 1, &vert_source, NULL);
		glCompileShaderARB(vertex_shader);
		glGetObjectParameterivARB(vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &vertex_compiled);
		
		fragment_shader   = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		glShaderSourceARB(fragment_shader, 1, &frag_source, NULL);
		glCompileShaderARB(fragment_shader);
		glGetObjectParameterivARB(fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &fragment_compiled);
		
		if (!vertex_compiled || !fragment_compiled) {
			if (vertex_shader) {
				glDeleteObjectARB(vertex_shader);
				vertex_shader   = NULL;
			}
			if (fragment_shader) {
				glDeleteObjectARB(fragment_shader);
				fragment_shader = NULL;
			}
			log += "shaders not compiled";
		}
		
		program_object = glCreateProgramObjectARB();
		if (vertex_shader != NULL)
		{
			glAttachObjectARB(program_object, vertex_shader);
			glDeleteObjectARB(vertex_shader);   /* Release */
		}
		if (fragment_shader != NULL)
		{
			glAttachObjectARB(program_object, fragment_shader);
			glDeleteObjectARB(fragment_shader); /* Release */
		}
		glLinkProgramARB(program_object);
		glGetObjectParameterivARB(program_object, GL_OBJECT_LINK_STATUS_ARB, &linked);
			
		if (!linked) {
			glDeleteObjectARB(program_object);
			program_object = NULL;
			log += "shaders not linked";
		}

		glUseProgramObjectARB(program_object);
		
		glUniform1iARB(glGetUniformLocationARB(program_object, "EarthNight"), 0);
		glUniform1fARB(glGetUniformLocationARB(program_object, "KNoise"), fKNoise);
		glUniform1fARB(glGetUniformLocationARB(program_object, "KDiffuse"), fKDiffuse);
		glUniform3fARB(glGetUniformLocationARB(program_object, "LightPosition"), fLightPos.x, fLightPos.y, fLightPos.z);

		glUseProgramObjectARB(NULL);
	
	fHasDiagnosis = 1;
	return 1;
}