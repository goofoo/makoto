#include "RFeather.h"
#include "../shared/zGlobal.h"
#include "../shared/FBSpline.h"
#include "../shared/zNoise.h"

#include "FBend.h"
#define MAX_NUMPARAM 32
RtInt nu = 2;
RtInt uorder = 2;
RtFloat uknot[4] = {0,0,1,1};
RtFloat umin = 0;
RtFloat umax = 1;
	
RtInt nv = 11;
RtInt vorder = 4;
RtFloat vknot[15] = {0,0,0,0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1,1,1,1};
RtFloat vmin = 0;
RtFloat vmax = 1;

RtToken tags[] = {"creasemethod", "interpolateboundary"};
RtInt nargs[] = {0,0,1,1,0,0};
RtInt intargs[] = {1};
RtFloat* floatargs;
RtToken stringargs[] = {"chaikin"};

int nvertices[NUMBENDSEG] = {4, 4, 4};
int vertices[4*NUMBENDSEG] = {0,1,3,2,2,3,5,4,4,5,7,6};
float scoord[4*NUMBENDSEG] = {1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0};
float tcoord[4*NUMBENDSEG] = {1.0, 1.0, 0.66, 0.66, 0.66, 0.66, 0.33, 0.33, 0.33, 0.33, 0.0, 0.0};
float roots[4*NUMBENDSEG];
float roott[4*NUMBENDSEG];

RFeather::RFeather(std::string& parameter):
m_n_dice(0.5f), m_f_wh(0.25f),feather_close(0)
{
	m_cache_name = new char[256];

	int n = sscanf(parameter.c_str(), "%s %f %f %f %f %f %f %d %d", 
	m_cache_name, 
	&m_n_dice, 
	&m_f_wh,
	&m_thickness, 
	&m_shutter_open, &m_shutter_close,
	&m_fps,
	&m_is_blur,
	&m_is_shd);
	
	mesh = new FXMLTriangleMap(m_cache_name);
	feather = new DFeather();
	feather->setWidth(m_f_wh);
	if(m_is_shd!=1) mesh->dice(m_n_dice, feather);
	else mesh->diceNoRT(m_n_dice, feather);
	
	if(m_is_blur==1)
	{
		string cclose(m_cache_name);
		zGlobal::changeFrameNumber(cclose, int(m_shutter_open*m_fps)+1);
		if(mesh->load(cclose.c_str()) ==1)
		{
			feather_close = new DFeather();
			feather_close->setWidth(m_f_wh);
			mesh->diceNoRT(m_n_dice, feather_close);
		}
	}
}

RFeather::~RFeather() 
{
	delete[] m_cache_name;
}

#define NUMCV 10
#define NUMPARAM 8
#define NUMSEG 6
#define NUMSPLCV 7

