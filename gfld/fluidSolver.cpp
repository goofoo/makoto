#include "fluidSolver.h"
#include <math.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MGlobal.h>
#include <string.h>
#include "../shared/gBase.h"

#ifndef __APPLE__
#include "../shared/gExtension.h"
#endif

typedef struct testExtensionEntry {
    char* name;
    GLboolean supported;
} testExtensionEntry;

testExtensionEntry entriesNeeded[] = {
{"GL_EXT_framebuffer_object",    0},
{"GL_ARB_texture_cube_map",     0},
{"GL_ARB_shader_objects",        0},
{"GL_ARB_shading_language_100",  0},
{"GL_ARB_fragment_shader",       0},
{"GL_ARB_vertex_buffer_object",      0},
{"GL_ARB_multitexture",      0},
{"GL_ARB_multisample",       0},
{"GL_ARB_vertex_program",       0},
{"GL_ARB_fragment_program",       0},
{"GL_ARB_texture_rectangle",       0},
};

inline void findST(int width, int height, int depth, int& s, int& t)
{
	float ratio = (float)height/(float)width;
	s = 2;
	t = depth/s;
	while(s<(t*ratio))
	{
		s *= 2;
		t = depth/s;
	}
}


//GLuint x_depthBuffer;
//GLuint y_depthBuffer;
//GLuint z_depthBuffer;
	

GLenum status;

FluidSolver::FluidSolver(void) 
: m_velocityField(0), m_densityField(0),
//m_temperatureField(0),
//m_obstacleField(0),
//m_fb(0),m_fbx(0),m_fby(0),m_fbz(0),
f_cg(0),
//zeros(0),
m_quad_p(0),
m_quad_pw(0),
m_quad_tz(0),
m_line_p(0),
m_buoyancy(0.05f),
m_gridSize(1.0f),
m_keepVelocity(1.0), m_keepTemperature(0.9),fInitialized(0),fUndiagnosed(1),
fTemperature(2.f),fWindX(0.f), fWindZ(0.f), m_conserve_denisty(1.0f)
{
}

FluidSolver::~FluidSolver(void)
{
	uninitialize();
}

void FluidSolver::uninitialize()
{
	if(m_velocityField) delete[] m_velocityField;
	if(m_densityField) delete[] m_densityField;

	if(m_rgba_fbo) glDeleteFramebuffersEXT(1, &m_rgba_fbo);
	if(m_rgb_fbo) glDeleteRenderbuffersEXT(1, &m_rgb_fbo);
	if(x_fbo) glDeleteFramebuffersEXT(1, &x_fbo);
	if(y_fbo) glDeleteFramebuffersEXT(1, &y_fbo);
	if(z_fbo) glDeleteFramebuffersEXT(1, &z_fbo);
	
	if(f_cg) delete f_cg;
	if(m_quad_p) delete[] m_quad_p;
	if(m_quad_pw) delete[] m_quad_pw;
	if(m_quad_tz)	delete[] m_quad_tz;
	if(m_line_p) delete[] m_line_p;
	glDeleteTextures(1, &i_velocityTexture);
	glDeleteTextures(1, &i_divergenceTexture);
	glDeleteTextures(1, &i_vorticityTexture);
	glDeleteTextures(1, &i_pressureTexture);
	glDeleteTextures(1, &i_offsetTexture);
	glDeleteTextures(1, &img_impuls);
	glDeleteTextures(1, &i_bufTexture);
	glDeleteTextures(1, &img_temper);
	glDeleteTextures(1, &img_density);
	glDeleteTextures(1, &i_xTexture);
	glDeleteTextures(1, &i_yTexture);
	glDeleteTextures(1, &i_zTexture);

	fInitialized = 0;
}

