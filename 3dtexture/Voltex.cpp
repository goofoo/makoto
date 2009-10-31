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
"varying vec3  TexCoord;"
"varying vec3  TexCoordNoise;"
"varying vec3 SurfaceColor;"
"uniform vec3 Origin;"
"uniform mat4 objspace;"
"void main(void)"
"{"
//"    TexCoord        = gl_MultiTexCoord1.xyz*1.5;"
//"    TexCoordNoise  = gl_MultiTexCoord0.xyz;"
"	vec4 pw = vec4(gl_MultiTexCoord0.xyz, 0.0);"
"	TexCoordNoise = vec3(objspace * pw);"
"    TexCoord        =  TexCoordNoise*1.5;"
"    TexCoordNoise  = (TexCoordNoise)/16.0+Origin;"
"	SurfaceColor = vec3(gl_Color);"
"    gl_Position     = ftransform();"
"}";

const char *frag_source =
"uniform sampler3D EarthNight;"

"varying vec3  TexCoord;"
"varying vec3  TexCoordNoise;"
"varying vec3 SurfaceColor;"
"void main (void)"
"{" 
"	vec3 pcoord = TexCoordNoise;"
"	float lacunarity = 2.0;"
"	float i=0.0;"
"	float freq= 1.0;"
"	float H = 1.0;"

"	vec3 fractal = texture3D(EarthNight, pcoord).rgb*pow(lacunarity, -H*i);" 
"	float turbulence = abs(texture3D(EarthNight, pcoord).a - 0.5)/freq;"
"	pcoord *= lacunarity; "

"	i++;"
"	freq *= 2.0;"

"	fractal += (texture3D(EarthNight, pcoord).rgb-vec3(0.5))*pow(lacunarity, -H*i);" 
"	turbulence += abs(texture3D(EarthNight, pcoord).a - 0.5)/freq;"
"	pcoord *= lacunarity; " 

"	i++;"
"	freq *= 2.0;"

"	fractal += (texture3D(EarthNight, pcoord).rgb-vec3(0.5))*pow(lacunarity, -H*i);" 
"	turbulence += abs(texture3D(EarthNight, pcoord).a - 0.5)/freq;"
"	pcoord *= lacunarity; "
 
"	i++;"
"	freq *= 2.0;"

"	fractal += (texture3D(EarthNight, pcoord).rgb-vec3(0.5))*pow(lacunarity, -H*i);" 
"	turbulence += abs(texture3D(EarthNight, pcoord).a - 0.5)/freq;" 
"	pcoord *= lacunarity; "

"	i++;"
"	freq *= 2.0;"

"	fractal += (texture3D(EarthNight, pcoord).rgb-vec3(0.5))*pow(lacunarity, -H*i);" 
"	turbulence += abs(texture3D(EarthNight, pcoord).a - 0.5)/freq;"
"	pcoord *= lacunarity; " 

"	i++;"
"	freq *= 2.0;"

"	fractal += (texture3D(EarthNight, pcoord).rgb-vec3(0.5))*pow(lacunarity, -H*i);" 
"	turbulence += abs(texture3D(EarthNight, pcoord).a - 0.5)/freq;"
//"	pcoord *= lacunarity; " 
//"	i++;"
//"	freq *= 2.0;"

"	float dist = length(TexCoord + (fractal.xyz - vec3(0.5))*0.5);"
"	if(dist > 0.5) dist = 0.5;"
"    gl_FragColor = vec4 (SurfaceColor, turbulence*(1.0 - dist/0.5*dist/0.5));"
"}";

static GLuint noisepool;

static FNoise noise;

Voltex::Voltex() : db(0), fHasDiagnosis(0), fHasExtensions(0) {}
Voltex::~Voltex() 
{
	if(db) delete db;
}

char Voltex::load(const char* filename)
{
	if(db) delete db;
	db = new Z3DTexture();
	if(!db->load(filename)) return 0;
	return 1;
}

void Voltex::setDraw(const char* name)
{
	if(db) db->setDraw(name);
}

void Voltex::setHDRLighting(XYZ* coe)
{
	if(db) db->setHDRLighting(coe);
}

void Voltex::setProjection(MATRIX44F cameraspace, float fov, int iperspective)
{
	if(db) db->setProjection(cameraspace, fov, iperspective);
}

void Voltex::drawCube() const
{
	if(db) db->drawCube();
}

void Voltex::drawSprite() const
{
	glBindTexture(GL_TEXTURE_3D, noisepool);
		
	if(db) {
		db->setProgram(program_object);
		db->drawSprite();
	}
	
	
}

void Voltex::getBBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const
{
	if(db) db->getBBox(xmin, xmax, ymin, ymax, zmin, zmax);
}

void Voltex::diagnose(string& log)
{
	float core_version;
	sscanf((char *)glGetString(GL_VERSION), "%f", &core_version);
	char sbuf[64];
	sprintf(sbuf, "%s version %s", (char *)glGetString(GL_RENDERER), (char *)glGetString(GL_VERSION));
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

	if(supported == 1) {
		fHasExtensions = 1;
		
// init textures
		glGenTextures(1, &noisepool);	
		glBindTexture(GL_TEXTURE_3D, noisepool);	
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
#define WIDTH	64
#define HEIGHT	64
#define DEPTH	64
		
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
		
		glUseProgramObjectARB(NULL);
	}
	
	fHasDiagnosis = 1;
}