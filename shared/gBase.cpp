/*
 *  gBase.cpp
 *  pcfViewer
 *
 *  Created by zhang on 07-9-29.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include <ostream>
#include "gBase.h"

#ifdef WIN32
PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB = NULL;
PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB = NULL;
PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB = NULL;
PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB = NULL;
PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
PFNGLGETQUERYIVPROC glGetQueryiv = NULL;
PFNGLGENQUERIESARBPROC glGenQueriesARB = NULL;
PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB = NULL;
#endif

int gBase::initExt()
{
#ifdef WIN32
	glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)wglGetProcAddress("glMultiTexCoord1iARB");
		glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)wglGetProcAddress("glMultiTexCoord2iARB");
		glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)wglGetProcAddress("glMultiTexCoord3iARB");
		glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)wglGetProcAddress("glMultiTexCoord4iARB");
		glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)wglGetProcAddress("glMultiTexCoord4fARB");
		glGetQueryiv = (PFNGLGETQUERYIVPROC)wglGetProcAddress("glGetQueryiv");
		glGenQueriesARB = (PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
		glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)wglGetProcAddress("glDeleteQueriesARB");
		
		if( !glMultiTexCoord1iARB || !glMultiTexCoord2iARB || !glMultiTexCoord3iARB || !glMultiTexCoord4iARB ||
		!glMultiTexCoord1fARB || !glMultiTexCoord2fARB || !glMultiTexCoord3fARB || !glMultiTexCoord4fARB || 
		!glGetQueryiv || !glGenQueriesARB || !glDeleteQueriesARB )
		{
			return 0;
		}
#endif
		const char *glversion = (const char*)glGetString(GL_VERSION);
		printf("GL Version: %s ",glversion);
		
		const char *ext = (const char*)glGetString( GL_EXTENSIONS );
	if( strstr( ext, "GL_ARB_occlusion_query" ) == NULL )
	{
		printf("ERROR: GL_ARB_occlusion_query extension was not found");
	}
		return 1;
}

const char* gBase::checkEXT(const char* name)
{
	const char *ext = (const char*)glGetString( GL_EXTENSIONS );
	//if( strstr( ext, name ) == NULL ) return 0;
	return ext;
}

void gBase::drawCoordinate()
{
		glBegin( GL_LINES );
		glColor3f(1,0,0);
		glVertex3i(0,0,0);
		glVertex3i(100,0,0);
		glColor3f(0,1,0);
		glVertex3i(0,0,0);
		glVertex3i(0,100,0);
		glColor3f(0,0,1);
		glVertex3i(0,0,0);
		glVertex3i(0,0,100);
		glColor3f(0.7f,0.7f,0.7f);
		/*for( int i = -10; i <= 10; i++ ) {
			if(i) {
				glVertex3i(i,0,-10);
				glVertex3i(i,0,10);
				glVertex3i(-10,0,i);
				glVertex3i(10,0,i);
			}
			else {
				glVertex3i(0,0,-10);
				glVertex3i(0,0,0);
				glVertex3i(0,0,0);
				glVertex3i(-10,0,0);
			}
		}*/
		glEnd();
}

void gBase::drawTex2DCoordinate()
{
	glBegin( GL_LINES );
		glColor3f(1,0,0);
		glVertex3i(0,0,0);
		glVertex3i(1,0,0);
		glColor3f(0,1,0);
		glVertex3i(0,0,0);
		glVertex3i(0,1,0);
		glColor3f(0.57f,0.57f,0.57f);
		for( int i = 1; i <= 10; i++ ) {
			
				glVertex3f(0,i*0.1f,0);
				glVertex3f(1,i*0.1f,0);
				
				glVertex3f(i*0.1f,0,0);
				glVertex3f(i*0.1f,1,0);
			
		}
		glEnd();
}

