#include "ftriangle.h"
#include <iostream>

FTriangle::FTriangle()
{}
	
FTriangle::~FTriangle(void)
{}
	
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
	/*
	if(grid_x ==1 || grid_y == 1)
	{
		alpha = beta = gamma = 1.0f/3.0f;
		p_interp = P[0]*alpha + P[1]*beta + P[2]*gamma;
		n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
		c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			//t_interp = tangent[0]*alpha + tangent[1]*beta + tangent[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = delta*delta*2.f;
			
			count++;
		return;
	}
	*/
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
			n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
			c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			//t_interp = tangent[0]*alpha + tangent[1]*beta + tangent[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = delta/1.414;
			
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
	/*
	if(grid_x ==1 || grid_y == 1)
	{
		alpha = beta = gamma = 1.0f/3.0f;
		p_interp = P[0]*alpha + P[1]*beta + P[2]*gamma;
		n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
		c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = delta*delta*2.f;
		
		for(int j=0; j<16; j++)
		{
			coe_res[count].data[j] = vertex_coe[id[0]].data[j] * alpha + vertex_coe[id[1]].data[j] * beta + vertex_coe[id[2]].data[j] * gamma;
		}
			
			count++;
		return;
	}
	*/
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
			n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
			c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = delta/1.414;
			
			for(int k=0; k<16; k++)
			{
				coe_res[count].data[k] = vertex_coe[id[0]].data[k] * alpha + vertex_coe[id[1]].data[k] * beta + vertex_coe[id[2]].data[k] * gamma;
			}
			
			count++;
		}
	}
}
/*
void FTriangle::rasterize(const float delta, const CoeRec* vertex_coe, pcdSample* res, CoeRec* coe_res, int& count, XYZ* hdr)
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
	
	if(grid_x ==1 || grid_y == 1)
	{
		alpha = beta = gamma = 1.0f/3.0f;
		p_interp = P[0]*alpha + P[1]*beta + P[2]*gamma;
		n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
		c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = delta;
		
		for(int j=0; j<16; j++)
		{
			coe_res[count].data[j] = vertex_coe[id[0]].data[j] * alpha + vertex_coe[id[1]].data[j] * beta + vertex_coe[id[2]].data[j] * gamma;
		}
		
		XYZ col0(0.f);
		for(int j=0; j<16; j++)
		{
			col0.x += coe_res[count].data[j].x * hdr[j].x;
			col0.y += coe_res[count].data[j].y * hdr[j].y;
			col0.z += coe_res[count].data[j].z * hdr[j].z;
		}
		
		glColor3f(col0.x, col0.y, col0.z);
		glVertex3f(res[count].pos.x, res[count].pos.y, res[count].pos.z);
			
			count++;
		return;
	}
	
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
			n_interp = normal[0]*alpha + normal[1]*beta + normal[2]*gamma;n_interp.normalize();
			c_interp = color[0]*alpha + color[1]*beta + color[2]*gamma;
			
			res[count].pos = p_interp;
			res[count].nor = n_interp;
			res[count].col = c_interp;
			res[count].area = delta/2;
			
			for(int k=0; k<16; k++)
			{
				coe_res[count].data[k] = vertex_coe[id[0]].data[k] * alpha + vertex_coe[id[1]].data[k] * beta + vertex_coe[id[2]].data[k] * gamma;
			}
			
			XYZ col0(0.f);
			for(int k=0; k<16; k++)
			{
				col0.x += coe_res[count].data[k].x * hdr[k].x;
				col0.y += coe_res[count].data[k].y * hdr[k].y;
				col0.z += coe_res[count].data[k].z * hdr[k].z;
			}
			
			glColor3f(col0.x, col0.y, col0.z);
			glVertex3f(res[count].pos.x, res[count].pos.y, res[count].pos.z);
			
			count++;
		}
	}
}
*/

//:~