void FluidSolver::initialize(int width, int height, int depth)
{
	if(fUndiagnosed) {
#ifndef __APPLE__
		gExtensionInit();
		
		float core_version;
		sscanf((char *)glGetString(GL_VERSION), "%f", &core_version);
		char sbuf[64];
		sprintf(sbuf, "%s version %s\n", (char *)glGetString(GL_RENDERER), (char *)glGetString(GL_VERSION));
		MGlobal::displayInfo(sbuf);
	
		int supported = 1;
		int j = sizeof(entriesNeeded)/sizeof(testExtensionEntry);

		 for (int i = 0; i < j; i++) {
			 if(!gCheckExtension(entriesNeeded[i].name)) {
				 sprintf(sbuf, "%-32s %d\n", entriesNeeded[i].name, 0);
				 supported = 0;
			 }
			else sprintf(sbuf, "%-32s %d\n", entriesNeeded[i].name, 1);
			MGlobal::displayInfo(sbuf);
		}
#endif
		fUndiagnosed = 0;
	}
	uninitialize();
	
	m_width = width;
	m_height = height;
	m_depth = depth;
	
	findST(width, height, depth, m_tile_s, m_tile_t);
	
	m_frame_width = width*m_tile_s;
	m_frame_height = height*m_tile_t;
	
	m_n_quad = m_tile_s*m_tile_t-2;
	
	// setup quad primitives
	m_quad_p = new XY[m_n_quad*4];
	m_quad_pw = new XYZ[m_n_quad*4];
	m_quad_tz = new XYZW[m_n_quad*4];
	m_line_p = new XY[m_n_quad*4];
	int acc = 0;
	int xback, yback, xfront, yfront;
	for(int j=m_tile_t-1; j>=0; j--)
	{
		for(int i=m_tile_s-1; i>=0; i--)
		{
			if((i+j)!=0&&(i+j)!=m_tile_s-1+m_tile_t-1)
			{
				m_quad_p[acc*4].x=m_width*i+1;	m_quad_p[acc*4].y=m_height*j+1;
				m_quad_p[acc*4+1].x=m_width*(i+1)-1;	m_quad_p[acc*4+1].y=m_height*j+1;
				m_quad_p[acc*4+2].x=m_width*(i+1)-1;	m_quad_p[acc*4+2].y=m_height*(j+1)-1;
				m_quad_p[acc*4+3].x=m_width*i+1;	m_quad_p[acc*4+3].y=m_height*(j+1)-1;
				
				m_quad_pw[acc*4].x=1;		m_quad_pw[acc*4].y=1;			m_quad_pw[acc*4].z=j*m_tile_s+i+0.5;
				m_quad_pw[acc*4+1].x=m_width-1;	m_quad_pw[acc*4+1].y=1;			m_quad_pw[acc*4+1].z=j*m_tile_s+i+0.5;
				m_quad_pw[acc*4+2].x=m_width-1;	m_quad_pw[acc*4+2].y=m_height-1;	m_quad_pw[acc*4+2].z=j*m_tile_s+i+0.5;
				m_quad_pw[acc*4+3].x=1;		m_quad_pw[acc*4+3].y=m_height-1;	m_quad_pw[acc*4+3].z=j*m_tile_s+i+0.5;
				
				xback = -m_width; yback=0;
				xfront = m_width; yfront=0;
				
				if(i==0) 
				{
					xback = (m_tile_s-1)*m_width;
					yback = -m_height;
				}
				
				
				if(i==m_tile_s-1)
				{
					xfront = (-m_tile_s+1)*m_width;
					yfront = m_height;
				}
				
				m_quad_tz[acc*4].x=xback; 
				m_quad_tz[acc*4].y=yback; 
				m_quad_tz[acc*4].z=xfront; 
				m_quad_tz[acc*4].w=yfront;
				
				m_quad_tz[acc*4+1] = m_quad_tz[acc*4];
				m_quad_tz[acc*4+2] = m_quad_tz[acc*4];
				m_quad_tz[acc*4+3] = m_quad_tz[acc*4];

				m_line_p[acc*4].x=m_width*i+0.5; 
				m_line_p[acc*4].y=m_height*j+0.5;
				
				m_line_p[acc*4+1].x=m_width*(i+1)-0.5; 
				m_line_p[acc*4+1].y=m_height*j+0.5; 

				m_line_p[acc*4+2].x=m_width*(i+1)-0.5; 
				m_line_p[acc*4+2].y=m_height*(j+1)-0.5;

				m_line_p[acc*4+3].x=m_width*i+0.5; 
				m_line_p[acc*4+3].y=m_height*(j+1)-0.5;
				
				acc++;
			}
		}
	}
	
	m_velocityField = new float[m_frame_width*m_frame_height*3];
	m_densityField = new float[m_frame_width*m_frame_height];

// create rgb texture and fbo
	glGenFramebuffersEXT(1, &m_rgb_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgb_fbo);
	
	glGenTextures(1, &img_density);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img_density);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach density to 0		
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, img_density, 0);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("density frame buffer object failed on creation.");

	glGenTextures(1, &img_temper);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img_temper);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach temperature to 1					
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, img_temper, 0);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("temperature frame buffer object failed on creation.");