void gBase::drawBoundingBox(XYZ* bbox)
{
	glColor3f(0.33f,0.5f,0.33f);
	glBegin( GL_LINES );
glVertex3f(bbox[0].x, bbox[0].y, bbox[0].z);
	glVertex3f(bbox[1].x, bbox[1].y, bbox[1].z);
	
	glVertex3f(bbox[2].x, bbox[2].y, bbox[2].z);
	glVertex3f(bbox[3].x, bbox[3].y, bbox[3].z);
	
	glVertex3f(bbox[4].x, bbox[4].y, bbox[4].z);
	glVertex3f(bbox[5].x, bbox[5].y, bbox[5].z);
	
	glVertex3f(bbox[6].x, bbox[6].y, bbox[6].z);
	glVertex3f(bbox[7].x, bbox[7].y, bbox[7].z);
	
	glVertex3f(bbox[0].x, bbox[0].y, bbox[0].z);
	glVertex3f(bbox[2].x, bbox[2].y, bbox[2].z);
	
	glVertex3f(bbox[1].x, bbox[1].y, bbox[1].z);
	glVertex3f(bbox[3].x, bbox[3].y, bbox[3].z);
	
	glVertex3f(bbox[4].x, bbox[4].y, bbox[4].z);
	glVertex3f(bbox[6].x, bbox[6].y, bbox[6].z);
	
	glVertex3f(bbox[5].x, bbox[5].y, bbox[5].z);
	glVertex3f(bbox[7].x, bbox[7].y, bbox[7].z);
	
	glVertex3f(bbox[0].x, bbox[0].y, bbox[0].z);
	glVertex3f(bbox[4].x, bbox[4].y, bbox[4].z);
	
	glVertex3f(bbox[2].x, bbox[2].y, bbox[2].z);
	glVertex3f(bbox[6].x, bbox[6].y, bbox[6].z);
	
	glVertex3f(bbox[1].x, bbox[1].y, bbox[1].z);
	glVertex3f(bbox[5].x, bbox[5].y, bbox[5].z);
	
	glVertex3f(bbox[3].x, bbox[3].y, bbox[3].z);
	glVertex3f(bbox[7].x, bbox[7].y, bbox[7].z);
	glEnd();
}

void gBase::drawSplatAt(XYZ& pos, XYZ& facing, float r)
{
	XYZ tang, binor;
	facing.perpendicular(tang);
	binor = facing^tang; binor.normalize();
		
	MATRIX44F mat;
	//mat.setIdentity();
	mat.setOrientations(tang, binor, facing);
	mat.setTranslation(pos);
	
	//0
	XYZ p1(r,0,0);
	//30
	XYZ p3(0.866f*r, 0.5f*r, 0);
	//60
	XYZ p5(0.5f*r, 0.866f*r, 0);
	//90
	XYZ p7(0, r, 0);
	//120
	XYZ p9(-0.5f*r, 0.866f*r, 0);
	//150
	XYZ p11(-0.866f*r, 0.5f*r,0);
	//180
	XYZ p13(-r, 0, 0);
	//210
	XYZ p15(-0.866f*r, -0.5f*r,0);
	//240
	XYZ p17(-0.5f*r, -0.866f*r,0);
	//270
	XYZ p19(0, -r, 0);
	//300
	XYZ p21(0.5f*r, -0.866f*r, 0);
	//330
	XYZ p23(0.866f*r, -0.5f*r, 0);
	
	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	mat.transform(p9);
	mat.transform(p11);
	mat.transform(p13);
	mat.transform(p15);
	mat.transform(p17);
	mat.transform(p19);
	mat.transform(p21);
	mat.transform(p23);

	
	//glColor4f(0.5f+data.nor.x/2, 0.5f+data.nor.y/2, 0.5f+data.nor.z/2, 0.0625);
	//glColor3f(data.col.x, data.col.y, data.col.z);
	glBegin(GL_POLYGON);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glEnd();
}

void gBase::drawQuadAt(XYZ& pos, XYZ& facing, float size)
{
	XYZ tang, binor;
	facing.perpendicular(tang);
	binor = facing^tang; binor.normalize();
		
	MATRIX44F mat;
	//mat.setIdentity();
	mat.setOrientations(tang, binor, facing);
	mat.setTranslation(pos);
	
	XYZ p1(-size,-size,0);
	XYZ p3(size, -size, 0);
	XYZ p5(size, size, 0);
	XYZ p7(-size, size, 0);
	
	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	
	glBegin(GL_QUADS);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glEnd();
}

