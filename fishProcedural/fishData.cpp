#include "fishData.h"
#include "../shared/zFMatrix44f.h"
#include "../shared/fishSkin.h"

fishData::fishData(string& parameter): pMesh(NULL)
{
	m_cache_body_0 = new char[128];
	m_cache_body_1 = new char[128];
	m_cache_body_2 = new char[128];
	m_cache_body_3 = new char[128];
	
	int n = sscanf(parameter.c_str(),
        "%s %s %s %s %f %f %f %f %f %f %f %i %f %f",
        m_cache_body_0, m_cache_body_1, m_cache_body_2, m_cache_body_3, 
        &m_time_offset, &m_angle_flap, &m_angle_bend, &m_angle_oscillate, &m_length, &m_bone_count, &m_frequency, 
        &m_do_mblur, 
        &m_motion_begin, &m_motion_end);
}

void fishData::generateRIB(RtFloat detail)
{
	pMesh = new CrmcMesh();
	int stat = 0;
	
	// level of detail operation
	if(detail<512.0f) 
	{
		emit(m_cache_body_3);
	}
	else if(detail<2048.0f)
	{
		emit(m_cache_body_2);
	}
	else if(detail<4096.0f)
	{
		emit(m_cache_body_1);
	}
	else
	{
		emit(m_cache_body_0);
	}
	
	return;
}

fishData::~fishData() 
{
	if(pMesh) delete pMesh;
	if(m_cache_body_0) delete m_cache_body_0;
	if(m_cache_body_1) delete m_cache_body_1;
	if(m_cache_body_2) delete m_cache_body_2;
	if(m_cache_body_3) delete m_cache_body_3;
	//printf("mesh data on destroy");
}



