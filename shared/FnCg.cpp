/*
 *  FnCg.cpp
 *  
 *
 *  Created by zhang on 07-8-11.
 *  Copyright 2007 __ZHANGJIAN__. All rights reserved.
 *
 */

#include "FnCg.h"

FnCg::FnCg(void):
m_context(0)
{
	errorlog = "Cg Error Log: ";
	m_context = cgCreateContext();
	
	if(m_context == NULL) cgCheckError("Failed To Create Cg Context");
	
	if (cgGLIsProfileSupported(CG_PROFILE_VP30))
		m_vert_profile = CG_PROFILE_VP30;
	else if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
        m_vert_profile = CG_PROFILE_ARBVP1;

	if(m_vert_profile == CG_PROFILE_UNKNOWN) cgCheckError("ERROR create vertex profile");

	if (cgGLIsProfileSupported(CG_PROFILE_FP30))
		m_frag_profile = CG_PROFILE_FP30;
	else if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
        	m_frag_profile = CG_PROFILE_ARBFP1;
	
	if (m_frag_profile == CG_PROFILE_UNKNOWN) cgCheckError("ERROR create fragment profile");
}

FnCg::~FnCg(void)
{
	if(m_context!= NULL) cgDestroyContext(m_context);
}

void FnCg::loadVertexProgram(CGprogram& prog, const char* source, const char* entry)
{
	prog = cgCreateProgram(m_context, CG_SOURCE, source, m_vert_profile, entry, 0);
	
	// Validate Success
	if(prog == NULL) cgCheckError("ERROR create vertex program");
	
	cgGLLoadProgram(prog);
}

void FnCg::loadFragmentProgram(CGprogram& prog, const char* source, const char* entry)
{
	prog = cgCreateProgram(m_context, CG_SOURCE, source, m_frag_profile, entry, 0);
	
	if (prog == NULL) cgCheckError("ERROR create fragment program");
	
	cgGLLoadProgram(prog);
}

void FnCg::end()
{
	cgGLDisableProfile(m_frag_profile);
	cgGLDisableProfile(m_vert_profile);
}

void FnCg::cgBegin()
{
	cgGLEnableProfile(m_vert_profile);
	cgGLEnableProfile(m_frag_profile);
}
