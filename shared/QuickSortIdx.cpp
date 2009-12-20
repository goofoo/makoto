#include "QuickSortIdx.h"

void QuickSortIdx::sort(float *key, unsigned int *idx, long first,long last)
{
	if(last < first) return;
	
	int low,high;
	float key_separator;
	float temp_key;
	unsigned int temp_idx;

	low = first;
	high = last;
	key_separator = key[(first+last)/2];
	do
	{
		while(key[low] < key_separator) low++;
		while(key[high] > key_separator) high--;

		if(low<=high) {
			temp_key = key[low];
			temp_idx = idx[low];
			
			idx[low] = idx[high];
			key[low++] = key[high];
			
			idx[high] = temp_idx;
			key[high--] = temp_key;
			
			
		}
	} while(low<=high);
	
	if(first<high) sort(key, idx, first, high);
	if(low<last) sort(key, idx, low, last);
}
//:~