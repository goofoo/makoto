/*
 *  Buffer2D.h
 *  
 *
 *  Created by jian zhang on 6/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef BUFFER_2D_H
#define BUFFER_2D_H

#include <half.h>

struct TRIANGLE2D
{
	float s[3], t[3];
};

class Buffer2D
{
public :
	Buffer2D::Buffer2D();
	Buffer2D::~Buffer2D();
	
	void create(int _width, int _height);
	
	const int getWidth() const {return m_width;}
	const int getHeight() const {return m_height;}
	const half* getR() const {return rPixels;}
	const half* getG() const {return gPixels;}
	const half* getB() const {return bPixels;}
	
	static float barycentric_coord(float ax, float ay, float bx, float by, float x, float y);
	
	void rasterize(const TRIANGLE2D& tiangle);
	
private :
	half* rPixels;
	half* gPixels;
	half* bPixels;
	
	int m_width, m_height;
	
};
#endif