void RFeather::generateRIB(RtFloat detail)
{
	if(m_is_shd!=1) RiSides(2);
	else RiSides(1);

	RtToken paramname[MAX_NUMPARAM];
	RtPointer paramvalue[MAX_NUMPARAM];
	
	MATRIX44F hair_space;
	XYZ wind, pw, side;
	float length, curl, root_s, root_t, noil;
	
	int n_curve;
	FBend* fb = new FBend();
	int num_hair = feather->getNumHair();
	
	n_curve = num_hair;
	
	float delta = (m_shutter_close-m_shutter_open)*m_fps;
	
	if(m_is_shd!=1)
	{
		XYZ* parray = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* parray_close = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col0 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col1 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col2 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col3 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col4 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col5 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col6 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col7 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col8 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col9 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col10 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col11 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col12 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col13 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col14 = new XYZ[2*(NUMBENDSEG+1)];
		XYZ* col15 = new XYZ[2*(NUMBENDSEG+1)];
		
		int g_seed = 13;
		CoeRec root_rt;
		for(int i=0; i<num_hair; i++)
		{
			feather->getAParam(i, hair_space, length, wind, side, curl, root_s, root_t, root_rt);
			
			float noil = 1.f+(randfint( g_seed )-0.5)*0.13;
			g_seed++;
			
			length *= noil;
			
			fb->create(length, wind.x, wind.y+length*(curl*noil-0.5)*2);
			
			for(int j=0; j<NUMBENDSEG+1; j++)
			{
				fb->getPoint(j, pw);
				hair_space.transform(pw);

				parray[j*2] = pw - side*length*m_f_wh*0.5;
				parray[j*2+1] = pw + side * length*m_f_wh*0.5;
			}
			
			for(int j=0; j<4*NUMBENDSEG; j++)
			{
				roots[j]=root_s;
				roott[j] = 1.f-root_t;
			}
			
			paramname[0] = "P";
			paramvalue[0] = (RtPoint*)parray;
			
			paramname[1] = "facevarying float s";
			paramvalue[1] = (RtFloat*)scoord;
			
			paramname[2] = "facevarying float t";
			paramvalue[2] = (RtFloat*)tcoord;
			
			paramname[3] = "facevarying float root_s";
			paramvalue[3] = (RtFloat*)roots;
			
			paramname[4] = "facevarying float root_t";
			paramvalue[4] = (RtFloat*)roott;
			
			for(int j=0; j<2*(NUMBENDSEG+1);j++) 
			{
				col0[j] = root_rt.data[0];
				col1[j] = root_rt.data[1];
				col2[j] = root_rt.data[2];
				col3[j] = root_rt.data[3];
				col4[j] = root_rt.data[4];
				col5[j] = root_rt.data[5];
				col6[j] = root_rt.data[6];
				col7[j] = root_rt.data[7];
				col8[j] = root_rt.data[8];
				col9[j] = root_rt.data[9];
				col10[j] = root_rt.data[10];
				col11[j] = root_rt.data[11];
				col12[j] = root_rt.data[12];
				col13[j] = root_rt.data[13];
				col14[j] = root_rt.data[14];
				col15[j] = root_rt.data[15];
			}
			paramname[5] = "vertex color coeff0";
			paramvalue[5] = (RtFloat*)col0;
			paramname[6] = "vertex color coeff1";
			paramvalue[6] = (RtFloat*)col1;
			paramname[7] = "vertex color coeff2";
			paramvalue[7] = (RtFloat*)col2;
			paramname[8] = "vertex color coeff3";
			paramvalue[8] = (RtFloat*)col3;
			paramname[9] = "vertex color coeff4";
			paramvalue[9] = (RtFloat*)col4;
			paramname[10] = "vertex color coeff5";
			paramvalue[10] = (RtFloat*)col5;
			paramname[11] = "vertex color coeff6";
			paramvalue[11] = (RtFloat*)col6;
			paramname[12] = "vertex color coeff7";
			paramvalue[12] = (RtFloat*)col7;
			paramname[13] = "vertex color coeff8";
			paramvalue[13] = (RtFloat*)col8;
			paramname[14] = "vertex color coeff9";
			paramvalue[14] = (RtFloat*)col9;
			paramname[15] = "vertex color coeff10";
			paramvalue[15] = (RtFloat*)col10;
			paramname[16] = "vertex color coeff11";
			paramvalue[16] = (RtFloat*)col11;
			paramname[17] = "vertex color coeff12";
			paramvalue[17] = (RtFloat*)col12;
			paramname[18] = "vertex color coeff13";
			paramvalue[18] = (RtFloat*)col13;
			paramname[19] = "vertex color coeff14";
			paramvalue[19] = (RtFloat*)col14;
			paramname[20] = "vertex color coeff15";
			paramvalue[20] = (RtFloat*)col15;
			
			if(m_is_blur ==1 && feather_close) 
			{
				RiMotionBegin(2, m_shutter_open, m_shutter_close);
				RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)3,  (RtInt*)nvertices, (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)21, paramname, paramvalue );
				
				feather_close->getAParam(i, hair_space, length, wind, side, curl, root_s, root_t);
				fb->create(length, wind.x, wind.y+length*(curl*noil-0.5)*2);
				
				for(int j=0; j<NUMBENDSEG+1; j++)
				{
					fb->getPoint(j, pw);
					hair_space.transform(pw);

					parray_close[j*2] = pw - side*length*m_f_wh*0.5;
					parray_close[j*2+1] = pw + side * length*m_f_wh*0.5;
					
					parray[j*2] += (parray_close[j*2] - parray[j*2])*delta;
					parray[j*2+1] += (parray_close[j*2+1] - parray[j*2+1])*delta;
				}
			
				paramvalue[0] = (RtPoint*)parray;

				RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)3,  (RtInt*)nvertices, (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)21, paramname, paramvalue );
				RiMotionEnd();
			}
			else RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)3,  (RtInt*)nvertices, (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)21, paramname, paramvalue );
		}

		delete[] parray;
		delete[] parray_close;
		delete[] col0;
		delete[] col1;
		delete[] col2;
		delete[] col3;
		delete[] col4;
		delete[] col5;
		delete[] col6;
		delete[] col7;
		delete[] col8;
		delete[] col9;
		delete[] col10;
		delete[] col11;
		delete[] col12;
		delete[] col13;
		delete[] col14;
		delete[] col15;
	}
	else
	{
		XYZ* parray = new XYZ[2*(NUMBENDSEG+1)];
		
		int g_seed = 13;
		for(int i=0; i<num_hair; i++)
		{
			feather->getAParam(i, hair_space, length, wind, side, curl, root_s, root_t);
			
			float noil = 1.f+(randfint( g_seed )-0.5)*0.13;
			g_seed++;
			
			length *= noil;
			
			fb->create(length, wind.x, wind.y+length*(curl*noil-0.5)*2);
			
			for(int j=0; j<NUMBENDSEG+1; j++)
			{
				fb->getPoint(j, pw);
				hair_space.transform(pw);

				parray[j*2] = pw - side*length*m_f_wh*0.5;
				parray[j*2+1] = pw + side * length*m_f_wh*0.5;
			}
			
			for(int j=0; j<4*NUMBENDSEG; j++)
			{
				roots[j]=root_s;
				roott[j] = 1.f-root_t;
			}
			
			paramname[0] = "P";
			paramvalue[0] = (RtPoint*)parray;
			
			paramname[1] = "facevarying float s";
			paramvalue[1] = (RtFloat*)scoord;
			
			paramname[2] = "facevarying float t";
			paramvalue[2] = (RtFloat*)tcoord;
			
			paramname[3] = "facevarying float root_s";
			paramvalue[3] = (RtFloat*)roots;
			
			paramname[4] = "facevarying float root_t";
			paramvalue[4] = (RtFloat*)roott;
			
			RiHierarchicalSubdivisionMeshV("catmull-clark", (RtInt)3,  (RtInt*)nvertices, (RtInt*)vertices, (RtInt)2, tags, nargs, intargs, floatargs, stringargs, (RtInt)5, paramname, paramvalue );
		}

		delete[] parray;
	}
	return;
}
//:~