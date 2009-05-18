/*
 *  FBacteria.h
 *  bacteria
 *
 *  Created by jian zhang on 5/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 #ifndef _F_BACTERIA_H
 #define _F_BACTERIA_H
#include "../shared/zData.h"

struct ABacteria
{
	XYZ pos, vel;
	float noi;
};

class FBacteria
{
public:
	FBacteria::FBacteria();
	FBacteria::~FBacteria();
	
	void setGlobal(float size, float rotate);
	void setSpace(MATRIX44F space);
	void updateCamera(float nc, float fc, float ha, float va, float fl);
	void updateData(unsigned num_elm, const XYZ* parray, const XYZ* varray);
	unsigned getNumBacteria() const {return num_bac;}
	XYZ& getPositionById(unsigned id) const {return data[id].pos;}
	float getNoiseById(unsigned id) const {return data[id].noi;}
	void getSideAndUpById(unsigned id, XYZ& side, XYZ& up) const;
	char isInViewFrustum(unsigned id) const;
private:
	MATRIX44F eyespace, eyespaceinv;
	ABacteria* data;
	unsigned num_bac;
	float nearClip, farClip, horizontal_apeture, vertical_apeture, focal_length, global_size, global_rotate, h_fov, v_fov;
};
#endif