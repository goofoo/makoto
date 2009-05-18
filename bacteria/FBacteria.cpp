/*
 *  FBacteria.cpp
 *  bacteria
 *
 *  Created by jian zhang on 5/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "../shared/FNoise.h"
#include "FBacteria.h"

FBacteria::FBacteria():data(0), num_bac(0) 
{
	num_bac = 4;
	data = new ABacteria[4];
}
FBacteria::~FBacteria() 
{
	if(data) delete[] data;
}

void FBacteria::setGlobal(float size, float rotate)
{
	global_size = size;
	global_rotate = rotate;
}

void FBacteria::setSpace(MATRIX44F space)
{
	eyespaceinv = eyespace = space;
	eyespaceinv.inverse();
}

void FBacteria::updateData(unsigned num_elm, const XYZ* parray, const XYZ* varray)
{
	if(data && num_elm != num_bac)
	{
		delete[] data;
		num_bac = num_elm;
		data = new ABacteria[num_bac];
	}
	FNoise fnoi;
	for(unsigned i=0; i<num_bac; i++)
	{
		data[i].pos = parray[i];
		data[i].vel = varray[i];
		int seed = 19 + i;
		data[i].noi = fnoi.randfint(seed);
		seed += ((seed+13)*3)%191;
		data[i].noi2 = fnoi.randfint(seed);
		seed += ((seed+17)*19)%131;
		data[i].noi3 = fnoi.randfint(seed);
	}
}

void FBacteria::updateCamera(float nc, float fc, float ha, float va, float fl)
{
	nearClip = nc;
	farClip = fc;
	horizontal_apeture = ha;
	vertical_apeture = va;
	focal_length = fl;
	h_fov = horizontal_apeture * 0.5 / ( focal_length * 0.03937 );
	v_fov = vertical_apeture * 0.5 / ( focal_length * 0.03937 );
}

void FBacteria::getSideAndUpById(unsigned id, XYZ& side, XYZ& up) const
{
	side = XYZ(1,0,0);
	up = XYZ(0,1,0);
	
	XYZ zz(0,0,1);
	
	side.rotateXY(global_rotate + data[id].noi*6.28);
	up = zz.cross(side);
	
		eyespace.transformAsNormal(side);
		eyespace.transformAsNormal(up);
	
		side *= global_size;
		up *= global_size;
}

char FBacteria::isInViewFrustum(unsigned id) const
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