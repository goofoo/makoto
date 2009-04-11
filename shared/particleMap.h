//#pragma once
#include "../shared/zData.h"

struct particleMapDesc
{
	int num_particles;
	float bbox_min[3];
	float bbox_max[3];
};

struct ptc
{
	XYZ position;
	float kd;
};

class particleMap
{
public:
	particleMap();
	~particleMap(void);
	
	void create(const XYZ* pos, const float* kd, int size);
	
	void getBBox(float bbmin[3], float bbmax[3]);
	float densityEstimate(const XYZ pos, const float max_dist, float& light);
	int save(const char* filename);
	int load(const char* filename);
	int isEmpty() {return m_isEmpty;}
	
	int particleCount() { return m_pDesc->num_particles; }
	const ptc* pParticle() { return m_particles; }
	
private:
	particleMapDesc* m_pDesc;
	ptc* m_particles;
	int m_isEmpty;
	
	void quickSort(ptc* a, int lo, int hi, short axis);
	void medianSplit(ptc* a, int lo, int hi, short axis);
	void findParticles(const XYZ pos, const float max_dist, int lo, int hi, short axis, float& hit, float &light, int& count);
};
