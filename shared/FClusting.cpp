#include "FClusting.h"
#include <math.h>
#include <iostream>
using namespace std;

const float epslon = 10e-8f;

void FClusting::findMean(const qnode* data, int lo, int hi,  XYZ& pos_mean, XYZ& nor_mean)
{
	pos_mean.x = pos_mean.y = pos_mean.z=nor_mean.x = nor_mean.y = nor_mean.z=0;
	
	for(int i=lo; i<=hi; i++)
	{
		pos_mean += data[i].pos;
		nor_mean += data[i].nor;
	}
	
	int count = hi - lo + 1;
	pos_mean /= count;
	//nor_mean /= count;
	nor_mean.normalize();
	//cout<<"/nnormal mean: "<<nor_mean.x<<" "<<nor_mean.y<<" "<<nor_mean.z;
}

void FClusting::calculateCovariance(const qnode* data, int lo, int hi, const XYZ& mean, MATRIX33F& covariance)
{
	//findMean(data, lo, hi);
	
	//MATRIX33F covariance;
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
	
	//cout<<"\nC: "<<covariance.v[0][0]<<" "<<covariance.v[0][1]<<" "<<covariance.v[0][2];
	//cout<<"\n   "<<covariance.v[1][0]<<" "<<covariance.v[1][1]<<" "<<covariance.v[1][2];
	//cout<<"\n   "<<covariance.v[2][0]<<" "<<covariance.v[2][1]<<" "<<covariance.v[2][2];
	/*
	eigenVector.x = eigenVector.y = eigenVector.z =1;
	XYZ tmp;
	float Cold, Cnew, error;
	
	for(int i=0; i<20; i++)
	{
		tmp.x = covariance.v[0][0]*eigenVector.x + covariance.v[0][1]*eigenVector.y + covariance.v[0][2]*eigenVector.z;
		tmp.y = covariance.v[1][0]*eigenVector.x + covariance.v[1][1]*eigenVector.y + covariance.v[1][2]*eigenVector.z;
		tmp.z = covariance.v[2][0]*eigenVector.x + covariance.v[2][1]*eigenVector.y + covariance.v[2][2]*eigenVector.z;
	
		Cnew = tmp.maxvalue();
	
		tmp /= Cnew;
		//cout<<"\niteration: "<<i;
		//cout<<"\nCnew :"<<Cnew;
	
		//cout<<"\nV: "<<tmp.x<<" "<<tmp.y<<" "<<tmp.z;
	
		eigenVector = tmp;
		if(i>0)
		{
			error = (Cnew-Cold)/Cnew;
			//cout<<" error "<<error;
			if(abs(error)<0.01) break;
		}
		Cold = Cnew;
	}
	
	cout<<"\nR: "<<eigenVector.x<<" "<<eigenVector.y<<" "<<eigenVector.z;
	*/
	

}
/*
void FClusting::split(qnode* data, int count, vector <qnode>& children, vector <qnode>& parent)
{
	XYZ* angles = new XYZ[count];
	float* alpha = new float[count];
	recursive_sort(data, angles, alpha, 0, count-1, 0, children, parent);
	
	delete[] angles;
	delete[] alpha;
}
*/
void FClusting::sort(pcdSample* data, int lo, int hi)
{
	int i=lo, j=hi;
	
	pcdSample h;
	
		float tmp = data[(lo+hi)/2].pos.x;
		do
		{
			while(data[i].pos.x < tmp) i++;
			while(data[j].pos.x > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				
				i++; j--;
			}
		} while (i<=j);
		//  recursion
    if (lo<j) sort(data, lo, j);
    if (i<hi) sort(data, i, hi);

}

void FClusting::quick_sort(qnode* data, int lo, int hi, float* value)
{
	int i=lo, j=hi;
	
	qnode h;
	float v;
	float tmp = value[(lo+hi)/2];
	do
		{
			while(value[i] < tmp) i++;
			while(value[j] > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				v = value[i]; value[i]=value[j]; value[j]=v;
				i++; j--;
			}
		} while (i<=j);
		//  recursion
    if (lo<j) quick_sort(data, lo, j, value);
    if (i<hi) quick_sort(data, i, hi, value);

}

void FClusting::sort(qnode* data, int lo, int hi, XYZ* angles, int axis)
{
	int i=lo, j=hi;
	
	qnode h;
	XYZ a;
	if(axis==0)
	{
		float tmp = angles[(lo+hi)/2].x;
		do
		{
			while(angles[i].x < tmp) i++;
			while(angles[j].x > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				a=angles[i]; angles[i]=angles[j]; angles[j]=a;
				i++; j--;
			}
		} while (i<=j);
	}
	else if(axis==1)
	{
		float tmp = angles[(lo+hi)/2].y;
		do
		{
			while(angles[i].y < tmp) i++;
			while(angles[j].y > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				a=angles[i]; angles[i]=angles[j]; angles[j]=a;
				i++; j--;
			}
		} while (i<=j);
	}
	else
	{
		float tmp = angles[(lo+hi)/2].z;
		do
		{
			while(angles[i].z < tmp) i++;
			while(angles[j].z > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				a=angles[i]; angles[i]=angles[j]; angles[j]=a;
				i++; j--;
			}
		} while (i<=j);
	}
	
	//  recursion
    if (lo<j) sort(data, lo, j, angles, axis);
    if (i<hi) sort(data, i, hi, angles, axis);
}

