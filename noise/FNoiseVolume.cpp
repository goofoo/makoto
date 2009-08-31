/*
 *  FNoiseVolume.cpp
 *  bacteria
 *
 *  Created by jian zhang on 5/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "../shared/FNoise.h"
#include "FNoiseVolume.h"
#include <string>
using namespace std;

FNoiseVolume::FNoiseVolume():data(0), num_bac(0) 
{
	num_bac = 4;
	data = new ANoiseVolume[4];
}
FNoiseVolume::~FNoiseVolume() 
{
	if(data) delete[] data;
}

void FNoiseVolume::setGlobal(float size)
{
	global_size = size;
}

void FNoiseVolume::setSpace(MATRIX44F space)
{
	eyespaceinv = eyespace = space;
	eyespaceinv.inverse();
}

void FNoiseVolume::updateData(unsigned num_elm, const XYZ* parray, const XYZ* varray, const float* aarray)
{
	if(data && num_elm != num_bac)
	{
		delete[] data;
		num_bac = num_elm;
		data = new ANoiseVolume[num_bac];
	}
	FNoise fnoi;
	for(unsigned i=0; i<num_bac; i++)
	{
		data[i].pos = parray[i];
		data[i].vel = varray[i];
		data[i].age = aarray[i];
		int seed = 19 + i;
		data[i].noi = fnoi.randfint(seed);
		seed += ((seed+13)*3)%191;
		data[i].noi2 = fnoi.randfint(seed);
		seed += ((seed+17)*19)%131;
		data[i].noi3 = fnoi.randfint(seed);
	}
}


void FNoiseVolume::updateCamera(float nc, float fc, float ha, float va, float fl)
{
	nearClip = nc;
	farClip = fc;
	horizontal_apeture = ha;
	vertical_apeture = va;
	focal_length = fl;
	h_fov = horizontal_apeture * 0.5*25.4 /  focal_length ;
	v_fov = vertical_apeture * 0.5*25.4 /  focal_length ;
}

void FNoiseVolume::getSideAndUpById(unsigned id, XYZ& side, XYZ& up) const
{
	side = XYZ(1,0,0);
	up = XYZ(0,1,0);
	
	XYZ zz(0,0,1);
	
	side.rotateXY(data[id].noi*6.28);
	up = zz.cross(side);	
		eyespace.transformAsNormal(side);
		eyespace.transformAsNormal(up);
	
		side *= global_size;
		up *= global_size;
}

char FNoiseVolume::isInViewFrustum(unsigned id) const
{
	XYZ pp = data[id].pos;
	eyespaceinv.transform(pp);
	pp.z *= -1;
	if(pp.z - global_size < nearClip) return 0;
	if(pp.z - global_size > farClip) return 0;
	
	float right = pp.z* h_fov;

	if(pp.x - global_size> right || pp.x + global_size< -right) return 0;
	
	float top = pp.z* v_fov;
	
	if(pp.y - global_size> top || pp.y + global_size< -top) return 0;
	return 1;
}