// velocity is 3rd
	glGenTextures(1, &i_velocityTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_velocityTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach velocity to 2
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_RECTANGLE_ARB, i_velocityTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("velocity frame buffer object failed on creation.");

	 glGenTextures(1, &i_vorticityTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_vorticityTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach vorticity to 3					
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_RECTANGLE_ARB, i_vorticityTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("vorticity frame buffer object failed on creation.");

	 glGenTextures(1, &i_divergenceTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_divergenceTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach divergence to 4				
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_TEXTURE_RECTANGLE_ARB, i_divergenceTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("divergence frame buffer object failed on creation.");
	
	glGenTextures(1, &i_pressureTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_pressureTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach pressure to 5				
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_TEXTURE_RECTANGLE_ARB, i_pressureTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("pressure frame buffer object failed on creation.");				

	glGenTextures(1, &i_bufTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_bufTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// attach buf to 6
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_TEXTURE_RECTANGLE_ARB, i_bufTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("buf frame buffer object failed on creation.");	
	
// end of rgb framebuffer				 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 	
	
	glGenFramebuffersEXT(1, &m_rgba_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgba_fbo);
	
	//glGenRenderbuffersEXT(1, &depthBuffer);
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
	//glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_frame_width, m_frame_height);
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);


// impulse is 1st	
	glGenTextures(1, &img_impuls);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img_impuls);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F_ARB, m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
		
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, img_impuls, 0);

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("impulse frame buffer object failed on creation.");

// offset is 8th
	glGenTextures(1, &i_offsetTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_offsetTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F_ARB,
                 m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
	 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT7_EXT, GL_TEXTURE_RECTANGLE_ARB, i_offsetTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("offset frame buffer object failed on creation.");

				
// end of framebuffer				 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);               
            
    glGenFramebuffersEXT(1, &x_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, x_fbo);
	                
        glGenTextures(1, &i_xTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_xTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_depth, m_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, i_xTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("x frame buffer object failed on creation.");				
// end of framebuffer				 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
                    
    
		 glGenFramebuffersEXT(1, &y_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, y_fbo);
		
		    glGenTextures(1, &i_yTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_yTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_width, m_depth, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, i_yTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("y frame buffer object failed on creation.");				
// end of framebuffer				 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    
					glGenFramebuffersEXT(1, &z_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, z_fbo);
		                
                    glGenTextures(1, &i_zTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_zTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                 m_width, m_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
	
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, i_zTexture, 0);
	
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) MGlobal::displayInfo("z frame buffer object failed on creation.");				
// end of framebuffer				 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        
                    
	f_cg = new CFluidProgram();
	MGlobal::displayInfo(f_cg->getErrorLog());
	
#ifdef WIN32	
	
#endif
	m_needDensity = 0;
	fInitialized = 1;
}

void FluidSolver::update()
{          
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);	
	glDepthMask( GL_FALSE );
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glClearColor(0,0,0,0);


	glViewport(0, 0, m_frame_width, m_frame_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_frame_width/2, m_frame_width/2, -m_frame_height/2, m_frame_height/2, .1, 1000.);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(m_frame_width/2, m_frame_height/2, 10,
			  m_frame_width/2, m_frame_height/2, -10,
			  0, 1, 0);
// update density
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgb_fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);        
	f_cg->begin();
	
	f_cg->advectBegin(i_velocityTexture, img_density, m_width, m_height, m_depth, m_tile_s, m_conserve_denisty);
	drawQuad();
	f_cg->advectEnd();

	f_cg->addTemperatureBegin(img_density, img_impuls, 1.0f);
	drawQuad();
	f_cg->addTemperatureEnd();
	
// record temperature
	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);        
	f_cg->begin();
	
	f_cg->advectBegin(i_velocityTexture, img_temper, m_width, m_height, m_depth, m_tile_s, 0.98f);
	drawQuad();
	f_cg->advectEnd();

	f_cg->addTemperatureBegin(img_temper, img_impuls, fTemperature);
	drawQuad();
	f_cg->addTemperatureEnd();
	
	
	
// buf velocity
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_bufTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, m_velocityField);
// record velocity	
	glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);

	f_cg->advectBegin(i_bufTexture, i_bufTexture, m_width, m_height, m_depth, m_tile_s, m_keepVelocity);
	drawQuad();
	f_cg->advectEnd();
	
	f_cg->buoyancyBegin(i_velocityTexture, img_temper, m_buoyancy);
	drawQuad();
	f_cg->buoyancyEnd();

	f_cg->addVelocityBegin(i_velocityTexture, img_impuls);
	drawQuad();
	f_cg->addVelocityEnd();

	f_cg->abcBegin(i_velocityTexture, i_offsetTexture, m_width, m_height, m_depth, m_tile_s);
	drawQuad();
	f_cg->abcEnd();

// record vorticity	
    glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
glClear(GL_COLOR_BUFFER_BIT);
        
	f_cg->vorticityBegin(i_velocityTexture);
	drawQuad();
	f_cg->vorticityEnd();
	
// record velocity
	glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);

	f_cg->swirlBegin(i_vorticityTexture, i_velocityTexture, m_swirl);
	drawQuad();
	f_cg->swirlEnd();
	
	f_cg->boundaryBegin(i_velocityTexture,-1.0f);
	drawBoundary();
	f_cg->boundaryEnd();
	
	
	
// record divergence
	glDrawBuffer(GL_COLOR_ATTACHMENT4_EXT);
	
	glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
	f_cg->divergenceBegin(i_velocityTexture);
	drawQuad();
	f_cg->divergenceEnd();
	
	f_cg->boundaryBegin(i_divergenceTexture,1.0f);
	drawBoundary();
	f_cg->boundaryEnd();

// record pressure
	glDrawBuffer(GL_COLOR_ATTACHMENT5_EXT);
	
	glClear(GL_COLOR_BUFFER_BIT);
