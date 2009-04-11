#include "fdcFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <maya/MGlobal.h>

fdcFile::fdcFile(void){}

fdcFile::~fdcFile(void){}

int fdcFile::save(const char* filename, 
			const MVectorArray &points,
			const MVectorArray &velocities,
			const MDoubleArray &ages)
{
	int num_ptc = points.length();
	
	if(num_ptc==0) return 0;
	
	FILE *fp = fopen( filename, "wb");

	if( fp==NULL ) return 0;
	
	
	fwrite(&num_ptc, sizeof(int),1,fp);
	
	float* ppos = new float[num_ptc*3];
	
	for(int i=0; i<num_ptc; i++)
	{
		ppos[i*3]=points[i].x;
		ppos[i*3+1]=points[i].y;
		ppos[i*3+2]=points[i].z;
	}
	
	fwrite(ppos, num_ptc*3*sizeof(float),1,fp);
	
	float* pvel = new float[num_ptc*3];
	
	for(int i=0; i<num_ptc; i++)
	{
		pvel[i*3]=velocities[i].x;
		pvel[i*3+1]=velocities[i].y;
		pvel[i*3+2]=velocities[i].z;
	}
	
	fwrite(pvel, num_ptc*3*sizeof(float),1,fp);
	
	float* page = new float[num_ptc];
	
	for(int i=0; i<num_ptc; i++)
	{
		page[i]=ages[i];
	}
	
	fwrite(page, num_ptc*sizeof(float),1,fp);
	
	fclose(fp);
	
	delete[] ppos;
	delete[] pvel;
	delete[] page;
	
	return 1;
}

int fdcFile::load(const char* filename, 
			MVectorArray &points,
			MVectorArray &velocities,
			MDoubleArray &ages)
{
	FILE *fp = fopen( filename, "rb");
	
	if( fp==NULL ) return 0;
	
	points.clear();
	velocities.clear();
	ages.clear();
	
	int num_ptc;
	fread(&num_ptc,sizeof(int),1,fp);
	
	float* ppos = new float[num_ptc*3];
	
	fread(ppos, num_ptc*3*sizeof(float),1,fp);
	
	for(int i=0; i<num_ptc; i++)
	{
		points.append(MVector(ppos[i*3], ppos[i*3+1], ppos[i*3+2]));
	}
	
	float* pvel = new float[num_ptc*3];
	
	fread(pvel, num_ptc*3*sizeof(float),1,fp);
	
	for(int i=0; i<num_ptc; i++)
	{
		velocities.append(MVector(pvel[i*3], pvel[i*3+1], pvel[i*3+2]));
	}
	
	float* page = new float[num_ptc];
	
	fread(page, num_ptc*sizeof(float),1,fp);
	
	for(int i=0; i<num_ptc; i++)
	{
		ages.append(page[i]);
	}
	
	fclose(fp);
	
	delete[] ppos;
	delete[] pvel;
	delete[] page;
	
	return 1;
}