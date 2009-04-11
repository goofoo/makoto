#ifndef Z_SHWORKS_H
#define Z_SHWORKS_H
#include "FQSPLAT.h"
#include "../shared/FXMLMesh.h"
#include "zSphericalHarmonics.h"
#include "VisibilityBuffer.h"
#include "FTriangle.h"
//#include "FTriangleMesh.h"
#include "../shared/hdrtexture.h"

#define SHW_NUM_RECON_SAMPLE 196
#define SHW_SQRT_NUM_RECON_SAMPLE 14
#define SHW_UNIT_RECON_SAMPLE 0.064114135

class FXMLScene;

class SHWorks
{
public:
	SHWorks(void);
	~SHWorks(void);

/*Calculate RT on trianglated scene */
	void saveRT(const FXMLScene* scene);
	void saveLightRT(const FXMLScene* scene, const FQSPLAT* light);
	void saveIndirectRT(const FXMLScene* scene);
	void saveScat(const FXMLScene* scene, int n_cell, int iscat, int ibackscat);

	void computeHDRCoeff(const HDRtexture* hdrimg);
	void computeHDRCoeffNoRotate(const HDRtexture* hdrimg);
	//void computeSHLighting(FXMLMesh* mesh);

	XYZ computeSHLighting(XYZ* coeff);
	XYZ computeSHLighting(XYZ* coeff, const XYZ& normal);
	XYZ computeSrcSHLighting(XYZ* coeff);
	float computeSrcSHLighting(XYZ* coeff, const XYZ& normal);
	XYZ computeSrcSHLighting(XYZ* coeff, const XYZ& normal, const int channel);
	XYZ computeSHLighting(XYZ* srcCoeff, XYZ* shdCoeff, const XYZ& normal);

/*Using new visibility buffer*/
	void projectInterreflect(const XYZ& ray, const FQSPLAT* tree, const CVisibilityBuffer* visibility, XYZ* coeff);
	void projectSubSurfaceScatter(const XYZ& ray, const FQSPLAT* tree, const CVisibilityBuffer* visibility, XYZ* coeff0, XYZ* coeff1);
	void projectAbsorbSubSurfaceScatter(const XYZ& ray, const FQSPLAT* tree, const CVisibilityBuffer* visibility, float grid, XYZ* coeff);
	void projectDiffuse(const XYZ& ray, const FQSPLAT* light, const CVisibilityBuffer* shdowVisibility, const CVisibilityBuffer* lightVisibility, XYZ* coeff0, XYZ* coeff2);
	void projectDiffuse(const XYZ& ray, const CVisibilityBuffer* visibility, XYZ* coeff0, XYZ* coeff1);
	
	void changeTheta(const float val) 
	{ 
		//double old_v = m_theta2pi;
		m_theta2pi += double(val); 
		if(m_theta2pi<0.0) m_theta= 2*PI+m_theta2pi; 
			else if(m_theta2pi>2*PI) m_theta2pi= m_theta2pi - 2*PI;

		if(m_theta2pi < PI) m_theta = m_theta2pi;
		else 
		{
			m_theta = m_theta2pi - PI;
		}
	}
	void changePhi(const float val) 
	{ 
		m_phi += double(val); 
		if(m_phi<0.0) m_phi= PI*2+m_phi; 
			else if(m_phi>PI*2) m_phi= m_phi - PI*2; 
	}
	void resetHDR()
	{
		m_theta2pi = m_phi = m_theta = 0.0;
	}
	
	double getTheta() { return m_theta;}
	double getPhi() { return m_phi;}
	void drawHDR(const HDRtexture* hdrimg);

	void 	setDrawType(const int type)
	{
		m_draw_type = type;
	}
	int getDrawType() {return m_draw_type;}
	
	void SHRotate(XYZ* coeff, XYZ* coeff_res, const double theta, const double phi);
	void diffuseTransfer(XYZ* coeff_in, XYZ* coeff_out, const XYZ& normal);
	void diffuseTransfer(XYZ* coeff_in, XYZ* coeff_out, const XYZ& normal, const int channel);
	void rgbDiffuseTransfer(XYZ* coeff_in, XYZ* coeff_out, const XYZ& normal);
	
// show sample points 
	void showSamples();
/*Show visibility*/
	void show(const CVisibilityBuffer* visibility);
/*set use lightsource*/
	void setUseLightsource(int val) {m_use_lightsource = val;}
	
	void printHDRCoeff() const;
	void printConstantCoeff() const;
	
private:
	sphericalHarmonics* f_sh;
	CVisibilityBuffer* b_vis;
	//CVisibilityBuffer* b_bone_vis;
	CVisibilityBuffer* b_light_vis;
	SHSample* recon_samp;
	XYZ tmpCoeff[SH_N_BASES];
	XYZ tmpCoeff1[SH_N_BASES];
	XYZ tmpCoeff2[SH_N_BASES];
	//XYZ tmpCoeff3[SH_N_BASES];
	//XYZ tmpCoeff4[SH_N_BASES];
	XYZ diffuseCoeff[SH_N_BASES];
	XYZ hdrCoeff[SH_N_BASES];
	XYZ constantCoeff[SH_N_BASES];
	XYZ rtCoeff[SH_N_BASES];
	float redCoeff[SH_N_BASES];
	double m_theta, m_phi, m_theta2pi;

/*max count of buf*/
	int m_max_buf;

	int m_draw_type;
	int m_use_lightsource;
};
#endif