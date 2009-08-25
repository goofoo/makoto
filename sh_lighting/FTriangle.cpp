#include "ftriangle.h"
#include <iostream>

FTriangle::FTriangle()
{}
	
FTriangle::~FTriangle(void)
{}

void FTriangle::createSimple(const XYZ& p0, const XYZ& p1, const XYZ& p2)
{
	P[0] = p0;
	P[1] = p1;
	P[2] = p2;
}

void FTriangle::project(double& fov)
{
	double tanhfov = tan(fov*0.5);
	
	double a = P[0].z*tanhfov;
	P[0].x /= a;
	P[0].y /= a;
	
	a = P[1].z*tanhfov;
	P[1].x /= a;
	P[1].y /= a;
	
	a = P[2].z*tanhfov;
	P[2].x /= a;
	P[2].y /= a;
	
	p_obj[0] = P[0];
	p_obj[1] = P[1];
	p_obj[2] = P[2];
	
	f120 = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y);
	f201 = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y);
	f012 = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y);
}

void FTriangle::onScreen(float* data, int map_w, int map_h)
{
	float x_max = -1, y_max = -1, x_min = 1, y_min = 1;
	for(int i=0; i<3; i++) {
		if(p_obj[i].x > x_max) x_max = p_obj[i].x;
		if(p_obj[i].y > y_max) y_max = p_obj[i].y;
		if(p_obj[i].x < x_min) x_min = p_obj[i].x;
		if(p_obj[i].y < y_min) y_min = p_obj[i].y;
	}
	
	if(x_max > 1) x_max = 1;
	if(y_max > 1) y_max = 1;
	if(x_min < -1) x_min = -1;
	if(y_min < -1) y_min = -1;
	
	if(x_min > x_max || y_min > y_max) return;
	
	float delta = 1.f/map_w;
	
	int grid_x = (x_max - x_min)/delta + 1;
	int grid_y = (y_max - y_min)/delta + 1;
	
	float alpha, beta, gamma, x, y, z_interp;
	int loc_x, loc_y;
	
	for(int j=0; j<grid_y; j++) {
		for(int i=0; i<grid_x; i++) {
			x = x_min + delta*(i+0.5);
			y = y_min + delta*(j+0.5);
			
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			z_interp = P[0].z*alpha + P[1].z*beta + P[2].z*gamma;
			
			if(z_interp > 0.1) {
			
				loc_x = (x + 1.0)/2.0*(map_w-1);
				loc_y = (y + 1.0)/2.0*(map_h-1);
			
				if(z_interp < data[loc_y*map_w + loc_x] ) data[loc_y*map_w + loc_x] = z_interp;
			}
		}
	}
}
	
float FTriangle::calculate_area(const XYZ& p0, const XYZ& p1, const XYZ& p2)
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

float FTriangle::calculate_longest_edge(const XYZ& p0, const XYZ& p1, const XYZ& p2)
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

void FTriangle::create(const XYZ& p0, const XYZ& p1, const XYZ& p2)
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
	
	int a, b;
	
	if(edge_length[0] > edge_length[1] && edge_length[0] > edge_length[2])
	{
		a = 0;
		b = 1;
		
		if(edge_length[1] > edge_length[2]) m_min_length = edge_length[2];
		else m_min_length = edge_length[1];
	}
	else if(edge_length[1] > edge_length[2] && edge_length[1] > edge_length[0])
	{
		a = 1; 
		b = 2;
		
		if(edge_length[2] > edge_length[0]) m_min_length = edge_length[0];
		else m_min_length = edge_length[2];
	}
	else
	{
		a = 2;
		b = 0;
		
		if(edge_length[1] > edge_length[0]) m_min_length = edge_length[0];
		else m_min_length = edge_length[1];
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

void FTriangle::setNormal(const XYZ& n0, const XYZ& n1, const XYZ& n2)
{
	normal[0] = n0;
	normal[1] = n1;
	normal[2] = n2;
}

void FTriangle::setColor(const XYZ& c0, const XYZ& c1, const XYZ& c2)
{
	color[0] = c0;
	color[1] = c1;
	color[2] = c2;
}

void FTriangle::setTangent(const XYZ& t0, const XYZ& t1, const XYZ& t2)
{
	tangent[0] = t0;
	tangent[1] = t1;
	tangent[2] = t2;
}

void FTriangle::setId(const int a, const int b, const int c)
{
	id[0] = a;
	id[1] = b;
	id[2] = c;
}

float FTriangle::barycentric_coord(float ax, float ay, float bx, float by, float x, float y)
{
	return (ay - by)*x + (bx - ax)*y +ax*by - bx*ay;
}
#include "../shared/gBase.h"
void FTriangle::rasterize(const float delta)
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
	XYZ p_interp;
	
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
			
			p_interp = P[0]*alpha + P[1]*beta + P[2]*gamma;
			glVertex3f(p_interp.x, p_interp.y, p_interp.z);

		}
	}
}

void FTriangle::rasterize(const float delta, pcdSample* res, int& count)
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
	XYZ p_interp, n_interp, c_interp, t_interp;

	float real_delta = delta;
	
	if(real_delta > m_min_length) real_delta = m_min_length;
	if(real_delta < delta/4) real_delta = delta/4;
	
	for(int j=0; j<grid_y; j++)
	{
		for(int i=0; i<grid_x; i++)
		{
			x = real_delta*(i+0.5);
			y = real_delta*(j+0.5);
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			p_interp = P[0]*alpha + P[1]*beta + P[2]*gamma;
			n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
			c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;

			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = real_delta/1.414;
			
			count++;
		}
	}
}

void FTriangle::rasterize(const float delta, const CoeRec* vertex_coe, pcdSample* res, CoeRec* coe_res, int& count)
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
	XYZ p_interp, n_interp, c_interp, t_interp;
	
	float real_delta = delta;
	
	if(real_delta > m_min_length) real_delta = m_min_length;
	if(real_delta < delta/4) real_delta = delta/4;

	for(int j=0; j<grid_y; j++)
	{
		for(int i=0; i<grid_x; i++)
		{
			x = real_delta*(i+0.5);
			y = real_delta*(j+0.5);
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			p_interp = P[0]*alpha + P[1]*beta + P[2]*gamma;
			n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
			c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = real_delta/1.414;
			
			for(int k=0; k<16; k++)
			{
				coe_res[count].data[k] = vertex_coe[id[0]].data[k] * alpha + vertex_coe[id[1]].data[k] * beta + vertex_coe[id[2]].data[k] * gamma;
			}
			
			count++;
		}
	}
}

//:~