#include "./fishskin.h"
#include "../shared/zMath.h"

CfishSkin::CfishSkin(void)
{
	m_id = NULL;
	m_weight = NULL;
}

CfishSkin::~CfishSkin(void)
{
	if(m_id != NULL) delete[] m_id;
	if(m_weight != NULL) delete[] m_weight;
}

void CfishSkin::bind(FMCFMesh* mesh, CfishBone* bone)
{
	int num_vertices = mesh->getNumVertex();
	m_id = new unsigned char[2*num_vertices];
	m_weight = new float[2*num_vertices];
	
	float bone_spacing = bone->getSpacing();
	int bone_count = bone->getBoneCount();
	
	for(int i=0; i<num_vertices; i++)
	{
		XYZ pt;
		mesh->getVertex(pt, i);
		float zcomp = pt.z;
		if(zcomp>0)
		{
			m_id[2*i] = m_id[2*i+1] = 0;
			m_weight[2*i] = 1;
			m_weight[2*i+1] = 0;
		}
		else if(-zcomp>(bone_count-1)*bone_spacing)
		{
			m_id[2*i] = m_id[2*i+1] = bone_count-1;
			m_weight[2*i] = 1;
			m_weight[2*i+1] = 0;
		}
		else
		{
			m_id[2*i] = (int)floor(-zcomp/bone_spacing);
			m_id[2*i+1] = m_id[2*i]+1;
			m_weight[2*i] = 1 - ( -zcomp/bone_spacing - m_id[2*i]);
			m_weight[2*i+1] = 1 - m_weight[2*i];
		}
	}
}

void CfishSkin::getWeightsById(unsigned int id, int& bone0, float& weight0, int& bone1, float& weight1)
{
	bone0 = m_id[2*id];
	bone1 = m_id[2*id+1];
	weight0 = m_weight[2*id];
	weight1 = m_weight[2*id+1];
}
