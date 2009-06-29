#include "QuickSort.h"

void QuickSort::sort(ValueAndId *array,int first,int last)
{
	if(last < first) return;
	
	int low,high;
	float list_separator;
	ValueAndId temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].val;
	do
	{
		while(array[low].val<list_separator) low++;
		while(array[high].val>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	} while(low<=high);
	
	if(first<high) sort(array,first,high);
	if(low<last) sort(array,low,last);
}
//:~