void FClusting::sortNormal(qnode* data, int lo, int hi, float* alpha)
{
	int i=lo, j=hi;
	
	qnode h;
	float a;
	float tmp = alpha[(lo+hi)/2]; //cout<<"\ndot"<<tmp;
		do
		{
			while(alpha[i] < tmp) i++;
			while(alpha[j] > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				a=alpha[i]; alpha[i]=alpha[j]; alpha[j]=a;
				i++; j--;
			}
		} while (i<=j);
		
	//  recursion
    if (lo<j) sortNormal(data, lo, j, alpha);
    if (i<hi) sortNormal(data, i, hi, alpha);
}

void FClusting::findZeroPlane(const XYZ* position, int lo, int hi, int& mid, int axis)
{
	if(mid==lo) return;
	
	int bound = mid;
	
	if(axis==0)
	{
		if(position[mid].x > 0.0f)
		{
			mid = (lo+mid)/2;
			findZeroPlane(position, lo, bound, mid, axis);
		}
		else
		{
			mid = (mid+hi)/2;
			findZeroPlane(position, bound, hi, mid, axis);
		}
	}
	else if(axis ==1)
	{
		if(position[mid].y > 0.0f)
		{
			mid = (lo+mid)/2;
			findZeroPlane(position, lo, bound, mid, axis);
		}
		else
		{
			mid = (mid+hi)/2;
			findZeroPlane(position, bound, hi, mid, axis);
		}
	}
	else
	{
		if(position[mid].z > 0.0f)
		{
			mid = (lo+mid)/2;
			findZeroPlane(position, lo, bound, mid, axis);
		}
		else
		{
			mid = (mid+hi)/2;
			findZeroPlane(position, bound, hi, mid, axis);
		}
	}
}

void FClusting::findZeroNormal(const float* alpha, int lo, int hi, int& mid)
{
	if(mid==lo) return;
	
	int bound = mid;
	if(alpha[mid] > 0.0f)
		{
			mid = (lo+mid)/2;
			findZeroNormal(alpha, lo, bound, mid);
		}
		else
		{
			mid = (mid+hi)/2;
			findZeroNormal(alpha, bound, hi, mid);
		}
}
/*
void FClusting::recursive_sort(qnode* data, XYZ* angles, float* alpha, int lo, int hi, int level, vector <qnode>& children, vector <qnode>& parent)
{
	XYZ pos_mean, nor_mean;
	findMean(data, lo, hi, pos_mean, nor_mean);
	MATRIX33F covariance;
	calculateCovariance(data, lo, hi, pos_mean, covariance);
	MATRIX33F eigenSystem;
	calculateEigenSystem(covariance, eigenSystem);
	
	eigenSystem.transpose();
	
	MATRIX44F matinv;
		matinv.setIdentity();
		matinv.setTranslation(pos_mean);
		matinv.setRotation(eigenSystem);
		
		XYZ tangent[3];
		tangent[0] = XYZ(matinv(0,0), matinv(0,1), matinv(0,2));
		tangent[1] = XYZ(matinv(1,0), matinv(1,1), matinv(1,2));
		tangent[2] = XYZ(matinv(2,0), matinv(2,1), matinv(2,2));
		
		matinv.inverse();
		
		for(int i=lo; i<=hi; i++)
		{
			angles[i]=data[i].pos;
			matinv.transform(angles[i]);
		}
		
		int need_normal =0;
		XYZ base_normal;
		
		int axis = findLargestAxis(data, angles, lo, hi, need_normal, base_normal);
		
		float alpha_t, cone = 1;
		for(int i=lo; i<=hi; i++)
		{
			alpha_t = nor_mean.dot(data[i].nor);
			if(alpha_t<cone) cone = alpha_t;
		}
		
		
		
	if((hi-lo)<3 )
	{
	// save out
	//cout<<" save: "<<length.size();
		qnode asample;
		//asample.start=children.size();
		float sum_area = 0;
		for(int i=lo; i<=hi; i++)
		{
			children.push_back(data[i]);
			sum_area += data[i].area;
		}
		//asample.end=children.size();
		
		asample.pos = pos_mean;
		
		asample.nor = nor_mean;
		asample.area = sum_area;
		
		XYZ side = tangent[axis];
		XYZ up = side^nor_mean;
		up.normalize();
		side = up^nor_mean;
		side.normalize();
		asample.tang = side;
		
		MATRIX44F matobjinv;
		matobjinv.setIdentity();
		matobjinv.setTranslation(pos_mean);
		matobjinv.setOrientations(side, up, nor_mean);
		matobjinv.inverse();
		
		for(int i=lo; i<=hi; i++)
		{
			angles[i]=data[i].pos;
			matobjinv.transform(angles[i]);
		}
		
		float x_min, x_max, y_min, y_max;
		x_min = x_max = angles[lo].x;
		y_min = y_max = angles[lo].y;

		for(int i=lo+1; i<=hi; i++)
		{
			if(angles[i].x < x_min) x_min = angles[i].x;
			if(angles[i].x > x_max) x_max = angles[i].x;
			if(angles[i].y < y_min) y_min = angles[i].y;
			if(angles[i].y > y_max) y_max = angles[i].y;
		}
	
		x_max = x_max - x_min;
		y_max = y_max - y_min;
		
		//float r_mean = sqrt(sum_area)/(hi-lo+1);
		
		//float ratio = sqrt((x_max+2*r_mean)/(y_max+2*r_mean));
		//asample.ratio = ratio;
		//cout<<" :"<<" "<<ratio;

		parent.push_back(asample);
		
		
	}
	else
	{
		level++;
		int mid = (lo+hi)/2;
		
		
	
		if(need_normal==0)
		{
			sort(data, lo, hi, angles, axis);
			findZeroPlane(angles, lo, hi, mid, axis);
		}
		else
		{
			for(int i=lo; i<=hi; i++)
			{
				alpha[i] = data[i].nor.dot(base_normal);
			}
			sortNormal(data, lo, hi, alpha);
			findZeroNormal(alpha, lo, hi, mid);
		}
		//cout<<"\nsplit at: "<<angles[mid].z;
		recursive_sort(data, angles, alpha, lo, mid, level, children, parent);
		recursive_sort(data, angles, alpha, mid+1, hi, level, children, parent);
	}

}
*/
void FClusting::calculateEigenSystem(const MATRIX33F& mat, MATRIX33F& eigenSystem)
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
	
		
	//cout<<"\nE: "<<eigenSystem(0,0)<<" "<<eigenSystem(0,1)<<" "<<eigenSystem(0,2);
	//cout<<"\n   "<<eigenSystem(1,0)<<" "<<eigenSystem(1,1)<<" "<<eigenSystem(1,2);
	//cout<<"\n   "<<eigenSystem(2,0)<<" "<<eigenSystem(2,1)<<" "<<eigenSystem(2,2);
}