void fishData::emit(const char* name) 
{
	if(!strcmp(name, "null") || pMesh->load(name) == 0) 
	{
		printf("Failed to open mesh cache file %s, DSO is skipped.\n", name);
		return;
	}
	
		CfishBone* m_bone = new CfishBone();
		CfishSkin* m_skin = new CfishSkin();

		m_bone->initialize(m_length, (int)m_bone_count);
		m_bone->setTime(m_time_offset);
		m_bone->setFrequency(m_frequency);
		m_bone->setAnglePrim(m_angle_flap);
		m_bone->setAngleOffset(m_angle_bend);
		m_bone->setOscillation(m_angle_oscillate);
		m_bone->compose();
		
		m_skin->bind(pMesh, m_bone);
		
		zFMatrix44f* fmatrix = new zFMatrix44f();
		int bone0, bone1;
		float weight0, weight1;
		XYZ vec0, vec1, vec3, vec4;
		MATRIX44F mat0, mat1;
		if(m_do_mblur == 0)
		{
			for(unsigned int i=0; i<pMesh->getVerticesCount(); i++)
			{
				vec0 = pMesh->getVertexById(i);
				vec1 = vec0;
				
				m_skin->getWeightsById(i, bone0, weight0, bone1, weight1);
		
				m_bone->getBoneById(bone0, mat0, vec3);
				m_bone->getBoneById(bone1, mat1, vec4);
				
				vec0.x -= vec3.x;
				vec0.y -= vec3.y;
				vec0.z -= vec3.z;
				
				vec1.x -= vec4.x;
				vec1.y -= vec4.y;
				vec1.z -= vec4.z;
				
				fmatrix->transform(vec0, mat0);
				fmatrix->transform(vec1, mat1);
				
				pMesh->getVertexById(i).x = vec0.x*weight0 + vec1.x*weight1;
				pMesh->getVertexById(i).y = vec0.y*weight0 + vec1.y*weight1;
				pMesh->getVertexById(i).z = vec0.z*weight0 + vec1.z*weight1;
				
				vec0 = pMesh->getNormalById(i);
				vec1 = vec0;
				
				fmatrix->ntransform(vec0, mat0);
				fmatrix->ntransform(vec1, mat1);
				
				pMesh->getNormalById(i).x = vec0.x*weight0 + vec1.x*weight1;
				pMesh->getNormalById(i).y = vec0.y*weight0 + vec1.y*weight1;
				pMesh->getNormalById(i).z = vec0.z*weight0 + vec1.z*weight1;
			}
			
			RiPointsGeneralPolygons( (RtInt)pMesh->getNumberOfFaces(), (RtInt*)pMesh->getNLoops(), (RtInt*)pMesh->getNVertices(), (RtInt*)pMesh->getVertices(), "P", (RtPoint*)pMesh->getCvs(), "N", (RtPoint*)pMesh->getNormals(), "facevarying float s", (RtFloat*)pMesh->getCoordS(), "facevarying float t", (RtFloat*)pMesh->getCoordT(), RI_NULL);
		}
		else
		{
			XYZ* upd_vertex = new XYZ[pMesh->getVerticesCount()];
			XYZ* upd_normal = new XYZ[pMesh->getVerticesCount()];
			
			for(unsigned int i=0; i<pMesh->getVerticesCount(); i++)
			{
				vec0 = pMesh->getVertexById(i);
				vec1 = vec0;
				
				m_skin->getWeightsById(i, bone0, weight0, bone1, weight1);
		
				m_bone->getBoneById(bone0, mat0, vec3);
				m_bone->getBoneById(bone1, mat1, vec4);
				
				vec0.x -= vec3.x;
				vec0.y -= vec3.y;
				vec0.z -= vec3.z;
				
				vec1.x -= vec4.x;
				vec1.y -= vec4.y;
				vec1.z -= vec4.z;
				
				fmatrix->transform(vec0, mat0);
				fmatrix->transform(vec1, mat1);
				
				upd_vertex[i].x = vec0.x*weight0 + vec1.x*weight1;
				upd_vertex[i].y = vec0.y*weight0 + vec1.y*weight1;
				upd_vertex[i].z = vec0.z*weight0 + vec1.z*weight1;
				
				vec0 = pMesh->getNormalById(i);
				vec1 = vec0;
				
				fmatrix->ntransform(vec0, mat0);
				fmatrix->ntransform(vec1, mat1);
				
				upd_normal[i].x = vec0.x*weight0 + vec1.x*weight1;
				upd_normal[i].y = vec0.y*weight0 + vec1.y*weight1;
				upd_normal[i].z = vec0.z*weight0 + vec1.z*weight1;
			}
			
			m_bone->setTime(m_time_offset + (m_motion_end - m_motion_begin)*6.0);
			m_bone->compose();
			
			for(unsigned int i=0; i<pMesh->getVerticesCount(); i++)
			{
				vec0 = pMesh->getVertexById(i);
				vec1 = vec0;
				
				m_skin->getWeightsById(i, bone0, weight0, bone1, weight1);
		
				m_bone->getBoneById(bone0, mat0, vec3);
				m_bone->getBoneById(bone1, mat1, vec4);
				
				vec0.x -= vec3.x;
				vec0.y -= vec3.y;
				vec0.z -= vec3.z;
				
				vec1.x -= vec4.x;
				vec1.y -= vec4.y;
				vec1.z -= vec4.z;
				
				fmatrix->transform(vec0, mat0);
				fmatrix->transform(vec1, mat1);
				
				pMesh->getVertexById(i).x = vec0.x*weight0 + vec1.x*weight1;
				pMesh->getVertexById(i).y = vec0.y*weight0 + vec1.y*weight1;
				pMesh->getVertexById(i).z = vec0.z*weight0 + vec1.z*weight1;
				
				vec0 = pMesh->getNormalById(i);
				vec1 = vec0;
				
				fmatrix->ntransform(vec0, mat0);
				fmatrix->ntransform(vec1, mat1);
				
				pMesh->getNormalById(i).x = vec0.x*weight0 + vec1.x*weight1;
				pMesh->getNormalById(i).y = vec0.y*weight0 + vec1.y*weight1;
				pMesh->getNormalById(i).z = vec0.z*weight0 + vec1.z*weight1;
			}
			
			RiMotionBegin( 2, (RtFloat)m_motion_begin, (RtFloat)m_motion_end );
			RiPointsGeneralPolygons( (RtInt)pMesh->getNumberOfFaces(), (RtInt*)pMesh->getNLoops(), (RtInt*)pMesh->getNVertices(), (RtInt*)pMesh->getVertices(), "P", (RtPoint*)upd_vertex, "N", (RtPoint*)upd_normal, "facevarying float s", (RtFloat*)pMesh->getCoordS(), "facevarying float t", (RtFloat*)pMesh->getCoordT(), RI_NULL);
			RiPointsGeneralPolygons( (RtInt)pMesh->getNumberOfFaces(), (RtInt*)pMesh->getNLoops(), (RtInt*)pMesh->getNVertices(), (RtInt*)pMesh->getVertices(), "P", (RtPoint*)pMesh->getCvs(), "N", (RtPoint*)pMesh->getNormals(), "facevarying float s", (RtFloat*)pMesh->getCoordS(), "facevarying float t", (RtFloat*)pMesh->getCoordT(), RI_NULL);
			RiMotionEnd();
			
			delete[] upd_vertex;
			delete[] upd_normal;
		}

		delete fmatrix;

		if(m_bone) delete m_bone;
		if(m_skin) delete m_skin;
}
