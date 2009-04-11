#include "zsphericalharmonics.h"
#include "SH Rotation/SH Rotation.h"
#include "../shared/zNoise.h"
#include <iostream>
#include <fstream>
using namespace std;

sphericalHarmonics::sphericalHarmonics(void):m_pSample(0)
{
	m_pSample = new SHSample[SH_N_SAMPLES];
	dx = new double[SH_N_SAMPLES];
	dy = new double[SH_N_SAMPLES];
	setupSampleStratified(m_pSample, dx, dy, SH_N_SQRT_SAMPLES);
}

sphericalHarmonics::~sphericalHarmonics(void)
{
	if(m_pSample) delete[] m_pSample;
		if(dx) delete[] dx;
}

void sphericalHarmonics::setupSampleStratified(SHSample* samp, const int n)
{
	double oneovern = 1.0f/n;
	for(int j=0, k=0; j<n; j++)
	{
		for(int i=0; i<n; i++, k++)
		{
			//double x = (i+double(rand()% 173)/173.f) * oneovern;
			//double y = (j+double(rand()% 191)/191.f) * oneovern;
			double x = (i+randfint(k)) * oneovern;
			double y = (j+randfint(k)) * oneovern;
			double theta = 2.0f *acos(sqrt(1.0f - x));
			double phi = 2.0f *PI* y;
			samp[k] = SHSample(theta, phi);
			samp[k].calcCoe();
		}
	}
}

void sphericalHarmonics::setupSampleStratified(SHSample* samp, double* dx, double* dy, const int n)
{
	double oneovern = 1.0f/n;
	for(int j=0, k=0; j<n; j++)
	{
		for(int i=0; i<n; i++, k++)
		{
			double x = (i+double(rand()% 173)/173.f) * oneovern;
			double y = (j+double(rand()% 191)/191.f) * oneovern;
			double theta = 2.0f *acos(sqrt(1.0f - x));
			double phi = 2.0f *PI* y;
			samp[k] = SHSample(theta, phi);
			samp[k].calcCoe();
			
			double theta0 = 2.0f *acos(sqrt(1.0f - (i)* oneovern));
			double theta1 = 2.0f *acos(sqrt(1.0f - (i+1)* oneovern));
			
			dx[k] = tan((theta1-theta0)/2);
			dy[k] = tan(PI/SH_N_SQRT_SAMPLES)*(sin((theta0+theta1)/2)+0.00005);
		}
	}
}

struct reconstructSH : public SHFunction
{
	reconstructSH(const float* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i];
	}
	
	reconstructSH(const XYZ* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i].x;
	}
	
	float getFloat(const SHSample& s) const
	{
		float value = 0;

		for(int i=0; i<SH_N_BASES;i++) value += coeffs[i] *s.coe[i];

		return (value);
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r = getFloat(s);
		return XYZ(r,r,r);
	}
	
	float coeffs[SH_N_BASES];
};

struct rReconstructSH : public SHFunction
{
	rReconstructSH(const XYZ* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i].x;
	}
	
	float getFloat(const SHSample& s) const
	{
		float value = 0;

		for(int i=0; i<SH_N_BASES;i++) value += coeffs[i] *s.coe[i];

		return (value);
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r = getFloat(s);
		return XYZ(r);
	}
	
	float coeffs[SH_N_BASES];
};

struct gReconstructSH : public SHFunction
{
	gReconstructSH(const XYZ* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i].y;
	}
	
	float getFloat(const SHSample& s) const
	{
		float value = 0;

		for(int i=0; i<SH_N_BASES;i++) value += coeffs[i] *s.coe[i];

		return (value);
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r = getFloat(s);
		return XYZ(r);
	}
	
	float coeffs[SH_N_BASES];
};

struct bReconstructSH : public SHFunction
{
	bReconstructSH(const XYZ* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i].z;
	}
	
	float getFloat(const SHSample& s) const
	{
		float value = 0;

		for(int i=0; i<SH_N_BASES;i++) value += coeffs[i] *s.coe[i];

		return (value);
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r = getFloat(s);
		return XYZ(r);
	}
	
	float coeffs[SH_N_BASES];
};

struct rgbReconstructSH : public SHFunction
{
	rgbReconstructSH(const XYZ* _coeffs)
	{
		for (int i = 0; i < SH_N_BASES; i++)
				coeffs[i] = _coeffs[i];
	}
	
	float getFloat(const SHSample& s) const
	{
		return 1.0f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		XYZ value(0,0,0);
		
		for(int i=0; i<SH_N_BASES;i++) value += coeffs[i] *s.coe[i];

		return (value);
	}
	
