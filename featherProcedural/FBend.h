/*
 *  FBend.h
 *  hairBase
 *
 *  Created by zhang on 08-12-29.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "../shared/zData.h"
using namespace std;

#define NUMBENDSEG 3

class FBend {
public:
  FBend() {}
  
  void create(float s, float x, float y);
  void getPoint(int idx, XYZ& p) const;
  void getNormal(int idx, XYZ& n) const;
  float getAngle(float param) const;

  void setParam(float s, float x, float y, int seg) { wind_x = x/s; wind_y = y/s; n_seg = seg;}
  void add(XYZ& p);
  void add(XYZ& p, XYZ& nor);
private:
  int n_seg;
  float wind_x, wind_y, max_ang;
  XYZ mp[NUMBENDSEG+1];
  XYZ mn[NUMBENDSEG+1];
};