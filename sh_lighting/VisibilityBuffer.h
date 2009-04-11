#ifndef _CVISIBILITYBUFFER_H
#define _CVISIBILITYBUFFER_H
#define VIS_N_GRID 3600
#define VIS_SQRT_N_GRID 60
#define VIS_SQRT_N_HALF 30
#define VIS_SQRT_N_GRID_MINUSONE 59
/*PI/VIS_SQRT_N_GRID*/
#define VIS_DELTA_THETA 0.0525
/*PI/VIS_SQRT_N_GRID/2*/
//#define VIS_DELTA_THETA_FINE 0.025
//#define VIS_DELTA_THETA2 0.21
#include "../shared/zMath.h"

struct VisPix
{
	VisPix() {dist = 10e8; id = 938461273;}
	float dist, scale;
	unsigned int id;
};

class CVisibilityBuffer
{
public:
	CVisibilityBuffer(void);
	~CVisibilityBuffer(void);
/*3D-2D convertion*/
	static void HemisphereToST(const XYZ& vec, int& coord_s, int& coord_t);
/*Set all distance to infinite*/
	void initialize();
/*Space of the buffer, normal is z-axis, tangent is x-axis*/
	void setSpace(const XYZ& _origin, const XYZ& normal, const XYZ& tangent);
/*Write by normal and solid angle and distance*/
	void write(XYZ& sample, int& bucket_size);
	void write(XYZ& sample, float& distance, int& bucket_size);
	void write(XYZ& sample, float& distance, unsigned int node_id, float k, int& bucket_size);
	void writeBlend(XYZ& sample, float& distance, unsigned int node_id, float k, int& bucket_size);
/*Read by sample vector*/
	void read(const XYZ& ray, float& distance) const;
	void read(const XYZ& ray, float& distance, unsigned int& node_id, float& K) const;
/*Read x,y buffer*/
	float read(int x, int y) const {return m_data[y*VIS_SQRT_N_GRID + x].dist;}
	//void read(int x, int y, XYZ& color) const {color.x = m_color[(y*VIS_SQRT_N_GRID + x)*3]*0.5+0.5;color.y = m_color[(y*VIS_SQRT_N_GRID + x)*3+1]*0.5+0.5;color.z = m_color[(y*VIS_SQRT_N_GRID + x)*3+2]*0.5+0.5;}
/*get depth in buffer space*/
	float getDepth(const XYZ& p) const;
	
private:
	VisPix* m_data;
	//float* m_dist;
	//float* m_color;
	//unsigned int* m_id;
	MATRIX44F m_space;
	XYZ origin, zenith;
	int coord_s,coord_t;
};
#endif