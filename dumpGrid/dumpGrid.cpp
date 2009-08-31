///:
// dumpdata.cpp - code for the DSO Shadeop writing P values of shading point to a specific file.
// 
// Date: 01/19/08
// 
// Thanks Larry Gritz for his "bake" program in the SIGGRAPH 2002 course notes.
//

#include <shadeop.h>
#include "../shared/zMath.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "../shared/zString.h"
#define BLOCKSIZE 955368
using namespace std;

class CDumpData {

public :
	CDumpData();
	int isInitialized() {return m_initialized;}
	void initialize(string _filename );
	void addpoint(const float *p, const float *n, const float *c, float a);
	virtual	~CDumpData ();


private :
	string filename;
	pcdSample* sampleArray;
	int writedata();
	int m_initialized;
	unsigned int m_num_samp, m_block_size, m_refresh;
};


CDumpData::CDumpData():
m_initialized(0),
sampleArray(0),
m_num_samp(0), m_block_size(0), m_refresh(1)
{}
	
// class destructor 
//
CDumpData::~CDumpData ()
{
	writedata();
	if(writedata() != 0) cout<<"Dumped "<<m_num_samp<<" samples to "<<filename.c_str();
	if(sampleArray) delete[] sampleArray;
}

void CDumpData::initialize(string _filename)
{ 
	filename = _filename;
	m_initialized = 1;
	sampleArray = new pcdSample[BLOCKSIZE];
}

int CDumpData::writedata()
{
	ofstream myfile;
	
	if(m_refresh==1)
	{
		myfile.open(filename.c_str(), ios::out | ios::binary);
		m_refresh = 0;
	}
	else
	{
		myfile.open(filename.c_str(), ios::out | ios::binary | ios::app);
	}
		
	if(!myfile.is_open()) 
	{
		cout<<"Cannot open "<<filename<<" to output!\n";	
		return 0;
	}
		

	myfile.write((char*)sampleArray, (m_block_size)*sizeof(pcdSample));

	myfile.close();

	m_block_size = 0;
	
	return 1;

}

// - write the buffer to file if block is full
void CDumpData::addpoint(const float *p, const float *n, const float *c, float a)
{
	XYZ tn(n[0],n[1],n[2]);
	normalize(tn);
	
	sampleArray[m_block_size] = pcdSample(p[0],p[1],p[2],tn.x, tn.y, tn.z,c[0],c[1],c[2], a);
	m_block_size++;
	m_num_samp++;
	
	if(m_block_size==BLOCKSIZE) writedata();
}


// declare the DSO Shadeop in "C"
//
extern "C" SHADEOP_TABLE(dumpGrid) = 
{ 
    { "void dumpData_f (string, point, normal, color, float)", "init", "cleanup"},
    { "" }  
};

// initialization function
// build and return the data block
//
extern "C" SHADEOP_INIT(init)
{
	CDumpData *data = new CDumpData;
	return data;
}

// shutdown function called after rendering is complete
// destroy the data block
//
extern "C" SHADEOP_CLEANUP(cleanup)
{
	CDumpData *db = ( CDumpData *) initdata;
	delete db;
}

// actual implementation of the shadeop
// take the arguments and call the routines
//
extern "C" SHADEOP(dumpData_f) 
{
	std::string file_name (*((char **) argv[1]));
	float *in_p = (float *)argv[2];
	float *in_n = (float *)argv[3];
	float *in_c = (float *)argv[4];
	float *in_area = (float *)argv[5];
	
	CDumpData *db = ( CDumpData *) initdata;
	
	if(db->isInitialized() != 1) 
	{
		windowspath(file_name);
		db->initialize(file_name);
	}

	db->addpoint(in_p, in_n, in_c, *in_area);
	
	return 0;
}
///:~