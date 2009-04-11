#ifndef _EIGENSYSTEM_H
#define _EIGENSYSTEM_H

const float epslon = 10e-8f;

inline XYZ findMean(pcdSample* data, int lo, int hi)
{
	XYZ res(0,0,0);
	for(int i = lo; i <=hi; i++)
	{
		res += data[i].pos;
	}
	res /= float(hi-lo+1);
	
	return res;
}

inline void calculateCovariance(pcdSample* data, int lo, int hi, const XYZ& mean, MATRIX33F& covariance)
{
	covariance.initialize();
		
	for(int i=lo; i<=hi; i++)
	{
		covariance.v[0][0] +=pow(data[i].pos.x-mean.x,2);
		covariance.v[0][1] +=(data[i].pos.x-mean.x)*(data[i].pos.y-mean.y);
		covariance.v[1][1] +=pow(data[i].pos.y-mean.y,2);
		covariance.v[0][2] +=(data[i].pos.x-mean.x)*(data[i].pos.z-mean.z);
		covariance.v[2][2] +=pow(data[i].pos.z-mean.z,2);
		covariance.v[1][2] +=(data[i].pos.y-mean.y)*(data[i].pos.z-mean.z);
	}
	
	int count = hi - lo + 1;
	
	covariance.v[0][0] /= count;
	covariance.v[0][1] /= count;
	covariance.v[1][1] /= count;
	covariance.v[0][2] /= count;
	covariance.v[2][2] /= count;
	covariance.v[1][2] /= count;
	covariance.v[1][0] = covariance.v[0][1];
	covariance.v[2][0] = covariance.v[0][2];
	covariance.v[2][1] = covariance.v[1][2];
}

inline void calculateEigenSystem(const MATRIX33F& mat, MATRIX33F& eigenSystem)
{
	float m11 = mat(0,0);
	float m12 = mat(0,1);
	float m13 = mat(0,2);
	float m22 = mat(1,1);
	float m23 = mat(1,2);
	float m33 = mat(2,2);
	
	eigenSystem.setIdentity();
	
	for(int a=0; a<32; a++)
	{
		if(fabs(m12)< epslon && fabs(m13)< epslon && fabs(m23)< epslon) break;
		
		if(m12 != 0.0f)
		{
			float u = (m22 - m11) * 0.5f / m12;
			float u2 = u * u;
			float u2p1 = u2 + 1.0f;
			float t = ( u2p1 != u2 ) ?
				((u<0.0f) ? -1.0f : 1.0f) * (sqrt(u2p1) - fabs(u)) 
				: (0.5f / u);
			float c = 1.0f / sqrt(t*t + 1.0f);
			float s = c * t;
			
			m11 -= t* m12;
			m22 += t * m12;
			m12 = 0.0f;
			
			float temp = c * m13 - s * m23;
			m23 = s * m13 + c * m23;
			m13 = temp;
			
			for(int i=0; i<3; i++)
			{
				float temp = c * eigenSystem(i,0) - s * eigenSystem(i,1);
				eigenSystem(i,1) = s * eigenSystem(i,0) + c * eigenSystem(i,1);
				eigenSystem(i,0) = temp;
			}
		}
		
		if(m13 != 0.0f)
		{
			float u = (m33 - m11) * 0.5f / m13;
			float u2 = u * u;
			float u2p1 = u2 + 1.0f;
			float t = ( u2p1 != u2 ) ?
				((u<0.0f) ? -1.0f : 1.0f) * (sqrt(u2p1) - fabs(u)) 
				: (0.5f / u);
			float c = 1.0f / sqrt(t*t + 1.0f);
			float s = c * t;
			
			m11 -= t* m13;
			m33 += t * m13;
			m13 = 0.0f;
			
			float temp = c * m12 - s * m23;
			m23 = s * m12 + c * m23;
			m12 = temp;
			
			for(int i=0; i<3; i++)
			{
				float temp = c * eigenSystem(i,0) - s * eigenSystem(i,2);
				eigenSystem(i,2) = s * eigenSystem(i,0) + c * eigenSystem(i,2);
				eigenSystem(i,0) = temp;
			}
		}
		
		if(m23 != 0.0f)
		{
			float u = (m33 - m22) * 0.5f / m23;
			float u2 = u * u;
			float u2p1 = u2 + 1.0f;
			float t = ( u2p1 != u2 ) ?
				((u<0.0f) ? -1.0f : 1.0f) * (sqrt(u2p1) - fabs(u)) 
				: (0.5f / u);
			float c = 1.0f / sqrt(t*t + 1.0f);
			float s = c * t;
			
			m22 -= t * m23;
			m33 += t * m23;
			m23 = 0.0f;
			
			float temp = c * m12 - s * m13;
			m13 = s * m12 + c * m13;
			m12 = temp;
			
			for(int i=0; i<3; i++)
			{
				float temp = c * eigenSystem(i,1) - s * eigenSystem(i,2);
				eigenSystem(i,2) = s * eigenSystem(i,1) + c * eigenSystem(i,2);
				eigenSystem(i,1) = temp;
			}
		}
	}
}

