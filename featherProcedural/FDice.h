#ifndef FDICE_H
#define FDICE_H
#include "../shared/zData.h"

struct DHair
{
	XYZ root, normal, tangent, binormal, up, wind;
	float scale, curl, density, s, t;
	int id;
};

class FDice
{
public:
	FDice();
	~FDice();

	void create(const XYZ& p0, const XYZ& p1, const XYZ& p2);

	void getP0(XYZ& in) const { in = p_obj[0]; }
	void getP1(XYZ& in) const { in = p_obj[1]; }
	void getP2(XYZ& in) const { in = p_obj[2]; }
	float getArea() const {return area;}

	void rasterize(const float delta, DHair* res, int& count);
	void rasterize(const float delta, const CoeRec* vertex_coe, DHair* res, CoeRec* coe_res, int& count);
	
	static float barycentric_coord(float ax, float ay, float bx, float by, float x, float y);
	static float calculate_area(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	static float calculate_longest_edge(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	
	void setData(const DHair& n0, const DHair& n1, const DHair& n2);
	
private:
	XYZ P[3], V[3], p_obj[3];
	int id[3];
	float edge_length[3];
	MATRIX44F m_space;
	float area;
	float f120, f201, f012;
	DHair data[3];
};
#endif