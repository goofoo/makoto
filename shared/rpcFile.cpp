/*
 *  rpcFile.cpp
 *
 *  Created by zhang on 07-3-14.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include <stdio.h>
#include "rpcFile.h"
#include "zMath.h"

rpcFile::rpcFile():
m_pDesc(0),
m_samples(0),
m_valid(0)
{
}

rpcFile::~rpcFile()
{
	if(m_pDesc) delete m_pDesc;
	if(m_samples) delete[] m_samples;
}

void rpcFile::loadSamples(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if(fp)
	{
		m_pDesc = new rpcDesc();
	
		int stat = fread(m_pDesc,sizeof(struct rpcDesc),1,fp);
		if(stat != 1) 
		{
			fclose(fp);
			return;
		}

		m_samples = new rpcSample[m_pDesc->num_samples];
		stat = fread(m_samples, m_pDesc->num_samples*sizeof(rpcSample), 1, fp);
		if(stat != 1) 
		{
			fclose(fp);
			return;
		}
		
		fclose(fp);
		
		m_valid = 1;
	}
	else printf("Cannot open file: %s\n", filename);
}

void rpcFile::drawSamples()
{

	//glBegin(GL_LINES);
	glBegin(GL_POINTS);
	for(int i=0; i<m_pDesc->num_samples; i++)
	{
		glColor3f(m_samples[i].col.x, m_samples[i].col.y, m_samples[i].col.z);
		glVertex3f(m_samples[i].pos.x, m_samples[i].pos.y, m_samples[i].pos.z);
		//glVertex3f(m_samples[i].pos.x+m_samples[i].nor.x, m_samples[i].pos.y+m_samples[i].nor.y, m_samples[i].pos.z+m_samples[i].nor.z);
	}
	glEnd();
}


void rpcFile::save(rpcSample* samples, int samples_count, const char* filename)
{
	rpcDesc* header = new rpcDesc;

	header->num_samples = samples_count;
	
	// create kd_tree
	median_split(samples, 0 , samples_count-1, 0);
	
	FILE *fp;

	fp = fopen(filename, "wb");
	if(fp)
	{
		fwrite(header,sizeof(struct rpcDesc),1,fp);
		fwrite(samples, samples_count*sizeof(rpcSample), 1, fp);
//		printf("%i points written to %s\n", count, filename);
		
		fclose(fp);
	}
	else
		printf("Dumpgrid failed to open %s to write", filename);
		
	delete header;
}

void rpcFile::getCenterAndSize(XYZ* parray, int count, XYZ& center, float& size)
{
	XYZ min, max;
	quick_sort(parray, 0, count-1, 0);
	min.x = parray[0].x;
	max.x = parray[count-1].x;
	
	quick_sort(parray, 0, count-1, 1);
	min.y = parray[0].y;
	max.y = parray[count-1].y;
	
	quick_sort(parray, 0, count-1, 2);
	min.z = parray[0].z;
	max.z = parray[count-1].z;

	center.x = (max.x + min.x)/2;
	center.y = (max.y + min.y)/2;
	center.z = (max.z + min.z)/2;
	
	size = max.x - min.x;
	float tmp = max.y - min.y;
	if(tmp>size) size = tmp;
	tmp = max.z - min.z;
	if(tmp>size) size = tmp;
}

void rpcFile::getCenterAndSize(XYZ& center, float& size)
{
	XYZ* parray = new XYZ[m_pDesc->num_samples];
	for(int i=0; i<m_pDesc->num_samples; i++) parray[i] = m_samples[i].pos;
	
	XYZ min, max;
	quick_sort(parray, 0, m_pDesc->num_samples-1, 0);
	min.x = parray[0].x;
	max.x = parray[m_pDesc->num_samples-1].x;
	
	quick_sort(parray, 0, m_pDesc->num_samples-1, 1);
	min.y = parray[0].y;
	max.y = parray[m_pDesc->num_samples-1].y;
	
	quick_sort(parray, 0, m_pDesc->num_samples-1, 2);
	min.z = parray[0].z;
	max.z = parray[m_pDesc->num_samples-1].z;

	center.x = (max.x + min.x)/2;
	center.y = (max.y + min.y)/2;
	center.z = (max.z + min.z)/2;
	
	size = max.x - min.x;
	float tmp = max.y - min.y;
	if(tmp>size) size = tmp;
	tmp = max.z - min.z;
	if(tmp>size) size = tmp;
	
	delete[] parray;
}

void rpcFile::getCenterAndSize(rpcSample* samples, int count, XYZ& center, float& size)
{
	XYZ* parray = new XYZ[count];
	for(int i=0; i<count; i++) parray[i] = samples[i].pos;
	
	XYZ min, max;
	quick_sort(parray, 0, count-1, 0);
	min.x = parray[0].x;
	max.x = parray[count-1].x;
	
	quick_sort(parray, 0, count-1, 1);
	min.y = parray[0].y;
	max.y = parray[count-1].y;
	
	quick_sort(parray, 0, count-1, 2);
	min.z = parray[0].z;
	max.z = parray[count-1].z;

	center.x = (max.x + min.x)/2;
	center.y = (max.y + min.y)/2;
	center.z = (max.z + min.z)/2;
	
	size = max.x - min.x;
	float tmp = max.y - min.y;
	if(tmp>size) size = tmp;
	tmp = max.z - min.z;
	if(tmp>size) size = tmp;
	
	delete[] parray;
}

void rpcFile::getMinMax(rpcSample* samples, int count, XYZ& min, XYZ& max)
{
	XYZ* parray = new XYZ[count];
	for(int i=0; i<count; i++) parray[i] = samples[i].pos;
	
	quick_sort(parray, 0, count-1, 0);
	min.x = parray[0].x;
	max.x = parray[count-1].x;
	
	quick_sort(parray, 0, count-1, 1);
	min.y = parray[0].y;
	max.y = parray[count-1].y;
	
	quick_sort(parray, 0, count-1, 2);
	min.z = parray[0].z;
	max.z = parray[count-1].z;
	
	delete[] parray;
	
	XYZ center;
	center.x = (max.x + min.x)/2;
	center.y = (max.y + min.y)/2;
	center.z = (max.z + min.z)/2;
/*	
	float size = max.x - min.x;
	float tmp = max.y - min.y;
	if(tmp>size) size = tmp;
	tmp = max.z - min.z;
	if(tmp>size) size = tmp;
	
	min.x = center.x - size/2;
	min.y = center.y - size/2;
	min.z = center.z - size/2;
	max.x = center.x + size/2;
	max.y = center.y + size/2;
	max.z = center.z + size/2;
	*/
}