// reset the pressure field texture before jacobi
	
	for(int i =0; i<36; i++) { 

		f_cg->jacobiBegin(i_pressureTexture, i_divergenceTexture);
		drawQuad();
		f_cg->jacobiEnd();

	}
	f_cg->boundaryBegin(i_pressureTexture,1.0f);
	drawBoundary();
	f_cg->boundaryEnd();
	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	
// record velocity
	glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);

	f_cg->boundaryBegin(i_velocityTexture,-1.0f);
	drawBoundary();
	f_cg->boundaryEnd();
	
	f_cg->gradientBegin(i_pressureTexture, i_velocityTexture);
	drawQuad();
	f_cg->gradientEnd();
	
	glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
glReadPixels( 0, 0,  m_frame_width, m_frame_height, GL_RGB, GL_FLOAT, m_velocityField);
// end of frame buffer
	glPopAttrib();
	f_cg->end();
	
	if(m_needDensity) {
		
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);        
	
		glReadPixels( 0, 0,  m_frame_width, m_frame_height, GL_RED, GL_FLOAT, m_densityField);
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FluidSolver::drawQuad()
{
	glBegin(GL_QUADS);
	for(int i=0; i<m_n_quad; i++)
	{
		glMultiTexCoord2i(GL_TEXTURE0_ARB, m_quad_p[i*4].x, m_quad_p[i*4].y);
		glMultiTexCoord3f(GL_TEXTURE1_ARB, m_quad_pw[i*4].x, m_quad_pw[i*4].y, m_quad_pw[i*4].z);
		glMultiTexCoord4i(GL_TEXTURE2_ARB, m_quad_tz[i*4].x, m_quad_tz[i*4].y, m_quad_tz[i*4].z, m_quad_tz[i*4].w);
		glVertex3i(m_quad_p[i*4].x, m_quad_p[i*4].y, 0);
		glMultiTexCoord2i(GL_TEXTURE0_ARB, m_quad_p[i*4+1].x, m_quad_p[i*4+1].y);
		glMultiTexCoord3f(GL_TEXTURE1_ARB, m_quad_pw[i*4+1].x, m_quad_pw[i*4+1].y, m_quad_pw[i*4+1].z);
		glMultiTexCoord4i(GL_TEXTURE2_ARB, m_quad_tz[i*4+1].x, m_quad_tz[i*4+1].y, m_quad_tz[i*4+1].z, m_quad_tz[i*4+1].w);	
		glVertex3i(m_quad_p[i*4+1].x, m_quad_p[i*4+1].y, 0);
		glMultiTexCoord2i(GL_TEXTURE0_ARB, m_quad_p[i*4+2].x, m_quad_p[i*4+2].y);
		glMultiTexCoord3f(GL_TEXTURE1_ARB, m_quad_pw[i*4+2].x, m_quad_pw[i*4+2].y, m_quad_pw[i*4+2].z);
		glMultiTexCoord4i(GL_TEXTURE2_ARB, m_quad_tz[i*4+2].x, m_quad_tz[i*4+2].y, m_quad_tz[i*4+2].z, m_quad_tz[i*4+2].w);	
		glVertex3i(m_quad_p[i*4+2].x, m_quad_p[i*4+2].y, 0);
		glMultiTexCoord2i(GL_TEXTURE0_ARB, m_quad_p[i*4+3].x, m_quad_p[i*4+3].y);
		glMultiTexCoord3f(GL_TEXTURE1_ARB, m_quad_pw[i*4+3].x, m_quad_pw[i*4+3].y, m_quad_pw[i*4+3].z);
		glMultiTexCoord4i(GL_TEXTURE2_ARB, m_quad_tz[i*4+3].x, m_quad_tz[i*4+3].y, m_quad_tz[i*4+3].z, m_quad_tz[i*4+3].w);
		glVertex3i(m_quad_p[i*4+3].x, m_quad_p[i*4+3].y, 0);
	}
	glEnd();
	
}

