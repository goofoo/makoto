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
	void setSwirl(float val) {m_swirl = val/10.0f;}
	void setConserveVelocity(float val) {m_keepVelocity = val;}
	void setTemperatureLoss(float val) {m_keepTemperature = val;}
	void setTemperature(const float& val) {fTemperature = val;}
	void setWind(const float& x, const float& z) {fWindX = x; fWindZ = z;}
	void setConserveDensity(float val) {m_conserve_denisty = val;}

	void setGridSize(float size) {m_gridSize = size;}
	float getGridSize() const {return m_gridSize;}
	void setOrigin(float x, float y, float z) {m_origin_x = x; m_origin_y = y; m_origin_z = z;}
	void getOrigin(float& x, float& y, float& z) const {x=m_origin_x; y=m_origin_y; z=m_origin_z; }
	
	void needDensity() {m_needDensity = 1;}
	void getDimension(int& x, int& y, int& z) const {x = m_width; y = m_height; z = m_depth;}
	
	void drawTriangleMesh(int count, const float* vert);
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
	
	GLuint img_temper, img_density;
	GLuint density_fbo;
GLuint fbo;
GLuint depthBuffer;

GLuint img_impuls;

GLuint i_velocityTexture;
GLuint i_divergenceTexture;
GLuint i_vorticityTexture;
GLuint i_pressureTexture;
GLuint i_bufTexture;
GLuint i_offsetTexture;

GLuint x_fbo;
GLuint y_fbo;
GLuint z_fbo;
GLuint i_xTexture;
	GLuint i_yTexture;
	GLuint i_zTexture;
	
	float m_conserve_denisty;
	char m_needDensity;
};

#endif