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

void QuickSort::sortDeep(DataAndId *array,int first,int last)
{
	if(last < first) return;
	
	int low,high;
	float list_separator;
	DataAndId temp;

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
	
	if(first<high) sortDeep(array,first,high);
	if(low<last) sortDeep(array,low,last);
}

void QuickSort::sortX(PosAndId *array,int first,int last)
{
	if(first<last){
	int low,high;float list_separator;
	PosAndId temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].pos.x;
	do
	{
		while(array[low].pos.x<list_separator) low++;
		while(array[high].pos.x>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	}while(low<=high);
	if(first<high)
		sortX(array,first,high);
	if(low<last)
		sortX(array,low,last); }
}

void QuickSort::sortY(PosAndId *array,int first,int last)
{
	if(first<last){
	int low,high;float list_separator;
	PosAndId temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].pos.y;
	do
	{
		while(array[low].pos.y<list_separator) low++;
		while(array[high].pos.y>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	}while(low<=high);
	if(first<high)
		sortY(array,first,high);
	if(low<last)
		sortY(array,low,last);}
}

void QuickSort::sortZ(PosAndId *array,int first,int last)
{
	if(first<last){
	int low,high;float list_separator;
	PosAndId temp;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].pos.z;
	do
	{
		while(array[low].pos.z<list_separator) low++;
		while(array[high].pos.z>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
		}
	}while(low<=high);
	if(first<high)
		sortZ(array,first,high);
	if(low<last)
		sortZ(array,low,last);}
}

void QuickSort::sortX(RGRID *array, unsigned *idx, int first,int last)
{
	if(first<last){
	int low,high;float list_separator;
	RGRID temp;
	unsigned tmpidx;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].pos.x;
	do
	{
		while(array[low].pos.x<list_separator) low++;
		while(array[high].pos.x>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
			
			tmpidx = idx[low];
			idx[low++] = idx[high];
			idx[high--] = tmpidx;
		}
	}while(low<=high);
	if(first<high)
		sortX(array, idx, first,high);
	if(low<last)
		sortX(array, idx, low,last); }
}

void QuickSort::sortY(RGRID *array, unsigned *idx, int first,int last)
{
	if(first<last){
	int low,high;float list_separator;
	RGRID temp;
	unsigned tmpidx;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].pos.y;
	do
	{
		while(array[low].pos.y<list_separator) low++;
		while(array[high].pos.y>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
			
			tmpidx = idx[low];
			idx[low++] = idx[high];
			idx[high--] = tmpidx;
		}
	}while(low<=high);
	if(first<high)
		sortY(array, idx, first,high);
	if(low<last)
		sortY(array, idx, low,last);}
}

void QuickSort::sortZ(RGRID *array, unsigned *idx, int first,int last)
{
	if(first<last){
	int low,high;float list_separator;
	RGRID temp;
	unsigned tmpidx;

	low = first;
	high = last;
	list_separator = array[(first+last)/2].pos.z;
	do
	{
		while(array[low].pos.z<list_separator) low++;
		while(array[high].pos.z>list_separator) high--;

		if(low<=high)
		{
			temp = array[low];
			array[low++] = array[high];
			array[high--]=temp;
			
			tmpidx = idx[low];
			idx[low++] = idx[high];
			idx[high--] = tmpidx;
		}
	}while(low<=high);
	if(first<high)
		sortZ(array, idx, first,high);
	if(low<last)
		sortZ(array, idx, low,last);}
}
//:~