void FClusting::findOptimalDims(qnode* data, XYZ* position, int lo, int hi, int& first, int& second)
{
// process bounding box
	float x_min, x_max, y_min, y_max, z_min, z_max;
	x_min = x_max = position[lo].x;
	y_min = y_max = position[lo].y;
	z_min = z_max = position[lo].z;
	
	for(int i=lo+1; i<=hi; i++)
	{
		if(position[i].x < x_min) x_min = position[i].x;
		if(position[i].x > x_max) x_max = position[i].x;
		if(position[i].y < y_min) y_min = position[i].y;
		if(position[i].y > y_max) y_max = position[i].y;
		if(position[i].z < z_min) z_min = position[i].z;
		if(position[i].z > z_max) z_max = position[i].z;
	}
	
	x_max = x_max - x_min;
	y_max = y_max - y_min;
	z_max = z_max - z_min;
	
//output
	if(x_max>y_max && x_max>z_max)
	{
		first = 0;
		if(y_max>z_max) second = 1;
		else second =2;
	}
	else if(y_max>x_max && y_max>z_max)
	{
		first = 1;
		if(x_max>z_max) second = 0;
		else second =2;
	}
	else
	{
		first = 2;
		if(x_max>y_max) second = 0;
		else second =1;
	}
}

int FClusting::findLargestAxis(qnode* data, XYZ* position, int lo, int hi, int& is_normal_based, XYZ& base)
{
	float x_min, x_max, y_min, y_max, z_min, z_max;
	x_min = x_max = position[lo].x;
	y_min = y_max = position[lo].y;
	z_min = z_max = position[lo].z;
	
	for(int i=lo+1; i<=hi; i++)
	{
		if(position[i].x < x_min) x_min = position[i].x;
		if(position[i].x > x_max) x_max = position[i].x;
		if(position[i].y < y_min) y_min = position[i].y;
		if(position[i].y > y_max) y_max = position[i].y;
		if(position[i].z < z_min) z_min = position[i].z;
		if(position[i].z > z_max) z_max = position[i].z;
	}
	
	x_max = x_max - x_min;
	y_max = y_max - y_min;
	z_max = z_max - z_min;
	
	int first, third;
	float biggest, smallest; 
	if(x_max>y_max && x_max>z_max) 
	{
		first =0;
		biggest = x_max;
	}
	else if(y_max>x_max && y_max>z_max) 
	{
		first =1;
		biggest = y_max;
	}
	else 
	{
		first =2;
		biggest = z_max;
	}
	
	if(x_max<y_max && x_max<z_max) 
	{	
		third =0;
		smallest = x_max;
	}
	else if(y_max<x_max && y_max<z_max) 
	{
		third =1;
		smallest = y_max;
	}
	else 
	{
		third =2;
		smallest = z_max;
	}
	/*
	if((smallest*2)>biggest ) {return first;}

	if(first!=0 && third!= 0) second = 0;
	else if(first!=1 && third!=1) second = 1;
	else second = 2;
	
	XYZ lonormalt(0,0,0), hinormalt(0,0,0);
	int mid = (lo+hi)/2;
	// sort the third
	sort(data, lo, hi, position, third);
	
	for(int i=lo; i<mid; i++)
	{
		lonormalt += data[i].nor;
	}
	
	for(int i=mid+1; i<=hi; i++)
	{
		hinormalt += data[i].nor;
	}
	
	lonormalt.normalize();
	hinormalt.normalize();
	
	float thirddot = lonormalt.dot(hinormalt);
	
	//sort second
	XYZ lonormals(0,0,0); XYZ hinormals(0,0,0);
	sort(data, lo, hi, position, second);
	for(int i=lo; i<mid; i++)
	{
		lonormals += data[i].nor;
	}
	
	for(int i=mid+1; i<=hi; i++)
	{
		hinormals += data[i].nor;
	}
	lonormals.normalize();
	hinormals.normalize();
	
	float seconddot = lonormals.dot(hinormals);
	

	if(thirddot<seconddot && thirddot<-0.9f) 
	{	
		is_normal_based = 1;
		base = hinormalt;
		return third;
	}
	else if(seconddot<thirddot && seconddot<-0.9f) 
	{
		is_normal_based = 1;
		base = hinormals;
		return second;
	}
	*/
	return first;
	
}

