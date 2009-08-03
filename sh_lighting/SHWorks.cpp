#include "SHWorks.h"
#include "../shared/FXMLScene.h"
#include "../shared/zGlobal.h"
#include <iostream>
#include <fstream>

struct HDRLight : public SHFunction
{
	HDRLight(const HDRtexture* _img): img( _img) {}
	float getFloat(const SHSample& s) const
	{
		return 128.0f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r, g, b;
		img->getRGBByVector(s.vec[0], s.vec[1], s.vec[2], r,g,b);
		
		return XYZ(r,g,b);
	}
	
	const HDRtexture* img;
};

struct HDRLightSph : public SHFunction
{
	HDRLightSph(const HDRtexture* _img): img( _img) {}
	float getFloat(const SHSample& s) const
	{
		return 128.0f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r, g, b;
		img->getRGBAt(s.theta, s.phi, r,g,b);
		
		return XYZ(r,g,b);
	}
	
	const HDRtexture* img;
};

struct ConstantSph : public SHFunction
{
	ConstantSph() {}
	float getFloat(const SHSample& s) const
	{
		return 1.f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		return XYZ(1.f);
	}
};


SHWorks::SHWorks(void):m_theta(1.57f),m_phi(-1.57f),m_theta2pi(0.0),
//coebuf(0),
m_draw_type(-1)
//,color_buf(0)
{
	f_sh = new sphericalHarmonics();
	recon_samp = new SHSample[SHW_NUM_RECON_SAMPLE];
	f_sh->setupSampleStratified(recon_samp, SHW_SQRT_NUM_RECON_SAMPLE);
	
	ConstantSph cl;
	f_sh->projectFunction(cl, constantCoeff);
	
	b_vis = new CVisibilityBuffer();
//	b_bone_vis = new CVisibilityBuffer();
	b_light_vis = new CVisibilityBuffer();
	m_use_lightsource = m_draw_type = 0;
}

SHWorks::~SHWorks(void)
{
	if(f_sh) delete f_sh;
		if(b_vis) delete b_vis;
	if(recon_samp)	delete[] recon_samp;
		//if(color_buf) delete[] color_buf;
}

/*Read visibility and project SH*/
void SHWorks::projectInterreflect(const XYZ& ray, const FQSPLAT* tree, const CVisibilityBuffer* visibility, XYZ* coeff)
{
	float dist, K;
	unsigned int node_id;
	
	for(unsigned int j=0; j<SH_N_SAMPLES; j++)
	{
		SHSample& s = f_sh->getSample(j);
		float H = ray.dot(s.vector);
		if(H>0)
		{
			visibility->read(s.vector, dist, node_id, K);
			if(dist < 10e7 && node_id<m_max_buf)
			{
				f_sh->addCoeff(coeff, tree->getCoe(node_id).data, tree->getColor(node_id)*H*SH_UNITAREA*K);
				
				//printf("%f ", tree->getCoe(node_id).data[0]);
				//printf("-%f-", tree->getColor(node_id).x);
			}
		}
	}
}

void SHWorks::projectSubSurfaceScatter(const XYZ& ray, const FQSPLAT* tree, const CVisibilityBuffer* visibility, XYZ* coeff0, XYZ* coeff1)
{
	float dist, K, H, sc;
	unsigned int node_id;
	f_sh->zeroCoeff(coeff0);
	f_sh->zeroCoeff(coeff1);
	for(unsigned int j=0; j<SH_N_SAMPLES; j++)
	{
		SHSample& s = f_sh->getSample(j);
		H = ray.dot(s.vector);
		if(H>0)
		{
			visibility->read(s.vector, dist, node_id, K);
			if(dist < 10e7 && node_id<m_max_buf)
			{
				sc= sqrt(H)*SH_UNITAREA*K*.29f;
				f_sh->addCoeff(coeff0, tree->getCoe(node_id).data, sc);
				f_sh->addCoeff(coeff1, tree->getCoe(node_id).data, sc*H);
			}
		}
	}
}

