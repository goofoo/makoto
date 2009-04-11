#include "particlemap.h"
#include "../shared/zMath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

particleMap::particleMap() : m_particles(0), m_pDesc(0), m_isEmpty(1)
{
}

particleMap::~particleMap(void)
{
	if(m_pDesc) delete m_pDesc;
	if(m_particles) delete[] m_particles;
}

void particleMap::create(const XYZ* pos, const float* kd, int size)
{
	m_pDesc = new particleMapDesc();
	m_pDesc->num_particles = size;
	m_particles = new ptc[size];
	for(int i=0; i<size; i++)
	{
		m_particles[i].position.x = pos[i].x;
		m_particles[i].position.y = pos[i].y;
		m_particles[i].position.z = pos[i].z;
		m_particles[i].kd = kd[i];
	}
	
	//alculate bounding box
	quickSort(m_particles, 0 , size-1, 0);
	m_pDesc->bbox_min[0] = m_particles[0].position.x; m_pDesc->bbox_max[0] = m_particles[size-1].position.x;
	
	quickSort(m_particles, 0 , size-1, 1);
	m_pDesc->bbox_min[1] = m_particles[0].position.y; m_pDesc->bbox_max[1] = m_particles[size-1].position.y;
	
	quickSort(m_particles, 0 , size-1, 2);
	m_pDesc->bbox_min[2] = m_particles[0].position.z; m_pDesc->bbox_max[2] = m_particles[size-1].position.z;
	
	// create kd_tree
	medianSplit(m_particles, 0 , size-1, 0);
}

void particleMap::getBBox(float bbmin[3], float bbmax[3])
{
	if(m_pDesc)
	{
		bbmin[0] = m_pDesc->bbox_min[0];
		bbmin[1] = m_pDesc->bbox_min[1];
		bbmin[2] = m_pDesc->bbox_min[2];
		bbmax[0] = m_pDesc->bbox_max[0];
		bbmax[1] = m_pDesc->bbox_max[1];
		bbmax[2] = m_pDesc->bbox_max[2];
	}
}

void particleMap::medianSplit(ptc* a, int lo, int hi, short axis)
{
	quickSort(m_particles, lo, hi, axis);
	int mid = (lo+hi)/2;
	if(lo<mid)
	{
		short new_axis = (axis+1)%3;
		medianSplit(m_particles, lo, mid-1, new_axis);
		medianSplit(m_particles, mid+1, hi, new_axis);
	}
}

void particleMap::quickSort(ptc* a, int lo, int hi, short axis)
{
//  lo is the lower index, hi is the upper index
//  of the region of array a that is to be sorted
    int i=lo, j=hi;
    ptc h;
    XYZ x=a[(lo+hi)/2].position;
	
	if(axis==0)
	{
	    //  partition
	    do
	    {    
	        while (a[i].position.x<x.x) i++; 
	        while (a[j].position.x>x.x) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else if(axis==1)
	{
		do
	    {    
	        while (a[i].position.y<x.y) i++; 
	        while (a[j].position.y>x.y) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}
	else
	{
		do
	    {    
	        while (a[i].position.z<x.z) i++; 
	        while (a[j].position.z>x.z) j--;
	        if (i<=j)
	        {
	            h=a[i]; a[i]=a[j]; a[j]=h;
	            i++; j--;
	        }
	    } while (i<=j);
	}

    //  recursion
    if (lo<j) quickSort(a, lo, j, axis);
    if (i<hi) quickSort(a, i, hi, axis);
}

float particleMap::densityEstimate(const XYZ pos, const float max_dist, float& light)
{
	float hit = 0.0f;
	int count = 0;
	light = 0.0f;
	findParticles(pos, max_dist, 0, m_pDesc->num_particles-1, 0, hit, light, count);
	if(count>0)light /= (float)count;
	return hit;
}

void particleMap::findParticles(const XYZ pos, const float max_dist, int lo, int hi, short axis, float& hit, float &light, int& count)
{
	float dist_to_sample, weight;
	int mid = (lo+hi)/2;
	XYZ h = m_particles[mid].position;
	
	dist_to_sample = distanceBetween(pos, h);
	if(dist_to_sample<max_dist) 
	{
		weight = exp(-2.5*dist_to_sample/max_dist);
		hit+= weight;
		light += 1-m_particles[mid].kd;
		count++;
	}
	
	if(hi==lo) return;
	else if(mid==lo)
	{
		dist_to_sample = distanceBetween(pos, m_particles[mid+1].position);
		if(dist_to_sample<max_dist) 
		{
			weight = exp(-2.5*dist_to_sample/max_dist);
			hit+=weight;
			light += 1-m_particles[mid+1].kd;
			count++;
		}
		return;
	}
	
	short new_axis = (axis+1)%3;
		
	float diff;
	if(axis==0)
	{
		diff = pos.x-h.x;
		
	}
	else if(axis==1)
	{
		diff = pos.y-h.y;
	}
	else
	{
		diff = pos.z-h.z;
	}
	

	
	if(diff<0)
	{
		findParticles(pos, max_dist, lo, mid-1, new_axis, hit, light, count);
		if(diff > -max_dist)
			findParticles(pos, max_dist, mid+1, hi, new_axis, hit, light, count);
	}
	
	

	if(diff>0)
	{
		findParticles(pos, max_dist, mid+1, hi, new_axis, hit, light, count);
		if(diff < max_dist)
			findParticles(pos, max_dist, lo, mid-1, new_axis, hit, light, count);
	}
		
		
	
}

int particleMap::save(const char* filename)
{
	FILE *fp = fopen( filename, "wb");

	if( fp==NULL ) return 0;
	
	fwrite(m_pDesc,sizeof(struct particleMapDesc),1,fp);
	fwrite(m_particles,m_pDesc->num_particles*sizeof(ptc),1,fp);
	
	fclose(fp);
	
	return 1;
}

int particleMap::load(const char* filename)
{
	FILE *fp = fopen( filename, "rb");

	if( fp==NULL ) return 0;
	int stat;
	
	m_pDesc = new particleMapDesc();
	
	stat = fread(m_pDesc,sizeof(struct particleMapDesc),1,fp);
	
	if(stat!=1) return 0;
	
	m_particles = new ptc[m_pDesc->num_particles];
	
	stat = fread(m_particles,m_pDesc->num_particles*sizeof(ptc),1,fp);
	
	if(stat!=1) return 0;
	
	fclose(fp);
	
	printf("loaded\n");
	m_isEmpty = 0;
	
	return 1;
}
