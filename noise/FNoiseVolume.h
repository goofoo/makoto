/*
 *  FNoiseVolume.h
 *  bacteria
 *
 *  Created by jian zhang on 5/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
 #ifndef _F_NOISEVOLUME_H
 #define _F_NOISEVOLUME_H
#include "../shared/zData.h"

struct ANoiseVolume
{
	XYZ pos, vel;
	float age,noi, noi2, noi3;
};

class FNoiseVolume
{
public:
	FNoiseVolume::FNoiseVolume();
	FNoiseVolume::~FNoiseVolume();
	
	void setGlobal(float size);
	void setSpace(MATRIX44F space);
	void updateCamera(float nc, float fc, float ha, float va, float fl);
	void updateData(unsigned num_elm, const XYZ* parray, const XYZ* varray, const float* aarray);
	unsigned getNumNoiseVolume() const {return num_bac;}
	XYZ& getPositionById(unsigned id) const {return data[id].pos;}
	float getNoiseById(unsigned id) const {return data[id].noi;}
	float getNoise2ById(unsigned id) const {return data[id].noi2;}
	float getNoise3ById(unsigned id) const {return data[id].noi3;}
	float getGlobalSize() const{return global_size;}
	float getAgeById(unsigned id) const {return data[id].age;}
	void getSideAndUpById(unsigned id, XYZ& side, XYZ& up) const;
	char isInViewFrustum(unsigned id) const;
private:
	MATRIX44F eyespace, eyespaceinv;
	ANoiseVolume* data;
	unsigned num_bac;
	float nearClip, farClip, horizontal_apeture, vertical_apeture, focal_length, global_size, h_fov, v_fov;
};
#endif