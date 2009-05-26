#include "RHair.h"
#include "HairCache.h"
#include "../shared/zGlobal.h"
using namespace std;

RHair::RHair(std::string& parameter):
m_ndice(24)
{
	m_cache_name = new char[256];

	int n = sscanf(parameter.c_str(), "%f %f %f %s %f %f %f %f %f %f %f %f %d", 
	&m_ndice,
	&m_width0, &m_width1,
	m_cache_name, 
	&m_twist, 
	&m_clumping, 
	&m_fuzz, 
	&m_kink,
	&m_shutter_open, &m_shutter_close,
	&m_hair_0, &m_hair_1,
	&m_is_blur);
}

RHair::~RHair() 
{
	delete[] m_cache_name;
}

void RHair::generateRIB(RtFloat detail)
{
	HairCache* pHair = new HairCache();
	pHair->setDiceNumber(m_ndice);
	
	string head = m_cache_name;
	zGlobal::cutByFirstDot(head);
	head += ".hairstart";
	pHair->loadStart(head.c_str());
	pHair->dice();
	pHair->bind();
	pHair->load(m_cache_name);
	
	pHair->setTwist(m_twist);
	pHair->setClumping(m_clumping);
	pHair->setFuzz(m_fuzz);
	pHair->setKink(m_kink);
	pHair->setWidth(m_width0, m_width1);
			
	pHair->create();
	
	float cwidth = 0.01f;
	RiCurves("cubic", (RtInt)pHair->getNumCurves(), (RtInt*)pHair->getNumVertices(), "nonperiodic", "P", (RtPoint*)pHair->points(), "width", (RtFloat*)pHair->getWidth(), RI_NULL);
	
	delete pHair;
	
/*
	FXMLMesh* pMesh = new FXMLMesh(m_cache_name, m_mesh_name);
	
	RtToken tags[] = {"creasemethod", "interpolateboundary"};
	RtInt nargs[] = {0,0,1,1,0,0};
	RtInt intargs[] = {1};
	RtFloat* floatargs;
	RtToken stringargs[] = {"chaikin"};
	
	RtToken paramname[MAX_NUMPARAM];
	RtPointer paramvalue[MAX_NUMPARAM];
	
	string sname(m_prt_name);
	
	if(!pMesh->isNull())
	{
		if(pMesh->getNumUVSet()<1) RiHierarchicalSubdivisionMesh("catmull-clark", (RtInt)pMesh->nfaces(),  (RtInt*)pMesh->nverts(), (RtInt*)pMesh->verts(), (RtInt)2, tags, nargs, intargs, floatargs, stringargs, "P", (RtPoint*)pMesh->points(), RI_NULL );
		else
		{
			paramname[0] = "P";
			paramvalue[0] =(RtPoint*)pMesh->points();
			
			int nparam = 1;

			for(int i=0; i<pMesh->getNumUVSet(); i++)
			{
				paramname[nparam] = (RtToken)pMesh->getSNameById(i);
				paramvalue[nparam] = (RtFloat*)pMesh->getSById(i);
				nparam++;
				paramname[nparam] = (RtToken)pMesh->getTNameById(i);
				paramvalue[nparam] = (RtFloat*)pMesh->getTById(i);
				nparam++;
			}
			
			if(m_i_hdr_shadowed == 1.0f || m_i_lightsrc_shadowed == 1.0f)
			{
				zGlobal::changeFilenameExtension(sname, "prtu");
				pMesh->appendColorSet("vertex color coeffu", sname.c_str());
				if(m_i_hdr_shadowed == 1.0f)
				{
					zGlobal::changeFilenameExtension(sname, "prt");
					pMesh->appendColorSet("vertex color coeff", sname.c_str());
				}
				if(m_i_lightsrc_shadowed == 1.0f)
				{
					zGlobal::changeFilenameExtension(sname, "prtl");
					pMesh->appendColorSet("vertex color coeffl", sname.c_str());
					zGlobal::changeFilenameExtension(sname, "prtj");
					pMesh->appendColorSet("vertex color coeffj", sname.c_str());
				}
			}
			
			if(m_i_hdr_indirect == 1.0f)
			{
				zGlobal::changeFilenameExtension(sname, "prti");
				pMesh->appendColorSet("vertex color coeffi", sname.c_str());
			}
			
			if(m_i_hdr_scat == 1.0f)
			{
				zGlobal::changeFilenameExtension(sname, "prts");
				pMesh->appendColorSet("vertex color coeffs", sname.c_str());
				zGlobal::changeFilenameExtension(sname, "prte");
				pMesh->appendColorSet("vertex color coeffe", sname.c_str());
			}
			
			if(m_i_hdr_backscat == 1)
			{
				zGlobal::changeFilenameExtension(sname, "prtb");
				pMesh->appendColorSet("vertex color coeffb", sname.c_str());
			}
			
			for(int i=0; i< pMesh->getNumColorSet(); i++)
			{
				paramname[nparam] = (RtToken)pMesh->getColorSetNameById(i);
				paramvalue[nparam] = (RtFloat*)pMesh->getColorSetById(i);
				nparam++;
			}
			
			if(m_is_blur==0) RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)pMesh->nfaces(),  (RtInt*)pMesh->nverts(), (RtInt*)pMesh->verts(), (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)nparam, paramname, paramvalue );
			else 
			{
				pMesh->setMotion(m_mesh_0, m_mesh_1);
				RiMotionBegin(2, m_shutter_open, m_shutter_close);
				
				paramvalue[0] =(RtPoint*)pMesh->pointsOpen();
					RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)pMesh->nfaces(),  (RtInt*)pMesh->nverts(), (RtInt*)pMesh->verts(), (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)nparam, paramname, paramvalue );
				paramvalue[0] =(RtPoint*)pMesh->pointsClose();
					RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)pMesh->nfaces(),  (RtInt*)pMesh->nverts(), (RtInt*)pMesh->verts(), (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)nparam, paramname, paramvalue );
					
				RiMotionEnd();
			}
		}
	}
  
	delete pMesh;
*/
	return;
}
//:~