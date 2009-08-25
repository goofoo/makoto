#include "../shared/zData.h"

class FTriangle
{
public:
	FTriangle();
	~FTriangle(void);

	void createSimple(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	void project(double& fov);
	void create(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	void onScreen(float* data, int map_w, int map_h);

	void getP0(XYZ& in) const { in = p_obj[0]; }
	void getP1(XYZ& in) const { in = p_obj[1]; }
	void getP2(XYZ& in) const { in = p_obj[2]; }
	float getArea() const {return m_area;}
	void rasterize(const float delta);
	void rasterize(const float delta, pcdSample* res, int& count);
	void rasterize(const float delta, const CoeRec* vertex_coe, pcdSample* res, CoeRec* coe_res, int& count);
	//void rasterize(const float delta, const CoeRec* vertex_coe, pcdSample* res, CoeRec* coe_res, int& count, XYZ* hdrCoeff);
	
	static float barycentric_coord(float ax, float ay, float bx, float by, float x, float y);
	static float calculate_area(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	static float calculate_longest_edge(const XYZ& p0, const XYZ& p1, const XYZ& p2);
	
	void setNormal(const XYZ& n0, const XYZ& n1, const XYZ& n2);
	void setColor(const XYZ& c0, const XYZ& c1, const XYZ& c2);
	void setTangent(const XYZ& t0, const XYZ& t1, const XYZ& t2);
	void setId(const int a, const int b, const int c);
	
private:
	XYZ P[3], V[3], p_obj[3];
	int id[3];
	float edge_length[3];
	MATRIX44F m_space;
	float m_area, m_min_length;
	float f120, f201, f012;
	XYZ normal[3], color[3], tangent[3];
};