	XYZ coeffs[SH_N_BASES];
};

struct SHSphere : public SHFunction
{
	SHSphere(const int _l, const int _m): l(_l), m(_m) { }
	
	float getFloat(const SHSample& s) const
	{
		return (SHMath::SH(l, m, s.theta, s.phi));
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r = getFloat(s);
		if(r>0)
			return XYZ(0,r,0);
		else
			return XYZ(-r,0,0);
	}
	
	int l, m;
};

void sphericalHarmonics::projectFunction(SHFunction& func, float* coeffs)
{
	for (int i = 0; i < SH_N_BASES; i++)
			coeffs[i] = 0;

	for (int i = 0; i < SH_N_SAMPLES; i++)
	{
			float sample = func.getFloat(m_pSample[i]);

			// Project the sample onto each SH basis
				for (int j = 0; j < SH_N_BASES; j++)
					coeffs[j] += sample * m_pSample[i].coe[j];
	}

	// Divide each coefficient by number of samples and multiply by weights
	for (int i = 0; i < SH_N_BASES; i++)
		coeffs[i] *= SH_UNITAREA;
}

void sphericalHarmonics::projectFunction(SHFunction& func, XYZ* coeffs)
{
	for (int i = 0; i < SH_N_BASES; i++)
			coeffs[i] = XYZ(0,0,0);
	
	for (int i = 0; i < SH_N_SAMPLES; i++)
	{
		XYZ tmp = func.getColor(m_pSample[i]);

			// Project the sample onto each SH basis
				for (int j = 0; j < SH_N_BASES; j++)
					coeffs[j] += tmp * m_pSample[i].coe[j];
	}

	// Divide each coefficient by number of samples and multiply by weights
	for (int i = 0; i < SH_N_BASES; i++)
	{
		coeffs[i] *= SH_UNITAREA;
		//cout<<" "<<coeffs[i].x<<" "<<coeffs[i].y<<" "<<coeffs[i].z;
	}
}

void sphericalHarmonics::drawSamples()
{
	glBegin(GL_POINTS);
	for(int i=0; i<SH_N_SAMPLES; i++)
		glVertex3f((float)m_pSample[i].vec[0], (float)m_pSample[i].vec[1], (float)m_pSample[i].vec[2]);
	glEnd();
}


void sphericalHarmonics::drawSHFunction(const SHFunction& func) const
{
	int n = 64;
	double du = PI / (n+1);
	double dv = 2 * PI/ (n+1);
	//float r; 
	XYZ col;
	glBegin(GL_QUADS);
	for (double v = 0; v < 2 * PI; v += dv)
	{
		for (double u = 0; u < PI; u += du)
		{
			XYZ p[4] =
			{
				func.getVector(u, v),
					func.getVector(u + du, v),
					func.getVector(u + du, v + dv),
					func.getVector(u, v + dv)
			};
			
			SHSample s00(u,v);
			SHSample s10(u+du,v);
			SHSample s01(u,v+dv);
			SHSample s11(u+du,v+dv);
#ifdef SH_DEBUG			
			s00.calcCoe();
			s10.calcCoe();
			s01.calcCoe();
			s11.calcCoe();
#endif
			//r = func.getFloat(s01);
			col = func.getColor(s01);
			
			glColor3f(col.x, col.y, col.z);
			glVertex3f(p[3].x, p[3].y, p[3].z);
			
			//r = func.getFloat(s11);
			col = func.getColor(s11);
			
			glColor3f(col.x, col.y, col.z);
			glVertex3f(p[2].x, p[2].y, p[2].z);
			
			//r = func.getFloat(s10);
			col = func.getColor(s10);
			
			glColor3f(col.x, col.y, col.z);
			glVertex3f(p[1].x, p[1].y, p[1].z);
			
			//r = func.getFloat(s00);
			col = func.getColor(s00);
			
			glColor3f(col.x, col.y, col.z);
			glVertex3f(p[0].x, p[0].y, p[0].z);
		}
	}
	glEnd();
}

void sphericalHarmonics::drawBands()
{
	for (int l = 0; l < SH_N_BANDS; l++)
		for (int m = -l; m <= l; m++)
		{
			SHSphere sphere(l,m);
			
			glPushMatrix();
			glTranslatef(m*2.f, l*-2.f, 0);
			
			drawSHFunction(sphere);
			
			glPopMatrix();
		}
}

