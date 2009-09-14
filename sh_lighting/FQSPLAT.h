#ifndef _FQSPLAT_H
#define _FQSPLAT_H
#include "../shared/zData.h"
#include <string>
#include <fstream>
#include "VisibilityBuffer.h"
using namespace std;

struct HierarchyQSPLATNode
{
	HierarchyQSPLATNode():child0(0), child1(0), child2(0), child3(0), is_leaf(1) {}
	float cone, r;
	unsigned index;
	XYZ mean, nor;
	char is_leaf;
	HierarchyQSPLATNode *child0;
	HierarchyQSPLATNode *child1;
	HierarchyQSPLATNode *child2;
	HierarchyQSPLATNode *child3;
};

template <class T>
class DivideBy
{
public:
	void X(pcdSample* data, const int lo, const int hi, const float value, int& end, int& start)
	{
		if(lo==hi) {end=start= lo; return;}
		pcdSample h;
		int i=lo, j=hi;

		do
		{
			while(data[i].pos.x <= value && i<hi) {i++;}
			while(data[j].pos.x > value && j>lo) {j--;}
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				i++; j--;
			}
			
			
		} while (i<=j);
		
		if(j<lo){ end=start= lo; return; }
		if(i>hi){ end=start= hi; return; }

		end=j; start = i;
	}
	
	void Y(pcdSample* data, const int lo, const int hi, const float value, int& end, int& start)
	{
		if(lo==hi) {end=start= lo; return;}
		pcdSample h;
		int i=lo, j=hi;

		do
		{
			while(data[i].pos.y <= value && i<hi) i++;
			while(data[j].pos.y > value && j>lo) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				i++; j--;
			}
		} while (i<=j);
		
		if(j<lo){ end=start= lo; return;}
		if(i>hi){ end=start= hi; return;}

		end=j; start = i;
	}
	
	void Z(pcdSample* data, const int lo, const int hi, const float value, int& end, int& start)
	{
		if(lo==hi) {end=start= lo; return;}
		pcdSample h;
		int i=lo, j=hi;

		do
		{
			while(data[i].pos.z <= value && i<hi) i++;
			while(data[j].pos.z > value && j>lo) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				i++; j--;
			}
		} while (i<=j);
		
		if(j<lo){ end=start= lo; return;}
		if(i>hi){ end=start= hi; return;}

		end=j; start = i;
	}
	
	void X(pcdSample* data, T* a_data, const int lo, const int hi, const float value, int& end, int& start)
	{
		if(lo==hi) {end=start= lo; return;}
		pcdSample h;
		T ch;
		int i=lo, j=hi;

		do
		{
			while(data[i].pos.x <= value && i<hi) {i++;}
			while(data[j].pos.x > value && j>lo) {j--;}
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				ch = a_data[i]; a_data[i]= a_data[j]; a_data[j]=ch;
				i++; j--;
			}
			
			
		} while (i<=j);
		
		if(j<lo){ end=start= lo; return; }
		if(i>hi){ end=start= hi; return; }

		end=j; start = i;
	}
	
	void Y(pcdSample* data, T* a_data, const int lo, const int hi, const float value, int& end, int& start)
	{
		if(lo==hi) {end=start= lo; return;}
		pcdSample h;
		T ch;
		int i=lo, j=hi;

		do
		{
			while(data[i].pos.y <= value && i<hi) i++;
			while(data[j].pos.y > value && j>lo) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				ch = a_data[i]; a_data[i]= a_data[j]; a_data[j]=ch;
				i++; j--;
			}
		} while (i<=j);
		
		if(j<lo){ end=start= lo; return;}
		if(i>hi){ end=start= hi; return;}

		end=j; start = i;
	}
	
	void Z(pcdSample* data, T* a_data, const int lo, const int hi, const float value, int& end, int& start)
	{
		if(lo==hi) {end=start= lo; return;}
		pcdSample h;
		T ch;
		int i=lo, j=hi;

		do
		{
			while(data[i].pos.z <= value && i<hi) i++;
			while(data[j].pos.z > value && j>lo) j--;
			if(i<=j)
			{
				h=data[i]; data[i]=data[j]; data[j]=h;
				ch = a_data[i]; a_data[i]= a_data[j]; a_data[j]=ch;
				i++; j--;
			}
		} while (i<=j);
		
		if(j<lo){ end=start= lo; return;}
		if(i>hi){ end=start= hi; return;}

		end=j; start = i;
	}
};