void rpcFile::findSample(const XYZ& P, int lo, int hi, short axis, float max_dist, float& absorba, const XYZ& vec_key, XYZ& cola, int& weighta)
{
	float pdotv, decay;
	int mid = (lo+hi)/2;
	XYZ h = m_samples[mid].pos;
	
	XYZ to_sample(P, h);
	float dist_to_sample = length(to_sample);
	if(dist_to_sample < max_dist) 
	{
		//if(dot(to_sample,m_samples[mid].nor) > 0 )
		{
			if(dot(to_sample, vec_key) > 0.1)
			{
				decay = exp(-length(to_sample)*absorba);
				cola += m_samples[mid].col*decay;
			
				weighta++;
			}
		}
	}
	
	if(hi==lo) return;
	
	if(mid==lo)
	{
		to_sample = XYZ(P, m_samples[mid+1].pos);
		dist_to_sample = length(to_sample);
		if(dist_to_sample < max_dist) 
		{
			//if(dot(to_sample, m_samples[mid+1].nor) > 0 )
			{
				if(dot(to_sample, vec_key) > 0.1)
				{
					decay = exp(-length(to_sample)*absorba);
					cola += m_samples[mid+1].col*decay;
				
					weighta++;
				}
				
			}
		}
		return;
	}
	
	short new_axis = (axis+1)%3;
		
	float diff;
	if(axis==0)
	{
		diff = P.x-h.x;
		
	}
	else if(axis==1)
	{
		diff = P.y-h.y;
	}
	else
	{
		diff = P.z-h.z;
	}
	

	
	if(diff<0)
	{
		findSample(P, lo, mid-1, new_axis, max_dist, absorba, vec_key, cola, weighta);
		if(diff > -max_dist) findSample(P, mid+1, hi, new_axis, max_dist, absorba, vec_key, cola, weighta);
	}
	else
	{
		findSample(P, mid+1, hi, new_axis, max_dist, absorba, vec_key, cola, weighta);
		if(diff <max_dist) findSample(P, lo, mid-1, new_axis, max_dist, absorba, vec_key, cola, weighta);
	}
}

