#include "DiceTriangle.h"
#include <iostream>

DiceTriangle::DiceTriangle()
{}
	
DiceTriangle::~DiceTriangle()
{}
	
float DiceTriangle::calculate_area(const XYZ& p0, const XYZ& p1, const XYZ& p2)
{
	XYZ vec[3]; float length[3];

	vec[0] = p1 - p0;
	length[0] = vec[0].length();
	
	vec[1] = p2 - p1;
	length[1] = vec[1].length();
	
	vec[2] = p0 - p2;
	length[2] = vec[2].length();
	
	return (0.5*sqrt(length[0]*length[0]*length[2]*length[2] - (vec[0].dot(vec[2]))*(vec[0].dot(vec[2]))));
}

float DiceTriangle::calculate_longest_edge(const XYZ& p0, const XYZ& p1, const XYZ& p2)
{
	XYZ vec[3]; float length[3];

	vec[0] = p1 - p0;
	length[0] = vec[0].length();
	
	vec[1] = p2 - p1;
	length[1] = vec[1].length();
	
	vec[2] = p0 - p2;
	length[2] = vec[2].length();
	
	if(length[0]>length[1] && length[0]>length[2]) return length[0];
	else if(length[1]>length[0] && length[1]>length[2]) return length[1];
		else return length[2];
}

void DiceTriangle::create(const XYZ& p0, const XYZ& p1, const XYZ& p2)
{
	p_obj[0] = P[0] = p0;
	p_obj[1] = P[1] = p1;
	p_obj[2] = P[2] = p2;
	
	V[0] = p1 - p0;
	edge_length[0] = V[0].length();
	
	V[1] = p2 - p1;
	edge_length[1] = V[1].length();
	
	V[2] = p0 - p2;
	edge_length[2] = V[2].length();
	
	m_area = 0.5*sqrt(edge_length[0]*edge_length[0]*edge_length[2]*edge_length[2] - (V[0].dot(V[2]))*(V[0].dot(V[2])));
	
	V[0].normalize();
	V[1].normalize();
	V[2].normalize();
	
	int a , b;
	
	if(edge_length[0] > edge_length[1] && edge_length[0] > edge_length[2])
	{
		a = 0;
		b = 1;
	}
	else if(edge_length[1] > edge_length[2] && edge_length[1] > edge_length[0])
	{
		a = 1;
		b = 2;
	}
	else
	{
		a = 2;
		b = 0;
	}
	
	XYZ side = V[a];
	XYZ front = side^V[b]; front.normalize();
	XYZ up = front^side;
	
	m_space.setIdentity();
	m_space.setOrientations(side, up, front);
	m_space.setTranslation(P[a]);
	m_space.inverse();
	
	m_space.transform(p_obj[0]);
	m_space.transform(p_obj[1]);
	m_space.transform(p_obj[2]);
	
	f120 = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y);
	f201 = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y);
	f012 = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y);
}

void DiceTriangle::setId(const int a, const int b, const int c)
{
	id[0] = a;
	id[1] = b;
	id[2] = c;
}

float DiceTriangle::barycentric_coord(float ax, float ay, float bx, float by, float x, float y)
{
	return (ay - by)*x + (bx - ax)*y +ax*by - bx*ay;
}

void DiceTriangle::rasterize(const float delta, DiceParam* res, int& count)
{
	float x_max = 0;
	float y_max = 0;
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x > x_max) x_max = p_obj[i].x;
		if(p_obj[i].y > y_max) y_max = p_obj[i].y;
	}
	
	int grid_x = x_max/delta + 1;
	int grid_y = y_max/delta + 1;
	
	float alpha, beta, gamma, x, y;

	for(int j=0; j<grid_y; j++)
	{
		for(int i=0; i<grid_x; i++)
		{
			x = delta*(i+0.5);
			y = delta*(j+0.5);
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			res[count].alpha = alpha;
			res[count].beta = beta;
			res[count].gamma = gamma;
			res[count].id0 = id[0];
			res[count].id1 = id[1];
			res[count].id2 = id[2];
			
			count++;
		}
	}
}

