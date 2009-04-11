#ifndef _CRPCFILE_H
#define _CRPCFILE_H
#pragma once
/*
 *  rpcFile.h
 *
 *  Created by zhang on 07-3-14.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "zData.h"
#include <vector>
struct rpcDesc
{
	int num_samples;
};

class rpcFile
{
public:
	rpcFile();
	~rpcFile();
	
	void loadRecords(const char* filename);
	void loadSamples(const char* filename);
	int isValid() {return m_valid;}
	void getCenterAndSize(XYZ& center, float& size);
	void getCenterAndSize(rpcSample* samples, int count, XYZ& center, float& size);
	static void getMinMax(rpcSample* samples, int count, XYZ& min, XYZ& max);
	void getMinMax(XYZ& min, XYZ& max);
	void getCenterAndSize(XYZ* parray, int count, XYZ& center, float& size);
	void getSamples(XYZ* rec);

	void drawSamples();
	void drawSamples(const XYZ& P, float radius);
	static void save(rpcSample* samples, int samples_count, const char* filename);
	
	const XYZ& getCenter() {return m_center;}
	float getSize() {return m_size;}
	const rpcSample* getSamples() {return m_samples;}
	int getSampleCount() {return m_pDesc->num_samples;}
	
	void accumulateDiffuse(const XYZ& p, float& radius, float& absorba, const XYZ& vec_key, XYZ& ca);
	void accumulateDiffuse(const XYZ& p, float& radius, const XYZ& V, float& absorb, float& val);
	void accumulateDiffuse1(const XYZ& p, float& radius, const XYZ& V, const XYZ& N, float& obsorb, float& val);
	
	void accumulateSamples(const XYZ& p, float& radius, std::vector<rpcSample>& samples);
	
private:
	rpcDesc* m_pDesc;
	rpcSample* m_samples;
	
	int m_valid;
	XYZ m_center;
	float m_size;
	
	void findSample(const XYZ& P, int lo, int hi, short axis, float max_dist, float& absorba, const XYZ& vec_key, XYZ& cola, int& weighta);
	void findSample(const XYZ& P, int lo, int hi, short axis, float max_dist);
	void findSample(const XYZ& P, int lo, int hi, short axis, float max_dist, const XYZ& V, float& obsorb, float& val, int& hits);
	void findSample1(const XYZ& P, int lo, int hi, short axis, float max_dist, const XYZ& V, const XYZ& N, float& obsorb, float& val, int& hits);
	void findSample(const XYZ& P, int lo, int hi, short axis, float max_dist, std::vector<rpcSample>& samples);
};

#endif