inline void calculateObjectSpace(pcdSample* data, int lo, int hi, MATRIX44F& space)
{
	//  calculate mean values	
	XYZ pos_mean=findMean(data, lo, hi);

	MATRIX33F covariance;
	calculateCovariance(data, lo, hi, pos_mean, covariance);
	MATRIX33F eigenSystem;
	calculateEigenSystem(covariance, eigenSystem);
	
	eigenSystem.transpose();
	
		space.setIdentity();
		space.setTranslation(pos_mean);
		space.setRotation(eigenSystem);

}

inline void findCube1(pcdSample* data, int lo, int hi, float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax)
{
	xmin = 10e8;
	xmax = -10e8;
	ymin = 10e8;
	ymax = -10e8;
	zmin = 10e8;
	zmax = -10e8;

	for(int i = lo; i <=hi; i++)
	{
		if(data[i].pos.x < xmin) xmin = data[i].pos.x;
		if(data[i].pos.y < ymin) ymin = data[i].pos.y;
		if(data[i].pos.z < zmin) zmin = data[i].pos.z;
			
		if(data[i].pos.x > xmax) xmax = data[i].pos.x;
		if(data[i].pos.y > ymax) ymax = data[i].pos.y;
		if(data[i].pos.z > zmax) zmax = data[i].pos.z;
	}
}

inline XYZ findMean(COESample* data, int lo, int hi)
{
	XYZ res(0,0,0);
	for(int i = lo; i <=hi; i++)
	{
		res += data[i].pos;
	}
	res /= float(hi-lo+1);
	
	return res;
}

inline void calculateCovariance(COESample* data, int lo, int hi, const XYZ& mean, MATRIX33F& covariance)
{
	covariance.initialize();
		
	for(int i=lo; i<=hi; i++)
	{
		covariance.v[0][0] +=pow(data[i].pos.x-mean.x,2);
		covariance.v[0][1] +=(data[i].pos.x-mean.x)*(data[i].pos.y-mean.y);
		covariance.v[1][1] +=pow(data[i].pos.y-mean.y,2);
		covariance.v[0][2] +=(data[i].pos.x-mean.x)*(data[i].pos.z-mean.z);
		covariance.v[2][2] +=pow(data[i].pos.z-mean.z,2);
		covariance.v[1][2] +=(data[i].pos.y-mean.y)*(data[i].pos.z-mean.z);
	}
	
	int count = hi - lo + 1;
	
	covariance.v[0][0] /= count;
	covariance.v[0][1] /= count;
	covariance.v[1][1] /= count;
	covariance.v[0][2] /= count;
	covariance.v[2][2] /= count;
	covariance.v[1][2] /= count;
	covariance.v[1][0] = covariance.v[0][1];
	covariance.v[2][0] = covariance.v[0][2];
	covariance.v[2][1] = covariance.v[1][2];
}

inline void calculateObjectSpace(COESample* data, int lo, int hi, MATRIX44F& space)
{
	//  calculate mean values	
	XYZ pos_mean=findMean(data, lo, hi);

	MATRIX33F covariance;
	calculateCovariance(data, lo, hi, pos_mean, covariance);
	MATRIX33F eigenSystem;
	calculateEigenSystem(covariance, eigenSystem);
	
	eigenSystem.transpose();
	
		space.setIdentity();
		space.setTranslation(pos_mean);
		space.setRotation(eigenSystem);

}

inline void findCube1(COESample* data, int lo, int hi, float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax)
{
	xmin = 10e8;
	xmax = -10e8;
	ymin = 10e8;
	ymax = -10e8;
	zmin = 10e8;
	zmax = -10e8;

	for(int i = lo; i <=hi; i++)
	{
		if(data[i].pos.x < xmin) xmin = data[i].pos.x;
		if(data[i].pos.y < ymin) ymin = data[i].pos.y;
		if(data[i].pos.z < zmin) zmin = data[i].pos.z;
			
		if(data[i].pos.x > xmax) xmax = data[i].pos.x;
		if(data[i].pos.y > ymax) ymax = data[i].pos.y;
		if(data[i].pos.z > zmax) zmax = data[i].pos.z;
	}
}
#endif