void FluidSolver::drawBoundary()
{
	int l,r,b,t;
	
	glBegin(GL_LINES);

	for(int i=0; i<m_n_quad; i++)
	{
		glTexCoord2f(m_line_p[i*4].x, m_line_p[i*4].y+0.5);		glVertex3f(m_line_p[i*4].x,m_line_p[i*4].y,0);
		glTexCoord2f(m_line_p[i*4+1].x, m_line_p[i*4+1].y+0.5);		glVertex3f(m_line_p[i*4+1].x,m_line_p[i*4+1].y,0);
		
		glTexCoord2f(m_line_p[i*4+1].x-1.5,m_line_p[i*4+1].y);		glVertex3f(m_line_p[i*4+1].x,m_line_p[i*4+1].y,0);
		glTexCoord2f(m_line_p[i*4+2].x-1.5,m_line_p[i*4+2].y);		glVertex3f(m_line_p[i*4+2].x,m_line_p[i*4+2].y,0);
		
		glTexCoord2f(m_line_p[i*4+2].x,m_line_p[i*4+2].y-1.5);		glVertex3f(m_line_p[i*4+2].x,m_line_p[i*4+2].y,0);
		glTexCoord2f(m_line_p[i*4+3].x,m_line_p[i*4+3].y-1.5);		glVertex3f(m_line_p[i*4+3].x,m_line_p[i*4+3].y,0);
		
		glTexCoord2f(m_line_p[i*4+3].x+0.5,m_line_p[i*4+3].y);		glVertex3f(m_line_p[i*4+3].x,m_line_p[i*4+3].y,0);
		glTexCoord2f(m_line_p[i*4].x+0.5,m_line_p[i*4].y);		glVertex3f(m_line_p[i*4].x,m_line_p[i*4].y,0);
	}
	glEnd();
	
		glBegin(GL_QUADS);
	
		l = 0;
		r = m_width;
		
		b = 0;
		t = m_height;
		
		glTexCoord2i(l+m_width,b);	glVertex3i(l,b,0);
		glTexCoord2i(r+m_width,b);	glVertex3i(r,b,0);
				
		glTexCoord2i(r+m_width,b);	glVertex3i(r,b,0);
		glTexCoord2i(r+m_width,t);	glVertex3i(r,t,0);
				
		glTexCoord2i(r+m_width,t);	glVertex3i(r,t,0);
		glTexCoord2i(l+m_width,t);	glVertex3i(l,t,0);
				
		glTexCoord2i(l+m_width,t);	glVertex3i(l,t,0);
		glTexCoord2i(l+m_width,b);	glVertex3i(l,b,0);
		
		l = (m_tile_s-1)*m_width;
		r = m_tile_s*m_width;
		
		b = (m_tile_t-1)*m_height;
		t = m_tile_t*m_height;
		
		glTexCoord2i(l-m_width,b);	glVertex3i(l,b,0);
		glTexCoord2i(r-m_width,b);	glVertex3i(r,b,0);
				
		glTexCoord2i(r-m_width,b);	glVertex3i(r,b,0);
		glTexCoord2i(r-m_width,t);	glVertex3i(r,t,0);
				
		glTexCoord2i(r-m_width,t);	glVertex3i(r,t,0);
		glTexCoord2i(l-m_width,t);	glVertex3i(l,t,0);
				
		glTexCoord2i(l-m_width,t);	glVertex3i(l,t,0);
		glTexCoord2i(l-m_width,b);	glVertex3i(l,b,0);
		
	glEnd();
	
}

void FluidSolver::display()
{
	f_cg->begin();
	f_cg->displayVectorBegin(i_velocityTexture);
		glBegin(GL_QUADS);
		
		glMultiTexCoord2i(GL_TEXTURE0_ARB, 0, 0);	
		glVertex3i(0, 0, 0);
		
		glMultiTexCoord2i(GL_TEXTURE0_ARB, m_frame_width, 0);
		glVertex3i(m_frame_width, 0, 0);
		
		glMultiTexCoord2i(GL_TEXTURE0_ARB, m_frame_width, m_frame_height);
		glVertex3i(m_frame_width, m_frame_height, 0);
		
		glMultiTexCoord2i(GL_TEXTURE0_ARB, 0, m_frame_height);
		glVertex3i(0, m_frame_height, 0);
		
		glEnd();
	f_cg->displayVectorEnd();
	f_cg->end();
}

void FluidSolver::drawArrow()
{
	glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(1,7,9);
				glEnd();
	
	//glColor3f(0.4f,0.4f,0.5f);

		for(int j=0;j<m_frame_height;j=j+1)
		{
			for(int i=0;i<m_frame_width;i=i+1)
			{
				int offsetx = -i/m_width*m_width;
				int offsety = -j/m_height*m_height;
				int offsetz = j/m_height*m_tile_s+i/m_width;
				//offsetx = offsety = offsetz = 0;
				glBegin(GL_LINES);
				glVertex3f(offsetx+i+0.5, offsety+j+0.5, offsetz+0.5);
				glVertex3f(offsetx+i+0.5+m_velocityField[(j*m_frame_width+i)*4], offsety+j+0.5+m_velocityField[(j*m_frame_width+i)*4+1], offsetz+m_velocityField[(j*m_frame_width+i)*4+2]+0.5);
				glEnd();
			}
		}
	
}

