#import "myGLView.h"
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <OpenGL/glext.h>

#import "noiseTest.h"

//GLhandleARB vertex_shader, fragment_shader, program_object;



// particle vertex shader
const char *particleVS = 
// point size in world space 
//"uniform float pointRadius;     \n"
// scale to calculate size in pixels \n
//"uniform float pointScale;"  
"uniform mat4 objspace;" 
"uniform vec4 eyePos;                                        \n"
"void main()                                                 \n"
"{                                                           \n"
"	vec4 wpos = vec4(gl_Vertex.xyz, 1.0);                   \n"
"    gl_Position = gl_ModelViewMatrix * wpos;      \n"
//"    gl_Position = gl_ProjectionMatrix * gl_Position;"
// calculate window-space point size                    
"    vec4 eyeSpacePos = gl_ModelViewMatrix * wpos;           \n"
//"    float dist = length(eyeSpacePos.xyz);                   \n"
//"    gl_PointSize = 128.0/dist;       \n"

"    gl_TexCoord[0] = gl_MultiTexCoord0; // sprite texcoord  \n"
"    gl_TexCoord[1] = eyeSpacePos;                           \n"
"	gl_TexCoord[2] = vec4(objspace * wpos);"
"	gl_TexCoord[3] = wpos;"

"}                                                           \n";

const char *particleGS = 
//"#version 120\n"
//"#extension GL_EXT_geometry_shader4 : enable\n"
 // point size in world space      
"uniform float pointRadius;  \n"
"void main()                                                    \n"
"{                                                              \n"
"	float radius = 1.0;                                \n"

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
"    gl_TexCoord[1] = gl_TexCoordIn[0][0].xyzw;"
"    gl_TexCoord[2] = gl_TexCoordIn[0][2].xyzw;"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz + x/16.0 + y/16.0, 1);"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos + x + y, 1);  \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(0, 1, 0, 1);                     \n"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz + x/16.0 - y/16.0, 1);"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos + x - y, 1);  \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(1, 0, 0, 1);                     \n"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz - x/16.0 + y/16.0, 1);"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos - x + y, 1); \n"
"    EmitVertex();                                              \n"

"    gl_TexCoord[0] = vec4(1, 1, 0, 1);                     \n"
"    gl_TexCoord[3] = vec4(gl_TexCoordIn[0][3].xyz - x/16.0 - y/16.0, 1);"
//"    gl_TexCoord[1] = gl_PositionIn[0];                         \n"
"    gl_Position = gl_ProjectionMatrix * vec4(pos - x - y, 1); \n"
"    EmitVertex();                                              \n"



"}                                                              \n";

const char *particleFS = 
"uniform float pointRadius;                                         \n"
"uniform sampler3D EarthNight;"

"uniform float Lacunarity;"
"uniform float Dimension;"
"vec4 fractal_func(vec3 pcoord)"
"{"
"	float f=1.0;"
"	vec4 fractal = texture3D(EarthNight, pcoord).rgba;" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f,3.0-Dimension);" 


"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f,3.0-Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f,3.0-Dimension);" 

"	f*= Lacunarity;"

"	fractal += (texture3D(EarthNight, pcoord*f).rgba-vec4(0.5))/pow(f,3.0-Dimension);" 
"return fractal;"
"}"

"float density_func(vec3 coord)"
"{"
"	float freq= 1.0;"
"	float turbulence = abs(texture3D(EarthNight, coord).a - 0.5);"
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord*freq).a - 0.5)/freq;" 
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord*freq).a - 0.5)/freq;"
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord*freq).a - 0.5)/freq;"
"	freq *= 2.0;"

"	turbulence += abs(texture3D(EarthNight, coord*freq).a - 0.5)/freq;"
"return turbulence;"
"}"

"void main()                                                        \n"
"{                                                                  \n"
// calculate eye-space sphere normal from texture coordinates
"    vec3 N;                                                        \n"
"    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);    \n"
"    float r2 = dot(N.xy, N.xy);                                    \n"
"    if (r2 > 1.0) discard;   // kill pixels outside circle         \n"
//"    N.z = sqrt(1.0-r2);                                            \n"

//  float alpha = saturate(1.0 - r2);
"    float alpha = clamp((1.0 - r2), 0.0, 1.0);                     \n"
"    alpha *= alpha;                                           \n"

"	vec4 fractal = fractal_func(gl_TexCoord[3].xyz);"

"	alpha *= fractal.a * 0.5;"
"    gl_FragColor = vec4(gl_TexCoord[1].xyz*alpha , alpha);              \n"
//"    gl_FragColor = vec4(gl_TexCoord[2].xyz*alpha , alpha);              \n"
"} ";


