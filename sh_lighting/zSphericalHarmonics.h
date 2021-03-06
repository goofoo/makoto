#ifndef _ZSPHERICALHARMONICS_H
#define _ZSPHERICALHARMONICS_H
#include "../shared/zData.h"
#include "../shared/zMath.h"
#include "../shared/gBase.h"
#define SH_N_BANDS 4
#define SH_N_BASES 16
#define SH_N_SAMPLES 6400
#define SH_N_SQRT_SAMPLES 80
/* 4*PI/n/n */
#define SH_UNITAREA 0.0019625
#include "SHMath.h"

//#define SH_DEBUG

struct SPHBufPix
{
	SPHBufPix() {d= 10e8;}
	XYZ pos, nor;
	float d, facing, k;
	char is_frontFace;
	int id;
};

struct SHSample
{
	SHSample() {}
	SHSample(const double _theta, const double _phi)
	{
		theta = _theta;
		phi = _phi;
		
		vec[0] = sin(theta) * cos(phi);
		vec[1] = sin(theta) * sin(phi);
		vec[2] = cos(theta);
		
		vector = XYZ(vec[0], vec[1], vec[2]);
	}
	
	void calcCoe()
	{
		for(int l=0; l<SH_N_BANDS; ++l) 
		{ 
			for(int m=-l; m<=l; ++m) 
			{ 
				int index = l*(l+1)+m; 
				coe[index] = SHMath::SH(l,m, theta, phi);
			}
		}
	}
	
	double vec[3];
	double theta, phi;
	double coe[SH_N_BASES];
	XYZ vector;
};

struct SHFunction
{
	virtual float getFloat(const SHSample& s) const = 0;
	
	virtual XYZ getColor(const SHSample& s) const = 0;
	
	XYZ getVector(const float u, const float v) const
	{
		XYZ res;
		
		SHSample s(u,v);
#ifdef SH_DEBUG
		s.calcCoe();
#endif
		float r = fabs(getFloat(s));
		
		res.x = r*s.vec[0];
		res.y = r*s.vec[1];
		res.z = r*s.vec[2];
		return res;
	}
};

class sphericalHarmonics
{
public:
	sphericalHarmonics(void);
	~sphericalHarmonics(void);
	
	void projectFunction(SHFunction& func, float* coeffs);
	void projectFunction(SHFunction& func, XYZ* coeffs);
	
	void reconstructAndDraw(float* coeffs);
	void reconstructAndDraw(XYZ* coeffs);
	void drawSHFunction(const SHFunction& func) const;
	void drawSamples();
	void drawBands();
	XYZ integrate(XYZ* coeffs0, XYZ* coeffs1);
	float rIntegrate(XYZ* coeffs0, XYZ* coeffs1);

	void rotateSH(XYZ* originalCoeff, XYZ* resultCoeff, double theta, double phi);

	void copyCoeff(float* from, float* to);
	void copyCoeff(XYZ* from, XYZ* to);
	void addCoeff(XYZ* a, XYZ* b, const float scale);
	void addCoeff(XYZ* a, float* b, const float scale);
	void addCoeff(XYZ* a, float* b, const XYZ& filter);
	void addCoeff(XYZ* a, XYZ* b, const XYZ& filter);
	void zeroCoeff(XYZ* coeffs) const;
	
	char isCloseTo(const XYZ& ray, const float threshold, unsigned int id) const;
	
	void projectASample(XYZ* coeffs, unsigned int id, float val) const;
	void projectASample(XYZ* coeffs, unsigned int id, const XYZ& col) const;
	void projectASample(XYZ* coeffs, const SHSample& samp, const XYZ& col) const;
	void projectASample(XYZ* coeffs, const SHSample& samp, const float val) const;
	void projectASample(float* coeffs, const SHSample& samp, const float val) const;
	void projectASample(XYZ* coeffs, unsigned int id);

	double getDx(unsigned int id) const {return dx[id];}
	void findGrid(const XYZ& ray, double& theta, double& phi, double& dgrid_x, double& dgrid_y) const
	{
		xyz2sph(ray, theta, phi);
		
		double cc = cos(theta/2.0);
		double x = 1.0 - cc*cc;
		double y = phi/2.0/PI;
		
		dgrid_x = x*SH_N_SQRT_SAMPLES;
		dgrid_y = y*SH_N_SQRT_SAMPLES;
	}
	
	void processBuf(char* buf, const XYZ& ray, const double solid_angle) const;
	void processBuf(SPHBufPix* buf, const XYZ& ray, const double solid_angle, const float distance, const int nodeId, const char is_frontFace, const float facing, const float k) const;

	void diffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, float* tmpCoeff, const XYZ& normal) const;
	void rgbDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const;
	void rDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const;
	void gDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const;
	void bDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const;
	
	void setupSampleStratified(SHSample* samp, const int n);
	void setupSampleStratified(SHSample* samp, double* dx, double* dy, const int n);
	
	SHSample& getSample(const int id) const {return m_pSample[id];}
	
private:
	SHSample* m_pSample;
	double* dx;
	double* dy;
	double redCoeff[SH_N_BASES];
	double greenCoeff[SH_N_BASES];
	double blueCoeff[SH_N_BASES];
	double redCoeffRes[SH_N_BASES];
	double greenCoeffRes[SH_N_BASES];
	double blueCoeffRes[SH_N_BASES];
};
#endif
