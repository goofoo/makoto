#ifndef _DICETRIANGLE_H
#define _DICETRIANGLE_H
#include "../shared/zData.h"

struct DiceParam
{
	float alpha, beta, gamma;
	int id0, id1, id2;
	float coords, coordt;
};

struct Dice2DParam
{
	float alpha, beta, gamma;
	int s, t;
};

class DiceTriangle
{
public:
	DiceTriangle();
	~DiceTriangle(void);

	void create(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	void create2D(const float* st0, const float* st1, const float* st2);
	
	int getGrid2D(int grid_width);
	int rasterize2D(Dice2DParam* res, const int seed, int grid_width);

	void getP0(XYZ& in) const { in = p_obj[0]; }
	void getP1(XYZ& in) const { in = p_obj[1]; }
	void getP2(XYZ& in) const { in = p_obj[2]; }
	float getArea() const {return m_area;}

	void rasterize(const float delta, DiceParam* res, int& count);
	void rasterize(const float delta, DiceParam* res, int& count, const int seed);

	static float barycentric_coord(float ax, float ay, float bx, float by, float x, float y);
	static float calculate_area(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	static float calculate_longest_edge(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	
	void setId(const int a, const int b, const int c);
	void setS(const float sa, const float sb, const float sc);
	void setT(const float ta, const float tb, const float tc);
	
private:
	XYZ P[3], V[3], p_obj[3];
	int id[3];
	float s[3], t[3];
	float edge_length[3];
	MATRIX44F m_space;
	float m_area;
	float f120, f201, f012;
};
#endif