float saturate(float in)
{
	if(in<0) return 0;
	else if(in>1) return 1;
	return in;
}

float solidAngle(const XYZ& v, const float& rsquared, const XYZ& receiverNormal, const XYZ& emitterNormal, const float& emitterArea)
{
	//XYZ resv(-v.x, -v.y, -v.z);
	return (1.0f - 1.0f/sqrt(emitterArea/rsquared + 1))*saturate(receiverNormal.dot(v))*saturate(emitterNormal.dot(v));
}
/*
void occ_lookup(qnode& receiver, const qnode* emitter, int id, float& occ, float& threshold)
{
	XYZ v(receiver.pos, emitter[id].pos);
	float rr = v.lengthSquare();
	v.normalize();
	
	if(v.dot(receiver.nor) < 0.f && rr > emitter[id].area*threshold/4) 
	{
		occ += solidAngle(v, rr, receiver.nor, emitter[id].nor, emitter[id].area);
		return;
	}
	
	if( rr > emitter[id].area*threshold)
	{
		
		occ += solidAngle(v, rr, receiver.nor, emitter[id].nor, emitter[id].area);
	}
	else
	{
		if(emitter[id].child[0]>0) occ_lookup(receiver, emitter, emitter[id].child[0], occ, threshold);
		if(emitter[id].child[1]>0) occ_lookup(receiver, emitter, emitter[id].child[1], occ, threshold);
		if(emitter[id].child[2]>0) occ_lookup(receiver, emitter, emitter[id].child[2], occ, threshold);
		if(emitter[id].child[3]>0) occ_lookup(receiver, emitter, emitter[id].child[3], occ, threshold);
	}
}

void FClusting::testOcclusion(qnode* data, unsigned int* enable, int& count, const qnode* emitter)
{
	cout<<" Calculating occlusion: ";
	int seg = count/10;
	for(int i=0; i<count; i++)
	{
		float value = 0;
		//if(enable[i]==1) occ_lookup(data[i], emitter, 0, value);
		data[i].col.x = data[i].col.y =data[i].col.z = 1.0f - value;
		if(i%seg == 0) cout<<" "<<i/seg<<"0% is done ";
	}
}

void FClusting::testOcclusion(qnode& data, const qnode* emitter, float& value, float& threshold)
{
	//occ_lookup(data, emitter, 0, value, threshold);
}
*/
int FClusting::QSplat(qnode* data, int count, qnode* result, XYZ bbox[8])
{
	MATRIX44F objectSpace;
		calculateObjectSpace(data, 0, count-1, objectSpace);
		
		MATRIX44F objectSpaceInverse = objectSpace;
		objectSpaceInverse.inverse();

// get points in object space		
		for(int i=0; i<count; i++) objectSpaceInverse.transform(data[i].pos);

// object bounding box
	float min[3], max[3];
	findBBox(data, 0, count-1, min[0], min[1], min[2], max[0], max[1], max[2]);
	
	bbox[0] = XYZ(min[0], min[1], min[2]);
	bbox[1] = XYZ(max[0], min[1], min[2]);
	bbox[2] = XYZ(min[0], max[1], min[2]);
	bbox[3] = XYZ(max[0], max[1], min[2]);
	bbox[4] = XYZ(min[0], min[1], max[2]);
	bbox[5] = XYZ(max[0], min[1], max[2]);
	bbox[6] = XYZ(min[0], max[1], max[2]);
	bbox[7] = XYZ(max[0], max[1], max[2]);

	int node_count = 0;
	recursive_qsplat(data, 0, count-1, result, -1, 0, node_count);
	
// get points back in world space
	for(int i=0; i<node_count; i++) objectSpace.transform(result[i].pos);
	for(int i=0; i<8; i++) objectSpace.transform(bbox[i]);

	return node_count;
}

