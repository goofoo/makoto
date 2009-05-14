#include "FDice.h"
#include "iostream"

FDice::FDice()
{}
	
FDice::~FDice(void)
{}
	
float FDice::calculate_area(const XYZ& p0, const XYZ& p1, const XYZ& p2)
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

float FDice::calculate_longest_edge(const XYZ& p0, const XYZ& p1, const XYZ& p2)
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

void FDice::create(const XYZ& p0, const XYZ& p1, const XYZ& p2)
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
	
	area = 0.5*sqrt(edge_length[0]*edge_length[0]*edge_length[2]*edge_length[2] - (V[0].dot(V[2]))*(V[0].dot(V[2])));
	
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

void FDice::setData(const DHair& n0, const DHair& n1, const DHair& n2)
{
	data[0] = n0; data[1] = n1; data[2] = n2;
}

float FDice::barycentric_coord(float ax, float ay, float bx, float by, float x, float y)
{
	return (ay - by)*x + (bx - ax)*y +ax*by - bx*ay;
}

void FDice::rasterize(const float delta, DHair* res, int& count)
{
	float x_max = 0, y_max = 0;
	float x_min = 10e8, y_min = 10e8;
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x > x_max) x_max = p_obj[i].x;
		if(p_obj[i].y > y_max) y_max = p_obj[i].y;
		if(p_obj[i].x < x_min) x_min = p_obj[i].x;
		if(p_obj[i].y < y_min) y_min = p_obj[i].y;
	}
	
	float dens_x_0, dens_x_1, dens_y_0, dens_y_1;
	
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x == x_min) dens_x_0 = data[i].density;
		if(p_obj[i].x == x_max) dens_x_1 = data[i].density;
		if(p_obj[i].y == y_min) dens_y_0 = data[i].density;
		if(p_obj[i].x == y_max) dens_y_1 = data[i].density;
	}
	
	//int grid_x = x_max/delta + 1;
	//int grid_y = y_max/delta + 1;
	
	//if(grid_x ==1 || grid_y == 1) return;
	
	float alpha, beta, gamma, x, y, varydens;
	XYZ p_interp, n_interp, t_interp, bn_interp, wind_interp;
	float s_interp, curl_interp, coord_s, coord_t;
	
	float dy = delta/(dens_y_0+0.1);
	float dx = delta/(dens_x_0+0.1);
	
	for(float jy=y_min; jy<y_max; jy+= dy)
	{
		y = jy + dy*0.5;
		
		varydens = dens_y_0 + (dens_y_1-dens_y_0)*(jy-y_min)/(y_max-y_min);
		
		dy = delta/(varydens+0.1);
		
		
		for(float ix=x_min; ix<x_max; ix+= dx)
		{
			x = ix+dx*0.5;
			
			varydens = dens_x_0 + (dens_x_1-dens_x_0)*(ix-x_min)/(x_max-x_min);
			
			dx = delta/(varydens+0.1);
			
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			p_interp = data[0].root*alpha + data[1].root*beta + data[2].root*gamma;
			n_interp = data[0].normal*alpha + data[1].normal*beta + data[2].normal*gamma;n_interp.normalize();
			t_interp = data[0].tangent*alpha + data[1].tangent*beta + data[2].tangent*gamma; t_interp.normalize();
			bn_interp = data[0].binormal*alpha + data[1].binormal*beta + data[2].binormal*gamma;bn_interp.normalize();
			s_interp = data[0].scale*alpha + data[1].scale*beta + data[2].scale*gamma;
			curl_interp = data[0].curl*alpha + data[1].curl*beta + data[2].curl*gamma;
			wind_interp = data[0].wind*alpha + data[1].wind*beta + data[2].wind*gamma;
			coord_s = data[0].s*alpha + data[1].s*beta + data[2].s*gamma;
			coord_t = data[0].t*alpha + data[1].t*beta + data[2].t*gamma;
			
			res[count].root = p_interp;
			res[count].normal = n_interp;
			res[count].tangent = t_interp;
			res[count].binormal = bn_interp;
			res[count].scale = s_interp;
			res[count].curl = curl_interp;
			res[count].wind = wind_interp;
			
			res[count].up = res[count].tangent^res[count].binormal;
			res[count].up.normalize();
			
			res[count].binormal = res[count].up^res[count].tangent;
			res[count].binormal.normalize();
			
			res[count].s = coord_s;
			res[count].t = coord_t;
			
			count++;
		}
	}
}

