#pragma once

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <string>
using namespace std;

class CFluidProgram
{
public:
	CFluidProgram(void);
	~CFluidProgram(void);
	
	void begin();
	void end();
	
	void cgCheckError(const char* log)
	{
	// We Need To Determine What Went Wrong
		CGerror Error = cgGetError();
	// Explaining What Went Wrong
		errorlog += log;
		errorlog += " : ";
		errorlog += cgGetErrorString(Error);
	}
	
	const char* getErrorLog() {return errorlog.c_str();}
	
	void advectBegin(GLuint iU, GLuint iQ, float x, float y, float z, float w, float alpha);
	void advectEnd();
	void divergenceBegin(GLuint i_texture);
	void divergenceEnd();
	void jacobiBegin(GLuint i_textureX, GLuint i_textureB);
	void jacobiEnd();
	void displayVectorBegin(GLuint i_texture);
	void displayVectorEnd();
	void gradientBegin(GLuint i_textureP, GLuint i_textureU);
	void gradientEnd();
	void impulseBegin(GLuint i_textureU, int type, float radius, float spread, float px, float py, float pz, float vx, float vy, float vz);
	void impulseEnd();
	void vorticityBegin(GLuint i_textureU);
	void vorticityEnd();
	void swirlBegin(GLuint i_textureV, GLuint i_textureU, float Ks);
	void swirlEnd();
	void boundaryBegin(GLuint i_textureU, float alpha);
	void boundaryEnd();
	void offsetBegin(GLuint i_textureO);
	void offsetEnd();
	void abcBegin(GLuint i_textureU, GLuint i_textureO, float x, float y, float z, float w);
	void abcEnd();
	//void addTemperatureBegin(GLuint i_textureU, float radius, float T, float px, float py, float pz);
	//void addTemperatureEnd();
	void buoyancyBegin(GLuint iU, GLuint iT, float Kb);
	void buoyancyEnd();
	void flatBegin();
	void flatEnd();
	void convexBegin(GLuint iX, GLuint iY, GLuint iZ, GLuint iold);
	void convexEnd();
	void addTemperatureBegin(GLuint i_textureU, GLuint i_textureImpulse, float k);
	void addTemperatureEnd();
	void addVelocityBegin(GLuint i_textureU, GLuint i_textureImpulse);
	void addVelocityEnd();
	void particleBegin();
	void particleEnd();
	
private:
	CGcontext m_context;
	
	CGprofile m_vert_profile;
	CGprofile m_frag_profile;
	
	CGprogram m_vert_program;
	CGprogram m_vert_particle;
	
	CGprogram m_frag_advect;
	CGprogram m_frag_divergence;
	CGprogram m_frag_jacobi;
	CGprogram m_frag_gradient;
	CGprogram m_frag_impulse;
	CGprogram m_frag_displayVector;
	CGprogram m_frag_vorticity;
	CGprogram m_frag_swirl;
	CGprogram m_frag_boundary;
	CGprogram m_frag_offset;
	CGprogram m_frag_abc;
	//CGprogram m_frag_addTemperature;
	CGprogram m_frag_buoyancy;
	CGprogram m_frag_flat;
	CGprogram m_frag_convex;
	CGprogram m_frag_addTemperature;
	CGprogram m_frag_addVelocity;
	CGprogram m_frag_particle;
	
	CGparameter modelViewMatrix;
	CGparameter advect_timeStep, advect_u, advect_q, advect_dimensions, advect_alpha;
	CGparameter divergence_q;
	CGparameter jacobi_x, jacobi_b;
	CGparameter displayVector_x;
	CGparameter gradient_p, gradient_u;
	CGparameter impulse_u, impulse_radius, impulse_c, impulse_p, impulse_spread;
	CGparameter vorticity_u;
	CGparameter swirl_v, swirl_u, swirl_Ks;
	CGparameter boundary_u, boundary_alpha;
	CGparameter offset_o;
	CGparameter abc_u, abc_o, abc_dimensions;
	//CGparameter addTemperature_u, addTemperature_T, addTemperature_radius, addTemperature_p;
	CGparameter buoyancy_u, buoyancy_t, buoyancy_Kb;
	CGparameter convex_x, convex_y, convex_z, convex_o;
	CGparameter addTemperature_u, addTemperature_impulse, addTemperature_scale;
	CGparameter addVelocity_u, addVelocity_impulse;
	CGparameter particle_modelViewMatrix;
	string errorlog;
};
