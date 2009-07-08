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

class QuickSort
{
public:
	QuickSort() {}
	~QuickSort() {}
	
	static void sort(ValueAndId *array,int first,int last);
	static void sortX(PosAndId *array,int first,int last);
	static void sortY(PosAndId *array,int first,int last);
	static void sortZ(PosAndId *array,int first,int last);
};