void SHWorks::projectAbsorbSubSurfaceScatter(const XYZ& ray, const FQSPLAT* tree, const CVisibilityBuffer* visibility, float grid, XYZ* coeff)
{
	float dist, K, H, sc;
	unsigned int node_id;
	f_sh->zeroCoeff(coeff);
	for(unsigned int j=0; j<SH_N_SAMPLES; j++)
	{
		SHSample& s = f_sh->getSample(j);
		H = ray.dot(s.vector);
		if(H>0)
		{
			visibility->read(s.vector, dist, node_id, K);
			if(dist < 10e7 && node_id<m_max_buf)
			{
				sc= H*H*SH_UNITAREA/(1+dist/grid/4);
				f_sh->addCoeff(coeff, tree->getCoe(node_id).data, sc);
			}
		}
	}
}

void SHWorks::projectDiffuse(const XYZ& ray, const CVisibilityBuffer* visibility, XYZ* coeff0, XYZ* coeff1)
{
	float dist;
	f_sh->zeroCoeff(coeff0);
	f_sh->zeroCoeff(coeff1);
	for(unsigned int j=0; j<SH_N_SAMPLES; j++)
	{
		SHSample s = f_sh->getSample(j);
		float H = ray.dot(s.vector);
		if(H>0)
		{
			visibility->read(s.vector, dist);
			if(dist > 10e7)
			{
				f_sh->projectASample(coeff0, j, H);
				f_sh->projectASample(coeff1, j);
			}
		}
	}
}

void SHWorks::projectDiffuse(const XYZ& ray, const FQSPLAT* light, const CVisibilityBuffer* shdowVisibility, const CVisibilityBuffer* lightVisibility, XYZ* coeff0, XYZ* coeff2)
{
	f_sh->zeroCoeff(coeff0);
	f_sh->zeroCoeff(coeff2);
	float dist, dist1, K1;
	unsigned int node_id;
	for(unsigned int j=0; j<SH_N_SAMPLES; j++)
	{
		SHSample s = f_sh->getSample(j);
		float H = ray.dot(s.vector);
		if(H>0)
		{
			shdowVisibility->read(s.vector, dist);
			
			lightVisibility->read(s.vector, dist1, node_id, K1);
			if(node_id>=m_max_buf) continue;

			XYZ src_col = light->getColor(node_id) * K1;
			
			if(dist1 < 10e8)
			{
				f_sh->projectASample(coeff2, j, src_col);
			}
			
			if(dist1 < dist)
			{
				src_col *= H;
				f_sh->projectASample(coeff0, j, src_col);
			}
		}
	}
}

void SHWorks::computeHDRCoeff(const HDRtexture* hdrimg)
{
	HDRLightSph hl(hdrimg);
	f_sh->projectFunction(hl, hdrCoeff);
	f_sh->rotateSH(hdrCoeff,  hdrCoeff, m_theta , m_phi);
}

void SHWorks::computeHDRCoeffNoRotate(const HDRtexture* hdrimg)
{
	HDRLight hl(hdrimg);
	f_sh->projectFunction(hl, hdrCoeff);//printHDRCoeff();
}

XYZ SHWorks::computeSHLighting(XYZ* coeff)
{
	return  f_sh->integrate(hdrCoeff, coeff);
}

XYZ SHWorks::computeSHLighting(XYZ* coeff, const XYZ& normal)
{
	diffuseTransfer(coeff, tmpCoeff, normal);
	return  f_sh->integrate(hdrCoeff, tmpCoeff);
}

XYZ SHWorks::computeSrcSHLighting(XYZ* coeff)
{
	return f_sh->integrate(constantCoeff, coeff);
}

float SHWorks::computeSrcSHLighting(XYZ* coeff, const XYZ& normal)
{
	diffuseTransfer(coeff, tmpCoeff, normal);
	return  f_sh->rIntegrate(constantCoeff, tmpCoeff);
}

XYZ SHWorks::computeSrcSHLighting(XYZ* coeff, const XYZ& normal, const int channel)
{
	diffuseTransfer(coeff, tmpCoeff, normal, channel);
	return  f_sh->integrate(constantCoeff, tmpCoeff);
}

XYZ SHWorks::computeSHLighting(XYZ* srcCoeff, XYZ* shdCoeff, const XYZ& normal)
{
	diffuseTransfer(shdCoeff, tmpCoeff, normal);
	return  f_sh->integrate(srcCoeff, tmpCoeff);
}