void rpcFile::findSample(const XYZ& P, int lo, int hi, short axis, float max_dist)
{
	int mid = (lo+hi)/2;
	XYZ h = m_samples[mid].pos;
	
	XYZ to_sample(P, h);
	float dist_to_sample = distance_between(P, h);
	if(dist_to_sample < max_dist) 
	{
		glColor4f(m_samples[mid].col.x, m_samples[mid].col.y, m_samples[mid].col.z ,1);
		glVertex3f(h.x, h.y, h.z);
	}
	
	if(hi==lo) return;
	
	if(mid==lo)
	{
		to_sample = XYZ(P, m_samples[mid+1].pos);
		dist_to_sample = distance_between(P, m_samples[mid+1].pos);
		if(dist_to_sample < max_dist) 
		{
			glColor4f(m_samples[mid+1].col.x, m_samples[mid+1].col.y, m_samples[mid+1].col.z ,1);
			glVertex3f(m_samples[mid+1].pos.x, m_samples[mid+1].pos.y, m_samples[mid+1].pos.z);
		}
		return;
	}
	
	short new_axis = (axis+1)%3;
		
	float diff;
	if(axis==0)
	{
		diff = P.x-h.x;
		
	}
	else if(axis==1)
	{
		diff = P.y-h.y;
	}
	else
	{
		diff = P.z-h.z;
	}
	

	
	if(diff<0)
	{
		findSample(P, lo, mid-1, new_axis, max_dist);
		if(diff > -max_dist) findSample(P, mid+1, hi, new_axis, max_dist);
	}
	else
	{
		findSample(P, mid+1, hi, new_axis, max_dist);
		if(diff <max_dist) findSample(P, lo, mid-1, new_axis, max_dist);
	}
}

void rpcFile::findSample(const XYZ& P, int lo, int hi, short axis, float max_dist, const XYZ& V, float& absorb, float& val, int& hits)
{
	float pdotv, decay;
	int mid = (lo+hi)/2;
	XYZ h = m_samples[mid].pos;
	
	XYZ to_sample(P, h);
	float leng = length(to_sample);
	if(leng < max_dist) 
	{
		float d = dot(to_sample, V);
		if(d>0)
		{
			//if(dot(to_sample, N)>0.5)
			{
				if(dot(to_sample, m_samples[mid].nor) > 0.1 )
				{
					//sssSample t_sample;
					//t_sample.length = leng;
					float d = dot(to_sample, V);
					//t_sample.col.x = m_samples[mid].col.y*d;
					//t_sample.col = m_samples[mid].col;
					//xyz2sph(to_sample, leng, t_sample.sph.y, t_sample.sph.x);
					//hits.push_back(t_sample);
					val += m_samples[mid].col.y*d*exp(-leng*absorb);
					hits++;
				}
			}
		}		
	}
	
	if(hi==lo) return;
	
	if(mid==lo)
	{
		to_sample = XYZ(P, m_samples[mid+1].pos);
		leng = length(to_sample);
		if(leng < max_dist) 
		{
			float d = dot(to_sample, V);
			if(d>0)
			{
				//if(dot(to_sample, N)>0.5)
				{
					if(dot(to_sample, m_samples[mid+1].nor) > 0.1 )
					{
						//sssSample t_sample;
						//t_sample.length = leng;
						
						//t_sample.col.x = m_samples[mid+1].col.y*d;
						//t_sample.col = m_samples[mid+1].col;
						//xyz2sph(to_sample, leng, t_sample.sph.y, t_sample.sph.x);
						//hits.push_back(t_sample);
						val += m_samples[mid+1].col.y*d*exp(-leng*absorb);
						hits++;
					}
				}
			}
		}
		return;
	}
	
	short new_axis = (axis+1)%3;
		
	float diff;
	if(axis==0)
	{
		diff = P.x-h.x;
		
	}
	else if(axis==1)
	{
		diff = P.y-h.y;
	}
	else
	{
		diff = P.z-h.z;
	}
	

	
	if(diff<0)
	{
		findSample(P, lo, mid-1, new_axis, max_dist, V, absorb, val, hits);
		if(diff > -max_dist) findSample(P, mid+1, hi, new_axis, max_dist, V, absorb, val, hits);
	}
	else
	{
		findSample(P, mid+1, hi, new_axis, max_dist, V, absorb, val, hits);
		if(diff <max_dist) findSample(P, lo, mid-1, new_axis, max_dist, V, absorb, val, hits);
	}
}

void rpcFile::findSample1(const XYZ& P, int lo, int hi, short axis, float max_dist, const XYZ& V, const XYZ& N, float& absorb, float& val, int& hits)
{
	float pdotv, decay;
	int mid = (lo+hi)/2;
	XYZ h = m_samples[mid].pos;
	
	XYZ to_sample(P, h);
	float leng = length(to_sample);
	if(leng < max_dist) 
	{
		if(dot(to_sample, V)>0.1)
		{
			if(dot(m_samples[mid].nor, N)>0.1)
			{
				if(dot(to_sample, m_samples[mid].nor) > 0 )
				{
					//sssSample t_sample;
					//t_sample.length = leng;
					//float d = dot(to_sample, V);
					//t_sample.col.x = m_samples[mid].col.x;
					//t_sample.col = m_samples[mid].col;
					//xyz2sph(to_sample, leng, t_sample.sph.y, t_sample.sph.x);
					//hits.push_back(t_sample);
					val += m_samples[mid].col.x*exp(-leng*absorb);
					hits++;
				}
			}
		}		
	}
	
	if(hi==lo) return;
	
	if(mid==lo)
	{
		to_sample = XYZ(P, m_samples[mid+1].pos);
		leng = length(to_sample);
		if(leng < max_dist) 
		{
			if(dot(to_sample, V)>0.1)
			{
				if(dot(m_samples[mid+1].nor, N)>0.1)
				{
					if(dot(to_sample, m_samples[mid+1].nor) > 0 )
					{
						//sssSample t_sample;
						//t_sample.length = leng;
						//float d = dot(to_sample, V);
						//t_sample.col.x = m_samples[mid+1].col.x;
						//t_sample.col = m_samples[mid+1].col;
						//xyz2sph(to_sample, leng, t_sample.sph.y, t_sample.sph.x);
						//hits.push_back(t_sample);
						val += m_samples[mid+1].col.x*exp(-leng*absorb);
						hits++;
					}
				}
			}
		}
		return;
	}
	
	short new_axis = (axis+1)%3;
		
	float diff;
	if(axis==0)
	{
		diff = P.x-h.x;
		
	}
	else if(axis==1)
	{
		diff = P.y-h.y;
	}
	else
	{
		diff = P.z-h.z;
	}
	

	
	if(diff<0)
	{
		findSample1(P, lo, mid-1, new_axis, max_dist, V, N, absorb, val, hits);
		if(diff > -max_dist) findSample1(P, mid+1, hi, new_axis, max_dist, V, N, absorb, val, hits);
	}
	else
	{
		findSample1(P, mid+1, hi, new_axis, max_dist, V, N, absorb, val, hits);
		if(diff <max_dist) findSample1(P, lo, mid-1, new_axis, max_dist, V, N, absorb, val, hits);
	}
}