GLuint ibo;
GLuint vbo;
GLuint tbo;
static int n_pt = 41;
float *pVertex;
float *pCoord;

static GLuint texname;

float fSpriteX[3] = {-1.0, 0, 0};
float fSpriteY[3] = {0, 1.0, 0};
float fSpriteZ[3] = {0, 0.0, -1.0};

static float offs;

void drawAParticle(float center[], float radius, int detail)
{
	float pw[3], ox[3], oy[3];
	ox[0] = fSpriteX[0] * radius*2;
	ox[1] = fSpriteX[1] * radius*2;
	ox[2] = fSpriteX[2] * radius*2;
	
	oy[0] = fSpriteY[0] * radius*2;
	oy[1] = fSpriteY[1] * radius*2;
	oy[2] = fSpriteY[2] * radius*2;
	
	int nslice = 3 + detail/5*2;
	if(nslice > 19) nslice = 19;
	float delta = 1.f / nslice;
	//glUniform1fARB(glGetUniformLocationARB(program_object, "scale"), delta*2);
	
	float start[3];
	
	float z;
	
	glBegin(GL_QUADS);
	int i;
	for(i=0; i<nslice; i++) {
			z = -1.0 + delta * (i+0.5f);
			
			start[0] = center[0] - fSpriteZ[0] * z;
			start[1] = center[1] - fSpriteZ[1] * z;
			start[2] = center[2] - fSpriteZ[2] * z;
			
			z += 0.5;
			
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, z);
			
			pw[0] = start[0] - ox[0] - oy[0];
			pw[1] = start[1] - ox[1] - oy[1];
			pw[2] = start[2] - ox[2] - oy[2];
			
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, z);
			pw[0] = start[0] + ox[0] - oy[0];
			pw[1] = start[1] + ox[1] - oy[1];
			pw[2] = start[2] + ox[2] - oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, z);
			pw[0] = start[0] + ox[0] + oy[0];
			pw[1] = start[1] + ox[1] + oy[1];
			pw[2] = start[2] + ox[2] + oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, z);
			pw[0] = start[0] - ox[0] + oy[0];
			pw[1] = start[1] - ox[1] + oy[1];
			pw[2] = start[2] - ox[2] + oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
	}		
	glEnd();
}

