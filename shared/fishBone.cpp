#include "fishbone.h"
#include "zFMatrix44f.h"
#include "zMath.h"

CfishBone::CfishBone(void):m_n_bones(20)
{
	m_frequency = 1.0f;
	m_spacing = 1.0f;
	m_angle_prim = 5.0f;
	m_angle_offset = 0.0f;
	m_angle_oscillate = 0.0f;
	
	m_skeleton = new MATRIX44F[20];
	m_pos_bind = new XYZ[20];
	for(int i=0; i<20; i++)
	{
		m_pos_bind[i].x =0.0f;
		m_pos_bind[i].y =0.0f;
		m_pos_bind[i].z =-(float)i*m_spacing;
	}
}

CfishBone::~CfishBone(void)
{
	if(m_skeleton != NULL) delete[] m_skeleton;
	if(m_pos_bind != NULL) delete[] m_pos_bind;
}

void CfishBone::setSpacing(float s)
{ 
	m_spacing = s; 
	if(m_pos_bind != NULL) delete[] m_pos_bind;
	m_pos_bind = new XYZ[m_n_bones];
	for(int i=0; i<m_n_bones; i++)
	{
		m_pos_bind[i].x =0.0f;
		m_pos_bind[i].y =0.0f;
		m_pos_bind[i].z =-(float)i*m_spacing;
	}
}

void CfishBone::setLength(float l)
{
	setSpacing(l/(m_n_bones-1));
}

void CfishBone::compose()
{
	zFMatrix44f* fmatrix = new zFMatrix44f();
	fmatrix->reset(m_skeleton[0]);
	
	float rad_t_0 = m_time*PI*m_frequency;
	float rad_t = rad_t_0;
	float angle = m_angle_prim*sin(rad_t);
	
	float fswing = sin(rad_t)*deg2rad(m_angle_oscillate);
	
	fmatrix->rotateY(m_skeleton[0], fswing);
	
	for(int i=1; i<m_n_bones; i++)
	{
		rad_t -= PI/(float)m_n_bones*(2.5f-2.0f*(float)i/(float)m_n_bones);
		
		angle = m_angle_prim*sin(rad_t)/(float)m_n_bones + m_angle_offset/m_n_bones;
		
		fmatrix->reset(m_skeleton[i]);
		
		fmatrix->rotateY(m_skeleton[i], -fswing/m_n_bones*2);
		fmatrix->rotateY(m_skeleton[i], deg2rad(angle));
		
		fmatrix->translate(m_skeleton[i], 0,0,-m_spacing);

		fmatrix->multiply(m_skeleton[i], m_skeleton[i-1]);
	}
	
	delete fmatrix;
}

const MATRIX44F& CfishBone::getBoneById(int id)
{
	return m_skeleton[id];
}

void CfishBone::getBoneById(int id, MATRIX44F& mat, XYZ& pos)
{
	mat = m_skeleton[id];
	pos = m_pos_bind[id];
}

void CfishBone::getBoneById(int id, MATRIX44F& mat)
{
	mat = m_skeleton[id];
}

void CfishBone::initialize(float length, int a)
{
	m_n_bones = a;
	delete[] m_skeleton;
	delete[] m_pos_bind;
	
	m_skeleton = new MATRIX44F[m_n_bones];
	m_pos_bind = new XYZ[m_n_bones];
	
	m_spacing = length/(m_n_bones-1);
	
	for(int i=0; i<m_n_bones; i++)
	{
		m_pos_bind[i].x =0.0f;
		m_pos_bind[i].y =0.0f;
		m_pos_bind[i].z =-(float)i*m_spacing;
	}
}