void gBase::drawSplat12(qnode& data, XYZ& eye, float scale)
{
	XYZ tang, binor;
	eye.perpendicular(tang);
	binor = eye^tang; binor.normalize();
		
	MATRIX44F mat;
	//mat.setIdentity();
	//mat.setOrientations(data.tang, data.binor, data.nor);
	mat.setOrientations(tang, binor, eye);
	mat.setTranslation(data.pos);
	
	float r=sqrt(data.area)*scale;
	
	//0
	XYZ p1(r,0,0);
	//30
	XYZ p3(0.866f*r, 0.5f*r, 0);
	//60
	XYZ p5(0.5f*r, 0.866f*r, 0);
	//90
	XYZ p7(0, r, 0);
	//120
	XYZ p9(-0.5f*r, 0.866f*r, 0);
	//150
	XYZ p11(-0.866f*r, 0.5f*r,0);
	//180
	XYZ p13(-r, 0, 0);
	//210
	XYZ p15(-0.866f*r, -0.5f*r,0);
	//240
	XYZ p17(-0.5f*r, -0.866f*r,0);
	//270
	XYZ p19(0, -r, 0);
	//300
	XYZ p21(0.5f*r, -0.866f*r, 0);
	//330
	XYZ p23(0.866f*r, -0.5f*r, 0);
	
	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	mat.transform(p9);
	mat.transform(p11);
	mat.transform(p13);
	mat.transform(p15);
	mat.transform(p17);
	mat.transform(p19);
	mat.transform(p21);
	mat.transform(p23);

	
	//glColor4f(0.5f+data.nor.x/2, 0.5f+data.nor.y/2, 0.5f+data.nor.z/2, 0.0625);
	//glColor3f(data.col.x, data.col.y, data.col.z);
	glBegin(GL_POLYGON);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glEnd();

}

void gBase::drawSplat121(qnode& data, float scale)
{
	MATRIX44F mat;
	//mat.setIdentity();
	XYZ tang, binor;
	data.nor.perpendicular(tang);
	binor = data.nor^tang; binor.normalize();
	mat.setOrientations(tang, binor, data.nor);
	mat.setTranslation(data.pos);
	
	float r=sqrt(data.area)*scale;
	
	//0
	XYZ p1(r,0,0);
	//30
	XYZ p3(0.866f*r, 0.5f*r, 0);
	//60
	XYZ p5(0.5f*r, 0.866f*r, 0);
	//90
	XYZ p7(0, r, 0);
	//120
	XYZ p9(-0.5f*r, 0.866f*r, 0);
	//150
	XYZ p11(-0.866f*r, 0.5f*r,0);
	//180
	XYZ p13(-r, 0, 0);
	//210
	XYZ p15(-0.866f*r, -0.5f*r,0);
	//240
	XYZ p17(-0.5f*r, -0.866f*r,0);
	//270
	XYZ p19(0, -r, 0);
	//300
	XYZ p21(0.5f*r, -0.866f*r, 0);
	//330
	XYZ p23(0.866f*r, -0.5f*r, 0);
	
	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	mat.transform(p9);
	mat.transform(p11);
	mat.transform(p13);
	mat.transform(p15);
	mat.transform(p17);
	mat.transform(p19);
	mat.transform(p21);
	mat.transform(p23);

	
	//glColor4f(0.5f+data.nor.x/2, 0.5f+data.nor.y/2, 0.5f+data.nor.z/2, 0.0625);
	//glColor3f(data.col.x, data.col.y, data.col.z);
	glBegin(GL_POLYGON);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glEnd();

}

void gBase::drawSplatST(float s, float t, float radius, float r, float g, float b)
{	
	MATRIX44F mat;
	//mat.setIdentity();
	mat.setTranslation(s,t,0);
	//0
	XYZ p1(radius,0,0);
	//30
	XYZ p3(0.866f*radius, 0.5f*radius, 0);
	//60
	XYZ p5(0.5f*radius, 0.866f*radius, 0);
	//90
	XYZ p7(0, radius, 0);
	//120
	XYZ p9(-0.5f*radius, 0.866f*radius, 0);
	//150
	XYZ p11(-0.866f*radius, 0.5f*radius,0);
	//180
	XYZ p13(-radius, 0, 0);
	//210
	XYZ p15(-0.866f*radius, -0.5f*radius,0);
	//240
	XYZ p17(-0.5f*radius, -0.866f*radius,0);
	//270
	XYZ p19(0, -radius, 0);
	//300
	XYZ p21(0.5f*radius, -0.866f*radius, 0);
	//330
	XYZ p23(0.866f*radius, -0.5f*radius, 0);

	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	mat.transform(p9);
	mat.transform(p11);
	mat.transform(p13);
	mat.transform(p15);
	mat.transform(p17);
	mat.transform(p19);
	mat.transform(p21);
	mat.transform(p23);
	
	glColor3f(r,g,b);
	
	glBegin(GL_POLYGON);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glEnd();

}