void draw2Particle(float center[], float radius, int detail)
{
	float pw[3], ox[3], oy[3];
	ox[0] = fSpriteX[0] * radius*2;
	ox[1] = fSpriteX[1] * radius*2;
	ox[2] = fSpriteX[2] * radius*2;
	
	oy[0] = fSpriteY[0] * radius*2;
	oy[1] = fSpriteY[1] * radius*2;
	oy[2] = fSpriteY[2] * radius*2;
	
	int nslice = 5 + detail/5*2;
	if(nslice > 19) nslice = 19;
	float delta = 1.f / nslice;
	//glUniform1fARB(glGetUniformLocationARB(program_object, "scale"), delta*2);
	
	float start[3];
	
	float z;
	
	glBegin(GL_QUADS);
	int i;
	float mf[3];
	for(i=0; i<nslice; i++) {
			z = -1.0 + delta * (i+0.5f);
			
			start[0] = center[0] - fSpriteZ[0] * z;
			start[1] = center[1] - fSpriteZ[1] * z;
			start[2] = center[2] - fSpriteZ[2] * z;
			
			z += 0.5;
			
			glMultiTexCoord3d(GL_TEXTURE1, 13.1f, .71f+offs, .61f);
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, -.5f);
			
			mf[0] = delta*2.0*(i-nslice/2);
			mf[1] = delta*4.0*(i-nslice/2);
			mf[2] = 0.f;
			
			pw[0] = start[0] - ox[0] - oy[0] +mf[0];
			pw[1] = start[1] - ox[1] - oy[1] +mf[1];
			pw[2] = start[2] - ox[2] - oy[2] +mf[2];
			
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, -.5f);
			
			pw[0] = start[0] + ox[0] - oy[0] +mf[0];
			pw[1] = start[1] + ox[1] - oy[1] +mf[1];
			pw[2] = start[2] + ox[2] - oy[2] +mf[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, .5f);
			pw[0] = start[0] + ox[0] + oy[0] +mf[0];
			pw[1] = start[1] + ox[1] + oy[1] +mf[1];
			pw[2] = start[2] + ox[2] + oy[2] +mf[2];
			glVertex3f(pw[0], pw[1], pw[2]);

			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, .5f);
			pw[0] = start[0] - ox[0] + oy[0] +mf[0];
			pw[1] = start[1] - ox[1] + oy[1] +mf[1];
			pw[2] = start[2] - ox[2] + oy[2] +mf[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			mf[0] = delta*2.0*(i-nslice/2+0.5);
			mf[1] = delta*4.0*(i-nslice/2+0.5);
			mf[2] = 0.f;
			
			pw[0] = start[0] - ox[0] - oy[0] +mf[0];
			pw[1] = start[1] - ox[1] - oy[1] +mf[1];
			pw[2] = start[2] - ox[2] - oy[2] +mf[2];
			
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, -.5f);
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, -.5f);
			
			pw[0] = start[0] + ox[0] - oy[0] +mf[0];
			pw[1] = start[1] + ox[1] - oy[1] +mf[1];
			pw[2] = start[2] + ox[2] - oy[2] +mf[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, .5f);
			pw[0] = start[0] + ox[0] + oy[0] +mf[0];
			pw[1] = start[1] + ox[1] + oy[1] +mf[1];
			pw[2] = start[2] + ox[2] + oy[2] +mf[2];
			glVertex3f(pw[0], pw[1], pw[2]);

			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, .5f);
			pw[0] = start[0] - ox[0] + oy[0] +mf[0];
			pw[1] = start[1] - ox[1] + oy[1] +mf[1];
			pw[2] = start[2] - ox[2] + oy[2] +mf[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			/*z = -1.0 + delta * (i+0.5f);
			
			start[0] = center[0] - fSpriteZ[0] * z - 1.5;
			start[1] = center[1] - fSpriteZ[1] * z;
			start[2] = center[2] - fSpriteZ[2] * z;
			
			z += 0.5;
			glMultiTexCoord3d(GL_TEXTURE1, -12.41f, .21f+offs, .71f);
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, z);
			glMultiTexCoord2d(GL_TEXTURE0, -.5f, -.5f);
			
			pw[0] = start[0] - ox[0] - oy[0];
			pw[1] = start[1] - ox[1] - oy[1];
			pw[2] = start[2] - ox[2] - oy[2];
			
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, -.5f);
			pw[0] = start[0] + ox[0] - oy[0];
			pw[1] = start[1] + ox[1] - oy[1];
			pw[2] = start[2] + ox[2] - oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, z);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, .5f);
			pw[0] = start[0] + ox[0] + oy[0];
			pw[1] = start[1] + ox[1] + oy[1];
			pw[2] = start[2] + ox[2] + oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, z);
			glMultiTexCoord2d(GL_TEXTURE0, -.5f, .5f);
			pw[0] = start[0] - ox[0] + oy[0];
			pw[1] = start[1] - ox[1] + oy[1];
			pw[2] = start[2] - ox[2] + oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);*/
	}		
	glEnd();
}

static void draw2Slice()
{

//glUniform1fARB(glGetUniformLocationARB(program_object, "scale"), 0.1);
	
	//glUniform3fARB(glGetUniformLocationARB(program_object, "foo"), 0,1,0);
	glBegin(GL_QUADS);
		//glUniform3fARB(glGetUniformLocationARB(program_object, "foo"), 1,0,0);
			glMultiTexCoord3d(GL_TEXTURE1, 13.1f, .71f+offs, .61f);
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, -.5f);
			
			glVertex3f(-0.5, -0.25, 0);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, -.5f);
			
			glVertex3f(0, -0.25, 0);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, .5f);
			
			glVertex3f(0, 0.25, 0);

			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, .5f);
			
			glVertex3f(-0.5, 0.25, 0);
			
			glMultiTexCoord3d(GL_TEXTURE1, 13.21f, .71f+offs, .91f);
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, -.5f);
			
			glVertex3f(0, -0.25, 0);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, -.5f);
			
			glVertex3f(0.5, -0.25, 0);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, .5f, .5f);
			
			glVertex3f(0.5, 0.25, 0);

			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, 0);
			glMultiTexCoord2d(GL_TEXTURE2, -.5f, .5f);
			
			glVertex3f(0, 0.25, 0);
			
			
	glEnd();
}

