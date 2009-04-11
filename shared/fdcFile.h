#pragma once
#include "../shared/zData.h"
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

class fdcFile
{
public:
	fdcFile(void);
	~fdcFile(void);
	
	static int save(const char* filename, 
			const MVectorArray &points,
			const MVectorArray &velocities,
			const MDoubleArray &ages);
			
	static int load(const char* filename, 
			MVectorArray &points,
			MVectorArray &velocities,
			MDoubleArray &ages);
	
private:
	
};