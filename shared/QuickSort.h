#pragma once

#include <iostream>
#include <stdlib.h>
#include "zData.h"
using namespace std;

struct ValueAndId
{
	float val;
	unsigned idx;
};

struct PosAndId
{
	XYZ pos;
	unsigned idx;
};

struct DataAndId
{
	XYZ pos;
	unsigned idx;
	float val;
	float size;
};

class QuickSort
{
public:
	QuickSort() {}
	~QuickSort() {}
	
	static void sort(ValueAndId *array,int first,int last);
	static void sortX(PosAndId *array,int first,int last);
	static void sortY(PosAndId *array,int first,int last);
	static void sortZ(PosAndId *array,int first,int last);
	static void sortDeep(DataAndId *array,int first,int last);
	static void sortX(RGRID *array, unsigned *idx, int first,int last);
	static void sortY(RGRID *array, unsigned *idx, int first,int last);
	static void sortZ(RGRID *array, unsigned *idx, int first,int last);
};