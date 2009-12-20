#pragma once

#include <iostream>
#include <stdlib.h>

class QuickSortIdx
{
public:
	QuickSortIdx() {}
	~QuickSortIdx() {}
	
	static void sort(float *key, unsigned int *idx, long first, long last);
};