void sphericalHarmonics::reconstructAndDraw(float* coeffs)
{
	
	
	//cout<<" "<<m_exampleCoeffs[0]<<endl;
	//cout<<" "<<m_exampleCoeffs[1]<<" "<<m_exampleCoeffs[2]<<" "<<m_exampleCoeffs[3]<<endl;
	//cout<<" "<<m_exampleCoeffs[4]<<" "<<m_exampleCoeffs[5]<<" "<<m_exampleCoeffs[6]<<" "<<m_exampleCoeffs[7]<<" "<<m_exampleCoeffs[8]<<endl;
	//cout<<" "<<m_exampleCoeffs[9]<<" "<<m_exampleCoeffs[10]<<" "<<m_exampleCoeffs[11]<<" "<<m_exampleCoeffs[12]<<" "<<m_exampleCoeffs[13]<<" "<<m_exampleCoeffs[14]<<" "<<m_exampleCoeffs[15]<<endl;
	
	reconstructSH rsh(coeffs);
	
	
	
	//glPushMatrix();
	//glTranslatef(1, 0, 0);
	drawSHFunction(rsh);
	//glPopMatrix();

}

void sphericalHarmonics::reconstructAndDraw(XYZ* coeffs)
{
	rgbReconstructSH rsh(coeffs);
	
	drawSHFunction(rsh);
}

XYZ sphericalHarmonics::integrate(XYZ* coeffs0, XYZ* coeffs1)
{
	XYZ value(0);

		// Calculate a weighted sum of all the SH bases
		for (int i = 0; i < SH_N_BASES; i++)
		{
			value.x += coeffs0[i].x * coeffs1[i].x;
			value.y += coeffs0[i].y * coeffs1[i].y;
			value.z += coeffs0[i].z * coeffs1[i].z;
		}

		return (value);
}

float sphericalHarmonics::rIntegrate(XYZ* coeffs0, XYZ* coeffs1)
{
	float value = 0;
	for (int i = 0; i < SH_N_BASES; i++) value += coeffs0[i].x * coeffs1[i].x;
	return value;
}

void sphericalHarmonics::rotateSH(XYZ* originalCoeff, XYZ* resultCoeff, double theta, double phi)
{
	
	for (int i = 0; i < SH_N_BASES; i++) 
	{
		redCoeff[i]  = (double) originalCoeff[i].x;
		greenCoeff[i]  = (double) originalCoeff[i].y;
		blueCoeff[i]  = (double) originalCoeff[i].z;
	}
		
	RotateSHCoefficients(SH_N_BANDS, redCoeff, redCoeffRes, theta, phi);
	RotateSHCoefficients(SH_N_BANDS, greenCoeff, greenCoeffRes, theta, phi);
	RotateSHCoefficients(SH_N_BANDS, blueCoeff, blueCoeffRes, theta, phi);
	
	for (int i = 0; i < SH_N_BASES; i++) 
	{
		resultCoeff[i].x = (float)redCoeffRes[i];
		resultCoeff[i].y = (float)greenCoeffRes[i];
		resultCoeff[i].z = (float)blueCoeffRes[i];
	}	
}

void sphericalHarmonics::copyCoeff(float* from, float* to)
{
	for (int j = 0; j < SH_N_BASES; j++) to[j] = from[j];
}

void sphericalHarmonics::copyCoeff(XYZ* from, XYZ* to)
{
	for (int j = 0; j < SH_N_BASES; j++) to[j] = from[j];
}

void sphericalHarmonics::addCoeff(XYZ* a, XYZ* b, const float scale)
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		a[j].x += b[j].x * scale;
		a[j].y += b[j].y * scale;
		a[j].z += b[j].z * scale;
	}
}

void sphericalHarmonics::addCoeff(XYZ* a, float* b, const float scale)
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		a[j].x += b[j] * scale;
		a[j].y += b[j] * scale;
		a[j].z += b[j] * scale;
	}
}

void sphericalHarmonics::addCoeff(XYZ* a, float* b, const XYZ& filter)
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		a[j].x += b[j] * filter.x;
		a[j].y += b[j] * filter.y;
		a[j].z += b[j] * filter.z;
	}
}

void sphericalHarmonics::addCoeff(XYZ* a, XYZ* b, const XYZ& filter)
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		a[j].x += b[j].x * filter.x;
		a[j].y += b[j].y * filter.y;
		a[j].z += b[j].z * filter.z;
	}
}

void sphericalHarmonics::zeroCoeff(XYZ* coeffs) const
{
	for(unsigned int j=0; j<SH_N_BASES; j++) coeffs[j] = XYZ(0,0,0);
}