static void drawAnObject ()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_BLEND);
    glColor4f(1.0f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
   
	//glColor4f(1.0f, 0, 0, 1.f);
		glTexCoord3d(0, 0, 0.5);
        glVertex3f(-0.5,  -0.5, 0.0);
		glTexCoord3d(1, 0, 0.5);
        glVertex3f(0.5, -0.5, 0.0);
		glTexCoord3d(1, 1, 0.5);
        glVertex3f(0.5, 0.5 ,0.0);
		glTexCoord3d(0, 1, 0.5);
        glVertex3f(-0.5, 0.5 ,0.0);
	//glColor4f(0, 1.f, 0, 1.f);
		glTexCoord3d(0, 0, 0.5-0.29);
        glVertex3f(-0.5+0.2,  -0.5+0.2, 0.0+0.1);
		glTexCoord3d(1, 0, 0.5-0.29);
        glVertex3f(0.5+0.2, -0.5+0.2, 0.0+0.1);
		glTexCoord3d(1, 1, 0.5-0.29);
        glVertex3f(0.5+0.2, 0.5+0.2 ,0.0+0.1);
		glTexCoord3d(0, 1, 0.5-0.29);
        glVertex3f(-0.5+0.2, 0.5+0.2 ,0.0+0.1);
	//glColor4f(0, 0, 1.f, 1.f);
		glTexCoord3d(0, 0, 0.5+0.29);
        glVertex3f(-0.5-0.2,  -0.5-0.2, 0.0+0.2);
		glTexCoord3d(1, 0, 0.5+0.29);
        glVertex3f(0.5-0.2, -0.5-0.2, 0.0+0.2);
		glTexCoord3d(1, 1, 0.5+0.29);
        glVertex3f(0.5-0.2, 0.5-0.2 ,0.0+0.2);
		glTexCoord3d(0, 1, 0.5+0.29);
        glVertex3f(-0.5-0.2, 0.5-0.2 ,0.0+0.2);

    glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_3D);	
	glEnable(GL_DEPTH_TEST);
}



float   gl_CoreVersion;
typedef struct glExtension {
    char		*name;
    GLfloat		promoted;
    GLboolean	supported;
} gl_Extension;