int sortByValue1D(qnode* data, int lo, int hi, int axis, float value)
{
	qnode h;
	int i=lo, j=hi;

		do
		{
			while(data[i].pos[axis] <= value) i++;
			while(data[j].pos[axis] > value) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				i++; j--;
			}
		} while (i<=j);
		
		return j;
}

int splitByValue1D(qnode* data, int lo, int hi, int axis, float min, float max)
{
	
	float val0 = min*0.5f + max*0.5f;

	return sortByValue1D(data, lo, hi, axis, val0);
/*	
	float vol0 = findBVol(data, lo, mid0) + findBVol(data, mid0+1, hi);
	
	float val1 = min*0.4f + max*0.6f;
		
	int mid1 = sortByValue(data, lo, hi, axis, val1);
	float vol1 = findBVol(data, lo, mid1) + findBVol(data, mid1+1, hi);
	
	float val2 = min*0.6f + max*0.4f;
		
	int mid2 = sortByValue(data, lo, hi, axis, val2);;
	float vol2 = findBVol(data, lo, mid2) + findBVol(data, mid2+1, hi);
	
	if(vol1 < vol0 && vol1<vol2)
	{
		return sortByValue(data, lo, hi, axis, val1);
	}
	
	if(vol2 < vol0 && vol2<vol1)
	{
		return sortByValue(data, lo, hi, axis, val2);
	}
	
	else return sortByValue(data, lo, hi, axis, val0);
*/
}

void FClusting::recursive_qsplat(qnode* data, int lo, int hi, qnode* result, int parent_id, int child_id, int& node_count)
{
	if(hi<lo) return;
	
	qnode asample;
	int nodeId;
	
	
//  calculate mean values	
		XYZ pos_mean, nor_mean, col_mean;
		float cone, sum_area;
	findMean(data, lo, hi, pos_mean, nor_mean, col_mean, sum_area, cone);
	
		asample.pos = pos_mean;
		asample.nor = nor_mean;
		asample.area = sum_area;
		asample.cone = cone;
		//asample.nor.perpendicular(asample.tang);
		//asample.binor = asample.nor^asample.tang; asample.binor.normalize();
//		asample.col = col_mean;

		//cout<<" "<<sum_area;
		
// calculate bounding box
		float min[3], max[3];
		float vol = findBBox(data, lo, hi, min[0], min[1], min[2], max[0], max[1], max[2]);
		
		int axis_second;
		int axis_first = findLongestAxis(min, max, axis_second);
		
// add node to list
		result[node_count] = asample;
		
		nodeId = node_count;
		
		node_count++;
		
// parse child[] of parent
		if(parent_id>=0) result[parent_id].child[child_id] = nodeId;
		
		if(hi==lo) return;
		
		/*if(max[axis_first]-min[axis_first] < 0.000001f) 
		{
			result[node_count].area /= hi-lo+1;
			return;
		}*/
		
		// a line		
		if(vol<0.000001f) 
		{
			//cout<<"zero"<<lo<<" "<<hi;
			//recursive_qsplat(data, lo, mid, result, nodeId, 0, node_count);
			//recursive_qsplat(data, mid+1, hi, result, nodeId, 1, node_count);
			result[node_count].area /= hi-lo+1;
			return;
		}

		//if(sum_area<8)return;

	if(hi-lo<4) 
	{
		/*for(int i=0; i<=hi-lo; i++) 
		{
			qnode leaf = data[lo+i];
			//leaf.nor.perpendicular(leaf.tang); 
			//leaf.binor = leaf.nor^leaf.tang; leaf.binor.normalize();

			result[node_count] = leaf;
			
			result[nodeId].child[i] = nodeId+i+1;
			node_count++;
		}*/
		return;
	}

			int mid, hmid, haxis;
			float hmin[3], hmax[3];

// find half id for possible 4 children
		mid = splitByValue1D(data, lo, hi, axis_first, min[axis_first], max[axis_first]);
				
// proccess left half				
		findBBox(data, lo, mid, hmin[0], hmin[1], hmin[2], hmax[0], hmax[1], hmax[2]);
		haxis = findLongestAxis(hmin, hmax);
				
				
				if( hmax[haxis] - hmin[haxis] > (max[haxis] - min[haxis])*0.67 )
				{
					hmid = splitByValue1D(data, lo, mid, haxis, hmin[haxis], hmax[haxis]);
					recursive_qsplat(data, lo, hmid, result, nodeId, 0, node_count);
					recursive_qsplat(data, hmid+1, mid, result, nodeId, 1, node_count);
				}
				else
					recursive_qsplat(data, lo, mid, result, nodeId, 0, node_count);

// proccess righ half					
				findBBox(data, mid+1, hi, hmin[0], hmin[1], hmin[2], hmax[0], hmax[1], hmax[2]);
				haxis = findLongestAxis(hmin, hmax);
				
				if( hmax[haxis] - hmin[haxis] > (max[haxis] - min[haxis])*0.67 )
				{
					hmid = splitByValue1D(data, mid+1, hi, haxis, hmin[haxis], hmax[haxis]);
					recursive_qsplat(data, mid+1, hmid, result, nodeId, 2, node_count);
					recursive_qsplat(data, hmid+1, hi, result, nodeId, 3, node_count);
				}
				else	
					recursive_qsplat(data, mid+1, hi, result, nodeId, 2, node_count);
}

