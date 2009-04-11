#include ".\raycast.h"

rayCast::rayCast(void)
{
}

rayCast::~rayCast(void)
{
}

int rayCast::ray_box_intersection(XYZ origin, XYZ ray, float boxmin[3], float boxmax[3], float offset, float& dist)
{
	//char buf[64];
	//sprintf(buf,"xyz: %f\n", max_dist);
	
	float Ti=0, To=0, tmp;
	
	float Tnear = -10e38;
	float Tfar = 10e38;
	
	// for X plane
	if(ray.x==0) 
	{
		//printf("paranel");
		if(origin.x<boxmin[0]-offset || origin.x>boxmax[0]+offset) return FALSE;
	}
	else
	{
		Ti = (boxmin[0]-offset - origin.x)/ray.x;
		To = (boxmax[0]+offset - origin.x)/ray.x;
		
		
		
		if(Ti>To) 
		{
			tmp = Ti;
			Ti = To;
			To = tmp;
		}
		
		if(Ti>Tnear) Tnear = Ti;
		if(To<Tfar) Tfar = To;
		
		if(Tnear > Tfar || Tfar<0) return FALSE;
	}
	
	// for Y plane
	if(ray.y==0) 
	{
		if(origin.y<boxmin[1]-offset || origin.y>boxmax[1]+offset) return FALSE;
	}
	else
	{
		Ti = (boxmin[1]-offset - origin.y)/ray.y;
		To = (boxmax[1]+offset - origin.y)/ray.y;
		
		if(Ti>To) 
		{
			tmp = Ti;
			Ti = To;
			To = tmp;
		}
		
		if(Ti>Tnear) Tnear = Ti;
		if(To<Tfar) Tfar = To;
		
		if(Tnear > Tfar || Tfar<0) return FALSE;
	}
	
	// for Z plane
	if(ray.z==0) 
	{
		if(origin.z<boxmin[2]-offset || origin.z>boxmax[2]+offset) return FALSE;
	}
	else
	{
		Ti = (boxmin[2]-offset - origin.z)/ray.z;
		To = (boxmax[2]+offset - origin.z)/ray.z;
		
		if(Ti>To) 
		{
			tmp = Ti;
			Ti = To;
			To = tmp;
		}
		
		if(Ti>Tnear) Tnear = Ti;
		if(To<Tfar) Tfar = To;
		
		if(Tnear > Tfar || Tfar<0) return FALSE;
	}
	
	//sprintf(buf,"Ti: %f\n", Tfar);
	//printf(buf);
	dist = Tfar;
	
	return TRUE;
}
