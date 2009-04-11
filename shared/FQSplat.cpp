/*
 *  CQSplat.cpp
 *  pcfViewer
 *
 *  Created by zhang on 07-10-13.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "FQSplat.h"

CQSplat::CQSplat() : is_valid(0), m_splat_count(0), m_full(0)
{
}

CQSplat::~CQSplat()
{
		if(m_full) delete[] m_full;
}

int CQSplat::loadPCF(const char* filename)
{
	is_valid = 0;
// cleanup	
	if(m_full){delete[] m_full; m_full=0;}
	
	ifstream ffile;
	ffile.open(filename, ios::in | ios::binary | ios::ate);
	if(ffile.is_open())
	{
		cout<<"Opening point cloud file "<<filename;
		pcdSample* pSamples;
		
		ifstream::pos_type size = ffile.tellg();
		if((int)size%(int)sizeof(pcdSample) == 0) 
		{
				m_point_count = (int)size/(int)sizeof(pcdSample);
				
				pSamples = new pcdSample[m_point_count];
				ffile.seekg(0, ios::beg);
				ffile.read((char*)pSamples, size);
				ffile.close();
#ifdef __APPLE__
				FClusting::convertToMac((char*)pSamples, (int)size);
#endif
		}
		else 
		{
			cout<<filename<<" is not a point cloud file. Nothing loaded.";
			ffile.close();
			return 0;
		}
			
		cout<<" point count: "<<m_point_count<<" Pre-processing... ";
				
			qnode* pcloud = new qnode[m_point_count];
			for(int i=0; i<m_point_count; i++) pcloud[i].parse(pSamples[i]);
			
			delete[] pSamples; 
	
			m_full = new qnode[unsigned int((float)m_point_count*0.8f)];

			m_splat_count = FClusting::QSplat(pcloud, m_point_count, m_full, m_bbox);
			cout<<"splat count: "<<m_splat_count<<" ";

			is_valid = 1;
			
			if(pcloud) delete[] pcloud;
		}
		else 
		{
			cout<<filename<<" file not found. Nothing loaded.";
			return 0;
		}
	return 1;
}

void CQSplat::occlusion(const XYZ& p, const XYZ& n, float threshold, float& value)
{
	value = 0;
		FClusting::occlusion(p, n, threshold, m_full, value);
		value = 1.f-value;
		if(value<0) value=0;
}

void CQSplat::occlusion(const XYZ& p, const XYZ& n, float threshold, float& value, XYZ& bent_normal)
{
	bent_normal = n;
	value = 0;
		FClusting::occlusionBentNormal(p, n, threshold, m_full, value, bent_normal);
		value = 1.f-value;
		if(value<0) value=0;
}

void CQSplat::soft_shadow(const XYZ& p, const XYZ& n, float threshold, float& value)
{
	value = 0;
		FClusting::shadow(p, n, threshold, m_full, value);
		value = 1.f-value;
}
//:~