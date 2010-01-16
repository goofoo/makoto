#pragma once
#ifndef _FLUIDSOLVER_H
#define _FLUIDSOLVER_H

#include "../shared/zData.h"
#include "fluidProgram.h"
#include <maya/MDagPath.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MDagPathArray.h>
#include <maya/MObjectArray.h>

class FluidSolver
{
public:
	FluidSolver(void);
	~FluidSolver(void);
	
	void initialize(int width, int height, int depth);
	void uninitialize();
	char isInitialized() const {return fInitialized;}
	void update();
	void drawQuad();
	void showTexture(int itex, int islice);
	void drawBoundary();
	void display();
	void drawArrow();
	int getImageWidth() {return m_frame_width;}
	int getImageHeight() {return m_frame_height;}
	void clear();
	void getVelocity(float& vx, float& vy, float& vz, float x, float y, float z);
	void flatTo2D(int& s, int& t, int x, int y, int z);
	float getVoxelDensity(int x, int y, int z);
	void getVoxelVelocity(float& vx, float& vy, float& vz, int x, int y, int z);
	const float* getVelocity() {return m_velocityField;}
	
	void processObstacles(const MObjectArray& obstacles);
	void processSources(const MVectorArray &points, const MVectorArray &velocities, const MDoubleArray& ages, const MObjectArray& sources);
	
	void setBuoyancy(float val) {m_buoyancy = val/20.0f;}
	void setSwirl(float val) {m_swirl = val;}
	void setConserveVelocity(float val) {m_keepVelocity = val;}
	void setTemperatureLoss(float val) {m_keepTemperature = val;}
	void setTemperature(const float& val) {fTemperature = val;}
	void setWind(const float& x, const float& z) {fWindX = x; fWindZ = z;}
	void setConserveDensity(float val) {m_conserve_denisty = val;}
	void setSourceDensity(float val) {m_source_density = val;}
	void setDiffusion(float val) { m_diffusion = val;}

	void setGridSize(float size) {m_gridSize = size;}
	float getGridSize() const {return m_gridSize;}
	void setOrigin(float x, float y, float z) {m_origin_x = x; m_origin_y = y; m_origin_z = z;}
	void getOrigin(float& x, float& y, float& z) const {x=m_origin_x; y=m_origin_y; z=m_origin_z; }
	
	void needDensity() {m_needDensity = 1;}
	void getDimension(int& x, int& y, int& z) const {x = m_width; y = m_height; z = m_depth;}
	
	void drawTriangleMesh(int count, const float* vert);
	void drawPoints(int count, const float* vert);
	void setFrameView();
	void showSlice(int i);
	
protected:
	float* m_velocityField;
	float *m_densityField;
	int m_n_quad;
	
	XY* m_quad_p;
	XYZ* m_quad_pw;
	XYZW* m_quad_tz;
	XY* m_line_p;

	CFluidProgram* f_cg;
	
	int m_width, m_height, m_depth, m_tile_s, m_tile_t;
	int m_frame_width, m_frame_height;

	float m_buoyancy, m_swirl, m_keepVelocity, m_keepTemperature, fTemperature, fWindX, fWindZ;
	
	float m_gridSize, m_origin_x, m_origin_y, m_origin_z;
	char fInitialized;
	char fUndiagnosed;
	
// rgb
	GLuint m_rgb_fbo;
	
// 1-component	
	GLuint img_density;	// attachment 0
// 1-component
	GLuint img_temper;	// attachment 1
// 3-component
	GLuint i_velocityTexture; // attachment 2
// 3-component
	GLuint i_vorticityTexture; // attachment 3
// 1-component
	GLuint i_divergenceTexture; // attachment 4
// 1-component
	GLuint i_pressureTexture; // attachment 5
// 3-component
	GLuint i_bufTexture; // attachment 6

// rgba
	GLuint m_rgba_fbo;
// 4-component
	GLuint img_impuls;
// 4-component
	GLuint i_offsetTexture;

// rgb
	GLuint x_fbo;
	GLuint y_fbo;
	GLuint z_fbo;
// 1-component
	GLuint i_xTexture;
	GLuint i_yTexture;
	GLuint i_zTexture;
	
	float m_conserve_denisty, m_source_density;
	char m_needDensity;
	float m_diffusion;
	char m_has_obstacles;
};

#endif