void sphericalHarmonics::projectASample(XYZ* coeffs, unsigned int id, float val) const
{
	// Project the sample onto each SH basis
	for (int j = 0; j < SH_N_BASES; j++)
	{
		coeffs[j].x += val * m_pSample[id].coe[j] * SH_UNITAREA;
		coeffs[j].y += val * m_pSample[id].coe[j] * SH_UNITAREA;
		coeffs[j].z += val * m_pSample[id].coe[j] * SH_UNITAREA;
	}
}

void sphericalHarmonics::projectASample(XYZ* coeffs, unsigned int id, const XYZ& col) const
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		coeffs[j].x += col.x * m_pSample[id].coe[j] * SH_UNITAREA;
		coeffs[j].y += col.y * m_pSample[id].coe[j] * SH_UNITAREA;
		coeffs[j].z += col.z * m_pSample[id].coe[j] * SH_UNITAREA;
	}
}

void sphericalHarmonics::projectASample(XYZ* coeffs, unsigned int id)
{
	// Project the sample onto each SH basis
	for (int j = 0; j < SH_N_BASES; j++)
	{
		coeffs[j].x += m_pSample[id].coe[j] * SH_UNITAREA;
		coeffs[j].y += m_pSample[id].coe[j] * SH_UNITAREA;
		coeffs[j].z += m_pSample[id].coe[j] * SH_UNITAREA;
	}
}

void sphericalHarmonics::projectASample(XYZ* coeffs, const SHSample& samp, const XYZ& col) const
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		coeffs[j].x += col.x * samp.coe[j];
		coeffs[j].y += col.y * samp.coe[j];
		coeffs[j].z += col.z * samp.coe[j];
	}
}

void sphericalHarmonics::projectASample(XYZ* coeffs, const SHSample& samp, const float val) const
{
	for (int j = 0; j < SH_N_BASES; j++)
	{
		coeffs[j].x += val * samp.coe[j];
		coeffs[j].y += val * samp.coe[j];
		coeffs[j].z += val * samp.coe[j];
	}
}

void sphericalHarmonics::projectASample(float* coeffs, const SHSample& samp, const float val) const
{
	for (int j = 0; j < SH_N_BASES; j++)
		coeffs[j] += val * samp.coe[j];
}

char sphericalHarmonics::isCloseTo(const XYZ& ray, const float threshold, unsigned int id) const
{
	SHSample& s = getSample(id);
	float c = ray.dot(s.vector);
	float t = sqrt(1-c*c)/c;
	if(t >threshold) return 0;
	return 1;
}

void sphericalHarmonics::processBuf(char* buf, const XYZ& ray, const double solid_angle) const
{
	double theta, phi;
	xyz2sph(ray, theta, phi);
		
		double cc = cos(theta/2.0);
		double x = 1.0 - cc*cc;
		double y = phi/2.0/PI;
		
		double dgridX = x*SH_N_SQRT_SAMPLES;
		double dgridY = y*SH_N_SQRT_SAMPLES;
	
	int gridX = dgridX;
	int gridY = dgridY;
	
	int id = gridY*SH_N_SQRT_SAMPLES+gridX;
	
		double vx = solid_angle/dx[id]/2;

		int ivx = vx;
			
		double vy = solid_angle/dy[id]/2;
		int ivy = vy;

		if(ivy>SH_N_SQRT_SAMPLES/2) ivy= SH_N_SQRT_SAMPLES/2;

		int xstart = gridX-ivx;
		int xend = gridX+ivx;
		
			//if(dgridX-gridX<0.5) xstart--;
			//else xend++;
	
		int ystart = gridY-ivy;
		int yend = gridY+ivy;

			//if(dgridY-gridY<0.5) ystart--;
			//else yend++;

			if(xstart<0) xstart =0;
				if(xend>SH_N_SQRT_SAMPLES-1) xend = SH_N_SQRT_SAMPLES-1;
	
	for(int i=xstart; i<=xend; i++ )
	{
		for(int j=ystart; j<=yend; j++ )
		{
					
			int yy = j;
			if(yy<0) yy = SH_N_SQRT_SAMPLES+yy;
			else if(yy>=SH_N_SQRT_SAMPLES) yy = yy - SH_N_SQRT_SAMPLES;
			
			id = yy*SH_N_SQRT_SAMPLES+i;
			
			buf[id] = 0;
			
		}
	}
}