struct hdrReconstructSH : public SHFunction
{
	hdrReconstructSH(const XYZ* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i];
	}
	
	float getFloat(const SHSample& s) const
	{
		return 128.0f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		XYZ value(0,0,0);

		// Calculate a weighted sum of all the SH bases
		for (int l = 0, i = 0; l < SH_N_BANDS; l++)
			for (int m = -l; m <= l; m++, i++)
				value += coeffs[i] * SHMath::SH(l, m, s.theta, s.phi);

		return (value);
	}
	
	XYZ coeffs[SH_N_BASES];
};

void SHWorks::drawHDR(const HDRtexture* hdrimg)
{
	HDRLightSph hl(hdrimg);
	
	glPushMatrix();

	glRotatef(m_theta/PI*180.f,0,1,0);
	
	MATRIX44F mat;
	XYZ front(sin(m_theta), 0, cos(m_theta)); front.normalize();
	XYZ up(0,1,0); 
	XYZ side = up.cross(front); side.normalize();
	mat.setOrientations(side, up, front); mat.inverse();
	
	XYZ new_z(0,0,1); mat.transform(new_z);
	
	glRotatef(m_phi/PI*180.f, new_z.x, new_z.y, new_z.z);
	
	f_sh->drawSHFunction(hl);
	glPopMatrix();
}

void SHWorks::SHRotate(XYZ* coeff, XYZ* coeff_res, const double theta, const double phi)
{
	f_sh->rotateSH(coeff, coeff_res, theta, phi);
}

void SHWorks::diffuseTransfer(XYZ* coeff_in, XYZ* coeff_out, const XYZ& normal)
{
	f_sh->diffuseTransfer(recon_samp, SHW_NUM_RECON_SAMPLE, SHW_UNIT_RECON_SAMPLE, coeff_in, coeff_out, redCoeff, normal);
}

void SHWorks::diffuseTransfer(XYZ* coeff_in, XYZ* coeff_out, const XYZ& normal, const int channel)
{
	switch(channel)
	{
		case 0:
			f_sh->rDiffuseTransfer(recon_samp, SHW_NUM_RECON_SAMPLE, SHW_UNIT_RECON_SAMPLE, coeff_in, coeff_out, normal);
			
		break;
		case 1:
			f_sh->gDiffuseTransfer(recon_samp, SHW_NUM_RECON_SAMPLE, SHW_UNIT_RECON_SAMPLE, coeff_in, coeff_out, normal);
			
		break;
		case 2:
			f_sh->bDiffuseTransfer(recon_samp, SHW_NUM_RECON_SAMPLE, SHW_UNIT_RECON_SAMPLE, coeff_in, coeff_out, normal);
			
		break;
		
		default:
			
		f_sh->rgbDiffuseTransfer(recon_samp, SHW_NUM_RECON_SAMPLE, SHW_UNIT_RECON_SAMPLE, coeff_in, coeff_out, normal);
	}
}

void SHWorks::rgbDiffuseTransfer(XYZ* coeff_in, XYZ* coeff_out, const XYZ& normal)
{
	f_sh->rgbDiffuseTransfer(recon_samp, SHW_NUM_RECON_SAMPLE, SHW_UNIT_RECON_SAMPLE, coeff_in, coeff_out, normal);
}

void SHWorks::showSamples()
{
	int x,y;
	glBegin(GL_POINTS);
	for(int i=0; i<SH_N_SAMPLES; i++)
	{
		SHSample& s = f_sh->getSample(i);
		if(s.vector.dot(XYZ(0,0,1)) > 0)
		{
		glColor3f(s.theta/PI, s.phi/2/PI, 0.f);
			CVisibilityBuffer::HemisphereToST(s.vector, x, y);
		//glVertex3d(s.vec[0], s.vec[1], s.vec[2]);
			glVertex3d((float)x/64, (float)y/64, 0.f);
			if(x<0 ||x>127 || y<0 || y>127) cout<<" warning";
		}
	}
	glEnd();
}