void FluidSolver::clear()
{
	float grad_x, grad_z;
	
	for(int j=0; j<m_frame_height; j++) {
		for(int i=0; i<m_frame_width; i++) {
			grad_x = i%m_width;
			if(grad_x < 10) grad_x = (float)grad_x/10.f;
			else if(grad_x > m_width - 11)  grad_x = (float)(m_width - 1 - grad_x)/10.f;
			else grad_x = 1.f;
			
			grad_x *= grad_x;
			
			grad_z = (j/m_height)*m_tile_s + i/m_width;
			if(grad_z < 10) grad_z = (float)grad_z/10.f;
			else if(grad_z > m_depth - 11)  grad_z = (float)(m_depth - 1 - grad_z)/10.f;
			else grad_z = 1.f;
			
			grad_z *= grad_z;
			
			m_velocityField[(m_frame_width*j+i)*3] = fWindX * grad_x;
			m_velocityField[(m_frame_width*j+i)*3+1] = 0;
			m_velocityField[(m_frame_width*j+i)*3+2] = fWindZ * grad_z;
		}
	}
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_velocityTexture);
   glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB,
                m_frame_width, m_frame_height, 0, GL_RGB, GL_FLOAT, m_velocityField);
					 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgb_fbo);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glClearColor(0,0,0,0);
//reset temperature
	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
	//glViewport(0, 0, m_frame_width, m_frame_height);
	glClear(GL_COLOR_BUFFER_BIT);
	
//reset density
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	//glViewport(0, 0, m_frame_width, m_frame_height);
	glClear(GL_COLOR_BUFFER_BIT);
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

float FluidSolver::getVoxelDensity(int x, int y, int z)
{
	int coordx, coordy;
	
	flatTo2D(coordx, coordy, x, y, z);
	
	return m_densityField[m_frame_width*coordy + coordx];
}

void FluidSolver::getVoxelVelocity(float& vx, float& vy, float& vz, int x, int y, int z)
{
	int coordx, coordy;
	
	flatTo2D(coordx, coordy, x, y, z);
	
	vx = m_velocityField[(m_frame_width*coordy + coordx)*3];
	vy = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	vz = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
}

void FluidSolver::getVelocity(float& vx, float& vy, float& vz, float x, float y, float z)
{
	int tx = x-0.5;
	int ty = y-0.5;
	int tz = z-0.5;
	
	float ax = x - tx;
	float ay = y - ty;
	float az = z - tz;
	
	int coordx, coordy;
	
	flatTo2D(coordx, coordy, tx, ty, tz);
	
	float vx000 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy000 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz000 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx+1, ty, tz);
	
	float vx100 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy100 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz100 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx, ty+1, tz);
	
	float vx010 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy010 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz010 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx, ty, tz+1);
	
	float vx001 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy001 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz001 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx+1, ty+1, tz);
	
	float vx110 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy110 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz110 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx, ty+1, tz+1);
	
	float vx011 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy011 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz011 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx+1, ty, tz+1);
	
	float vx101 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy101 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz101 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	flatTo2D(coordx, coordy, tx+1, ty+1, tz+1);
	
	float vx111 = m_velocityField[(m_frame_width*coordy + coordx)*3];
	float vy111 = m_velocityField[(m_frame_width*coordy + coordx)*3+1];
	float vz111 = m_velocityField[(m_frame_width*coordy + coordx)*3+2];
	
	vx = (1-ax)*(1-ay)*(1-az)*vx000 + ax*(1-ay)*(1-az)*vx100 + (1-ax)*ay*(1-az)*vx010 + (1-ax)*(1-ay)*az*vx001 + ax*ay*(1-az)*vx110 + (1-ax)*ay*az*vx011 + ax*(1-ay)*az*vx101 + ax*ay*az*vx111;
	vy = (1-ax)*(1-ay)*(1-az)*vy000 + ax*(1-ay)*(1-az)*vy100 + (1-ax)*ay*(1-az)*vy010 + (1-ax)*(1-ay)*az*vy001 + ax*ay*(1-az)*vy110 + (1-ax)*ay*az*vy011 + ax*(1-ay)*az*vy101 + ax*ay*az*vy111;
	vz = (1-ax)*(1-ay)*(1-az)*vz000 + ax*(1-ay)*(1-az)*vz100 + (1-ax)*ay*(1-az)*vz010 + (1-ax)*(1-ay)*az*vz001 + ax*ay*(1-az)*vz110 + (1-ax)*ay*az*vz011 + ax*(1-ay)*az*vz101 + ax*ay*az*vz111; 
}

void FluidSolver::flatTo2D(int& s, int& t, int x, int y, int z)
{
	int row = z/m_tile_s;
	int col = z - row*m_tile_s;
	
	s = col*m_width + x;
	t = row*m_height + y;
}
/*
void FluidSolver::addImpulse(int _type, float _radius, float _spread, float posx, float posy, float posz, float velx, float vely, float velz)
{
	if (posx < 1 || posx > m_width-1 || posy < 1 || posy > m_height-1 || posz<1 || posz>m_depth-1)
        return;

    ImpulseDesc imp(_type, _radius, _spread, posx, posy, posz, velx, vely, velz);

    m_impulseList.push_back(imp);
}
*/
void FluidSolver::processObstacles(const MObjectArray& obstacles)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0,0,0,0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgba_fbo);
// clear offset
		glDrawBuffer(GL_COLOR_ATTACHMENT7_EXT);
		glClear(GL_COLOR_BUFFER_BIT);
		
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgb_fbo);
// clear buf 
	glDrawBuffer(GL_COLOR_ATTACHMENT6_EXT);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

// no object		
	if(obstacles.length() < 1) {
		glPopAttrib();
		return;
	}

	MStatus status;
	
	for(int iter = 0; iter<obstacles.length(); iter++)
	{
		MItMeshPolygon faceIter(obstacles[iter], &status );
		if(status) {
			int n_tri = faceIter.count();
			float *pp = new float[n_tri*3*3];
			int acc = 0;
			for( ; !faceIter.isDone(); faceIter.next() ) {
			
				MPointArray pts;
				faceIter.getPoints(pts,  MSpace::kWorld);
				
				pp[acc*3] = pts[0].x;
				pp[acc*3+1] = pts[0].y;
				pp[acc*3+2] = pts[0].z;
				acc++;
				pp[acc*3] = pts[1].x;
				pp[acc*3+1] = pts[1].y;
				pp[acc*3+2] = pts[1].z;
				acc++;
				pp[acc*3] = pts[2].x;
				pp[acc*3+1] = pts[2].y;
				pp[acc*3+2] = pts[2].z;
				acc++;
			}
			
// x view

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, x_fbo);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	//glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT );
	glViewport(0, 0, m_depth, m_height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_depth/2*m_gridSize, m_depth/2*m_gridSize, -m_height/2*m_gridSize, m_height/2*m_gridSize, .01, 1000.*m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_origin_x, m_origin_y + m_height/2*m_gridSize, m_origin_z + m_depth/2*m_gridSize,
			  m_origin_x+1, m_origin_y + m_height/2*m_gridSize, m_origin_z + m_depth/2*m_gridSize,
			  0, 1, 0);
	
	glColor3f(1, 1, 1);
	
	drawTriangleMesh(n_tri, pp);
	
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
// y view	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, y_fbo);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT );
	glViewport(0, 0, m_width, m_depth);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_width/2*m_gridSize, m_width/2*m_gridSize, -m_depth/2*m_gridSize, m_depth/2*m_gridSize, .01, 1000.*m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y, m_origin_z + m_depth/2*m_gridSize,
			  m_origin_x + m_width/2*m_gridSize, m_origin_y + 1, m_origin_z + m_depth/2*m_gridSize,
			  0, 0, 1);
	
	drawTriangleMesh(n_tri, pp);
	

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
// z view	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, z_fbo);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT );
	glViewport(0, 0, m_width, m_height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_width/2*m_gridSize, m_width/2*m_gridSize, -m_height/2*m_gridSize, m_height/2*m_gridSize, .01, 1000.*m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + m_depth*m_gridSize,
			  m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + m_depth*m_gridSize-1,
			  0, 1, 0);
	
	drawTriangleMesh(n_tri, pp);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
// end of object
			delete[] pp;
			
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgb_fbo);
// update buf 
	glDrawBuffer(GL_COLOR_ATTACHMENT6_EXT);
			  
			  setFrameView();

	f_cg->begin();
	f_cg->convexBegin(i_xTexture, i_yTexture, i_zTexture, i_bufTexture);
	drawQuad();
	f_cg->convexEnd();
	f_cg->end();
		}
	}
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgba_fbo);
// update offset field
	glDrawBuffer(GL_COLOR_ATTACHMENT7_EXT);
	glClear(GL_COLOR_BUFFER_BIT);

	f_cg->begin();
	f_cg->offsetBegin(i_bufTexture);
	drawQuad();
	f_cg->offsetEnd();
	f_cg->end();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	glPopAttrib();
}

void FluidSolver::processSources(const MVectorArray &points, const MVectorArray &velocities, const MDoubleArray& ages, const MObjectArray& sources)
{
	int n_ptc = points.length();
	if(n_ptc != ages.length()) return;
	float* pVertex = new float[n_ptc*3];
	float* pColor = new float[n_ptc*4];
	float decay;
// set impulse RGB by velocity, A by decay
	for(int i=0; i<n_ptc; i++) {
		pVertex[i*3]=points[i].x;
		pVertex[i*3+1]=points[i].y;
		pVertex[i*3+2]=points[i].z;
// normalize by grid size
		decay = exp(-40*(ages[i]+.01))/m_gridSize;
		pColor[i*4]=velocities[i].x*decay;
		pColor[i*4+1]=velocities[i].y*decay;
		pColor[i*4+2]=velocities[i].z*decay;
		pColor[i*4+3]= exp(-2.0f*m_keepTemperature*(ages[i]+.01f));
	}
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_rgba_fbo);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
		//glClearDepth(1.0f);							
		glDisable(GL_DEPTH_TEST);					
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);	
		glDepthMask( GL_FALSE );
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glClearColor(0,0,0,0);
		glPointSize(1.0);
// record impulse
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_width/2*m_gridSize, m_width/2*m_gridSize, -m_height/2*m_gridSize, m_height/2*m_gridSize, .009, m_gridSize*2);
	glMatrixMode(GL_MODELVIEW);
	
	int x_offset, y_offset, i_frame = m_frame_width/m_width;
	for(int i=1; i<m_depth-1; i++) {
		x_offset = i%i_frame*m_width;
		y_offset = i/i_frame*m_height;
		glViewport(x_offset, y_offset, m_width, m_height);
		glLoadIdentity();
		gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + (i+1)*m_gridSize,
				  m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + (i-1)*m_gridSize,
				  0, 1, 0);
		f_cg->particleBegin();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (float*)pVertex);
		glTexCoordPointer(4, GL_FLOAT, 0, (float*)pColor);
		glDrawArrays(GL_POINTS, 0, n_ptc);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		f_cg->particleEnd();
	}
	
	delete[] pVertex;
	delete[] pColor;
	
	if(m_source_density > 0) {
	MStatus status;
	
	for(int iter = 0; iter<sources.length(); iter++)
	{
		MItMeshPolygon faceIter(sources[iter], &status );
		if(status) {
			int n_tri = faceIter.count();
			float *pp = new float[n_tri*3*3];
			int acc = 0;
			for( ; !faceIter.isDone(); faceIter.next() ) {
			
				MPointArray pts;
				faceIter.getPoints(pts,  MSpace::kWorld);
				
				pp[acc*3] = pts[0].x;
				pp[acc*3+1] = pts[0].y;
				pp[acc*3+2] = pts[0].z;
				acc++;
				pp[acc*3] = pts[1].x;
				pp[acc*3+1] = pts[1].y;
				pp[acc*3+2] = pts[1].z;
				acc++;
				pp[acc*3] = pts[2].x;
				pp[acc*3+1] = pts[2].y;
				pp[acc*3+2] = pts[2].z;
				acc++;
			}
		
		for(int i=1; i<m_depth-1; i++) {
			x_offset = i%i_frame*m_width;
			y_offset = i/i_frame*m_height;
			glViewport(x_offset, y_offset, m_width, m_height);
			glLoadIdentity();
			gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + (i+1)*m_gridSize,
					  m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + (i-1)*m_gridSize,
					  0, 1, 0);
// velocity is up
			glTexCoord4f(0,1,0,m_source_density);
			f_cg->particleBegin();
			drawTriangleMesh(n_tri, pp);
			f_cg->particleEnd();
		}
// end of object
			delete[] pp;
		}
	}
	}
	
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FluidSolver::drawTriangleMesh(int count, const float* vert)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (float*)vert);
	
	glDrawArrays(GL_TRIANGLES, 0, count*3);
	
	glDisableClientState(GL_VERTEX_ARRAY);
}

void FluidSolver::setFrameView()
{
		glViewport(0, 0, m_frame_width, m_frame_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_frame_width/2, m_frame_width/2, -m_frame_height/2, m_frame_height/2, .1, 1000.*m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_frame_width/2, m_frame_height/2, 10,
			  m_frame_width/2, m_frame_height/2, -10,
			  0, 1, 0);
}

void FluidSolver::showTexture(int itex, int islice)
{
	glColor3f(1,1,1);
	switch (itex)
	{
		case 1:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img_density);
			break;
		case 2:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img_temper);
			break;
		case 3:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_pressureTexture);
			break;
		case 4:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, img_impuls);
			break;
		case 5:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_offsetTexture);
			break;
		case 6:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_vorticityTexture);
			break;
		case 7:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_divergenceTexture);
			break;
		default:
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, i_velocityTexture);
			break;
	}
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	showSlice(islice);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

void FluidSolver::showSlice(int i)
{
	if( i > m_depth ) i = m_depth;
	int ii = m_depth -i;
	glBegin(GL_QUADS);
		glTexCoord2i( m_quad_p[ii*4].x, m_quad_p[ii*4].y);
		glVertex3f(m_origin_x, m_origin_y, m_origin_z + m_gridSize * (i+0.5));
		
		glTexCoord2i( m_quad_p[ii*4+1].x, m_quad_p[ii*4+1].y);
		glVertex3f(m_origin_x + m_gridSize * m_width, m_origin_y, m_origin_z + m_gridSize * (i+0.5));
		
		glTexCoord2i( m_quad_p[ii*4+2].x, m_quad_p[ii*4+2].y);
		glVertex3f(m_origin_x  + m_gridSize * m_width, m_origin_y  + m_gridSize * m_height, m_origin_z + m_gridSize * (i+0.5));
		
		glTexCoord2i( m_quad_p[ii*4+3].x, m_quad_p[ii*4+3].y);
		glVertex3f(m_origin_x, m_origin_y  + m_gridSize * m_height, m_origin_z + m_gridSize * (i+0.5));
	
	glEnd();
	
}
