#ifndef _COCTREE_H
#define _COCTREE_H
#pragma once
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <vector>
#include "zData.h"
#include "zMath.h"

#define OCTREE_N_MAX_LEVEL 10
#define OCTREE_N_MAX_COUNT 25

class zOctree
{
public:
	zOctree::zOctree();
	zOctree::~zOctree();
	
	void create(const rpcSample* data, int count, const XYZ& box_min, const XYZ& box_max, int level, std::vector<rpcSample>& cachelist);
	
	void setEmpty() {m_is_leaf = 1;}
	void draw(const XYZ& P, const XYZ& N);
	void draw();
	void displayCube();
	void displayPoints();
	int rayHitTest(const XYZ& origin, const XYZ& ray);

private:
	int m_num_record, m_level, m_is_leaf;
	XYZ m_center, m_min, m_max, m_median, m_normal, m_color;
	rpcSample m_sample;
	rpcSample* m_data;
	zOctree* m_children;
};

#endif