void SHWorks::show(const CVisibilityBuffer* visibility)
{
	XYZ rgb;
	glBegin(GL_QUADS);
	
	for(int j=0; j<VIS_SQRT_N_GRID; j++)
		for(int i=0; i<VIS_SQRT_N_GRID; i++)
		{
			if(visibility->read(i,j) < 10e8)
			{
				//visibility->read(i,j,rgb);
				glColor3f(rgb.x, rgb.y, rgb.z);
				glVertex3f(i, j, 0.f);
				glVertex3f(i+1, j, 0.f);
				glVertex3f(i+1, j+1, 0.f);
				glVertex3f(i, j+1, 0.f);
			}
		}
	glEnd();
}

void SHWorks::saveRT(const FXMLScene* scene)
{
	float gridsize;
	MeshList list = scene->getMesh();
	TreeList shadow = scene->getShadowTree();
	for(MeshList::iterator mesh=list.begin(); mesh != list.end(); mesh++)
	{
		const FXMLMesh* pMesh = *mesh;
		cout<<" computing HDR Direct on "<<pMesh->getMeshName()<<endl;
		//cout<<" grid size "<<gridsize<<endl;
		int n_vert = pMesh->getNumVertex();
		cout<<" n vertex "<<n_vert<<endl;
		int n_bucket = n_vert/39;
		
		float* coebf = new float[n_vert*SH_N_BASES];
		float* coebf1 = new float[n_vert*SH_N_BASES];
	
		XYZ Q, ray, tang;
	
		for(int i=0; i<n_vert; i++)
		{
			pMesh->getVertex(Q, i);
			pMesh->getNormal(ray , i);
			gridsize = pMesh->getGrid(i);
			Q += ray*gridsize;
			pMesh->getTangent(tang , i);//if(n_vert>10000) cout<<" "<<Q.length();

			b_vis->initialize();
			b_vis->setSpace(Q, ray, tang);
			
			for(TreeList::iterator shd=shadow.begin(); shd != shadow.end(); shd++) (*shd)->lookupNoDist(Q,  ray, b_vis, gridsize);
			
			projectDiffuse(ray, b_vis, tmpCoeff, tmpCoeff1);
			
			for(unsigned int j=0; j<SH_N_BASES; j++) 
			{
				coebf[i*SH_N_BASES+j] = tmpCoeff[j].x;
				coebf1[i*SH_N_BASES+j] = tmpCoeff1[j].x;
			}
		
			if(i%n_bucket ==0) printf("\r%.1f%%", ((float)i/((float)n_vert-1))*100);
		}
		cout<<" \r Done."<<endl;
		
		string coeName = scene->getXMLName();
		string meshName = pMesh->getMeshName();
		zGlobal::replacefilename(coeName, meshName);
		zGlobal::changeFilenameExtension(coeName, "prt");
		zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebf);
		Sleep(1999);
		zGlobal::changeFilenameExtension(coeName, "prtu");
		zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebf1);

		delete[] coebf;
		delete[] coebf1;
	}
}

