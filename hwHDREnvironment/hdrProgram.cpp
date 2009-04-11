/*
 *  hdrProgram.cpp
 *  furOccGen
 *
 *  Created by zhang on 07-12-24.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "hdrProgram.h"

static const char *hdr_source =
"struct appdata	\n"
"{ \n"
"	float4 position : POSITION; \n"
"	float4 texCoord : TEXCOORD0;"
"	float4 color	: COLOR; \n"
"}; \n"
" \n"
"struct vfconn\n"
"{ \n"
"	float4 HPos	: POSITION; \n"
"	float4 texCoord : TEXCOORD0;"
"	half4 color : COLOR;"
"}; \n"
" \n"
"vfconn vertmain(appdata IN, uniform float4x4 ModelView, uniform float4x4 Projection) \n"
"{"
"	vfconn OUT; \n"
"	OUT.HPos = mul(ModelView, IN.position);"
"	OUT.HPos = mul(Projection, OUT.HPos);"
"	OUT.texCoord = IN.texCoord;"
"	OUT.color = IN.color;"
"	return OUT; \n"
"}"
"void fragmain(vfconn IN, out half4 oColor : COLOR, samplerRECT TexLatlong, uniform float Exposure)"
"{"
"	oColor = half4(h3texRECT(TexLatlong, IN.texCoord.xy)*Exposure,1);"
"}"
"\n";

void hdrProgram::init()
{
	loadVertexProgram(m_vert_voronoi, hdr_source, "vertmain");
	loadFragmentProgram(m_frag_voronoi, hdr_source, "fragmain");
	m_voronoi_modelView = cgGetNamedParameter(m_vert_voronoi, "ModelView");
	m_voronoi_projection = cgGetNamedParameter(m_vert_voronoi, "Projection");
	
	m_voronoi_tex = cgGetNamedParameter(m_frag_voronoi, "TexLatlong");
	m_voronoi_exposure = cgGetNamedParameter(m_frag_voronoi, "Exposure");
}


void hdrProgram::voronoiBegin(GLuint tex, float exposure)
{
	cgBegin();
	cgGLBindProgram(m_vert_voronoi);
	cgGLBindProgram(m_frag_voronoi);
	cgGLSetStateMatrixParameter(m_voronoi_modelView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(m_voronoi_projection, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	
	cgGLSetTextureParameter(m_voronoi_tex, tex);
	cgGLSetParameter1f(m_voronoi_exposure, exposure);
	cgGLEnableTextureParameter(m_voronoi_tex);
}

void hdrProgram::voronoiEnd()
{
	cgGLDisableTextureParameter(m_voronoi_tex);
	end();
}
//:~