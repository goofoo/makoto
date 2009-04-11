/*
 *  CQSplat.h
 *  pcfViewer
 *
 *  Created by zhang on 07-10-13.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include "../shared/zMath.h"
#include "../shared/FClusting.h"

using  namespace std;

class CQSplat
{
public :
	CQSplat();
	~CQSplat();
	
	int loadPCF(const char* filename);
	int isValid() {return is_valid;}
	void getCenterAndWidth(XYZ& center, float& width) { center = m_bbox[0] + m_bbox[1] + m_bbox[2] + m_bbox[3] + m_bbox[4] + m_bbox[5] + m_bbox[6] + m_bbox[7]; center = center / 8.f; width = m_bbox[1].x - m_bbox[0].x + m_bbox[2].y - m_bbox[0].y + m_bbox[4].z - m_bbox[0].z; width /=3.f; }
	void occlusion(const XYZ& p, const XYZ& n, float threshold, float& value);
	void occlusion(const XYZ& p, const XYZ& n, float threshold, float& value, XYZ& bent_normal);
	void soft_shadow(const XYZ& p, const XYZ& n, float threshold, float& value);
	qnode& node(unsigned int index) {return m_full[index];}
	int getNodeCount() {return m_splat_count;}
	
private :
	qnode* m_full;
	XYZ m_bbox[8];
	int is_valid;
	int m_splat_count, m_point_count;
};
//:~