void SHWorks::saveLightRT(const FXMLScene* scene, const FQSPLAT* light)
{
	float gridsize;
	m_max_buf = light->getNumSplat();
	MeshList list = scene->getMesh();
	TreeList shadow = scene->getShadowTree();
	for(MeshList::iterator it=list.begin(); it != list.end(); ++it)
	{
		cout<<" computing HDR and Lightsource Direct on "<<(*it)->getMeshName()<<endl;
		//cout<<" grid size "<<gridsize<<endl;
		int n_vert = (*it)->getNumVertex();
		cout<<" n vertex "<<n_vert<<endl;
		int n_bucket = n_vert/37;
		
		float* coebf = new float[n_vert*SH_N_BASES];
		float* coebfu = new float[n_vert*SH_N_BASES];
		XYZ* coebfl = new XYZ[n_vert*SH_N_BASES];
		XYZ* coebfj = new XYZ[n_vert*SH_N_BASES];
	
		XYZ Q, ray, tang;
	
		for(int i=0; i<n_vert; i++)
		{
			(*it)->getVertex(Q, i);
			(*it)->getNormal(ray , i);
			gridsize = (*it)->getGrid(i);
			Q += ray*gridsize;
			(*it)->getTangent(tang , i);

			b_vis->initialize();
			b_vis->setSpace(Q, ray, tang);
			
			for(TreeList::iterator shd=shadow.begin(); shd != shadow.end(); shd++) (*shd)->lookupNoDist(Q,  ray, b_vis, gridsize);
			
			projectDiffuse(ray, b_vis, tmpCoeff, tmpCoeff1);
			
			for(unsigned int j=0; j<SH_N_BASES; j++)
			{
				coebf[i*SH_N_BASES+j] = tmpCoeff[j].x;
				coebfu[i*SH_N_BASES+j] = tmpCoeff1[j].x;
			}
			
			b_light_vis->initialize();
			b_light_vis->setSpace(Q, ray, tang);
			light->lookupLight(Q,  ray, b_light_vis);
			
			projectDiffuse(ray, light, b_vis, b_light_vis, tmpCoeff, tmpCoeff2);
		
			for(unsigned int j=0; j<SH_N_BASES; j++)
			{
				coebfl[i*SH_N_BASES+j] = tmpCoeff[j];
				coebfj[i*SH_N_BASES+j] = tmpCoeff2[j];
			}
		
			if(i%n_bucket ==0) printf("\r%.1f%%", ((float)i/((float)n_vert-1))*100);
		}
		cout<<" \r Done."<<endl;
		
		string coeName = scene->getXMLName();
		string meshName = (*it)->getMeshName();
		zGlobal::replacefilename(coeName, meshName);
		zGlobal::changeFilenameExtension(coeName, "prt");
		zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebf);
		Sleep(1999);
		zGlobal::changeFilenameExtension(coeName, "prtu");
		zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebfu);
		Sleep(1999);
		zGlobal::changeFilenameExtension(coeName, "prtl");
		zGlobal::saveFloatFile(coeName.c_str(), n_vert*3*SH_N_BASES, (float*)coebfl);
		Sleep(1999);
		zGlobal::changeFilenameExtension(coeName, "prtj");
		zGlobal::saveFloatFile(coeName.c_str(), n_vert*3*SH_N_BASES, (float*)coebfj);

		delete[] coebfl;
		delete[] coebfj;
	}
}

