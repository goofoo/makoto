/*
 *  FFeather.cpp
 *  featherTest
 *
 *  Created by zhang on 08-12-9.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <fstream>
#include "FFeather.h"
#include "gBase.h"

FFeather::FFeather():is_null(1),m_patch_list(0) {}
FFeather::~FFeather() 
{
	release();
}

FFeather::FFeather(const char* filename):is_null(1),m_patch_list(0)
{
	if(!load(filename)) cout<<"ERROR: cannot open file "<<filename<<endl;
}

void FFeather::release()
{
	if(m_patch_list) delete[] m_patch_list;
}

char FFeather::load(const char* filename)
{
	ifstream ffin;
	ffin.open(filename, ios::in | ios::ate);
	if(!ffin.is_open()) 
	{
		cout<<"ERROR: cannot open file "<<filename<<endl;
		return 0;
	}
	
	int length = ffin.tellg();
	
	const char* sbuf = new char[length];
	
	ffin.seekg(0, ios::beg);
	ffin.read((char*)sbuf, length);
	
	ffin.close();
	is_null = 1;
	release();
	
	parse(sbuf);
	
	delete[] sbuf;
	is_null = 0;
	return 1;
}

void FFeather::parse(const char* content)
{
	
	string str = string(content);
	
	size_t loc0 = str.find_first_of('[',0);
	size_t loc1 = str.find_first_of(']', loc0);
	
	char* sbuf = new char[loc1-loc0-1];
	str.copy(sbuf, loc1 - loc0-1, loc0+1);
	countCurves(sbuf);
	
	m_patch_list = new DPatch[m_n_curve];
	parseNCurves(sbuf);
	delete[] sbuf;
	
	loc0 = str.find_first_of('[', loc1);
	loc1 = str.find_first_of(']', loc0);
	
	sbuf = new char[loc1-loc0-1];
	str.copy(sbuf, loc1 - loc0-1, loc0+1);

	parseP(sbuf);
	delete[] sbuf;
	
	loc0 = str.find_first_of('[', loc1);
	loc1 = str.find_first_of(']', loc0);
	
	loc0 = str.find_first_of('[', loc1);
	loc1 = str.find_first_of(']', loc0);
	
	sbuf = new char[loc1-loc0-1];
	str.copy(sbuf, loc1 - loc0-1, loc0+1);
	
	parseN(sbuf);
	delete[] sbuf;

	is_null = 0;
}

size_t findNewLineORSpace(const string& str, const size_t& start)
{
	size_t locnline = str.find_first_of('\n', start);
	size_t locspace = str.find_first_of(' ', start);
		
	if(locnline != string::npos && locspace != string::npos && locnline < locspace) return locnline;
	
	return locspace;
}

void FFeather::countCurves(const char*content)
{
	string str = string(content);
	
	size_t loc0, loc1;
	loc0 = loc1 = 0;
	
	m_n_curve = 0;
	while(loc1 != string::npos)
	{
		loc1 = findNewLineORSpace(str, loc0);
		
		if(loc1 != string::npos) 
		{
			m_n_curve++;
			loc0 = loc1+1;
		}
		else if(loc0<str.size()) 
		{
			m_n_curve++;
		}
	}
	cout<<"N feather "<<m_n_curve;
}

void FFeather::parseNCurves(const char*content)
{
	string str = string(content);
	
	int ncurve;
	size_t loc0, loc1;
	loc0 = loc1 = 0;
	
	char sbuf[8];
	int icurve = 0;
	while(loc1 != string::npos)
	{
		loc1 = findNewLineORSpace(str, loc0);
		
		if(loc1 != string::npos) 
		{
			str.copy(sbuf, loc1 - loc0+1, loc0);
			ncurve = 0;
			sscanf(sbuf, "%i", &ncurve);
		//cout<<" "<<ncurve;
		
			m_patch_list[icurve].nseg = ncurve;
			
			loc0 = loc1+1;
			
			icurve++;
		}
		else if(loc0<str.size()) 
		{
			str.copy(sbuf, str.size() - loc0, loc0);
			sscanf(sbuf, "%i", &ncurve);
			
			m_patch_list[icurve].nseg = ncurve;

		}
	}
}

void FFeather::parseP(const char*content)
{
	string str = string(content);
	
	size_t loc0, loc1;
	loc0 = loc1 = 0;
	
	int acc=0;

	int cv, comp;
	float val;
	char sbuf[16];

	int nblock = m_patch_list[0].nseg*3;
	int icurve = 0;
	while(loc1 != string::npos)
	{
		loc1 = findNewLineORSpace(str, loc0);
		
		if(loc1 != string::npos) 
		{
			str.copy(sbuf, loc1 - loc0, loc0);
			sscanf(sbuf, "%f", &val);
			
			cv = acc/3;
			comp = acc%3;
//cout<<" "<<m_patch_list[icurve].nseg;
			m_patch_list[icurve].cvs[cv].setcomp(comp, val);
			
			acc++;
			loc0 = loc1+1;
			
			if(acc==nblock)
			{
				icurve++;
				nblock = m_patch_list[icurve].nseg*3;
				acc = 0;
			}
		}
		else if(loc0<str.size()) 
		{
			str.copy(sbuf, loc1 - loc0, loc0);
			sscanf(sbuf, "%f", &val);
			
			cv = acc/3;
			comp = acc%3;
			m_patch_list[icurve].cvs[cv].setcomp(comp, val);
			acc++;
		}
	}
	
	m_bbox_min.set(10e8);
	m_bbox_max.set(-10e8);
	
	//vector<DPatch*>::iterator it;
	for(int i=0; i<m_n_curve; i++)
	{
		for(int j=0; j<m_patch_list[i].nseg; j++)
		{
			XYZ& cv = m_patch_list[i].cvs[j];
			if(cv.x < m_bbox_min.x) m_bbox_min.x = cv.x;
			if(cv.x > m_bbox_max.x) m_bbox_max.x = cv.x;
			if(cv.y < m_bbox_min.y) m_bbox_min.y = cv.y;
			if(cv.y > m_bbox_max.y) m_bbox_max.y = cv.y;
			if(cv.z < m_bbox_min.z) m_bbox_min.z = cv.z;
			if(cv.z > m_bbox_max.z) m_bbox_max.z = cv.z;
		}
	}
	
cout<<" "<<m_bbox_min.x<<" "<<m_bbox_max.x<<" "<<m_bbox_min.y<<" "<<m_bbox_max.y<<" "<<m_bbox_min.z<<" "<<m_bbox_max.z;

}

void FFeather::parseN(const char*content)
{
	string str = string(content);
	size_t loc0, loc1;
	loc0 = loc1 = 0;
	
	int comp = 0;
	float val;
	char sbuf[16];
	int i = 0;
	while(loc1 != string::npos)
	{
		loc1 = findNewLineORSpace(str, loc0);
		 
		if(loc1 != string::npos) 
		{
			str.copy(sbuf, loc1 - loc0, loc0);
			sscanf(sbuf, "%f", &val);
			
			m_patch_list[i].Up.setcomp(comp, val);
			comp++;
			loc0 = loc1+1;
			
			if(comp==3)
			{
				i++;
				comp = 0;
			}
		}
		else if(loc0<str.size()) 
		{
			str.copy(sbuf, loc1 - loc0, loc0);
			sscanf(sbuf, "%f", &val);
			
			m_patch_list[i].Up.setcomp(comp, val);
			comp++;
		}
	}
	cout<<"comp "<<comp;
	
	XYZ mean, vt;
	
	
	for(int i=0; i<m_n_curve; i++)
	{
		mean.set(0);
		m_patch_list[i].length = 0;
		for(int j=1; j<m_patch_list[i].nseg; j++)
		{
			vt = m_patch_list[i].cvs[j] - m_patch_list[i].cvs[j-1];
			mean += vt;
			m_patch_list[i].length += vt.length();
		}
		mean.normalize();
//cout<<" "<<mean.x<<" "<<mean.y<<" "<<mean.z;
		m_patch_list[i].Up.normalize();
		vt = m_patch_list[i].Up;
		m_patch_list[i].Up = vt.cross(mean);
		m_patch_list[i].Up.normalize();
//tup = (*it)->Up;
//cout<<" "<<tup.x<<" "<<tup.y<<" "<<tup.z;
	}
}

void FFeather::draw(float ratio)
{
	glBegin(GL_QUADS);
	float size;
	XYZ cen, vt;
	int acc=0;
	//vector<DPatch*>::iterator it;
	//for(it=m_patch_list.begin();it!=m_patch_list.end();it++)

	for(int i=0; i<m_n_curve; i++)
	{
		size = m_patch_list[i].length * ratio * 0.5;
		//if((*it)->length>4) cout<<" "<<acc;
		//for(int i=0; i<(*it)->nseg-1; i++)
		{
			cen = m_patch_list[i].cvs[0];
			
			vt = cen - m_patch_list[i].Up * size;
			glVertex3f(vt.x, vt.y, vt.z);
		
			vt = cen + m_patch_list[i].Up * size;
			glVertex3f(vt.x, vt.y, vt.z);
			
			cen = m_patch_list[i].cvs[m_patch_list[i].nseg-1];
		
			vt = cen + m_patch_list[i].Up * size;
			glVertex3f(vt.x, vt.y, vt.z);
		
			vt = cen - m_patch_list[i].Up * size;
			glVertex3f(vt.x, vt.y, vt.z);
			
			
		}
		acc++;
	}
	glEnd();
}
//:~