void sphericalHarmonics::processBuf(SPHBufPix* buf, const XYZ& ray, const double solid_angle, const float distance, const int nodeId, const char is_frontFace, const float facing, const float k) const
{
		double theta, phi;
	xyz2sph(ray, theta, phi);
		
		double cc = cos(theta/2.0);
		double x = 1.0 - cc*cc;
		double y = phi/2.0/PI;
		
		double dgridX = x*SH_N_SQRT_SAMPLES;
		double dgridY = y*SH_N_SQRT_SAMPLES;
	
	int gridX = dgridX;
	int gridY = dgridY;
	
		int id = gridY*SH_N_SQRT_SAMPLES+gridX;
	
		double vx = solid_angle/dx[id]/2;

		int ivx = vx;
			
		double vy = solid_angle/dy[id]/2;
		int ivy = vy;

		if(ivy>SH_N_SQRT_SAMPLES/2) ivy= SH_N_SQRT_SAMPLES/2;
			
		int xstart = gridX-ivx;
		int xend = gridX+ivx;
		
			//if(dgridX-gridX<0.5) xstart--;
			//else xend++;

		int ystart = gridY-ivy;
		int yend = gridY+ivy;

			//if(dgridY-gridY<0.5) ystart--;
			//else yend++;

			if(xstart<0) xstart =0;
				if(xend>SH_N_SQRT_SAMPLES-1) xend = SH_N_SQRT_SAMPLES-1;
	
	for(int i=xstart; i<=xend; i++ )
	{
		for(int j=ystart; j<=yend; j++ )
		{
					
			int yy = j;
			if(yy<0) yy = SH_N_SQRT_SAMPLES+yy;
			else if(yy>=SH_N_SQRT_SAMPLES) yy = yy - SH_N_SQRT_SAMPLES;
			
			id = yy*SH_N_SQRT_SAMPLES+i;
			
			if(distance < buf[id].d)
			{
				buf[id].d = distance;
				buf[id].is_frontFace = is_frontFace;
				buf[id].id = nodeId;
				buf[id].facing = facing;
				buf[id].k = k;
			}
			
		}
	}
}

void sphericalHarmonics::diffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, float* tmpCoeff, const XYZ& normal) const
{
	reconstructSH rsh(inCoeff);
	
	for (int j = 0; j < SH_N_BASES; j++)
		tmpCoeff[j] = 0;
	
	for(int j=0; j<num_samp; j++)
	{
		SHSample& s = samp[j];
		float H = normal.dot(s.vector);
		if(H>0)
		{
			float shd = rsh.getFloat(s) * H * unitarea;
			projectASample(tmpCoeff, s, shd);
		}
	}
	
	for (int j = 0; j < SH_N_BASES; j++)
		outCoeff[j].x = outCoeff[j].y = outCoeff[j].z = tmpCoeff[j];
}

void sphericalHarmonics::rgbDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const
{
	rgbReconstructSH rsh(inCoeff);
	for (int j = 0; j < SH_N_BASES; j++)
		outCoeff[j] = XYZ(0);
	
	for(int j=0; j<num_samp; j++)
	{
		SHSample& s = samp[j];
		float H = normal.dot(s.vector);
		if(H>0)
		{
			XYZ col = rsh.getColor(s) * H * unitarea;
			projectASample(outCoeff, s, col);
		}
	}
}

void sphericalHarmonics::rDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const
{
	rReconstructSH rsh(inCoeff);
	for (int j = 0; j < SH_N_BASES; j++) outCoeff[j] = XYZ(0);
	
	for(int j=0; j<num_samp; j++)
	{
		SHSample& s = samp[j];
		float H = normal.dot(s.vector);
		if(H>0)
		{
			XYZ col = rsh.getColor(s) * H * unitarea;
			projectASample(outCoeff, s, col);
		}
	}
}

void sphericalHarmonics::gDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const
{
	gReconstructSH rsh(inCoeff);
	for (int j = 0; j < SH_N_BASES; j++) outCoeff[j] = XYZ(0);
	
	for(int j=0; j<num_samp; j++)
	{
		SHSample& s = samp[j];
		float H = normal.dot(s.vector);
		if(H>0)
		{
			XYZ col = rsh.getColor(s) * H * unitarea;
			projectASample(outCoeff, s, col);
		}
	}
}

void sphericalHarmonics::bDiffuseTransfer(SHSample* samp, const int num_samp, const float unitarea, XYZ* inCoeff, XYZ* outCoeff, const XYZ& normal) const
{
	bReconstructSH rsh(inCoeff);
	for (int j = 0; j < SH_N_BASES; j++) outCoeff[j] = XYZ(0);
	
	for(int j=0; j<num_samp; j++)
	{
		SHSample& s = samp[j];
		float H = normal.dot(s.vector);
		if(H>0)
		{
			XYZ col = rsh.getColor(s) * H * unitarea;
			projectASample(outCoeff, s, col);
		}
	}
}
//:~