void SHWorks::saveScat(const FXMLScene* scene, int n_cell, int iscat, int ibackscat)
{
	float gridsize;
	MeshList list = scene->getMesh();
	for(MeshList::iterator it=list.begin(); it != list.end(); ++it)
	{
		if((*it)->hasAttrib("skipScatter")==1 && (*it)->hasAttrib("skipBackscatter")==1) cout<<" skip Sub-Surface and back-light Scatter on "<<(*it)->getMeshName()<<endl;
		else if( iscat==1 && ibackscat==0 && (*it)->hasAttrib("skipScatter")==1) cout<<" skip Sub-Surface scatter on "<<(*it)->getMeshName()<<endl;
		else if( iscat==0 && ibackscat==1 && (*it)->hasAttrib("skipBackscatter")==1) cout<<" skip back-light Scatter on "<<(*it)->getMeshName()<<endl;
		else
		{
			FQSPLAT* tree = new FQSPLAT();
			int estimate_ncell = (*it)->getNumTriangle()*n_cell*2;
			estimate_ncell += estimate_ncell/9;
	
			pcdSample* samp = new pcdSample[estimate_ncell];
			CoeRec* coe_samp = new CoeRec[estimate_ncell];
			int n_samp = 0;
	
			float epsilon = sqrt((*it)->getArea()/(*it)->getNumTriangle()/2/n_cell);
			(*it)->diceWithRT(epsilon, samp, coe_samp, n_samp);

			cout<<" n samples: "<<n_samp<<endl;
			tree->createRT(samp, coe_samp, n_samp);
			delete[] samp;
			delete[] coe_samp;
	
			if(iscat==1 && (*it)->hasAttrib("skipScatter")!=1)
			{
				cout<<" computing Sub-Surface Scatter on "<<(*it)->getMeshName()<<endl;
				
				//cout<<" grid size "<<gridsize<<endl;
				int n_vert = (*it)->getNumVertex();
				cout<<" n vertex "<<n_vert<<endl;
				int n_bucket = n_vert/37;
				
				float* coebf = new float[n_vert*SH_N_BASES];
				float* coebf1 = new float[n_vert*SH_N_BASES];
			
				XYZ Q, ray, tang;
				
				float glb_gridsize = sqrt((*it)->getArea()/(*it)->getNumTriangle())/2;
			
				for(int i=0; i<n_vert; i++)
				{
					(*it)->getVertex(Q, i);
					(*it)->getNormal(ray , i);
					gridsize = ((*it)->getGrid(i) + glb_gridsize)/2;
					Q -= ray*gridsize;
					(*it)->getTangent(tang , i);

					b_vis->initialize();
					b_vis->setSpace(Q, ray, tang);
					
					tree->lookupSubsurface(Q,  ray, b_vis, gridsize);
					
					m_max_buf = tree->getNumSplat();
					projectSubSurfaceScatter(ray, tree, b_vis, tmpCoeff, tmpCoeff1);
					
					for(unsigned int j=0; j<SH_N_BASES; j++) 
					{
						coebf[i*SH_N_BASES+j] = tmpCoeff[j].x;
						coebf1[i*SH_N_BASES+j] = tmpCoeff1[j].x;
					}
				
					if(i%n_bucket ==0) printf("\r%.1f%%", ((float)i/((float)n_vert-1))*100);
				}
				cout<<" \r Done."<<endl;
				
				string coeName = scene->getXMLName();
				string meshName = (*it)->getMeshName();
				zGlobal::replacefilename(coeName, meshName);
				zGlobal::changeFilenameExtension(coeName, "prts");
				zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebf);
				Sleep(1999);
				zGlobal::changeFilenameExtension(coeName, "prte");
				zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebf1);

				delete[] coebf;
				delete[] coebf1;
			}
			else cout<<" skip Sub-Surface scatter on "<<(*it)->getMeshName()<<endl;
			
			if(ibackscat==1 && (*it)->hasAttrib("skipBackscatter")!=1)
			{
	cout<<" computing Back-light Scatter on "<<(*it)->getMeshName()<<endl;
				XYZ planez, planex;
				scene->getSpaceZ(planez);
				scene->getSpaceX(planex);
				cout<<" back light source: "<<planez.x<<" "<<planez.y<<" "<<planez.z<<endl;
				//cout<<" grid size "<<gridsize<<endl;
				int n_vert = (*it)->getNumVertex();
				cout<<" n vertex "<<n_vert<<endl;
				int n_bucket = n_vert/37;
				
				float* coebf = new float[n_vert*SH_N_BASES];
			
				XYZ Q, ray, tang;
				
				float glb_gridsize = sqrt((*it)->getArea()/(*it)->getNumTriangle())/2;
			
				for(int i=0; i<n_vert; i++)
				{
					(*it)->getVertex(Q, i);
					(*it)->getNormal(ray , i);
					gridsize = ((*it)->getGrid(i) + glb_gridsize)/2;
					Q -= ray*gridsize/3;

					b_vis->initialize();
					b_vis->setSpace(Q, planez, planex);
					
					tree->lookupSubsurfaceB(Q, planez, b_vis, gridsize);
					
					m_max_buf = tree->getNumSplat();
					projectAbsorbSubSurfaceScatter(planez, tree, b_vis, gridsize, tmpCoeff);

					for(unsigned int j=0; j<SH_N_BASES; j++) coebf[i*SH_N_BASES+j] = tmpCoeff[j].x;
				
					if(i%n_bucket ==0) printf("\r%.1f%%", ((float)i/((float)n_vert-1))*100);
				}
				cout<<" \r Done."<<endl;
				
				string coeName = scene->getXMLName();
				string meshName = (*it)->getMeshName();
				zGlobal::replacefilename(coeName, meshName);
				zGlobal::changeFilenameExtension(coeName, "prtb");
				zGlobal::saveFloatFile(coeName.c_str(), n_vert*SH_N_BASES, coebf);

				delete[] coebf;
			}
			else cout<<" skip back-light scatter on "<<(*it)->getMeshName()<<endl;
			
			delete tree;
		}
	}
}