void FDice::rasterize(const float delta, const CoeRec* vertex_coe, DHair* res, CoeRec* coe_res, int& count)
{
	float x_max = 0, y_max = 0;
	float x_min = 10e8, y_min = 10e8;
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x > x_max) x_max = p_obj[i].x;
		if(p_obj[i].y > y_max) y_max = p_obj[i].y;
		if(p_obj[i].x < x_min) x_min = p_obj[i].x;
		if(p_obj[i].y < y_min) y_min = p_obj[i].y;
	}
	
	float dens_x_0, dens_x_1, dens_y_0, dens_y_1;
	
	for(int i=0; i<3; i++)
	{
		if(p_obj[i].x == x_min) dens_x_0 = data[i].density;
		if(p_obj[i].x == x_max) dens_x_1 = data[i].density;
		if(p_obj[i].y == y_min) dens_y_0 = data[i].density;
		if(p_obj[i].x == y_max) dens_y_1 = data[i].density;
	}

	float alpha, beta, gamma, x, y, varydens;
	XYZ p_interp, n_interp, t_interp, bn_interp, wind_interp;
	float s_interp, curl_interp, coord_s, coord_t;
	
	float dy = delta/(dens_y_0+0.1);
	float dx = delta/(dens_x_0+0.1);
	
	for(float jy=y_min; jy<y_max; jy+= dy)
	{
		y = jy + dy*0.5;
		
		varydens = dens_y_0 + (dens_y_1-dens_y_0)*(jy-y_min)/(y_max-y_min);
		
		dy = delta/(varydens+0.1);
		
		
		for(float ix=x_min; ix<x_max; ix+= dx)
		{
			x = ix+dx*0.5;
			
			varydens = dens_x_0 + (dens_x_1-dens_x_0)*(ix-x_min)/(x_max-x_min);
			
			dx = delta/(varydens+0.1);
			
			alpha = barycentric_coord(p_obj[1].x, p_obj[1].y, p_obj[2].x, p_obj[2].y, x, y)/f120;
			if(alpha<0 || alpha>1) continue;
			beta = barycentric_coord(p_obj[2].x, p_obj[2].y, p_obj[0].x, p_obj[0].y, x, y)/f201;
			if(beta<0 || beta>1) continue;
			gamma = barycentric_coord(p_obj[0].x, p_obj[0].y, p_obj[1].x, p_obj[1].y, x, y)/f012;
			if(gamma<0 || gamma>1) continue;
			
			p_interp = data[0].root*alpha + data[1].root*beta + data[2].root*gamma;
			n_interp = data[0].normal*alpha + data[1].normal*beta + data[2].normal*gamma;n_interp.normalize();
			t_interp = data[0].tangent*alpha + data[1].tangent*beta + data[2].tangent*gamma; t_interp.normalize();
			bn_interp = data[0].binormal*alpha + data[1].binormal*beta + data[2].binormal*gamma;bn_interp.normalize();
			s_interp = data[0].scale*alpha + data[1].scale*beta + data[2].scale*gamma;
			curl_interp = data[0].curl*alpha + data[1].curl*beta + data[2].curl*gamma;
			wind_interp = data[0].wind*alpha + data[1].wind*beta + data[2].wind*gamma;
			coord_s = data[0].s*alpha + data[1].s*beta + data[2].s*gamma;
			coord_t = data[0].t*alpha + data[1].t*beta + data[2].t*gamma;
			
			res[count].root = p_interp;
			res[count].normal = n_interp;
			res[count].tangent = t_interp;
			res[count].binormal = bn_interp;
			res[count].scale = s_interp;
			res[count].curl = curl_interp;
			res[count].wind = wind_interp;
			
			res[count].up = res[count].tangent^res[count].binormal;
			res[count].up.normalize();
			
			res[count].binormal = res[count].up^res[count].tangent;
			res[count].binormal.normalize();
			
			res[count].s = coord_s;
			res[count].t = coord_t;
			
			for(int k=0; k<16; k++)
			{
				coe_res[count].data[k] = vertex_coe[data[0].id].data[k] * alpha + vertex_coe[data[1].id].data[k] * beta + vertex_coe[data[2].id].data[k] * gamma;
			}
			
			count++;
		}
	}
}
//:~