void gBase::drawSplat12(qnode& data, float scale)
{
	MATRIX44F mat;
	//mat.setIdentity();
		XYZ tang, binor;
	data.nor.perpendicular(tang);
	binor = data.nor^tang; binor.normalize();
	mat.setOrientations(tang, binor, data.nor);
	mat.setTranslation(data.pos);
	
	float r=sqrt(data.area)*scale;
	
	//0
	XYZ p1(r,0,0);
	//30
	XYZ p3(0.866f*r, 0.5f*r, 0);
	//60
	XYZ p5(0.5f*r, 0.866f*r, 0);
	//90
	XYZ p7(0, r, 0);
	//120
	XYZ p9(-0.5f*r, 0.866f*r, 0);
	//150
	XYZ p11(-0.866f*r, 0.5f*r,0);
	//180
	XYZ p13(-r, 0, 0);
	//210
	XYZ p15(-0.866f*r, -0.5f*r,0);
	//240
	XYZ p17(-0.5f*r, -0.866f*r,0);
	//270
	XYZ p19(0, -r, 0);
	//300
	XYZ p21(0.5f*r, -0.866f*r, 0);
	//330
	XYZ p23(0.866f*r, -0.5f*r, 0);
	
	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	mat.transform(p9);
	mat.transform(p11);
	mat.transform(p13);
	mat.transform(p15);
	mat.transform(p17);
	mat.transform(p19);
	mat.transform(p21);
	mat.transform(p23);

	
	//glColor4f(0.5f+data.nor.x/2, 0.5f+data.nor.y/2, 0.5f+data.nor.z/2, 0.0625);
//	glColor3f(data.col.x, data.col.y, data.col.z);
	glBegin(GL_POLYGON);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glEnd();

}

void gBase::drawSplat24(qnode& data)
{
	MATRIX44F mat;
	//mat.setIdentity();
		XYZ tang, binor;
	data.nor.perpendicular(tang);
	binor = data.nor^tang; binor.normalize();
	mat.setOrientations(tang, binor, data.nor);
	mat.setTranslation(data.pos);
	
	float r=sqrt(data.area)*1.414;
	
	//0
	XYZ p1(r,0,0);
	//15
	XYZ p2(0.966f*r, 0.259f*r, 0);
	//30
	XYZ p3(0.866f*r, 0.5f*r, 0);
	//45
	XYZ p4(0.707f*r, 0.707f*r,0);
	//60
	XYZ p5(0.5f*r, 0.866f*r, 0);
	//75
	XYZ p6(0.259f*r, 0.966f*r, 0);
	//90
	XYZ p7(0, r, 0);
	//105
	XYZ p8(-0.259f*r, 0.966f*r, 0);
	//120
	XYZ p9(-0.5f*r, 0.866f*r, 0);
	//135
	XYZ p10(-0.707f*r, 0.707f*r,0);
	//150
	XYZ p11(-0.866f*r, 0.5f*r,0);
	//165
	XYZ p12(-0.966f*r, 0.259f*r, 0);
	//180
	XYZ p13(-r, 0, 0);
	//195
	XYZ p14(-0.966f*r, -0.259f*r, 0);
	//210
	XYZ p15(-0.866f*r, -0.5f*r,0);
	//225
	XYZ p16(-0.707f*r, -0.707f*r,0);
	//240
	XYZ p17(-0.5f*r, -0.866f*r,0);
	//255
	XYZ p18(-0.259f*r, -0.966f*r, 0);
	//270
	XYZ p19(0, -r, 0);
	//285
	XYZ p20(0.259f*r, -0.966f*r, 0);
	//300
	XYZ p21(0.5f*r, -0.866f*r, 0);
	//315
	XYZ p22(0.707f*r, -0.707f*r, 0);
	//330
	XYZ p23(0.866f*r, -0.5f*r, 0);
	//345
	XYZ p24(0.966f*r, -0.259f*r, 0);
	
	mat.transform(p1);
	mat.transform(p2);
	mat.transform(p3);
	mat.transform(p4);
	mat.transform(p5);
	mat.transform(p6);
	mat.transform(p7);
	mat.transform(p8);
	mat.transform(p9);
	mat.transform(p10);
	mat.transform(p11);
	mat.transform(p12);
	mat.transform(p13);
	mat.transform(p14);
	mat.transform(p15);
	mat.transform(p16);
	mat.transform(p17);
	mat.transform(p18);
	mat.transform(p19);
	mat.transform(p20);
	mat.transform(p21);
	mat.transform(p22);
	mat.transform(p23);
	mat.transform(p24);

	//glColor4f(0.5f+data.nor.x/2, 0.5f+data.nor.y/2, 0.5f+data.nor.z/2, 0.0625);
	//glColor3f(data.col.x, data.col.y, data.col.z);
	glBegin(GL_POLYGON);
	
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p4.x, p4.y, p4.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p6.x, p6.y, p6.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p8.x, p8.y, p8.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p10.x, p10.y, p10.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p12.x, p12.y, p12.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p14.x, p14.y, p14.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p16.x, p16.y, p16.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p18.x, p18.y, p18.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p20.x, p20.y, p20.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p22.x, p22.y, p22.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glVertex3f(p24.x, p24.y, p24.z);

	glEnd();
}