void SHWorks::saveIndirectRT(const FXMLScene* scene)
{
	float gridsize;
	MeshList list = scene->getMesh();
	//TreeList shadow = scene->getShadowTree();
	const FQSPLAT* shadow = scene->getRTTree();
	m_max_buf = shadow->getNumSplat();
	for(MeshList::iterator it=list.begin(); it != list.end(); ++it)
	{
		if((*it)->hasAttrib("skipIndirect")==1) cout<<" skip Interreflection on "<<(*it)->getMeshName()<<endl;
		else
		{
			cout<<" computing Interreflection on "<<(*it)->getMeshName()<<endl;
			//cout<<" grid size "<<gridsize<<endl;
			int n_vert = (*it)->getNumVertex();
			cout<<" n vertex "<<n_vert<<endl;
			int n_bucket = n_vert/37;
			
			XYZ* coebf = new XYZ[n_vert*SH_N_BASES];
		
			XYZ Q, ray, tang;
		
			for(int i=0; i<n_vert; i++)
			{
				(*it)->getVertex(Q, i);
				(*it)->getNormal(ray , i);
				gridsize = (*it)->getGrid(i);
				Q += ray*gridsize;
				(*it)->getTangent(tang , i);
				
				f_sh->zeroCoeff(tmpCoeff);
				//for(TreeList::iterator shd=shadow.begin(); shd != shadow.end(); shd++) 
				//{
					b_vis->initialize();
					b_vis->setSpace(Q, ray, tang);
					shadow->lookupInterreflect(Q,  ray, b_vis);
					
					projectInterreflect(ray, shadow, b_vis, tmpCoeff);
				//}
				
				for(unsigned int j=0; j<SH_N_BASES; j++) 
				{
					coebf[i*SH_N_BASES+j] = tmpCoeff[j];
				}
			
				if(i%n_bucket ==0) printf("\r%.1f%%", ((float)i/((float)n_vert-1))*100);
			}
			cout<<" \r Done."<<endl;
			
			string coeName = scene->getXMLName();
			string meshName = (*it)->getMeshName();
			zGlobal::replacefilename(coeName, meshName);
			zGlobal::changeFilenameExtension(coeName, "prti");
			zGlobal::saveFloatFile(coeName.c_str(), n_vert*3*SH_N_BASES, (float*)coebf);

			delete[] coebf;
		}
	}
}

void SHWorks::printHDRCoeff() const
{
	printf("HDR coeff\n");
	for(int i=0; i<SH_N_BASES; i++) printf("%f %f %f\n", hdrCoeff[i].x, hdrCoeff[i].y, hdrCoeff[i].z);
}

void SHWorks::printConstantCoeff() const
{
	printf("Constant coeff\n");
	for(int i=0; i<SH_N_BASES; i++) printf("%f %f %f\n", constantCoeff[i].x, constantCoeff[i].y, constantCoeff[i].z);
}

void SHWorks::cheat(const FXMLScene* scene, int lo, int hi, int frame)
{
	float wei1 = 1.f;
	if(hi != lo) wei1 = float(frame - lo)/float(hi - lo);
	
	MeshList list = scene->getMesh();
	for(MeshList::iterator it=list.begin(); it != list.end(); ++it) {
		if((*it)->hasAttrib("asLightsource") !=1 && (*it)->hasAttrib("invisible") !=1) {
			cout<<" cheat "<<(*it)->getMeshName()<<" at frame "<<frame<<endl;
			
			(*it)->interpolateRT(lo, hi, frame, wei1, "prt", 16);
			(*it)->interpolateRT(lo, hi, frame, wei1, "prtu", 16);
			(*it)->interpolateRT(lo, hi, frame, wei1, "prtl", 48);
			(*it)->interpolateRT(lo, hi, frame, wei1, "prtj", 48);
			
			if((*it)->hasAttrib("skipScatter") !=1) {
				(*it)->interpolateRT(lo, hi, frame, wei1, "prts", 16);
				(*it)->interpolateRT(lo, hi, frame, wei1, "prte", 16);
			}
			
			if((*it)->hasAttrib("skipBackscatter") !=1) (*it)->interpolateRT(lo, hi, frame, wei1, "prtb", 16);
			
			if((*it)->hasAttrib("skipIndirect") !=1) (*it)->interpolateRT(lo, hi, frame, wei1, "prti", 48);
		}
	}
}
//:~