void FClusting::findMean(const qnode* data, int lo, int hi,  XYZ& pos_mean)
{
	pos_mean.x = pos_mean.y = pos_mean.z = 0;
	float sum_area = 0;
	for(int i=lo; i<=hi; i++) 
	{
		pos_mean += data[i].pos * data[i].area;
		sum_area += data[i].area;
		}
	
	pos_mean /= sum_area;
}

void FClusting::findMean(const qnode* data, int lo, int hi,  XYZ& pos_mean, XYZ& nor_mean, XYZ& col_mean, float& area, float& cone)
{
	pos_mean.x = pos_mean.y = pos_mean.z=nor_mean.x = nor_mean.y = nor_mean.z = col_mean.x = col_mean.y = col_mean.z =0;
	
	area = 0;
	
	for(int i=lo; i<=hi; i++)
	{
		pos_mean += data[i].pos * data[i].area;
		nor_mean += data[i].nor;
//		col_mean += data[i].col;
		area += data[i].area;
	}
	
	pos_mean /= area;
	nor_mean.normalize();
	
	float count = float(hi-lo+1);
	col_mean /= count;
	
	// process normal cone
	cone = 1.0f;
	float alpha_t;
	for(int i=lo; i<=hi; i++)
	{
		alpha_t = nor_mean.dot(data[i].nor);
		if(alpha_t<cone) cone = alpha_t;
	}
}

float FClusting::findBBox(const qnode* data, int lo, int hi, float& xmin, float& ymin, float& zmin, float& xmax, float& ymax, float& zmax)
{
	xmin = xmax = data[lo].pos.x;
	ymin = ymax = data[lo].pos.y;
	zmin = zmax = data[lo].pos.z;
	
	for(int i=lo+1; i<=hi; i++)
	{
		if(data[i].pos.x <= xmin) xmin = data[i].pos.x;
		if(data[i].pos.x > xmax) xmax = data[i].pos.x;
		if(data[i].pos.y <= ymin) ymin = data[i].pos.y;
		if(data[i].pos.y > ymax) ymax = data[i].pos.y;
		if(data[i].pos.z <= zmin) zmin = data[i].pos.z;
		if(data[i].pos.z > zmax) zmax = data[i].pos.z;
	}
	
	return (xmax - xmin)*(ymax - ymin)*(zmax - zmin);
}

float FClusting::findBVol(const qnode* data, int lo, int hi)
{
	float xmin, ymin, zmin, xmax, ymax, zmax;
	xmin = xmax = data[lo].pos.x;
	ymin = ymax = data[lo].pos.y;
	zmin = zmax = data[lo].pos.z;
	
	for(int i=lo+1; i<=hi; i++)
	{
		if(data[i].pos.x <= xmin) xmin = data[i].pos.x;
		if(data[i].pos.x > xmax) xmax = data[i].pos.x;
		if(data[i].pos.y <= ymin) ymin = data[i].pos.y;
		if(data[i].pos.y > ymax) ymax = data[i].pos.y;
		if(data[i].pos.z <= zmin) zmin = data[i].pos.z;
		if(data[i].pos.z > zmax) zmax = data[i].pos.z;
	}
	
	return (xmax - xmin)*(ymax - ymin)*(zmax - zmin) + (xmax - xmin) + (ymax - ymin) + (zmax - zmin);
}

int FClusting::findLongestAxis(float min[3], float max[3])
{
	if(max[0]-min[0]>max[1]-min[1] && max[0]-min[0]>max[2]-min[2]) 
	{
		return 0;
	}
	else if(max[1]-min[1]>max[0]-min[0] && max[1]-min[1]>max[2]-min[2])
	{
		return 1;
	}
	else 
	{
		return 2;
	}
}

