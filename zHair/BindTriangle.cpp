/*
 *  BindTriangle.cpp
 *  hair
 *
 *  Created by jian zhang on 6/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "BindTriangle.h"

char BindTriangle::set(const XY* corner, const XY& at, triangle_bind_info& res)
{
	float f120 = barycentric_coord(corner[1].x, corner[1].y, corner[2].x, corner[2].y, corner[0].x, corner[0].y);
	float f201 = barycentric_coord(corner[2].x, corner[2].y, corner[0].x, corner[0].y, corner[1].x, corner[1].y);
	float f012 = barycentric_coord(corner[0].x, corner[0].y, corner[1].x, corner[1].y, corner[2].x, corner[2].y);
	res.wei[0] = 1.0f;
	res.wei[1] = res.wei[2] = 0.0f;

	float alpha, beta, gamma;
	
	alpha = barycentric_coord(corner[1].x,corner[1].y, corner[2].x, corner[2].y, at.x, at.y)/f120;
	if(alpha<0 || alpha>1) return 0;
	beta = barycentric_coord(corner[2].x, corner[2].y, corner[0].x, corner[0].y, at.x, at.y)/f201;
	if(beta<0 || beta>1) return 0;
	gamma = barycentric_coord(corner[0].x, corner[0].y, corner[1].x, corner[1].y, at.x, at.y)/f012;
	if(gamma<0 || gamma>1) return 0;
			
	res.wei[0] = alpha;
	res.wei[1] = beta;
	res.wei[2] = gamma;
	
	return 1;
}

char BindTriangle::set(const XY* corner, const XY& at, const XYZ* positions, const XYZ& topos, const float* distance, triangle_bind_info& res)
{
	//res.wei[0] = 1.0f;
	//res.wei[1] = res.wei[2] = 0.0f;
	
	XYZ dto = topos - positions[0];
	if(dto.length() > distance[0]*2) return 0;
	
	dto = topos - positions[1];
	if(dto.length() > distance[1]*2) return 0;
	
	dto = topos - positions[2];
	if(dto.length() > distance[2]*2) return 0;
	
	float f120 = barycentric_coord(corner[1].x, corner[1].y, corner[2].x, corner[2].y, corner[0].x, corner[0].y);
	float f201 = barycentric_coord(corner[2].x, corner[2].y, corner[0].x, corner[0].y, corner[1].x, corner[1].y);
	float f012 = barycentric_coord(corner[0].x, corner[0].y, corner[1].x, corner[1].y, corner[2].x, corner[2].y);
	
	float alpha, beta, gamma;
	
	alpha = barycentric_coord(corner[1].x,corner[1].y, corner[2].x, corner[2].y, at.x, at.y)/f120;
	if(alpha<0 || alpha>1) return 0;
	beta = barycentric_coord(corner[2].x, corner[2].y, corner[0].x, corner[0].y, at.x, at.y)/f201;
	if(beta<0 || beta>1) return 0;
	gamma = barycentric_coord(corner[0].x, corner[0].y, corner[1].x, corner[1].y, at.x, at.y)/f012;
	if(gamma<0 || gamma>1) return 0;
			
	res.wei[0] = alpha;
	res.wei[1] = beta;
	res.wei[2] = gamma;
	
	return 1;
}

float BindTriangle::barycentric_coord(float ax, float ay, float bx, float by, float x, float y)
{
	return (ay - by)*x + (bx - ax)*y +ax*by - bx*ay;
}


