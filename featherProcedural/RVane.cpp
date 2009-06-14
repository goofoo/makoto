#include "RVane.h"
#include "../shared/zGlobal.h"
#include "../shared/zNoise.h"
#include "../shared/FBSpline.h"
#include "FVane.h"

#define NUMCV 10
#define NUMPARAM 8
#define NUMSEG 5

float ang_offset[NUMSEG] = {9.f, -13.f, -11.f, -9.f, 9.f};

#define NUMSPLCV 7

float plencv[NUMSPLCV] = {.1f, 1.f, 1.f, 1.03f, 1.05f, .95f, .05f};
float pangcv[NUMSPLCV] = {40.f, 40.f, 40.f, 40.f, 40.f, 60.f, 90.f};


RVane::RVane(std::string& parameter):
m_g_width(1.f)
{
	m_cache_name = new char[256];

	int n = sscanf(parameter.c_str(), "%s %f %f %f %f %d", 
	m_cache_name, 
	&m_g_width, 
	&m_shutter_open, &m_shutter_close,
	&m_fps,
	&m_is_blur);
	
	map = new FXMLVaneMap(m_cache_name);
}

RVane::~RVane() 
{
	delete[] m_cache_name;
	delete map;
}

void RVane::generateRIB(RtFloat detail)
{
	int g_seed = 13;
	float bnoi, vnoi;

	FVane* fv = new FVane();
	FBSpline* lenspline = new FBSpline();
	lenspline->create(NUMSPLCV, plencv);
	lenspline->setEnd(-5,-5);
	
	FBSpline* angspline = new FBSpline();
	angspline->create(NUMSPLCV, pangcv);
	angspline->setEnd(30, 90);
	
	int n_cv=0;
	int n_curve, n_curve_up, n_curve_down;
	int jn;
	float epsilon_up, epsilon_down, S, T, rT, ss, sv, vane_length, rang, lsegup, lsegdown, rot;
	while(map->getVane(n_cv))
	{
		
		XYZ* plist = new XYZ[n_cv];
		XYZ* ulist = new XYZ[n_cv];
		XYZ* dlist = new XYZ[n_cv];
		XYZ* nknot = new XYZ[n_cv];

		map->getData(plist, ulist, dlist, nknot);
		
		fv->create(n_cv, plist, ulist, dlist, nknot);
		
		vane_length = fv->getLength();
		
		n_curve_up = vane_length*0.99/m_g_width/1.19;//n_curve_up/=10;
		epsilon_up = 1.0/n_curve_up;
		
		n_curve_down = n_curve_up;//n_curve_down/=10;
		epsilon_down = epsilon_up;
		
		n_curve = n_curve_up + n_curve_down;
		
		int* nvertices = new int[n_curve];
		float* widtharray = new float[n_curve*NUMPARAM];
		
// setup vertex width
		for(int i=0; i<n_curve; i++)
		{
			nvertices[i] = NUMCV;
			widtharray[i*NUMPARAM]= m_g_width*1.f;
			widtharray[i*NUMPARAM+1]= m_g_width*1.f;
			widtharray[i*NUMPARAM+2]= m_g_width*1.f;
			widtharray[i*NUMPARAM+3]= m_g_width*1.f;
			widtharray[i*NUMPARAM+4]= m_g_width*1.f;
			widtharray[i*NUMPARAM+5]= m_g_width*1.f;
			widtharray[i*NUMPARAM+6]= m_g_width*0.8f;
			widtharray[i*NUMPARAM+7]= m_g_width*0.3f;
		}

		float unitsup = ulist[0].length()*2;
		float unitsdown = dlist[0].length()*2;
		float unitt = fv->getLength();
		
// get up curve seg length
		float glsegup = ulist[0].length()*1.414f/NUMSEG;
		
		
// get down curve seg length
		float glsegdown = dlist[0].length()*1.414f/NUMSEG;
		
// set mean angle
		float ang_mean;
		
		XYZ* pp = new XYZ[n_curve*NUMCV];
		XYZ* pnormal = new XYZ[n_curve*NUMPARAM];
		float* pCoordS = new float[n_curve*NUMPARAM];
		float* pCoordT = new float[n_curve*NUMPARAM];
		
		bnoi = randfint( g_seed )*0.13;
		g_seed++;
		vnoi = randfint( g_seed );
		g_seed++;
		
		
		int vxacc = 0, paramacc = 0;

		for(int i=0; i<n_curve_up; i++)
		{
// init start coordinate
			S = 0.5;
			T = epsilon_up*i;
			
			lsegup = glsegup*lenspline->getValue(T);
			ang_mean = angspline->getValue(T)*0.01745331;
			
			if(T>bnoi)
			{
				vnoi = randfint( g_seed );
				g_seed++;
				bnoi += randfint( g_seed )*0.1;
				g_seed++;
				
			}
			
			if(i%7==0) 
			{
				rot = (randfint( g_seed )-.5) * 0.06; g_seed++;
			}
			
			ang_mean = angspline->getValue(T)*0.01745331 + vnoi*.05f;
						
			pp[vxacc] = fv->getPointAt(S, T, pnormal[paramacc]);
			pp[vxacc+1] = pp[vxacc+2] = pp[vxacc];
			pnormal[paramacc+1] = pnormal[paramacc];
			pCoordS[paramacc] = pCoordS[paramacc+1] = S;
			pCoordT[paramacc] = pCoordT[paramacc+1] = 1.-T;
			
			for(int j=1; j<=NUMSEG; j++)
			{
				rang = ang_offset[j-1]* 0.01745331; 
				
				if(j>3)
				{
					rang += rot;
				}
				
				S -= lsegup*cos(ang_mean + rang)/unitsup;
				T += lsegup*sin(ang_mean + rang)/unitt;

				pp[vxacc+j+2] = fv->getPointAt(S, T, pnormal[paramacc+j+1]);
				pCoordS[paramacc+j+1] = S;
				pCoordT[paramacc+j+1] = 1.-T;
			}
			
			pp[vxacc+NUMSEG+3] = pp[vxacc+NUMSEG+4] = pp[vxacc+NUMSEG+2];
			pnormal[paramacc+NUMSEG+2] = pnormal[paramacc+NUMSEG+1];
			pCoordS[paramacc+NUMSEG+2] = S;
			pCoordT[paramacc+NUMSEG+2] = 1.-T;
			
			vxacc += NUMCV;
			paramacc += NUMPARAM;
		}
		
		bnoi = randfint( g_seed )*0.13;
		g_seed++;
		vnoi = randfint( g_seed );
		g_seed++;
		
		
		
		for(int i=0; i<n_curve_down; i++)
		{
// init start coordinate
			S = 0.5;
			T = epsilon_down*i;
			
			lsegdown = glsegdown*lenspline->getValue(T);
			ang_mean = angspline->getValue(T)*0.01745331;
			
			if(T>bnoi)
			{
				vnoi = randfint( g_seed );
				g_seed++;
				bnoi += randfint( g_seed )*0.13;
				g_seed++;
				
			}
			
			if(i%7==0) 
			{
				rot = (randfint( g_seed )-.5) * 0.06; g_seed++;
			}
			
			ang_mean = angspline->getValue(T)*0.01745331 + vnoi*.05f;
						
			pp[vxacc] = fv->getPointAt(S, T, pnormal[paramacc]);
			pp[vxacc+1] = pp[vxacc+2] = pp[vxacc];
			pnormal[paramacc+1] = pnormal[paramacc];
			pCoordS[paramacc] = pCoordS[paramacc+1] = S;
			pCoordT[paramacc] = pCoordT[paramacc+1] = 1.-T;
			
			for(int j=1; j<=NUMSEG; j++)
			{
				rang = ang_offset[j-1]* 0.01745331; 
				
				if(j>3) rang += rot;
				
				S += lsegdown*cos(ang_mean + rang)/unitsdown;
				T += lsegdown*sin(ang_mean + rang)/unitt;

				pp[vxacc+j+2] = fv->getPointAt(S, T, pnormal[paramacc+j+1]);
				pCoordS[paramacc+j+1] = S;
				pCoordT[paramacc+j+1] = 1.-T;
			}
			
			pp[vxacc+NUMSEG+3] = pp[vxacc+NUMSEG+4] = pp[vxacc+NUMSEG+2];
			pnormal[paramacc+NUMSEG+2] = pnormal[paramacc+NUMSEG+1];
			pCoordS[paramacc+NUMSEG+2] = S;
			pCoordT[paramacc+NUMSEG+2] = 1.-T;
			
			vxacc += NUMCV;
			paramacc += NUMPARAM;
		}

		RiCurves ( "cubic", (RtInt)n_curve, (RtInt*) nvertices, "nonperiodic", "P", (RtPoint*)pp, "width",  (RtFloat*)widtharray, "varying normal surface_n", (RtPoint*)pnormal, "varying float s", (RtFloat*)pCoordS, "varying float t", (RtFloat*)pCoordT, RI_NULL);
		
		delete[] pp;
		delete[] pnormal;
		delete[] pCoordS;
		delete[] pCoordT;
		delete[] nvertices;
		delete[] widtharray;
		
		delete[] plist;
		delete[] ulist;
		delete[] dlist;
		delete[] nknot;
		
		map->next();
	}
	
	return;
}
//:~