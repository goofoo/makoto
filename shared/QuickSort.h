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

class QuickSort
{
public:
	QuickSort() {}
	~QuickSort() {}
	
	static void sort(ValueAndId *array,int first,int last);
};