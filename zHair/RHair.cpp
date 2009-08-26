#include "RHair.h"
#include "../shared/zGlobal.h"
#include "../shared/zFnEXR.h"
using namespace std;

RHair::RHair(std::string& parameter):
m_ndice(24),pHair(0),has_densmap(0),pDepthMap(0)
{
	m_cache_name = new char[256];
	m_dens_name = new char[256];
	m_depth_name = new char[256];

	int n = sscanf(parameter.c_str(), "%f %f %f %f %f %s %s %s %f %f %f %f %f %f %f %f %d %d", 
	&m_ndice,
	&m_width0, &m_width1,
	&dof_min, &dof_max,
	m_cache_name, 
	m_dens_name, 
	m_depth_name,
	&m_fuzz, 
	&m_kink,
	&m_clumping,
	&m_bald,
	&m_shutter_open, &m_shutter_close,
	&m_hair_0, &m_hair_1,
	&m_is_blur, &m_is_shd);
}

RHair::~RHair() 
{
	delete[] m_cache_name;
	delete[] m_dens_name;
	delete[] m_depth_name;
	if(pHair) delete pHair;
	if(pDepthMap) delete[] pDepthMap;
}

void RHair::init()
{
	pHair = new HairCache();
	
	string head = m_cache_name;
	zGlobal::cutByFirstDot(head);
	head += ".hairstart";
	pHair->loadStart(head.c_str());
	
	m_epsilon = pHair->getEpsilon(m_ndice);
	
	pHair->load(m_cache_name);
	if(m_is_blur==1) pHair->loadNext();
	
	if(strcmp(m_dens_name, "nil")!=0) has_densmap = pHair->setDensityMap(m_dens_name);
}

unsigned RHair::getNumTriangle() const
{
	return pHair->getNumTriangle();
}

void RHair::setAttrib(float* attrib) const
{
	attrib[0] = m_fuzz;
	attrib[1] = m_kink;
	attrib[2] = m_clumping;
	attrib[3] = m_width0;
	attrib[4] = m_width1;
}

void RHair::setMBlur(unsigned& idx, XYZ* velocities, float* shutters) const
{
	shutters[0] = m_shutter_open;
	shutters[1] = m_shutter_close;
	float fract = m_hair_1 - m_hair_0;
	pHair->lookupVelocity(idx, fract, velocities);
}

char RHair::setDepth()
{
	depthmap_w =-1, depthmap_h = -1;
	if(pDepthMap) {
		delete[] pDepthMap;
		pDepthMap = 0;
	}
	
	try 
	{
		ZFnEXR::checkFile(m_depth_name, &depthmap_w, &depthmap_h);
	}
	catch (const std::exception &exc) 
    { 
		cout<<exc.what()<<endl; 
	}

	if(depthmap_w > 16 && depthmap_h > 16) {
		pDepthMap = new float[depthmap_w*depthmap_h];

		M44f mat;
		float fov;
		ZFnEXR::readCameraNZ(m_depth_name, depthmap_w, depthmap_h, pDepthMap, mat, fov);
		
		cameraspaceinv.v[0][0] = mat[0][0];
		cameraspaceinv.v[0][1] = mat[0][1];
		cameraspaceinv.v[0][2] = mat[0][2];
		cameraspaceinv.v[0][3] = mat[0][3];
		cameraspaceinv.v[1][0] = mat[1][0];
		cameraspaceinv.v[1][1] = mat[1][1];
		cameraspaceinv.v[1][2] = mat[1][2];
		cameraspaceinv.v[1][3] = mat[1][3];
		cameraspaceinv.v[2][0] = mat[2][0];
		cameraspaceinv.v[2][1] = mat[2][1];
		cameraspaceinv.v[2][2] = mat[2][2];
		cameraspaceinv.v[2][3] = mat[2][3];
		cameraspaceinv.v[3][0] = mat[3][0];
		cameraspaceinv.v[3][1] = mat[3][1];
		cameraspaceinv.v[3][2] = mat[3][2];
		cameraspaceinv.v[3][3] = mat[3][3];
		
		cameraspaceinv.inverse();
		
		tanhfov = tan(fov*0.5);
		return 1;
	}
	return 0;
}

char RHair::cullBBox(float *box) const
{
	XYZ Q(box[0], box[2], box[4]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[1], box[2], box[4]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[0], box[3], box[4]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[1], box[3], box[4]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[0], box[2], box[5]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[1], box[2], box[5]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[0], box[3], box[5]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	Q = XYZ(box[1], box[3], box[5]);
	cameraspaceinv.transform(Q);
	if(Q.z < 0.1) return 1;
	if(!cullPoint(Q)) return 0;
	
	return 1;
}

char RHair::cullPoint(XYZ& Q) const
{
	double a = Q.z*tanhfov;
	Q.x /= a;
	Q.y /= a;
	
	int loc_x, loc_y;
	if(Q.x > -1 && Q.x < 1 && Q.y > -1 && Q.y < 1) {
		loc_x = (Q.x + 1.0)/2.0*(depthmap_w-1);
		loc_y = (Q.y + 1.0)/2.0*(depthmap_h-1);
		
		
		if(Q.z < pDepthMap[loc_y*depthmap_w + loc_x] ) return 0;
	}
	
	return 1;
}

float RHair::clostestDistance(const XYZ* pos) const
{
	XYZ Q = pos[0];
	cameraspaceinv.transform(Q);
	float dist = Q.z;
	
	Q = pos[1];
	cameraspaceinv.transform(Q);
	
	if(Q.z < dist) dist = Q.z;
	
	Q = pos[2];
	cameraspaceinv.transform(Q);
	
	if(Q.z < dist) dist = Q.z;
	
	return dist;
}

void RHair::meanDisplace(unsigned& idx, XYZ& disp) const
{
	float fract = m_hair_1 - m_hair_0;
	pHair->lookupMeanDisplace(idx, fract, disp);
}

void RHair::simplifyDOF(unsigned& idx, float& depth, float& val) const
{
	if(depth > dof_min) {
		if(depth > dof_max) val = 0.25;
		else val = 1.f - 0.75f*sqrt((depth-dof_min)/(dof_max - dof_min));
	}
}

void RHair::simplifyMotion(unsigned& idx, float& val) const
{
	float fract = m_hair_1 - m_hair_0;
	XYZ p0, p1;
	pHair->lookupMeanPos(idx, fract, p0, p1);
	
	cameraspaceinv.transform(p0);
	cameraspaceinv.transform(p1);
	
	double a = p0.z*tanhfov;
	p0.x /= a;
	p0.y /= a;
	
	a = p1.z*tanhfov;
	p1.x /= a;
	p1.y /= a;
	
	float dpix = sqrt((p0.x - p1.x)*(p0.x - p1.x) + (p0.y - p1.y)*(p0.y - p1.y))*2000;
	if(dpix>2) {
		fract = (a-2)/16;
		if(fract > 1) fract = 1;
		val -= val*0.5*fract*fract;
	}
}
//:~