gl_Extension	extension[] = {
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
 
@implementation myGLView

+ (NSOpenGLPixelFormat*) basicPixelFormat
{
    NSOpenGLPixelFormatAttribute attributes [] = {
        NSOpenGLPFAWindow,
        NSOpenGLPFADoubleBuffer,	// double buffered
        NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16, // 16 bit depth buffer
        (NSOpenGLPixelFormatAttribute)nil
    };
    return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
}

- (void) resizeGL
{
	NSRect rectView = [self bounds];
	
	// ensure camera knows size changed
	//if ((camera.viewHeight != rectView.size.height) ||
	//    (camera.viewWidth != rectView.size.width)) {
	//	camera.viewHeight = rectView.size.height;
	//	camera.viewWidth = rectView.size.width;
		
		glViewport (0, 0, rectView.size.width, rectView.size.height);
		[self updateProjection];  // update projection matrix
		//[self updateInfoString];
	//}
}

- (void) updateProjection
{
	GLdouble ratio, radians, wd2;
	GLdouble left, right, top, bottom, near, far;

    NSRect rectView = [self bounds];
	
	ratio = rectView.size.width / rectView.size.height;

	// set projection
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	near = 0.05;
	
	far = 1000.0;
	
	radians = 0.0174532925 * 35 / 2; // half aperture degrees to radians 
	wd2 = near * tan(radians);
	
	if (ratio >= 1.0) {
		left  = -ratio * wd2;
		right = ratio * wd2;
		top = wd2;
		bottom = -wd2;	
	} else {
		left  = -wd2;
		right = wd2;
		top = wd2 / ratio;
		bottom = -wd2 / ratio;	
	}
	glFrustum (left, right, bottom, top, near, far);
	
}

- (void) updateModelView
{
   // [[self openGLContext] makeCurrentContext];
	
	// move view
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt (eyex, eyey, eyez,
			   eyex, eyey, eyez - 1.f,
			  0,1,0);

	
}

//- (id) initWithFrame: (NSRect) theFrame
//{long swapInt = 1;

   // [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
	/*
	NSOpenGLPixelFormatAttribute attribs [] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		0
   };
   NSOpenGLPixelFormat *fmt;*/
   /*
   {
	*/
	/*
	fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: attribs];
	NSOpenGLContext *preflight = [[NSOpenGLContext alloc] initWithFormat:fmt shareContext:nil];
	[preflight makeCurrentContext];
	[fmt release];*/
/*
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	if ((GL_FALSE == gluCheckExtension((GLubyte *)"GL_ARB_shader_objects",       extensions)) ||
		(GL_FALSE == gluCheckExtension((GLubyte *)"GL_ARB_shading_language_100", extensions)) ||
		(GL_FALSE == gluCheckExtension((GLubyte *)"GL_ARB_vertex_shader",        extensions)) ||
		(GL_FALSE == gluCheckExtension((GLubyte *)"GL_ARB_fragment_shader",      extensions)))
		{
		
			attribs [3] = NSOpenGLPFARendererID;
			attribs [4] = kCGLRendererGenericFloatID;
		}*/
	//[preflight release];
 /*  }
	*/
   /* Create a GL Context to use - i.e. init the superclass */
  /* fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: attribs];
   [super initWithFrame: theFrame pixelFormat: fmt];
   [[super openGLContext] makeCurrentContext];
   [fmt release];*/
   	
	/* Set up the projection */
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glFrustum(-0.3, 0.3, -0.3, 0.3, 1.0, 8.0);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(0.0, 0.0, -2.0);

//#ifdef USE_BLENDING
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//#endif

	/* Turn on depth test */
	//glEnable(GL_DEPTH_TEST);
	
	//[self setFrameSize: theFrame.size];

	/* Create an update timer */
	//timer = [NSTimer scheduledTimerWithTimeInterval: (1.0f/150.0f) target: self
                    //selector: @selector(heartbeat) userInfo: nil
                   // repeats: YES];
	//[timer retain];

	//[[NSRunLoop currentRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];
	//[[NSRunLoop currentRunLoop] addTimer: timer forMode: NSEventTrackingRunLoopMode];

	/* Create the podium */
	//podium = [[Podium alloc] init];
	
	//lastFrameReferenceTime = -1;
	//leftMouseIsDown = NO;
	//rightMouseIsDown = NO;
	
	//angle = 0;
	//pitch = 25;
	//zoom = 1;
	
	//{
		/* Sync to VBL to avoid tearing. */
		//long VBL = 1;
		//[[self openGLContext] setValues:&VBL forParameter:NSOpenGLCPSwapInterval];
	//}
	
	//return self;
//}
/*
- (void)prepareOpenGL
{
	// check core version and extensions we're interested in
	{
		int supported = 1;
		
		sscanf((char *)glGetString(GL_VERSION), "%f", &gl_CoreVersion);
		NSLog(@"%s version %s", (char *)glGetString(GL_RENDERER), (char *)glGetString(GL_VERSION));

		const GLubyte * strExt = glGetString (GL_EXTENSIONS);
		int j = sizeof(extension)/sizeof(gl_Extension);
		int i;
		for (i = 0; i < j; i++) {
			extension[i].supported = gluCheckExtension(extension[i].name, strExt) |
									 (extension[i].promoted && (gl_CoreVersion >= extension[i].promoted));
			NSLog(@"%-32s %d", extension[i].name, extension[i].supported);
			supported &= extension[i].supported;
		}
		
		if (!supported) {
			NSLog(@"Required functionality not available on this renderer.");
			// A robust app could fall back to other methods here, like glCopyTexImage.
			// This is just a demo, so quit.
			exit(0);
		}
	}
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	    
	    	glShadeModel(GL_SMOOTH);
	    
	    	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	
}

-(void) drawRect: (NSRect) theRect
{
	NSRect bounds = [self bounds];
	
	float aspect = NSWidth(bounds) / NSHeight(bounds);
	 [[self openGLContext] makeCurrentContext];
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	float lr, bt;
	lr = 0.3 * aspect ;
			bt = 0.3 ;
			
	glFrustum(-lr, lr, -bt, bt, 1.0, 8.0);
	
	glMatrixMode(GL_MODELVIEW);

	glViewport(0, 0, NSWidth(bounds), NSHeight(bounds));
	
	//[self resizeGL];
    glClearColor(0.1, 0.1, 0.1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,1,0,0,0,0,1,0);
	
	 glColor4f(1.0f, 1.f, 1.0f, 1.f);
	 glPushMatrix();
	  glTranslatef(0.1, 0, 0);
	 glBegin(GL_TRIANGLES);
	 glColor3f(1,1,0);
	 glVertex3f(-0.15,0,0);
	 glVertex3f(0.15,0,0);
	 glVertex3f(0.15,0.5,0);
	 glEnd();
	  glPopMatrix();
	 glPushMatrix();
	 glTranslatef(0, 0, -5.8);
     drawAnObject ();
	 glPopMatrix();
    glFlush();
	
	[[self openGLContext] flushBuffer];
}
/*
- (void) resizeGL
{
 [[self openGLContext] makeCurrentContext];
	NSRect rectView = [self bounds];
	
	glViewport(0, 0, rectView.size.width, rectView.size.height);
	float aspect = (float)rectView.size.width/(float)rectView.size.height;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
    GLdouble clipnear = 0.1f, clipfar = 1000.f;
	GLdouble wd2 = tan(0.4) *clipnear;
	GLdouble left = -aspect*wd2;
	GLdouble right = aspect*wd2;
	GLdouble top = wd2;
	GLdouble bottom = -wd2;
	glFrustum(left, right, bottom, top, clipnear, clipfar);
	NSLog(@"%f %f %f %f %f", aspect, left, right, bottom, top);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,1,1,0,0,0,0,1,0);

}*/

- (void) drawRect:(NSRect)rect
{		
	// setup viewport and prespective
	[self resizeGL]; // forces projection matrix update (does test for size changes)
	[self updateModelView];  // update model view matrix for object
	
	double now, deltaTime;
	now = (double)[NSDate timeIntervalSinceReferenceDate];
	
	if (lastFrameReferenceTime < 0) deltaTime = 0;
	else deltaTime = now - lastFrameReferenceTime;
	
	lastFrameReferenceTime = now;
	
//offs += 0.0001;
	// clear our drawable
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// model view and projection matricies already set
glTranslatef(0,0,0);

	//glUseProgram(program);
	
	float mm[16];
	
	float ang = 0.f;
	
		mm[0] = cos(ang); mm[1] = 0.f; mm[2] = sin(ang); mm[3] = 0.f;
		
		
		
		mm[4] = 0.f; mm[5] = 1.f; mm[6] = 0.f; mm[7] = 0.f;
		
		ang += 0.5*3.14;
		mm[8] = cos(ang); mm[9] = 0.f; mm[10] = sin(ang); mm[11] = 0.f;
		
		mm[12] = 0.f; mm[13] = 1.f; mm[14] = 0.f; mm[15] = 1.f;
		
		//glUniformMatrix4fv(glGetUniformLocation(program, "objspace"), 1, 0, (float*)mm);
		

//glUniform1f(glGetUniformLocation(program, "Lacunarity"), 2.0);
		//glUniform1f(glGetUniformLocation(program, "Dimension"), 1.8);
	/*	float ang = (posz+0.5)*3.14;
		//float xx = cos(ang)*0.5;
		//float zz = sin(ang)*0.5;
		ang = 0.0;
				
		//glUniform3fARB(glGetUniformLocationARB(program_object, "Origin"), 29.3, -134.1, 3.5);
		glUniform3fARB(glGetUniformLocationARB(program_object, "LightPosition"), -130.0, 100, -11.0);
		glUniform1fARB(glGetUniformLocationARB(program_object, "KNoise"), knoise);
		glUniform3fARB(glGetUniformLocationARB(program_object, "CIBL"), 0.0, 0.4, 0.3);
		glUniform1fARB(glGetUniformLocationARB(program_object, "gscale"), 20.0);
		glUniform1fARB(glGetUniformLocationARB(program_object, "VFreq"), freq);
		*/

	//glEnable(GL_BLEND);
	glDepthMask( GL_FALSE );
	
	//drawTexQuad();
	
	
	if(piece) {
		if(![piece isGLInited]) [piece initGL];
		[piece draw];
	}
	//glPointSize(4.0f);
	/*glBegin(GL_POINTS);
	glMultiTexCoord3d(GL_TEXTURE0, 1,1,-1);
	glColor3f(1,1,0);
	glVertex3f(1,1,-1);
	glMultiTexCoord3d(GL_TEXTURE0, 1,0,0);
	glColor3f(0.5,1,0);
	glVertex3f(1,0,0);
	glMultiTexCoord3d(GL_TEXTURE0, -0.1,-0.5,1);
	glColor3f(0,1,0.5);
	glVertex3f(-0.1,-0.5,1);
	glEnd();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
		
		
		glBindBufferARB(GL_ARRAY_BUFFER, tbo);
        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(4, GL_FLOAT, 0, 0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		
				
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ibo);
        glDrawElements(GL_POINTS, n_pt, GL_UNSIGNED_INT, 0 );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
	
	//draw2Particle( center, radius, nslice/2*5);
	//draw2Slice();
	glMultiTexCoord3d(GL_TEXTURE1, 13.1f, .71f, .61f);
	drawAParticle( center, radius, nslice/2*5);
	center[0] = -.5f;
	center[1] = 0.f;
	center[2] = -3.1f;
	//glUniform3fARB(glGetUniformLocationARB(program_object, "Origin"), 29.1, -54.4, 13.1);
	glMultiTexCoord3d(GL_TEXTURE1, -12.41f, .21f, .71f);
	drawAParticle( center, radius, nslice/2*5);*/
	/*glUniform1fARB(glGetUniformLocationARB(program_object, "scale"), 0.3);
	glBegin(GL_QUADS);
	
			float z = 0.f;
			
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, -.5f, z);
			float pw[3], ox[3], oy[3];
	ox[0] = fSpriteX[0] * radius*2;
	ox[1] = fSpriteX[1] * radius*2;
	ox[2] = fSpriteX[2] * radius*2;
	
	oy[0] = fSpriteY[0] * radius*2;
	oy[1] = fSpriteY[1] * radius*2;
	oy[2] = fSpriteY[2] * radius*2;
	
			pw[0] = center[0] - ox[0] - oy[0];
			pw[1] = center[1] - ox[1] - oy[1];
			pw[2] = center[2] - ox[2] - oy[2];
			
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, -.5f, z);
			pw[0] = center[0] + ox[0] - oy[0];
			pw[1] = center[1] + ox[1] - oy[1];
			pw[2] = center[2] + ox[2] - oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, .5f, .5f, z);
			pw[0] = center[0] + ox[0] + oy[0];
			pw[1] = center[1] + ox[1] + oy[1];
			pw[2] = center[2] + ox[2] + oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
			
			glMultiTexCoord3d(GL_TEXTURE0, -.5f, .5f, z);
			pw[0] = center[0] - ox[0] + oy[0];
			pw[1] = center[1] - ox[1] + oy[1];
			pw[2] = center[2] - ox[2] + oy[2];
			glVertex3f(pw[0], pw[1], pw[2]);
	
	glEnd();*/
	glDepthMask( GL_TRUE );	


	//glDisable(GL_BLEND);
	//glUseProgram(0);
	
	//glBegin(GL_LINES);
	//glColor3f(1,0,0);
	//glVertex3f(0,0,0);
	//glVertex3f(4,0,0);
	//glEnd();
		glFlush ();
		
		//NSLog(@"time %f", posz);
		[[self openGLContext] flushBuffer];
}

- (void) prepareOpenGL
{
   // long swapInt = 1;

    //[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; // set to vbl sync

	// init GL stuff here
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	    
	// front-to-back
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	// back-to-front
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	// front-to-back accumulate color
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
	// back-to-front accumulate color
	//glBlendFunc(GL_ONE,  GL_ONE_MINUS_SRC_COLOR);
	
	glShadeModel(GL_SMOOTH);    
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glPolygonOffset (1.0f, 1.0f);
	
	glClearColor(0.2f, 0.2f,0.2f, 0.0f);
	
	glGenTextures(1, &texname);	
	glBindTexture(GL_TEXTURE_3D, texname);	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
#define WIDTH	128
#define HEIGHT	128
#define DEPTH	128
	unsigned char *texels = malloc(WIDTH*HEIGHT*DEPTH*4*sizeof(unsigned char));
	int u, v, w;
	for(w=0; w<DEPTH; w++) {
		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4] = random()%256;//(float)(random()%313)/313.f;
				texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4+1] = random()%256;//(float)(random()%313)/313.f;
				texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4+2] = random()%256;//(float)(random()%313)/313.f;
				texels[(w*(WIDTH * HEIGHT)+v*WIDTH+u)*4+3] = random()%256;//(float)(random()%313)/313.f;
			}
		}
	}
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
	free(texels);
	/*float tc;
	texels = malloc(WIDTH*HEIGHT*4*sizeof(float));
		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				tc = sqrt((u - 16)*(u - 16) + (v - 16)*(v - 16))/13;
				
				texels[(v*WIDTH+u)*4] = 1-tc;
				
				
				
				texels[(v*WIDTH+u)*4+1] = 1-tc;
				texels[(v*WIDTH+u)*4+2] = 0;
				texels[(v*WIDTH+u)*4+3] = 1;
			}
		}
	
	glGenTextures(1, &img2);
	glBindTexture(GL_TEXTURE_2D, img2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, texels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	free(texels);
	
	texels = malloc(WIDTH*HEIGHT*4*sizeof(float));
		for(v=0; v<HEIGHT; v++) {
			for(u=0; u<WIDTH; u++) {
				texels[(v*WIDTH+u)*4] = (float)(random()%253)/253.f;
				texels[(v*WIDTH+u)*4+1] = texels[(v*WIDTH+u)*4];
				texels[(v*WIDTH+u)*4+2] = texels[(v*WIDTH+u)*4];
				texels[(v*WIDTH+u)*4+3] = 1;
			}
		}
	
	glGenTextures(1, &img3);
	glBindTexture(GL_TEXTURE_2D, img3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, texels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	free(texels);*/
	