void gBase::drawSplat12(float px, float py, float pz, float nx, float ny, float nz, float r)
{
	XYZ pos(px, py, pz), nor(nx, ny, nz);
	nor.normalize();
	
	XYZ tang, binor;
	
	nor.perpendicular(tang);
	binor = nor^tang; binor.normalize();
	
	MATRIX44F mat;
	//mat.setIdentity();
	mat.setOrientations(tang, binor, nor);
	mat.setTranslation(pos);
	
	//0
	XYZ p1(r,0,0);
	//30
	XYZ p3(0.866f*r, 0.5f*r, 0);
	//60
	XYZ p5(0.5f*r, 0.866f*r, 0);
	//90
	XYZ p7(0, r, 0);
	//120
	XYZ p9(-0.5f*r, 0.866f*r, 0);
	//150
	XYZ p11(-0.866f*r, 0.5f*r,0);
	//180
	XYZ p13(-r, 0, 0);
	//210
	XYZ p15(-0.866f*r, -0.5f*r,0);
	//240
	XYZ p17(-0.5f*r, -0.866f*r,0);
	//270
	XYZ p19(0, -r, 0);
	//300
	XYZ p21(0.5f*r, -0.866f*r, 0);
	//330
	XYZ p23(0.866f*r, -0.5f*r, 0);
	
	mat.transform(p1);
	mat.transform(p3);
	mat.transform(p5);
	mat.transform(p7);
	mat.transform(p9);
	mat.transform(p11);
	mat.transform(p13);
	mat.transform(p15);
	mat.transform(p17);
	mat.transform(p19);
	mat.transform(p21);
	mat.transform(p23);

	glBegin(GL_POLYGON);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p9.x, p9.y, p9.z);
	glVertex3f(p11.x, p11.y, p11.z);
	glVertex3f(p13.x, p13.y, p13.z);
	glVertex3f(p15.x, p15.y, p15.z);
	glVertex3f(p17.x, p17.y, p17.z);
	glVertex3f(p19.x, p19.y, p19.z);
	glVertex3f(p21.x, p21.y, p21.z);
	glVertex3f(p23.x, p23.y, p23.z);
	glEnd();

}

void gBase::drawQuad(float x, float y, float z, float w, float t)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex3f(x, y, z);
	glTexCoord2f(t, 0); glVertex3f(x+w, y, z);
	glTexCoord2f(t, t); glVertex3f(x+w, y+w, z);
	glTexCoord2f(0, t); glVertex3f(x, y+w, z);
	glEnd();
}

void gBase::texCoord4f(GLenum target, float x, float y, float z, float w)
{
	glMultiTexCoord4fARB(target, x, y, z, w);
}