#include "../shared/zNoise.h"

void DiceTriangle::rasterize(const float delta, DiceParam* res, int& count, const int seed)
{
	float x_max = 0;
	float y_max = 0;
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x > x_max) x_max = p_obj[i].x;
		if(p_obj[i].y > y_max) y_max = p_obj[i].y;
	}
	
	int grid_x = x_max/delta + 1;
	int grid_y = y_max/delta + 1;
	
	float alpha, beta, gamma, x, y;
	
	int g_seed = 13 + seed;

	for(int j=0; j<grid_y; j++)
	{
		for(int i=0; i<grid_x; i++)
		{
			x = delta*(i+0.5+(randfint( g_seed )-0.5)*0.5);
			g_seed++;
			y = delta*(j+0.5+(randfint( g_seed )-0.5)*0.5);
			g_seed++;
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			res[count].alpha = alpha;
			res[count].beta = beta;
			res[count].gamma = gamma;
			res[count].id0 = id[0];
			res[count].id1 = id[1];
			res[count].id2 = id[2];
			
			count++;
		}
	}
}

void DiceTriangle::create2D(const float* st0, const float* st1, const float* st2)
{
	p_obj[0] = XYZ(st0[0], st0[1], 0);
	p_obj[1] = XYZ(st1[0], st1[1], 0);
	p_obj[2] = XYZ(st2[0], st2[1], 0);
	
	f120 = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y);
	f201 = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y);
	f012 = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y);
}

int DiceTriangle::getGrid2D()
{
	float dy = p_obj[1].y - p_obj[0].y;
	if(dy<0) dy = -dy;
	
	float dya = p_obj[2].y - p_obj[1].y;
	if(dya<0) dya = -dya;
	
	if(dya > dy) dy = dya;
	
	dya = p_obj[0].y - p_obj[2].y;
	if(dya<0) dya = -dya;
	
	if(dya > dy) dy = dya;
	
	float dx = p_obj[1].x - p_obj[0].x;
	if(dx<0) dx = -dx;
	
	float dxa = p_obj[2].x - p_obj[1].x;
	if(dxa<0) dxa = -dxa;
	
	if(dxa > dx) dx = dxa;
	
	dxa = p_obj[0].x - p_obj[2].x;
	if(dxa<0) dxa = -dxa;
	
	if(dxa > dx) dx = dxa;
	
	return (1+dx*1024)*(1+dy*1024);
}

int DiceTriangle::rasterize2D(Dice2DParam* res, const int seed)
{
	float s_max = 0;
	float t_max = 0;
	float s_min = 1;
	float t_min = 1;
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x > s_max) s_max = p_obj[i].x;
		if(p_obj[i].y > t_max) t_max = p_obj[i].y;
		if(p_obj[i].x < s_min) s_min = p_obj[i].x;
		if(p_obj[i].y < t_min) t_min = p_obj[i].y;
	}
	
	float delta = 1.f/1024;
	
	s_min = int(s_min*1024)*delta;
	t_min = int(t_min*1024)*delta;
	
	int grid_x = (s_max-s_min)/delta + 1;
	int grid_y = (t_max-t_min)/delta + 1;
	
	float alpha, beta, gamma, x, y;
	
	int g_seed = 13 + seed;
	
	int count = 0;
	for(int j=0; j<=grid_y; j++)
	{
		for(int i=0; i<=grid_x; i++)
		{
			x = s_min + delta*(i+0.5);
			g_seed++;
			y = t_min + delta*(j+0.5);
			g_seed++;
			
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			res[count].alpha = alpha;
			res[count].beta = beta;
			res[count].gamma = gamma;
			res[count].s = s_min / delta + i;
			res[count].t = t_min / delta + j;
			
			count++;
		}
	}
	return count;
}
//:~