void rpcFile::getMinMax(XYZ& min, XYZ& max)
{
	min.x = m_center.x - m_size/2;
	min.y = m_center.y - m_size/2;
	min.z = m_center.z - m_size/2;
	max.x = m_center.x + m_size/2;
	max.y = m_center.y + m_size/2;
	max.z = m_center.z + m_size/2;
}

void rpcFile::accumulateDiffuse(const XYZ& p, float& radius, float& absorba, const XYZ& vec_key, XYZ& ca)
{
	ca.x = ca.y = ca.z = 0;
	int weighta = 0;
//printf("%f %f %f\n", v.x, v.y, v.z);
	findSample(p, 0, m_pDesc->num_samples-1, 0, radius, absorba, vec_key, ca, weighta);
	
//printf("%i  ", weighta);
	if(weighta>0) ca /= weighta;
}

void rpcFile::accumulateDiffuse(const XYZ& p, float& radius, const XYZ& V, float& absorb, float& val)
{
	val = 0;
	int hits = 0;
	findSample(p, 0, m_pDesc->num_samples-1, 0, radius, V, absorb, val, hits);
	if(hits>0) val/=(float)hits;
}

void rpcFile::accumulateDiffuse1(const XYZ& p, float& radius, const XYZ& V, const XYZ& N, float& absorb, float& val)
{
	val = 0;
	int hits = 0;
	findSample1(p, 0, m_pDesc->num_samples-1, 0, radius, V, N, absorb, val, hits);
	if(hits>0) val/=(float)hits;
}

void rpcFile::drawSamples(const XYZ& P, float radius)
{
	XYZ c;
	glBegin(GL_POINTS);
	findSample(P, 0, m_pDesc->num_samples-1, 0, radius);
	glEnd();
}

void rpcFile::getSamples(XYZ* rec)
{
	for(int i=0; i<m_pDesc->num_samples; i++) rec[i] = m_samples[i].pos;
}

void rpcFile::accumulateSamples(const XYZ& p, float& radius, std::vector<rpcSample>& samples)
{
	findSample(p, 0, m_pDesc->num_samples-1, 0, radius, samples);
}

void rpcFile::findSample(const XYZ& P, int lo, int hi, short axis, float max_dist, std::vector<rpcSample>& samples)
{
	int mid = (lo+hi)/2;
	XYZ h = m_samples[mid].pos;
	
	XYZ to_sample(P, h);
	float dist_to_sample = length(to_sample);
	if(dist_to_sample < max_dist) 
	{
		if(dot(to_sample, m_samples[mid+1].nor) > 0 )
		{
			samples.push_back(m_samples[mid]);
		}
	}
	
	if(hi==lo) return;
	
	if(mid==lo)
	{
		to_sample = XYZ(P, m_samples[mid+1].pos);
		dist_to_sample = length(to_sample);
		if(dist_to_sample < max_dist) 
		{
			if(dot(to_sample, m_samples[mid+1].nor) > 0 )
			{
				samples.push_back(m_samples[mid+1]);
			}
		}
		return;
	}
	
	short new_axis = (axis+1)%3;
		
	float diff;
	if(axis==0)
	{
		diff = P.x-h.x;
		
	}
	else if(axis==1)
	{
		diff = P.y-h.y;
	}
	else
	{
		diff = P.z-h.z;
	}
	

	
	if(diff<0)
	{
		findSample(P, lo, mid-1, new_axis, max_dist, samples);
		if(diff > -max_dist) findSample(P, mid+1, hi, new_axis, max_dist, samples);
	}
	else
	{
		findSample(P, mid+1, hi, new_axis, max_dist, samples);
		if(diff < max_dist) findSample(P, lo, mid-1, new_axis, max_dist, samples);
	}
}