void gBase::genTexture(GLuint& tex, GLenum target, int width, int height, GLint internalformat, GLenum format, GLenum type)
{
	if(tex>0) glDeleteTextures(1, &tex);
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexImage2D(target, 0, internalformat, width, height, 0, format, type, 0);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void gBase::genTexture(GLuint& tex, GLenum target, int width, int height, GLint internalformat, GLenum format, GLenum type, const float* data)
{
	if(tex>0) glDeleteTextures(1, &tex );
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexImage2D(target, 0, internalformat, width, height, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void gBase::checkOcclusionQuerySupported()
{
	GLint bitsSupported;
// check to make sure functionality is supported
	glGetQueryiv(GL_QUERY_COUNTER_BITS_ARB, GL_QUERY_COUNTER_BITS, &bitsSupported);
	if (bitsSupported == 0) {
// render scene without using occlusion queries
		printf("no occlusion");
	}
}

void gBase::genQuery(GLuint n, GLuint* ids)
{
	glGenQueriesARB( n, ids );
}

void gBase::deleteQuery(GLuint n, GLuint* ids)
{
	glDeleteQueriesARB( n, ids );
}

void gBase::drawTextureQuad(GLuint width, GLint height, const float* data, float exposure)
{
	float h = 0.00613f;
	glBegin(GL_POINTS);
	for(int j=0; j< (int)height; j++)
	{
		for(int i=0; i< (int)width; i++)
		{
			glColor4f(data[(j*width+i)*4]*exposure, data[(j*width+i)*4+1]*exposure, data[(j*width+i)*4+2]*exposure, data[(j*width+i)*4+3]*exposure);
			glVertex3f(i*h, j*h, 0.0f);
		}
	}
	glEnd();
}

void sphparam(float u, float v, float r, float& x, float& y, float& z)
{
	float theta = PI*v, phi = 2.0f * PI * u + 0.5*PI;
			
    x = r*sin(theta) * cos(phi);
    y = -r*cos(theta);
    z = -r*sin(theta) * sin(phi);
}

void gBase::sph2vec(float u, float v, float& x, float& y, float& z)
{
	float theta = PI*v, phi = 2.0f * PI * u + 0.5*PI;
			
    x = sin(theta) * cos(phi);
    y = -cos(theta);
    z = -sin(theta) * sin(phi);
}

void gBase::drawTextureSphere(GLuint width, GLint height, const float* data, float exposure, XYZ* coord)
{
	float mag = 10;
	int i1, j1;
	glBegin(GL_QUADS);
	for(int j=0; j< (int)height-1; j++)
	{
		j1 = j+1;
		for(int i=0; i< (int)width; i++)
		{
			i1 = (i+1)%width;

			glColor4f(data[(j*width+i)*4]*exposure, data[(j*width+i)*4+1]*exposure, data[(j*width+i)*4+2]*exposure, data[(j*width+i)*4+3]*exposure);
			glVertex3f(coord[j*width+i].x*mag, coord[j*width+i].y*mag, coord[j*width+i].z*mag);
			
			glColor4f(data[(j*width+i1)*4]*exposure, data[(j*width+i1)*4+1]*exposure, data[(j*width+i1)*4+2]*exposure, data[(j*width+i1)*4+3]*exposure);
			glVertex3f(coord[j*width+i1].x*mag, coord[j*width+i1].y*mag, coord[j*width+i1].z*mag);
			
			glColor4f(data[(j1*width+i1)*4]*exposure, data[(j1*width+i1)*4+1]*exposure, data[(j1*width+i1)*4+2]*exposure, data[(j1*width+i1)*4+3]*exposure);
			glVertex3f(coord[j1*width+i1].x*mag, coord[j1*width+i1].y*mag, coord[j1*width+i1].z*mag);
			
			glColor4f(data[(j1*width+i)*4]*exposure, data[(j1*width+i)*4+1]*exposure, data[(j1*width+i)*4+2]*exposure, data[(j1*width+i)*4+3]*exposure);
			glVertex3f(coord[j1*width+i].x*mag, coord[j1*width+i].y*mag, coord[j1*width+i].z*mag);
		}
	}
	glEnd();
}

void gBase::getSphCoord(GLuint width, GLint height, XYZ* data)
{
	float longh = 1.0f/width;
	float lath = 1.0f/height;
	float x, y, z, mag = 1.0f;

	for(int j=0; j< (int)height; j++)
	{
		for(int i=0; i< (int)width; i++)
		{
			sphparam(longh*i, lath*j, mag, x, y, z);
			data[j*width+i].x = x;
			data[j*width+i].y = y;
			data[j*width+i].z = z;
		}
	}

}

void gBase::drawQuadArray(int width, int height, float exposure, float* data)
{
	int i1, j1;
	glBegin(GL_QUADS);
	for(int j=0; j< height-1; j++)
	{
		j1 = j+1;
		for(int i=0; i< width-1; i++)
		{
			i1 = i+1;

			glColor3f(data[(j*width+i)*6]*exposure, data[(j*width+i)*6+1]*exposure, data[(j*width+i)*6+2]*exposure);
			glVertex3f(data[(j*width+i)*6+3], data[(j*width+i)*6+4], data[(j*width+i)*6+5]);
			
			glColor3f(data[(j*width+i1)*6]*exposure, data[(j*width+i1)*6+1]*exposure, data[(j*width+i1)*6+2]*exposure);
			glVertex3f(data[(j*width+i1)*6+3], data[(j*width+i1)*6+4], data[(j*width+i1)*6+5]);
			
			glColor3f(data[(j1*width+i1)*6]*exposure, data[(j1*width+i1)*6+1]*exposure, data[(j1*width+i1)*6+2]*exposure);
			glVertex3f(data[(j1*width+i1)*6+3], data[(j1*width+i1)*6+4], data[(j1*width+i1)*6+5]);
			
			glColor3f(data[(j1*width+i)*6]*exposure, data[(j1*width+i)*6+1]*exposure, data[(j1*width+i)*6+2]*exposure);
			glVertex3f(data[(j1*width+i)*6+3], data[(j1*width+i)*6+4], data[(j1*width+i)*6+5]);
		}
	}
	glEnd();
}

void gBase::drawPointArray(int length, float exposure, float* data)
{
	glBegin(GL_POINTS);
		for(int i=0; i< length; i++)
		{
			glColor3f(data[i*6]*exposure, data[i*6+1]*exposure, data[i*6+2]*exposure);
			glVertex3f(data[i*6+3], data[i*6+4], data[i*6+5]);
		}

	glEnd();
}

void gBase::drawSprite(XYZ& center, float width, MATRIX44F& space)
{
	XYZ p = center + XYZ(-width, -width , 0);
	space.transform(p);
	glVertex3f(p.x, p.y, p.z);
	
	p = center + XYZ(+width, -width , 0);
	space.transform(p);
	glVertex3f(p.x, p.y, p.z);
	
	p = center + XYZ(+width, +width , 0);
	space.transform(p);
	glVertex3f(p.x, p.y, p.z);
	
	p = center + XYZ(-width, +width , 0);
	space.transform(p);
	glVertex3f(p.x, p.y, p.z);
}

void gBase::drawBox(const float xmin, const float xmax, const float ymin, const float ymax, const float zmin, const float zmax)
{
	glBegin(GL_QUADS);
			glVertex3f(xmin, ymin, zmin);
			glVertex3f(xmax, ymin, zmin);
			glVertex3f(xmax, ymax, zmin);
			glVertex3f(xmin, ymax, zmin);
		
			glVertex3f(xmin, ymin, zmax);
			glVertex3f(xmax, ymin, zmax);
			glVertex3f(xmax, ymax, zmax);
			glVertex3f(xmin, ymax, zmax);

			glVertex3f(xmin, ymin, zmax);
			glVertex3f(xmax, ymin, zmax);
			glVertex3f(xmax, ymin, zmin);
			glVertex3f(xmin, ymin, zmin);
			
			glVertex3f(xmin, ymax, zmax);
			glVertex3f(xmax, ymax, zmax);
			glVertex3f(xmax, ymax, zmin);
			glVertex3f(xmin, ymax, zmin);
	
			glVertex3f(xmax, ymin, zmax);
			glVertex3f(xmax, ymin, zmin);
			glVertex3f(xmax, ymax, zmin);
			glVertex3f(xmax, ymax, zmax);
		
			glVertex3f(xmin, ymin, zmin);
			glVertex3f(xmin, ymin, zmax);
			glVertex3f(xmin, ymax, zmax);
			glVertex3f(xmin, ymax, zmin);
	glEnd();
}

void gBase::drawLocator(const XYZ& origin)
{
	glColor3f(0.1, 0.9, 0.1);
	glBegin(GL_LINES);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x+1, origin.y, origin.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x-1, origin.y, origin.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y+1, origin.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y-1, origin.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y, origin.z+1);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y, origin.z-1);
	glEnd();
}
//~: