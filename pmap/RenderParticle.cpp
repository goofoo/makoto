/*
 *  RenderParticle.cpp
 *  pmap
 *
 *  Created by jian zhang on 12/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderParticle.h"
#include "../shared/QuickSortIdx.h"
#include "../shared/zData.h"

#ifndef __APPLE__
#include "../shared/gExtension.h"
#endif

#include <ImfHeader.h>
#include <ImfArray.h>
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfTiledOutputFile.h>
#include <ImfTiledInputFile.h>
#include <ImfChannelList.h>

#include <ImfBoxAttribute.h>
#include <ImfChannelListAttribute.h>
#include <ImfCompressionAttribute.h>
#include <ImfChromaticitiesAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfEnvmapAttribute.h>
#include <ImfDoubleAttribute.h>
#include <ImfIntAttribute.h>
#include <ImfLineOrderAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfOpaqueAttribute.h>
#include <ImfStringAttribute.h>
#include <ImfVecAttribute.h>

using namespace Imf;
using namespace Imath;

RenderParticle::RenderParticle() : m_isInitialized(0),
m_image_fbo(0),
m_image_tex(0),
m_shadow_fbo(0),
m_shadow_tex(0),
m_noise_tex(0),
m_downSample(2)
{}

RenderParticle::~RenderParticle() {uninitialize();}

void RenderParticle::initialize()
{
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &tbo);
	shader = new GLSLSprite();
	
	glGenFramebuffersEXT(1, &m_shadow_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_shadow_fbo);
	
	glGenTextures(1, &m_shadow_tex);
	glBindTexture(GL_TEXTURE_2D, m_shadow_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_shadow_tex, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	
	glGenTextures(1, &m_noise_tex);	
	glBindTexture(GL_TEXTURE_3D, m_noise_tex);	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	
#define PARTICLENOISE_WIDTH	128
#define PARTICLENOISE_HEIGHT 128
#define PARTICLENOISE_DEPTH	128
	unsigned char *texels = new unsigned char[PARTICLENOISE_WIDTH*PARTICLENOISE_HEIGHT*PARTICLENOISE_DEPTH*4];
	int u, v, w;
	for(w=0; w<PARTICLENOISE_DEPTH; w++) {
		for(v=0; v<PARTICLENOISE_HEIGHT; v++) {
			for(u=0; u<PARTICLENOISE_WIDTH; u++) {
#ifndef __APPLE__
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4] = rand()%256;
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4+1] = rand()%256;
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4+2] = rand()%256;
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4+3] = rand()%256;
#else
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4] = random()%256;
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4+1] = random()%256;
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4+2] = random()%256;
				texels[(w*(PARTICLENOISE_WIDTH * PARTICLENOISE_HEIGHT)+v*PARTICLENOISE_WIDTH+u)*4+3] = random()%256;
#endif
			}
		}
	}
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, PARTICLENOISE_WIDTH, PARTICLENOISE_HEIGHT, PARTICLENOISE_DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
	delete[] texels;
	
	m_isInitialized = 1;
}

void RenderParticle::uninitialize()
{
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &tbo);
	delete shader;
	if(m_shadow_fbo) glDeleteFramebuffersEXT(1, &m_shadow_fbo);
	if(m_shadow_tex) glDeleteTextures(1, &m_shadow_tex);
	if(m_image_fbo) glDeleteFramebuffersEXT(1, &m_image_fbo);
	if(m_image_tex) glDeleteTextures(1, &m_image_tex);
	if(m_noise_tex) glDeleteTextures(1, &m_noise_tex);
}

void RenderParticle::setVertex(float* vertex) 
{ 
	m_pVertex = vertex;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	unsigned int size = m_num_particle * sizeof(float) * 4;
        glBufferData(GL_ARRAY_BUFFER, size, vertex, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void RenderParticle::setCoord(float* coord) 
{ 
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	unsigned int size = m_num_particle * sizeof(float) * 4;
        glBufferData(GL_ARRAY_BUFFER, size, coord, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void RenderParticle::sort()
{
	unsigned int size = m_num_particle * sizeof(unsigned int);
	unsigned int* idxbuffer = new unsigned int[m_num_particle];
	unsigned int i;
	for(i=0; i<m_num_particle; i++) idxbuffer[i]= i;
	
// calc sort vec
	XYZ dirLight(m_light_vec_x, m_light_vec_y, m_light_vec_z);
	dirLight.normalize();
	
	XYZ dirView(m_view_x, m_view_y, m_view_z);
	dirView.normalize();
	
// check invert
	m_invert = 0;
	if(dirView.dot(dirLight) < 0.f) {
		dirView *= -1.0f;
		m_invert = 1;
	}
	
	XYZ vec_sort = dirView + dirLight;
	vec_sort.normalize();
	
	float *key = new float[m_num_particle];
	for(i=0; i<m_num_particle; i++) {
// calc depth 
		key[i]= (m_pVertex[i*4] -m_eye_x) * vec_sort.x + (m_pVertex[i*4+1] -m_eye_y) * vec_sort.y + (m_pVertex[i*4+2] -m_eye_z) * vec_sort.z;
	}
	
	QuickSortIdx::sort(key, idxbuffer, 0, m_num_particle-1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idxbuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	delete[] idxbuffer;
	delete[] key;
}

void RenderParticle::render()
{
// set shadow matrix
	MATRIX44F mat;
	mat.v[0][0] = -m_light_side_x * m_light_size;
	mat.v[0][1] = -m_light_side_y * m_light_size;
	mat.v[0][2] = -m_light_side_z * m_light_size;
	mat.v[1][0] = m_light_up_x * m_light_size;
	mat.v[1][1] = m_light_up_y * m_light_size;
	mat.v[1][2] = m_light_up_z * m_light_size;
	mat.v[2][0] = -m_light_vec_x * m_light_size;
	mat.v[2][1] = -m_light_vec_y * m_light_size;
	mat.v[2][2] = -m_light_vec_z * m_light_size;
	mat.v[3][0] = m_light_x;
	mat.v[3][1] = m_light_y;
	mat.v[3][2] = m_light_z;
	
	mat.inverse();
	
	float mm[16];
		mm[0] = mat.v[0][0]; mm[1] = mat.v[0][1]; mm[2] = mat.v[0][2]; mm[3] = mat.v[0][3];
		mm[4] = mat.v[1][0]; mm[5] = mat.v[1][1]; mm[6] = mat.v[1][2]; mm[7] = mat.v[1][3];
		mm[8] = mat.v[2][0]; mm[9] = mat.v[2][1]; mm[10] = mat.v[2][2]; mm[11] = mat.v[2][3];
		mm[12] = mat.v[3][0]; mm[13] = mat.v[3][1]; mm[14] = mat.v[3][2]; mm[15] = mat.v[3][3];
		
	shader->setShadowMatrix(mm);
	//
	
// set states
	glShadeModel(GL_SMOOTH);
	glColor4f(1,1,1,1);
	glClearColor(0.0f, 0.0f,0.0f, 0.0f);
	glEnable(GL_BLEND);
	glDepthMask( GL_FALSE );
	glDisable(GL_DEPTH_TEST);
	
	shader->setNoiseTex(m_noise_tex);
	shader->setShadowTex(m_shadow_tex);
	
// clean up
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
	glViewport(0,0,m_image_width, m_image_height);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_shadow_fbo);
	glViewport(0,0, 512, 512);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
// calc per-slice count
	int count_per_slice = 39;
	int num_slice = m_num_particle / count_per_slice;
	
	for(int i = 0; i<= num_slice; i++) {
		int start = i * count_per_slice;
		int end = start + count_per_slice;
		if(end > m_num_particle) end = m_num_particle;
		
		int count = end - start;
		
		if(count > 0) {
// primary draw
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
			glViewport(0,0,m_image_width, m_image_height);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			
			if(m_invert) glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
			else glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

			setPrimProjection();
			
	
			shader->enable();
			
			drawPoints(start, count);
			
			shader->disable();
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
// shadow draw
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_shadow_fbo);
			glViewport(0,0, 512, 512);
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
			
			setShadowProjection();
	
			shader->enableShadow();
			
			drawPoints(start, count);
			
			shader->disable();
			
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			
		}
	}
}

void RenderParticle::compose()
{
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	glViewport(0,0, m_image_width*m_downSample, m_image_height*m_downSample);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-1, 1, -1, 1, 0.1, 10.0);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
		gluLookAt(0,0,1,
				  0,0,-1,
				  0, 1, 0);
	
	glBindTexture(GL_TEXTURE_2D, m_image_tex);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
    glEnd();
	glDisable(GL_TEXTURE_2D);

}

void RenderParticle::showShadow()
{
	glDisable(GL_BLEND);
	glViewport(0,0, 256, 256);
	
	glBindTexture(GL_TEXTURE_2D, m_shadow_tex);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	
	glDepthMask( GL_TRUE );
	
	glEnable(GL_DEPTH_TEST);
}

void RenderParticle::drawPoints(int start, int count)
{/*
// calc per-slice count
	int num_slice = RENDERPARTICLE_NUM_SLICE;
	int count_per_slice = m_num_particle / num_slice;
	if(count_per_slice < 1) {
		num_slice = m_num_particle;
		count_per_slice = 1;
	}
	
	if(num_slice * count_per_slice < m_num_particle) num_slice++;
	
	for(int i = 0; i< num_slice; i++) {
		int start = i * count_per_slice;
		int end = start + count_per_slice;
		if(end > m_num_particle) end = m_num_particle;
		
		int count = end - start;
		
		if(count > 0) {*/
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			
			
			glVertexPointer(4, GL_FLOAT, 0, 0);
			glEnableClientState(GL_VERTEX_ARRAY);
				
			glBindBuffer(GL_ARRAY_BUFFER, tbo);

			glTexCoordPointer(4, GL_FLOAT, 0, 0);
			glClientActiveTexture(GL_TEXTURE0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, (void*) (start*sizeof(unsigned int)) );
				
				//glDrawElements(GL_POINTS, m_num_particle, GL_UNSIGNED_INT, 0 );
				
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				//glDrawArrays(GL_POINTS, 0, m_num_particle);
				
			glDisableClientState(GL_VERTEX_ARRAY);	
			glClientActiveTexture(GL_TEXTURE0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		/*}
	}
*/	
}

void RenderParticle::setImageDim(int w, int h) 
{
	int tw = w/m_downSample;
	int th = h/m_downSample;
	
	if(tw != m_image_width || th != m_image_height || !m_image_fbo || !m_image_tex) {
		m_image_width = w/m_downSample; m_image_height = h/m_downSample;
		
		if(m_image_fbo) glDeleteFramebuffersEXT(1, &m_image_fbo);
		if(m_image_tex) glDeleteTextures(1, &m_image_tex);
		
		glGenFramebuffersEXT(1, &m_image_fbo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
		
		glGenTextures(1, &m_image_tex);
		glBindTexture(GL_TEXTURE_2D, m_image_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, m_image_width, m_image_height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, 
						GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, 
						GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, 
						GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, 
						GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_image_tex, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
}

void RenderParticle::setLightVector(float x, float y, float z) 
{
	m_light_vec_x=x - m_light_x; 
	m_light_vec_y=y - m_light_y; 
	m_light_vec_z=z - m_light_z;
	
	XYZ dirLight(m_light_vec_x, m_light_vec_y, m_light_vec_z);
	m_light_clipfar = dirLight.length() + m_light_size * 4;
	dirLight.normalize();
	
	m_light_vec_x = dirLight.x; 
	m_light_vec_y = dirLight.y; 
	m_light_vec_z = dirLight.z;
	
	XYZ up(0,1,0);
	if(dirLight.y > 0.9f || dirLight.y < -0.9f) up = XYZ(1,0,0);
	
	XYZ side = up^dirLight;
	side.normalize();
	up = dirLight^side;
	up.normalize();
	
	m_light_up_x = up.x;
	m_light_up_y = up.y;
	m_light_up_z = up.z;
	
	m_light_side_x = side.x;
	m_light_side_y = side.y;
	m_light_side_z = side.z;
}

void RenderParticle::setFrustum(float l, float r, float b, float t, float n, float f)
{
	m_left = l;
	m_right = r; 
	m_bottom = b; 
	m_top = t; 
	m_near = n; 
	m_far = f;
}

void RenderParticle::setPrimProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glFrustum (m_left, m_right, m_bottom, m_top, m_near, m_far);
	
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	
	gluLookAt (m_eye_x, m_eye_y, m_eye_z,
			   m_eye_x + m_view_x, m_eye_y + m_view_y, m_eye_z + m_view_z,
			  m_up_x, m_up_y, m_up_z);
}

void RenderParticle::setShadowProjection()
{
		glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glOrtho(-m_light_size, m_light_size, -m_light_size, m_light_size, 1.0, m_light_clipfar);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
		gluLookAt(m_light_x,m_light_y,m_light_z,
				  m_light_x + m_light_vec_x, m_light_y + m_light_vec_y, m_light_z + m_light_vec_z,
				  m_light_up_x,m_light_up_y,m_light_up_z);
}

void RenderParticle::updateParam(GLSLSpritePARAM& param)
{
	shader->setParam(param);
}

void RenderParticle::setDownSample(int val)
{
	if(val==0) m_downSample = 1;
	else if(val==1) m_downSample = 2;
	else m_downSample = 4;
}

void RenderParticle::saveToFile(const char *filename, int outWidth, int outHeight)
{
	setImageDim(outWidth, outHeight);
// calculate display widnow	
	double ratio = (double)outHeight / (double)outWidth;
	double radians = 0.0174532925 * m_fov * 0.5; // half aperture degrees to radians 
	double wd2 = m_near * tan(radians);
	
	GLdouble left, right, top, bottom;
	left  = -wd2;
	right = left + wd2*2;
	
	top = wd2 * ratio;
	bottom = top -  wd2*2*ratio;
// update, keep clipping	
	setFrustum(left, right, bottom, top, m_near, m_far);
	render();
	
// get pixels
	float * tmp = new float[m_image_width * m_image_height * 4];
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_image_fbo);
	glViewport(0,0,m_image_width, m_image_height);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);        

	glReadPixels( 0, 0,  m_image_width, m_image_height, GL_RGBA, GL_FLOAT, tmp);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
// write to exr
	
	half *idr_r = new half[m_image_width * m_image_height];
	half *idr_g = new half[m_image_width * m_image_height];
	half *idr_b = new half[m_image_width * m_image_height];
	half *idr_a = new half[m_image_width * m_image_height];
	
	for(int j=0; j< m_image_height; j++) {
		int invj = m_image_height - 1 -j;
		for(int i=0; i<m_image_width; i++) {
			idr_r[j*m_image_width + i] = (half)tmp[(invj*m_image_width + i)*4];
			idr_g[j*m_image_width + i] = (half)tmp[(invj*m_image_width + i)*4+1];
			idr_b[j*m_image_width + i] = (half)tmp[(invj*m_image_width + i)*4+2];
			idr_a[j*m_image_width + i] = (half)tmp[(invj*m_image_width + i)*4+3];
		}
	}
// write exr
	Header idrheader (m_image_width, m_image_height); 

		idrheader.channels().insert ("R", Channel (HALF));
		idrheader.channels().insert ("G", Channel (HALF));                                   // 1 
        idrheader.channels().insert ("B", Channel (HALF));
		idrheader.channels().insert ("A", Channel (HALF));                   // 2  
    
        OutputFile idrfile (filename, idrheader);                               // 4 
        FrameBuffer idrframeBuffer;
		 idrframeBuffer.insert ("R",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_r,            // base   // 8 
                                   sizeof (*idr_r) * 1,       // xStride// 9 
                                   sizeof (*idr_r) * m_image_width));
        idrframeBuffer.insert ("G",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_g,            // base   // 8 
                                   sizeof (*idr_g) * 1,       // xStride// 9 
                                   sizeof (*idr_g) * m_image_width));
		 idrframeBuffer.insert ("B",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_b,            // base   // 8 
                                   sizeof (*idr_b) * 1,       // xStride// 9 
                                   sizeof (*idr_b) * m_image_width));
		 idrframeBuffer.insert ("A",                                // name   // 6 
                            Slice (HALF,                        // type   // 7 
                                   (char *) idr_a,            // base   // 8 
                                   sizeof (*idr_a) * 1,       // xStride// 9 
                                   sizeof (*idr_a) * m_image_width));
       
        idrfile.setFrameBuffer (idrframeBuffer);                                // 16 
        idrfile.writePixels (m_image_height); 
        
// cleanup
	delete[] idr_r;
	delete[] idr_g;
	delete[] idr_b;
	delete[] idr_a;
	delete[] tmp;
}
//:~