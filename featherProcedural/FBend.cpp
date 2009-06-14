/*
 *  FBend.cpp
 *  hairBase
 *
 *  Created by zhang on 08-12-29.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "FBend.h"

void FBend::create(float s, float x, float y)
{
	float ds = s/NUMBENDSEG;
	float ang_zy = atan(y/s)*2;
	float ang_zx = atan(x/s)*2;
	max_ang = ang_zy;
	
	mp[0] = XYZ(0,0,0);
	mn[0] = XYZ(0,1,0);
	for(int i=1; i<=NUMBENDSEG; i++)
	{
		float vv = (float)i/NUMBENDSEG;
		vv *= vv;
		float rotzy = ang_zy*vv;
		XYZ dv(0,ds*sin(rotzy),ds*cos(rotzy));

		float rotzx = ang_zx*vv;
		dv.x = dv.z*sin(rotzx);
		dv.z = dv.z*cos(rotzx);
		
		mp[i] = mp[i-1] + dv;
		mn[i] = XYZ(0, sin(rotzy+1.57), cos(rotzy+1.57));
	}
}

void FBend::getPoint(int idx, XYZ& p) const
{
	p = mp[idx];
}

void FBend::getNormal(int idx, XYZ& n) const
{
	n = mn[idx];
}

float FBend::getAngle(float param) const
{
	return max_ang*param;
}

void FBend::add(XYZ& p)
{
	static int num=0;
	float radius = 2.f/(wind_y+0.0001) * (1.f - 0.025*num);
	if(radius<0.4) radius = 0.4;
	
	float alpha = p.z/radius;
	p.z = radius * sin(alpha);
	p.y = radius - radius * cos(alpha);
	
	radius = p.z;
	alpha = wind_x/7.f*((float)num/(float)n_seg);
	p.x = radius * sin(alpha);
	p.z = radius * cos(alpha);

	num++;
	if(num > n_seg) num=0;
}

void FBend::add(XYZ& p, XYZ& nor)
{
	static int num=0;
	float radius = 2.f/(wind_y+0.0001) * (1.f - 0.025*num);
	if(radius<0.4) radius = 0.4;
	
	float alpha = p.z/radius;
	p.z = radius * sin(alpha);
	p.y = radius - radius * cos(alpha);
	
	nor.z = -p.z;
	nor.y = radius - p.y;
	nor.x = 0;
	nor.normalize();
	
	radius = p.z;
	alpha = wind_x/7.f*((float)num/(float)n_seg);
	p.x = radius * sin(alpha);
	p.z = radius * cos(alpha);

	num++;
	if(num > n_seg) num=0;

}
