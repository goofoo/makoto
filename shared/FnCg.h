#pragma once
#include <string.h>
#include <iostream>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <Cg/cg.h>
#include <Cg/cgGL.h>

using namespace std;

class FnCg
{
public:
	FnCg(void);
	~FnCg(void);

	static const char* showError()
	{
		CGerror Error = cgGetError();
		return cgGetErrorString(Error);
	}
	
	CGcontext m_context;
	CGprofile m_vert_profile;
	CGprofile m_frag_profile;
	
	void cgCheckError(const char* log)
{
	// We Need To Determine What Went Wrong
	CGerror Error = cgGetError();
	// Explaining What Went Wrong
	cout<<log<<" : "<<cgGetErrorString(Error);
}
	
	void loadVertexProgram(CGprogram& prog, const char* source, const char* entry);
	void loadFragmentProgram(CGprogram& prog, const char* source, const char* entry);
	
	void end();
	void cgBegin();
};

