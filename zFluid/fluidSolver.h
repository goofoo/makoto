#pragma once
#ifndef _FLUIDSOLVER_H
#define _FLUIDSOLVER_H

#include "../shared/zData.h"
#include "../shared/zFrameBuffer.h"
#include "fluidProgram.h"
#include <maya/MDagPath.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MDagPathArray.h>
#include <maya/MObjectArray.h>
#include <vector>
/*
struct ImpulseDesc
{
	ImpulseDesc(int _type, float _radius, float _spread, float posx, float posy, float posz, float velx, float vely, float velz)
	{
		type = _type;
		radius = _radius;
		spread = _spread;
		px = posx;
		py = posy;
		pz = posz;
		vx = velx;
		vy = vely;
		vz = velz;
	}
	int type;
	float radius, spread;
	float px, py, pz;
	float vx, vy, vz;
};
*/
class FluidSolver
{
public:
	FluidSolver(void);
	~FluidSolver(void);
	
	void initialize(int width, int height, int depth);
	void uninitialize();
	void update();
	void drawQuad();
	void drawBoundary();
	void display();
	void drawArrow();
	int getImageWidth() {return m_frame_width;}
	int getImageHeight() {return m_frame_height;}
	void clear();
	void getVelocity(float& vx, float& vy, float& vz, float x, float y, float z);
	void flatTo2D(int& s, int& t, int x, int y, int z);
	
	const float* getVelocity() {return m_velocityField;}
	const float* getTemperature() {return m_temperatureField;}
	const float* getOffset() {return m_obstacleField;}
	
	//void addImpulse(int _type, float _radius, float _spread, float posx, float posy, float posz, float velx, float vely, float velz);
	//void addObstacle(MDagPath mesh) {m_obstacles.push_back(mesh);}
	void processObstacles(const MObjectArray& obstacles);
	void processSources(const MVectorArray &points, const MVectorArray &velocities, const MDoubleArray& ages);
	
	void setBuoyancy(float val) {m_buoyancy = val/20.0f;}
	void setSwirl(float val) {m_swirl = val/10.0f;}
	void setConserveVelocity(float val) {m_keepVelocity = val;}
	void setTemperatureLoss(float val) {m_keepTemperature = val;}
	
	void drawList(const MObjectArray& obstacles);
	
	void setGridSize(float size) {m_gridSize = size;}
	void setOrigin(float x, float y, float z) {m_origin_x = x; m_origin_y = y; m_origin_z = z;}
	
protected:
	float* m_velocityField;
	float* m_temperatureField;
	float* m_obstacleField;
	float* zeros;
	
	int m_n_quad;
	
	XY* m_quad_p;
	XYZ* m_quad_pw;
	XYZW* m_quad_tz;
	XY* m_line_p;
	
	zFrameBuffer* m_fb;
	zFrameBuffer* m_fbx;
	zFrameBuffer* m_fby;
	zFrameBuffer* m_fbz;
	CFluidProgram* f_cg;
	
	int m_width, m_height, m_depth, m_tile_s, m_tile_t;
	int m_frame_width, m_frame_height;
	GLuint i_velocityTexture;
	GLuint i_divergenceTexture;
	GLuint i_vorticityTexture;
	GLuint i_pressureTexture;
	GLuint i_bufTexture;
	GLuint i_offsetTexture;
	GLuint i_impulseTexture;
	GLuint i_temperatureTexture;
	GLuint i_xTexture;
	GLuint i_yTexture;
	GLuint i_zTexture;
	
	//std::vector<ImpulseDesc> m_impulseList;
	//std::vector<MDagPath> m_obstacles;
	
	float m_buoyancy, m_swirl, m_keepVelocity, m_keepTemperature;
	
	float m_gridSize, m_origin_x, m_origin_y, m_origin_z;
};

#endif