int FClusting::findLongestAxis(float min[3], float max[3], int& second)
{
	if(max[0]-min[0]>max[1]-min[1] && max[0]-min[0]>max[2]-min[2]) 
	{
		if(max[2]-min[2]>max[1]-min[1]) second = 2;
		else second = 1;
		return 0;
	}
	else if(max[1]-min[1]>max[0]-min[0] && max[1]-min[1]>max[2]-min[2])
	{
		if(max[2]-min[2]>max[0]-min[0]) second = 2;
		else second = 0;
		return 1;
	}
	else 
	{
		if(max[0]-min[0]>max[1]-min[1]) second = 0;
		else second = 1;
		return 2;
	}
}

int FClusting::findSmallestVolume(float vol[3])
{
	if(vol[0] < vol[1] && vol[0] < vol[2]) return 0;
	else if(vol[1] < vol[0] && vol[1] < vol[2]) return 1;
	else return 2;
}

int FClusting::splitByValue(qnode* data, int lo, int hi, int axis, float value)
{
	int i=lo, j=hi;
	
	qnode h;
	
		do
		{
			while(data[i].pos[axis] <= value) i++;
			while(data[j].pos[axis] > value) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				i++; j--;
			}
		} while (i<=j);
	
	
	return j;
}

int FClusting::sortByValue(qnode* data, int lo, int hi, int axis, float value)
{
	qnode h;
	int i=lo, j=hi;

		do
		{
			while(data[i].pos[axis] <= value) i++;
			while(data[j].pos[axis] > value) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				i++; j--;
			}
		} while (i<=j);
		
		return j;
}

/*
int FClusting::splitByValue(qnode* data, int lo, int hi, int axis, float min, float max)
{
	
	float val0 = min*0.5f + max*0.5f;

	int mid0 = sortByValue(data, lo, hi, axis, val0);
return mid0;	
	float vol0 = findBVol(data, lo, mid0) + findBVol(data, mid0+1, hi);
	
	float val1 = min*0.4f + max*0.6f;
		
	int mid1 = sortByValue(data, lo, hi, axis, val1);
	float vol1 = findBVol(data, lo, mid1) + findBVol(data, mid1+1, hi);
	
	float val2 = min*0.6f + max*0.4f;
		
	int mid2 = sortByValue(data, lo, hi, axis, val2);;
	float vol2 = findBVol(data, lo, mid2) + findBVol(data, mid2+1, hi);
	
	if(vol1 < vol0 && vol1<vol2)
	{
		return sortByValue(data, lo, hi, axis, val1);
	}
	
	if(vol2 < vol0 && vol2<vol1)
	{
		return sortByValue(data, lo, hi, axis, val2);
	}
	
	else return sortByValue(data, lo, hi, axis, val0);

}
*/
void FClusting::sort(qnode* data, int lo, int hi, int axis)
{
	int i=lo, j=hi;
	
	qnode h;
	
		float tmp = data[(lo+hi)/2].pos[axis];
		do
		{
			while(data[i].pos[axis] < tmp) i++;
			while(data[j].pos[axis] > tmp) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				
				i++; j--;
			}
		} while (i<=j);
		//  recursion
    if (lo<j) sort(data, lo, j, axis);
    if (i<hi) sort(data, i, hi, axis);
}

void FClusting::calculateObjectSpace(qnode* data, int lo, int hi, MATRIX44F& space)
{
	//  calculate mean values	
	XYZ pos_mean;
	findMean(data, lo, hi, pos_mean);

	MATRIX33F covariance;
	calculateCovariance(data, lo, hi, pos_mean, covariance);
	MATRIX33F eigenSystem;
	calculateEigenSystem(covariance, eigenSystem);
	
	eigenSystem.transpose();
	
		space.setIdentity();
		space.setTranslation(pos_mean);
		space.setRotation(eigenSystem);

}

void FClusting::convertToMac(char* data, int length)
{
	char tmp[4];
	for(int i=0; i<length; i=i+4)
	{
		tmp[0] = data[i];
		tmp[1] = data[i+1];
		tmp[2] = data[i+2];
		tmp[3] = data[i+3]; 
		data[i] = tmp[3];
		data[i+1] = tmp[2];
		data[i+2] = tmp[1];
		data[i+3] = tmp[0];
	}
}

