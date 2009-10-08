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

PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB = NULL;
PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB = NULL;
PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB = NULL;
PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB = NULL;
PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;

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

FluidSolver::FluidSolver(void) 
: m_velocityField(0),
m_temperatureField(0),
m_obstacleField(0),
m_fb(0),m_fbx(0),m_fby(0),m_fbz(0),
f_cg(0),
zeros(0),
m_quad_p(0),
m_quad_pw(0),
m_quad_tz(0),
m_line_p(0),
m_buoyancy(0.05f),
m_gridSize(1.0f),
m_keepVelocity(1.0), m_keepTemperature(0.9)
{
}

FluidSolver::~FluidSolver(void)
{
	uninitialize();
}

void FluidSolver::uninitialize()
{
	if(m_obstacleField) delete[] m_obstacleField;
	if(m_velocityField) delete[] m_velocityField;
	if(m_temperatureField) delete[] m_temperatureField;
	if(m_fb) delete m_fb;
	if(m_fbx) delete m_fbx;
	if(m_fby) delete m_fby;
	if(m_fbz) delete m_fbz;
	if(f_cg) delete f_cg;
	if(zeros) delete[] zeros;
	if(m_quad_p) delete[] m_quad_p;
	if(m_quad_pw) delete[] m_quad_pw;
	if(m_quad_tz)	delete[] m_quad_tz;
	if(m_line_p) delete[] m_line_p;
	glDeleteTextures(1, &i_velocityTexture);
	glDeleteTextures(1, &i_divergenceTexture);
	glDeleteTextures(1, &i_vorticityTexture);
	glDeleteTextures(1, &i_pressureTexture);
	glDeleteTextures(1, &i_offsetTexture);
	glDeleteTextures(1, &i_impulseTexture);
	glDeleteTextures(1, &i_bufTexture);
	glDeleteTextures(1, &i_temperatureTexture);
	glDeleteTextures(1, &i_xTexture);
	glDeleteTextures(1, &i_yTexture);
	glDeleteTextures(1, &i_zTexture);
}

void FluidSolver::initialize(int width, int height, int depth)
{
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
	
	m_obstacleField = new float[m_frame_width*m_frame_height*4];
	//m_obstacleField = new float[m_depth*m_height*4];
	//m_obstacleField = new float[m_width*m_depth*4];
	
	m_velocityField = new float[m_frame_width*m_frame_height*4];
	m_temperatureField = new float[m_frame_width*m_frame_height];
	zeros = new float[m_frame_width*m_frame_height*4];
	for(int j=0; j<m_frame_height; j++)
	{
		for(int i=0; i<m_frame_width; i++)
		{
			m_velocityField[(m_frame_width*j+i)*4] = 0;
			m_velocityField[(m_frame_width*j+i)*4+1] = 0;
			m_velocityField[(m_frame_width*j+i)*4+2] = 0;
			m_velocityField[(m_frame_width*j+i)*4+3] = 0;
			m_temperatureField[m_frame_width*j+i] = 0;
			zeros[(m_frame_width*j+i)*4] = 0;
			zeros[(m_frame_width*j+i)*4+1] = 0;
			zeros[(m_frame_width*j+i)*4+2] = 0;
			zeros[(m_frame_width*j+i)*4+3] = 0;
		}
	}
	
	m_fb = new zFrameBuffer(m_frame_width, m_frame_height);
	m_fbx = new zFrameBuffer(m_depth, m_height);
	m_fby = new zFrameBuffer(m_width, m_depth);
	m_fbz = new zFrameBuffer(m_width, m_height);
	
	glGenTextures(1, &i_velocityTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_velocityTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, zeros);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
                    glGenTextures(1, &i_temperatureTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_temperatureTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV,
                 m_frame_width, m_frame_height, 0, GL_RED, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
             
                    glGenTextures(1, &i_divergenceTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_divergenceTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV,
                 m_frame_width, m_frame_height, 0, GL_RED, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
                    
                    glGenTextures(1, &i_pressureTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_pressureTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV,
                 m_frame_width, m_frame_height, 0, GL_RED, GL_FLOAT, zeros);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
                    glGenTextures(1, &i_bufTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_bufTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV,
                 m_frame_width, m_frame_height, 0, GL_RED, GL_FLOAT, zeros);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
                    glGenTextures(1, &i_vorticityTexture);
    	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_vorticityTexture);

   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
	glGenTextures(1, &i_offsetTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_offsetTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, zeros);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
                    glGenTextures(1, &i_impulseTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_impulseTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
        glGenTextures(1, &i_xTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_xTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_depth, m_height, 0, GL_RGBA, GL_FLOAT, zeros);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
        glGenTextures(1, &i_yTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_yTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_width, m_depth, 0, GL_RGBA, GL_FLOAT, zeros);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    
                    glGenTextures(1, &i_zTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_zTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_width, m_height, 0, GL_RGBA, GL_FLOAT, zeros);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
                    
	f_cg = new CFluidProgram();
#ifdef WIN32	
	char *ext = (char*)glGetString( GL_EXTENSIONS );

	if( strstr( ext, "EXT_framebuffer_object" ) == NULL )
	{

		MessageBox(NULL,"EXT_framebuffer_object extension was not found",
			"ERROR",MB_OK|MB_ICONEXCLAMATION);
	}
	else
	{
		glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)wglGetProcAddress("glMultiTexCoord1iARB");
		glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)wglGetProcAddress("glMultiTexCoord2iARB");
		glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)wglGetProcAddress("glMultiTexCoord3iARB");
		glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)wglGetProcAddress("glMultiTexCoord4iARB");
		glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)wglGetProcAddress("glMultiTexCoord4fARB");
		
		if( !glMultiTexCoord1iARB || !glMultiTexCoord2iARB || !glMultiTexCoord3iARB || !glMultiTexCoord4iARB ||
		!glMultiTexCoord1fARB || !glMultiTexCoord2fARB || !glMultiTexCoord3fARB || !glMultiTexCoord4fARB)
		{
			MessageBox(NULL,"One or more EXT_framebuffer_object functions were not found",
				"ERROR",MB_OK|MB_ICONEXCLAMATION);
		}
	}
#endif
}

void FluidSolver::update()
{         
	//processObstacles();
	//processSources();
       
        m_fb->begin(i_temperatureTexture, GL_TEXTURE_RECTANGLE_NV);
        glViewport(0, 0, m_frame_width, m_frame_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_frame_width/2, m_frame_width/2, -m_frame_height/2, m_frame_height/2, .1, 1000.);
	glMatrixMode(GL_MODELVIEW);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(m_frame_width/2, m_frame_height/2, 10,
			  m_frame_width/2, m_frame_height/2, -10,
			  0, 1, 0);
        f_cg->begin();
      
	f_cg->advectBegin(i_velocityTexture, i_temperatureTexture, m_width, m_height, m_depth, m_tile_s, 0.93f);
	drawQuad();
	f_cg->advectEnd();

	f_cg->addTemperatureBegin(i_temperatureTexture, i_impulseTexture);
	drawQuad();
	f_cg->addTemperatureEnd();
	
	m_fb->readR(m_temperatureField);
	
	m_fb->end();
	
	// save the velocity
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_bufTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, m_velocityField);
	
	m_fb->begin(i_velocityTexture, GL_TEXTURE_RECTANGLE_NV);

	f_cg->advectBegin(i_bufTexture, i_bufTexture, m_width, m_height, m_depth, m_tile_s, m_keepVelocity);
	drawQuad();
	f_cg->advectEnd();
	
	f_cg->buoyancyBegin(i_velocityTexture, i_temperatureTexture, m_buoyancy);
	drawQuad();
	f_cg->buoyancyEnd();

	f_cg->addVelocityBegin(i_velocityTexture, i_impulseTexture);
	drawQuad();
	f_cg->addVelocityEnd();

        f_cg->abcBegin(i_velocityTexture, i_offsetTexture, m_width, m_height, m_depth, m_tile_s);
	drawQuad();
	f_cg->abcEnd();
	
	m_fb->end();
    
        m_fb->begin(i_vorticityTexture, GL_TEXTURE_RECTANGLE_NV);
        f_cg->vorticityBegin(i_velocityTexture);
	drawQuad();
	f_cg->vorticityEnd();
	
	m_fb->end();
      
        m_fb->begin(i_velocityTexture, GL_TEXTURE_RECTANGLE_NV);
        f_cg->swirlBegin(i_vorticityTexture, i_velocityTexture, m_swirl);
	drawQuad();
	f_cg->swirlEnd();
	
	f_cg->boundaryBegin(i_velocityTexture,-1.0f);
	drawBoundary();
	f_cg->boundaryEnd();
	
	m_fb->end();
	
	m_fb->begin(i_divergenceTexture, GL_TEXTURE_RECTANGLE_NV);
	f_cg->divergenceBegin(i_velocityTexture);
	drawQuad();
	f_cg->divergenceEnd();
	
	f_cg->boundaryBegin(i_divergenceTexture,1.0f);
	drawBoundary();
	f_cg->boundaryEnd();
	
	m_fb->end();
	
		
	//glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_pressureTexture);
	//glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV,
        //        m_frame_width, m_frame_height, 0, GL_RED, GL_FLOAT, zeros);

	m_fb->begin(i_pressureTexture, GL_TEXTURE_RECTANGLE_NV);
	// reset the pressure field texture before jacobi
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(int i =0; i<60; i++)
	{ 

		f_cg->jacobiBegin(i_pressureTexture, i_divergenceTexture);
		drawQuad();
		f_cg->jacobiEnd();
		
		
	
		f_cg->boundaryBegin(i_pressureTexture,1.0f);
		drawBoundary();
		f_cg->boundaryEnd();
		
		
	}
	m_fb->end();
	
	m_fb->begin(i_velocityTexture, GL_TEXTURE_RECTANGLE_NV);
	
	f_cg->boundaryBegin(i_velocityTexture,-1.0f);
	drawBoundary();
	f_cg->boundaryEnd();
	
	f_cg->gradientBegin(i_pressureTexture, i_velocityTexture);
	drawQuad();
	f_cg->gradientEnd();
	

	
	m_fb->readRGBA(m_velocityField);

        f_cg->end();
        m_fb->end();
        
        
}

void FluidSolver::drawQuad()
{
	glBegin(GL_QUADS);
	for(int i=0; i<m_n_quad; i++)
	{
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, m_quad_p[i*4].x, m_quad_p[i*4].y);
		glMultiTexCoord3fARB(GL_TEXTURE1_ARB, m_quad_pw[i*4].x, m_quad_pw[i*4].y, m_quad_pw[i*4].z);
		glMultiTexCoord4iARB(GL_TEXTURE2_ARB, m_quad_tz[i*4].x, m_quad_tz[i*4].y, m_quad_tz[i*4].z, m_quad_tz[i*4].w);
		glVertex3i(m_quad_p[i*4].x, m_quad_p[i*4].y, 0);
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, m_quad_p[i*4+1].x, m_quad_p[i*4+1].y);
		glMultiTexCoord3fARB(GL_TEXTURE1_ARB, m_quad_pw[i*4+1].x, m_quad_pw[i*4+1].y, m_quad_pw[i*4+1].z);
		glMultiTexCoord4iARB(GL_TEXTURE2_ARB, m_quad_tz[i*4+1].x, m_quad_tz[i*4+1].y, m_quad_tz[i*4+1].z, m_quad_tz[i*4+1].w);	
		glVertex3i(m_quad_p[i*4+1].x, m_quad_p[i*4+1].y, 0);
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, m_quad_p[i*4+2].x, m_quad_p[i*4+2].y);
		glMultiTexCoord3fARB(GL_TEXTURE1_ARB, m_quad_pw[i*4+2].x, m_quad_pw[i*4+2].y, m_quad_pw[i*4+2].z);
		glMultiTexCoord4iARB(GL_TEXTURE2_ARB, m_quad_tz[i*4+2].x, m_quad_tz[i*4+2].y, m_quad_tz[i*4+2].z, m_quad_tz[i*4+2].w);	
		glVertex3i(m_quad_p[i*4+2].x, m_quad_p[i*4+2].y, 0);
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, m_quad_p[i*4+3].x, m_quad_p[i*4+3].y);
		glMultiTexCoord3fARB(GL_TEXTURE1_ARB, m_quad_pw[i*4+3].x, m_quad_pw[i*4+3].y, m_quad_pw[i*4+3].z);
		glMultiTexCoord4iARB(GL_TEXTURE2_ARB, m_quad_tz[i*4+3].x, m_quad_tz[i*4+3].y, m_quad_tz[i*4+3].z, m_quad_tz[i*4+3].w);
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
		
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, 0, 0);	
		glVertex3i(0, 0, 0);
		
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, m_frame_width, 0);
		glVertex3i(m_frame_width, 0, 0);
		
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, m_frame_width, m_frame_height);
		glVertex3i(m_frame_width, m_frame_height, 0);
		
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB, 0, m_frame_height);
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
	//isupd = 1;
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_velocityTexture);
   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                 m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, zeros);
                 
                 //glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_bufTexture);
   //glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
    //             m_frame_width, m_frame_height, 0, GL_RGBA, GL_FLOAT, zeros);
                 
                 glBindTexture(GL_TEXTURE_RECTANGLE_NV, i_temperatureTexture);
   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV,
                 m_frame_width, m_frame_height, 0, GL_RED, GL_FLOAT, zeros);
                 
        for(int j=0; j<m_frame_height; j++)
	{
		for(int i=0; i<m_frame_width; i++)
		{
			m_velocityField[(m_frame_width*j+i)*4] = 0;
			m_velocityField[(m_frame_width*j+i)*4+1] = 0;
			m_velocityField[(m_frame_width*j+i)*4+2] = 0;
			m_velocityField[(m_frame_width*j+i)*4+3] = 0;
			m_temperatureField[m_frame_width*j+i] = 0;
		}
	}
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
	
	float vx000 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy000 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz000 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx+1, ty, tz);
	
	float vx100 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy100 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz100 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx, ty+1, tz);
	
	float vx010 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy010 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz010 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx, ty, tz+1);
	
	float vx001 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy001 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz001 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx+1, ty+1, tz);
	
	float vx110 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy110 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz110 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx, ty+1, tz+1);
	
	float vx011 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy011 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz011 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx+1, ty, tz+1);
	
	float vx101 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy101 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz101 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
	flatTo2D(coordx, coordy, tx+1, ty+1, tz+1);
	
	float vx111 = m_velocityField[(m_frame_width*coordy + coordx)*4];
	float vy111 = m_velocityField[(m_frame_width*coordy + coordx)*4+1];
	float vz111 = m_velocityField[(m_frame_width*coordy + coordx)*4+2];
	
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
	MStatus status;

	m_fbx->begin(i_xTexture, GL_TEXTURE_RECTANGLE_NV);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	
	drawList(obstacles);
	

	m_fbx->end();
	
	m_fby->begin(i_yTexture, GL_TEXTURE_RECTANGLE_NV);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_width, m_depth);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_width/2*m_gridSize, m_width/2*m_gridSize, -m_depth/2*m_gridSize, m_depth/2*m_gridSize, .01, 1000.*m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y, m_origin_z + m_depth/2*m_gridSize,
			  m_origin_x + m_width/2*m_gridSize, m_origin_y + 1, m_origin_z + m_depth/2*m_gridSize,
			  0, 0, 1);
	
	drawList(obstacles);
	

	m_fby->end();
	
	m_fbz->begin(i_zTexture, GL_TEXTURE_RECTANGLE_NV);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_width, m_height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_width/2*m_gridSize, m_width/2*m_gridSize, -m_height/2*m_gridSize, m_height/2*m_gridSize, .01, 1000.*m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + m_depth*m_gridSize,
			  m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + m_depth*m_gridSize-1,
			  0, 1, 0);
	
	drawList(obstacles);
	
	
	m_fbz->end();
	

	m_fb->begin(i_bufTexture, GL_TEXTURE_RECTANGLE_NV);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_frame_width, m_frame_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_frame_width/2, m_frame_width/2, -m_frame_height/2, m_frame_height/2, .1, 1000.);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_frame_width/2, m_frame_height/2, 10,
			  m_frame_width/2, m_frame_height/2, -10,
			  0, 1, 0);
	f_cg->begin();
	f_cg->convexBegin(i_xTexture, i_yTexture, i_zTexture);
	drawQuad();
	f_cg->convexEnd();
	f_cg->end();
	
	m_fb->readRGBA(m_obstacleField);
	m_fb->end();
	
	m_fb->begin(i_offsetTexture, GL_TEXTURE_RECTANGLE_NV);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	glViewport(0, 0, m_frame_width, m_frame_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_frame_width/2, m_frame_width/2, -m_frame_height/2, m_frame_height/2, .1, 1000.);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	gluLookAt(m_frame_width/2, m_frame_height/2, 10,
			  m_frame_width/2, m_frame_height/2, -10,
			  0, 1, 0);
	*/
	f_cg->begin();
	f_cg->offsetBegin(i_bufTexture);
	drawQuad();
	f_cg->offsetEnd();
	f_cg->end();
	//m_fb->readRGBA(m_obstacleField);
	
	m_fb->end();

	//m_obstacles.clear();
}

void FluidSolver::processSources(const MVectorArray &points, const MVectorArray &velocities, const MDoubleArray& ages)
{
	int n_ptc = points.length();
	if(n_ptc != ages.length()) return;
	float* pVertex = new float[n_ptc*3];
	float* pColor = new float[n_ptc*4];
	float decay;
	for(int i=0; i<n_ptc; i++)
	{
		pVertex[i*3]=points[i].x;
		pVertex[i*3+1]=points[i].y;
		pVertex[i*3+2]=points[i].z;
		decay = exp(-40*ages[i]);
		pColor[i*4]=velocities[i].x*decay;
		pColor[i*4+1]=velocities[i].y*decay;
		pColor[i*4+2]=velocities[i].z*decay;
		pColor[i*4+3]=exp(-2*m_keepTemperature*ages[i]);
	}
	
	m_fb->begin(i_impulseTexture, GL_TEXTURE_RECTANGLE_NV);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_width/2*m_gridSize, m_width/2*m_gridSize, -m_height/2*m_gridSize, m_height/2*m_gridSize, .009, m_gridSize);
	glMatrixMode(GL_MODELVIEW);
	
	int x_offset, y_offset, i_frame = m_frame_width/m_width;
	for(int i=1; i<m_depth-1; i++)
	{
		x_offset = i%i_frame*m_width;
		y_offset = i/i_frame*m_height;
		glViewport(x_offset, y_offset, m_width, m_height);
		glLoadIdentity();
		gluLookAt(m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + i*m_gridSize,
				  m_origin_x + m_width/2*m_gridSize, m_origin_y + m_height/2*m_gridSize, m_origin_z + i*m_gridSize-1,
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
	
	//m_fb->readRGBA(m_obstacleField);
	m_fb->end();

	delete[] pVertex;
	delete[] pColor;
}

void FluidSolver::drawList(const MObjectArray& obstacles)
{
	f_cg->begin();
	f_cg->flatBegin();
	
	MStatus status;
	//for (std::vector<MDagPath>::iterator ms = obstacles.begin(); ms != obstacles.end(); ms++)
	for(int iter = 0; iter<obstacles.length(); iter++)
	{
		MItMeshPolygon faceIter(obstacles[iter], &status );
		if(status)
		{
			for( ; !faceIter.isDone(); faceIter.next() ) 
			{
				MPointArray pts;
				faceIter.getPoints(pts,  MSpace::kWorld);
				glBegin(GL_POLYGON);
				for(int k=0; k<pts.length(); k++)
				{
					glVertex3f(pts[k].x, pts[k].y, pts[k].z);
				}
				glEnd();
			}
		}
	}
	f_cg->flatEnd();
	f_cg->end();
}