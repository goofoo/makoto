#include "fishData.h"
#include "../shared/fishSkin.h"

fishData::fishData(string& parameter): pMesh(NULL),sphere(0)
{
	m_cache_body_0 = new char[128];
	m_cache_body_1 = new char[128];
	m_cache_body_2 = new char[128];
	m_cache_body_3 = new char[128];
	seye_surf = new char[128];
	
	eye0space.setIdentity();
	eye1space.setIdentity();
	
	int n = sscanf(parameter.c_str(),
        "%s %s %s %s %f %f %f %f %f %f %f %i %f %f %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
        m_cache_body_0, m_cache_body_1, m_cache_body_2, m_cache_body_3, 
        &m_time_offset, &m_angle_flap, &m_angle_bend, &m_angle_oscillate, &m_length, &m_bone_count, &m_frequency, 
        &m_do_mblur, 
        &m_motion_begin, &m_motion_end,
		seye_surf,
		&eye0space.v[0][0],&eye0space.v[0][1],&eye0space.v[0][2],
		&eye0space.v[1][0],&eye0space.v[1][1],&eye0space.v[1][2],
		&eye0space.v[2][0],&eye0space.v[2][1],&eye0space.v[2][2],
		&eye0space.v[3][0],&eye0space.v[3][1],&eye0space.v[3][2],
		&eye0scale.x, &eye0scale.y, &eye0scale.z,
		&eye1space.v[0][0],&eye1space.v[0][1],&eye1space.v[0][2],
		&eye1space.v[1][0],&eye1space.v[1][1],&eye1space.v[1][2],
		&eye1space.v[2][0],&eye1space.v[2][1],&eye1space.v[2][2],
		&eye1space.v[3][0],&eye1space.v[3][1],&eye1space.v[3][2],
		&eye1scale.x, &eye1scale.y, &eye1scale.z);
		
	sphere = new RSphere();
}

void fishData::generateRIB(RtFloat detail)
{
	pMesh = new FMCFMesh();
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
//printf("mesh data on destroy");
	if(pMesh) delete pMesh;
	if(m_cache_body_0) delete m_cache_body_0;
	if(m_cache_body_1) delete m_cache_body_1;
	if(m_cache_body_2) delete m_cache_body_2;
	if(m_cache_body_3) delete m_cache_body_3;
	if(seye_surf) delete seye_surf;
	if(sphere) delete sphere;
}



void fishData::emit(const char* name) 
{
	if(!strcmp(name, "null") || pMesh->load(name) == 0) 
	{
		printf("Failed to open mesh cache file %s, DSO is skipped.\n", name);
		return;
	}
	
	float* uarray = new float[pMesh->getNumFaceVertex()];
	float* varray = new float[pMesh->getNumFaceVertex()];
	int* vertices = new int[pMesh->getNumFaceVertex()];
	
	pMesh->facevaryingS(uarray);
	pMesh->facevaryingT(varray);
	pMesh->rmanVertices(vertices);
	
	RtToken tags[] = {"creasemethod", "interpolateboundary"};
	RtInt nargs[] = {0,0,1,1,0,0};
	RtInt intargs[] = {1};
	RtFloat* floatargs;
	RtToken stringargs[] = {"chaikin"};
	
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
		
		MATRIX44F headmat;
		m_bone->getBoneById(0, headmat);
		
		eye0space *= headmat;
		eye1space *= headmat;
		sphere->emit(eye0space, eye0scale, eye1space, eye1scale, seye_surf);
		
		
		
		int bone0, bone1;
		float weight0, weight1;
		XYZ vec0, vec1, vec3, vec4;
		MATRIX44F mat0, mat1;
		if(m_do_mblur == 0)
		{
			for(unsigned int i=0; i<pMesh->getNumVertex(); i++)
			{
				pMesh->getVertex(vec0, i);
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

				mat0.transform(vec0);
				mat1.transform(vec1);
				
				pMesh->getVertexById(i).x = vec0.x*weight0 + vec1.x*weight1;
				pMesh->getVertexById(i).y = vec0.y*weight0 + vec1.y*weight1;
				pMesh->getVertexById(i).z = vec0.z*weight0 + vec1.z*weight1;
			}
			
			RiHierarchicalSubdivisionMesh("catmull-clark", (RtInt)pMesh->getNumFace(),  (RtInt*)pMesh->nverts(), (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, "P", (RtPoint*)pMesh->points(), "facevarying float s", (RtFloat*)uarray, "facevarying float t", (RtFloat*)varray, RI_NULL );
		}
		else
		{
			XYZ* upd_vertex = new XYZ[pMesh->getNumVertex()];
			
			for(unsigned int i=0; i<pMesh->getNumVertex(); i++)
			{
				pMesh->getVertex(vec0, i);
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

				mat0.transform(vec0);
				mat1.transform(vec1);
				
				upd_vertex[i].x = vec0.x*weight0 + vec1.x*weight1;
				upd_vertex[i].y = vec0.y*weight0 + vec1.y*weight1;
				upd_vertex[i].z = vec0.z*weight0 + vec1.z*weight1;
			}
			
			m_bone->setTime(m_time_offset + (m_motion_end - m_motion_begin)*6.0);
			m_bone->compose();
			
			for(unsigned int i=0; i<pMesh->getNumVertex(); i++)
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

				mat0.transform(vec0);
				mat1.transform(vec1);
				
				pMesh->getVertexById(i).x = vec0.x*weight0 + vec1.x*weight1;
				pMesh->getVertexById(i).y = vec0.y*weight0 + vec1.y*weight1;
				pMesh->getVertexById(i).z = vec0.z*weight0 + vec1.z*weight1;
			}
			
			RiMotionBegin( 2, (RtFloat)m_motion_begin, (RtFloat)m_motion_end );
			
			RiHierarchicalSubdivisionMesh("catmull-clark", (RtInt)pMesh->getNumFace(),  (RtInt*)pMesh->nverts(), (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, "P", (RtPoint*)upd_vertex, "facevarying float s", (RtFloat*)uarray, "facevarying float t", (RtFloat*)varray, RI_NULL );
			RiHierarchicalSubdivisionMesh("catmull-clark", (RtInt)pMesh->getNumFace(),  (RtInt*)pMesh->nverts(), (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, "P", (RtPoint*)pMesh->points(), "facevarying float s", (RtFloat*)uarray, "facevarying float t", (RtFloat*)varray, RI_NULL );
			RiMotionEnd();
			
			delete[] upd_vertex;
		}

	delete[] uarray;
	delete[] varray;
	delete[] vertices;

		if(m_bone) delete m_bone;
		if(m_skin) delete m_skin;
}