void occ_lookup(const XYZ& origin, const XYZ& ray, const qnode* emitter, int id, float& occ, float& threshold)
{
	XYZ v(origin, emitter[id].pos);
	float rr = v.lengthSquare();
	v.normalize();
	
	float vdotr = v.dot(ray);
	if(vdotr < 0 && rr*vdotr*vdotr > emitter[id].area) 
	{
		//occ += solidAngle(v, rr, ray, emitter[id].nor, emitter[id].area);
		return;
	}
	
	if( rr > emitter[id].area*threshold)
	{
		
		occ += solidAngle(v, rr, ray, emitter[id].nor, emitter[id].area);
	}
	else
	{
		if(emitter[id].child[0]>0) occ_lookup(origin, ray, emitter, emitter[id].child[0], occ, threshold);
		if(emitter[id].child[1]>0) occ_lookup(origin, ray, emitter, emitter[id].child[1], occ, threshold);
		if(emitter[id].child[2]>0) occ_lookup(origin, ray, emitter, emitter[id].child[2], occ, threshold);
		if(emitter[id].child[3]>0) occ_lookup(origin, ray, emitter, emitter[id].child[3], occ, threshold);
	}
}

void occ_lookup_bent_n(const XYZ& origin, const XYZ& ray, const qnode* emitter, int id, float& occ, float& threshold, XYZ& bent_n)
{
	XYZ v(origin, emitter[id].pos);
	float rr = v.lengthSquare();
	v.normalize();
	
	float value;
	
	float vdotr = v.dot(ray);
	//if(v.dot(ray) < 0 && rr > emitter[id].area*2) 
	if(vdotr < 0 && rr*vdotr*vdotr > emitter[id].area) 
	{
		//value = solidAngle(v, rr, ray, emitter[id].nor, emitter[id].area);
		//bent_n.x -= v.x * value;
		//bent_n.y -= v.y * value;
		//bent_n.z -= v.z * value;
		//occ += value;
		return;
	}

	if( rr > emitter[id].area*threshold)
	{
		value = solidAngle(v, rr, ray, emitter[id].nor, emitter[id].area);
		bent_n.x -= v.x * value;
		bent_n.y -= v.y * value;
		bent_n.z -= v.z * value;
		occ += value;
	}
	else
	{
		if(emitter[id].child[0]>0) occ_lookup_bent_n(origin, ray, emitter, emitter[id].child[0], occ, threshold, bent_n);
		if(emitter[id].child[1]>0) occ_lookup_bent_n(origin, ray, emitter, emitter[id].child[1], occ, threshold, bent_n);
		if(emitter[id].child[2]>0) occ_lookup_bent_n(origin, ray, emitter, emitter[id].child[2], occ, threshold, bent_n);
		if(emitter[id].child[3]>0) occ_lookup_bent_n(origin, ray, emitter, emitter[id].child[3], occ, threshold, bent_n);
	}
}
void FClusting::occlusion(const XYZ& origin, const XYZ& dir, float threshold, const qnode* emitter, float& value)
{
	occ_lookup(origin, dir, emitter, 0, value, threshold);
}

void FClusting::occlusionBentNormal(const XYZ& origin, const XYZ& dir, float threshold, const qnode* emitter, float& value, XYZ& bent_n)
{
	occ_lookup_bent_n(origin, dir, emitter, 0, value, threshold, bent_n);
}

float shadowVisibility(const XYZ& v, const float& rsquared, const XYZ& receiverNormal, const XYZ& emitterNormal, const float& emitterArea)
{
	XYZ resv(-v.x, -v.y, -v.z);
	return (1.0f - 1.0f/sqrt(emitterArea/rsquared*16 + 1))*saturate(receiverNormal.dot(v)/4)*saturate(emitterNormal.dot(resv));
}

void shadow_lookup(const XYZ& origin, const XYZ& ray, const qnode* emitter, int id, float& occ, float& threshold)
{
	XYZ v(origin, emitter[id].pos);
	float rr = v.lengthSquare();
	v.normalize();
	
	float vdotr = v.dot(ray);
	if(vdotr + emitter[id].area*threshold /rr < 0.8 ) 
	{
		//value = solidAngle(v, rr, ray, emitter[id].nor, emitter[id].area);
		//bent_n.x -= v.x * value;
		//bent_n.y -= v.y * value;
		//bent_n.z -= v.z * value;
		//occ += value;
		occ += shadowVisibility(v, rr, ray, emitter[id].nor, emitter[id].area);
		return;
	}
	
	if( rr > emitter[id].area*threshold)
	{
		occ += shadowVisibility(v, rr, ray, emitter[id].nor, emitter[id].area);
	}
	else
	{
		if(emitter[id].child[0]>0) shadow_lookup(origin, ray, emitter, emitter[id].child[0], occ, threshold);
		if(emitter[id].child[1]>0) shadow_lookup(origin, ray, emitter, emitter[id].child[1], occ, threshold);
		if(emitter[id].child[2]>0) shadow_lookup(origin, ray, emitter, emitter[id].child[2], occ, threshold);
		if(emitter[id].child[3]>0) shadow_lookup(origin, ray, emitter, emitter[id].child[3], occ, threshold);
	}
}

void FClusting::shadow(const XYZ& origin, const XYZ& dir, float threshold, const qnode* emitter, float& value)
{
	shadow_lookup(origin, dir, emitter, 0, value, threshold);
}

//~: