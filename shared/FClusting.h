#include <iostream>
#include <vector>
#include "zData.h"
using  namespace std;



class FClusting
{
public :
	FClusting() {}
	~FClusting() {}
	static void findMean(const qnode* data, int lo, int hi, XYZ& pos_mean, XYZ& nor_mean);
	static void calculateCovariance(const qnode* data, int lo, int hi, const XYZ& mean, MATRIX33F& covariance);
	//static void split(qnode* data, int count, vector <qnode>& children, vector <qnode>& parent);
	static void sort(qnode* data, int lo, int hi, XYZ* angles, int axis);
	static void sortNormal(qnode* data, int lo, int hi, float* alpha);
	//static void recursive_sort(qnode* data, XYZ* angles, float* alpha, int lo, int hi, int level, vector <qnode>& children, vector <qnode>& parent);
	static void calculateEigenSystem(const MATRIX33F& mat, MATRIX33F& eigenSystem);
	static void findZeroPlane(const XYZ* position, int lo, int hi, int& mid, int axis);
	static void findZeroNormal(const float* alpha, int lo, int hi, int& mid);
	static int findLargestAxis(qnode* data, XYZ* position, int lo, int hi, int& is_normal_based, XYZ& base);
	//static void testOcclusion(qnode* data, unsigned int* enable, int& count, const qnode* emitter);
	//static void testOcclusion(qnode& data, const qnode* emitter, float& value, float& threshold);
	
	static int QSplat(qnode* data, int count, qnode* result, XYZ bbox[8]);
	static void recursive_qsplat(qnode* data, int lo, int hi, qnode* result, int parent_id, int child_id, int& node_count);
	static float findBBox(const qnode* data, int lo, int hi, float& xmin, float& ymin, float& zmin, float& xmax, float& ymax, float& zmax);
	static float findBVol(const qnode* data, int lo, int hi);
	static void findOptimalDims(qnode* data, XYZ* position, int lo, int hi, int& first, int& second);
	static int splitByValue(qnode* data, int lo, int hi, int axis, float value);
	//static int splitByValue(qnode* data, int lo, int hi, int axis, float min, float max);
	static void findMean(const qnode* data, int lo, int hi, XYZ& pos_mean, XYZ& nor_mean, XYZ& col_mean, float& area, float& cone);
	static void findMean(const qnode* data, int lo, int hi, XYZ& pos_mean);
	static void sort(pcdSample* data, int lo, int hi);
	static int findLongestAxis(float min[3], float max[3], int& second);
	static int findLongestAxis(float min[3], float max[3]);
	static int findSmallestVolume(float vol[3]);
	static void sort(qnode* data, int lo, int hi, int axis);
	static int sortByValue(qnode* data, int lo, int hi, int axis, float value);
	static void calculateObjectSpace(qnode* data, int lo, int hi, MATRIX44F& space);
	static void quick_sort(qnode* data, int lo, int hi, float* value);
	static void convertToMac( char* data, int length);
	static void occlusion(const XYZ& origin, const XYZ& dir, float threshold, const qnode* emitter, float& value);
	static void shadow(const XYZ& origin, const XYZ& dir, float threshold, const qnode* emitter, float& value);
	static void occlusionBentNormal(const XYZ& origin, const XYZ& dir, float threshold, const qnode* emitter, float& value, XYZ& bent_n);
	
private :

};