// setup shaders
	
	GLint statshader = initShaders();
	if (!statshader) NSLog(@"shaders not compiled");
	
	initTextures();

	//glUseProgram(program);
		//glUniform3fARB(glGetUniformLocationARB(program_object, "Origin"), 13.3, 3.1, -13.5);
		//glUniform1i(glGetUniformLocation(program, "EarthNight"), 0);
		glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texname);
	glUseProgram(0);
	
// create index buffer
	glGenBuffers(1, &ibo);
	unsigned int size = n_pt * sizeof(unsigned int);
	unsigned int* idxbuffer = malloc(sizeof(unsigned int)* n_pt );
	unsigned int i;
	for(i=0; i<n_pt; i++) {
		idxbuffer[i]= i;
		idxbuffer[i]= n_pt -1-i;
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idxbuffer, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		free(idxbuffer);
// create points for vertex and texcoord
	pVertex = malloc(sizeof(float)* n_pt*3 );
	pCoord = malloc(sizeof(float)* n_pt*4 );
	
	for(i=0; i<n_pt; i++) {
		pVertex[i*3]=((float)(rand()%312)/312.f-0.5)*2.f;
		pVertex[i*3+1]=((float)(rand()%312)/312.f-0.5)*2.f;
		pVertex[i*3+2]= -5.f + (float)i/12.f;
		
		pCoord[i*4] = 0.9+0.2*((float)(rand()%171)/171.f-0.5)-(float)i/100.f;
		pCoord[i*4+1] = 0.5+(float)i/100.f;
		pCoord[i*4+2] = 0.4+0.6*((float)(rand()%171)/171.f-0.5);
		pCoord[i*4+3] = 0.6+((float)(rand()%171)/171.f-0.5)*0.3;
	}
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	size = n_pt * sizeof(float) * 3;
        glBufferData(GL_ARRAY_BUFFER, size, pVertex, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	size = n_pt * sizeof(float) * 4;
        glBufferData(GL_ARRAY_BUFFER, size, pCoord, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

- (void) heartbeat
{
	if(![[NSApplication sharedApplication] isHidden])
		[self setNeedsDisplay:YES];
}

-(id) initWithFrame: (NSRect) frameRect
{
	NSOpenGLPixelFormat * pf = [myGLView basicPixelFormat];

	[super initWithFrame: frameRect pixelFormat: pf];
	
	/* Create an update timer */
	timer = [NSTimer scheduledTimerWithTimeInterval: (1.0f/30.0f) target: self
                    selector: @selector(heartbeat) userInfo: nil
                    repeats: YES];
	[timer retain];

	[[NSRunLoop currentRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];
	[[NSRunLoop currentRunLoop] addTimer: timer forMode: NSEventTrackingRunLoopMode];
	
	dir = -1.f;
	knoise = 0.f;
	zcam = 0.f;
	nslice = 25;
	freq = 1.0;
	lacunarity = 1.25;
	dimension = 0.0;
	offs = 0.f;
	
	eyex = 0.f;
	 eyey = 0.f;
	  eyez = 2.f;
	
	piece = NULL;
    return self;
}

- (TestPiece *)piece 
{
    return piece;
}

- (void)setPiece:(TestPiece *) apiece
{
     piece = apiece;
}



- (void)mouseDown:(NSEvent *)event
{
	lastMousePoint = [self convertPoint:[event locationInWindow] fromView:nil];
	leftMouseIsDown = YES;
}

- (void)rightMouseDown:(NSEvent *)event
{
	lastMousePoint = [self convertPoint:[event locationInWindow] fromView:nil];
	rightMouseIsDown = YES;
}

- (void)mouseUp:(NSEvent *)event
{
	leftMouseIsDown = NO;
}

- (void)rightMouseUp:(NSEvent *)event
{
	rightMouseIsDown = NO;
}

- (void)mouseDragged:(NSEvent *)event
{
	if ([event modifierFlags] & 1)
	{
		[self rightMouseDragged:event];
	}
	else
	{
		NSPoint mouse = [self convertPoint:[event locationInWindow] fromView:nil];
		
		NSRect rectView = [self bounds];
	
		eyey += (lastMousePoint.y - mouse.y)/rectView.size.width* eyez;
		eyex += (lastMousePoint.x - mouse.x)/rectView.size.width* eyez;

		lastMousePoint = mouse;
		
		[self setNeedsDisplay:YES];
	}
}

- (void)rightMouseDragged:(NSEvent *)event
{
	NSPoint mouse = [self convertPoint:[event locationInWindow] fromView:nil];
	
	eyez += .01f * eyez * (lastMousePoint.y - mouse.y);
	if (eyez < .05f)
	{
		eyez = .05f;
	}
	else if (eyez > 100.0f)
	{
		eyez = 100.0f;
	}

	lastMousePoint = mouse;

	[self setNeedsDisplay:YES];
}

@end