class FQSPLAT
{
public:
	FQSPLAT();
	~FQSPLAT(void);

	void create(pcdSample* rec, int n_rec);
	void createNoColor(pcdSample* rec, int n_rec);
	void createRT(pcdSample* rec, CoeRec* coe_rec, int n_rec);
	void createRTandColor(pcdSample* rec, CoeRec* coe_rec, int n_rec);

	//void setDrawLevel(unsigned short val) {m_draw_level = val;}
	//void finerDraw() {m_draw_level++;}
	//void coarserDraw() {m_draw_level--;}
	//void setMaxLevel(unsigned short val) {m_max_level = val;}
	
/*Write hemispherical visibility*/
	void lookup(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const;
	void lookupNoDist(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const;
	void lookupInterreflect(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility) const;
	void lookupSubsurface(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const;
	void lookupSubsurfaceB(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const;
	void lookupLight(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility) const;
	
	char isNull() {return is_null;}
	/*void getCenterAndSize(XYZ& center, float& size) const
	{
		center = m_himap[0].mean;
		size = m_himap[0].r;
	}*/
	
	unsigned int getNumSplat() {return m_num_hipix;}
	unsigned int getNumSplat() const {return m_num_hipix;}
	unsigned int getNumLeaf() const {return m_num_leaf;}
	
	/*void getCenterAndNormal(const int id, XYZ& center, XYZ& normal) const
	{
		center = m_himap[id].mean;
		normal = m_himap[id].nor;
	}
	
	char isFull(const int id) const
	{
		return (m_himap[id].is_leaf);
	}

	HierarchyQSPLATNode& getNode(const int id) const {return m_himap[id];}*/
	
	string filename;
	
	CoeRec getCoe(int id) const { return m_coe_buf[id]; }
	char hasNoCoe() const { return m_coe_buf == 0;}
	
	XYZ getColor(int id) const { return m_col_buf[id]; }
	char hasNoColor() const { return m_col_buf == 0;}
	
	void draw(const float& eta) const;
	
private:
	HierarchyQSPLATNode* m_root;
	CoeRec* m_coe_buf;
	XYZ* m_col_buf;
	unsigned int m_num_hipix, m_num_leaf, m_record;

	void createTreeNoColor(pcdSample* data, HierarchyQSPLATNode* res, unsigned int& count, int lo, int hi);
	void createTree(pcdSample* data, HierarchyQSPLATNode* res, XYZ* res_color, unsigned int& count, int lo, int hi);
	void createTreeRT(pcdSample* data, CoeRec* coe_data, HierarchyQSPLATNode* res, CoeRec* coe_res, unsigned int& count, int lo, int hi);
	void createTreeRTNColor(pcdSample* data, CoeRec* coe_data, HierarchyQSPLATNode* res, CoeRec* coe_res, XYZ* color_res, unsigned int& count, int lo, int hi);

	char is_null;
	
	void release(HierarchyQSPLATNode* node);

/*recursive lookup*/
	void recursive_lookup(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const ;
	void recursive_lookupNoDist(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const ;
	void recursive_lookupInterreflect(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility) const ;
	void recursive_lookupSubsurface(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const ;
	void recursive_lookupSubsurfaceB(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const ;
	//void recursive_lookupBone(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const ;
	void recursive_lookupLight(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility) const ;
	
/*recursive draw*/
	void recursive_draw(const HierarchyQSPLATNode* node,  const float& eta) const;
	
/*combine samples*/
	void combineSamplesNoColor(pcdSample* data, int lo, int hi, HierarchyQSPLATNode& node);
	void combineSamples(pcdSample* data, int lo, int hi, HierarchyQSPLATNode* node, XYZ& color);
	void combineSamples(pcdSample* data, CoeRec* coe_data, int lo, int hi, HierarchyQSPLATNode* node, CoeRec& node_coe);
	void combineSamples(pcdSample* data, CoeRec* coe_data, int lo, int hi, HierarchyQSPLATNode* node, CoeRec& node_coe, XYZ& color);
	void combineSamples(pcdSample* data, int lo, int hi, HierarchyQSPLATNode *node);
	
/*divide methods*/
	void divideByAngle(DivideBy<XYZ>adivide, pcdSample* data, float cx, float cy, float cz, int lo, int hi, int& a, int& b);
	void divideByAngle(DivideBy<CoeRec>adivide, pcdSample* data, CoeRec* coe_data, float cx, float cy, float cz, int lo, int hi, int& a, int& b);
};
#endif
