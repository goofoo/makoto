#pragma once
#include "../shared/zData.h"

#define VBUFWIDTH 80
#define VBUFWIDTHHALF 40
#define VBUFLENGTH 6400

class FVisibility
{
public:
	FVisibility(int w, int h, double fov, float light_size);
	~FVisibility(void);

	void setSpace(const XYZ& eye, const XYZ& view, const XYZ& up, const XYZ& side);
	int dice(const qnode& data, float threshold, XYZ& objectP);
	//int isBackVisible(const qnode& data, float threshold, XYZ& objectP);
	int isVisible(XYZ& p);
	int isInFrustum(XYZ& p, XYZ& objP);
	float occlusion(XYZ& p, const MINMAX* map);
	double getFOV() {return m_fov;}
	void getLookAt(float& eyex, float& eyey, float& eyez, float& viewx, float& viewy, float& viewz, float& upx, float& upy, float& upz);
	int backProject(const XYZ& objectP, const MINMAX& pixel, float& xmin, float& xmax, float& ymin, float& ymax);
	void writeBuffer(float* buf, float& xmin, float& xmax, float& ymin, float& ymax);
	float backProject(const XYZ& objectP, const MINMAX& pixel);
	void backProject(const XYZ& objectP, const MINMAX& pixel, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3);
	void onScreen(const MINMAX& pixel, float clip, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3);
	void croppedProject(const XYZ& objectP, const MINMAX& pixel, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3);
	void croppedProject(const XYZ& objectP, const XYZ& center, float pixelsize, XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3);
	void getLightPlane(XYZ& corner0, XYZ& corner1, XYZ& corner2, XYZ& corner3);
	//void getPixel(float depth, int u, int v, short level, XYZ& pp, float& pixelsize);
	void toWorld(XYZ& p);
	void toObject(XYZ& p);
	void toObject(qnode& n);
	void uvToObject(float& u, float& v, float depth);
	void uvToObject(int& u, int& v, float depth);
	void objectToUV(float& u, float& v, const XYZ& q);
	void objectToUV(int& u, int& v, const XYZ& q);
	void normalToObject(XYZ& n) {m_spaceinv.transformAsNormal(n);}
	float getFactor() {return m_factor;}
	float getArea() {return m_light_area;}
	void getView(XYZ& v) {v.x = m_space.v[2][0]; v.y = m_space.v[2][1]; v.z = m_space.v[2][2]; }
	void getKernel(XYZ& q, short& umin, short& umax, short& vmin, short& vmax);
	void setNearClip(float val) {m_clipnear = val;}
	void getVPixel(int i, int j, XYZ& a, XYZ& b, XYZ& c, XYZ& d);
	int backProject(const XYZ& objectP, const qnode& node, float& xmin, float& xmax, float& ymin, float& ymax);

private:
	int m_width, m_height;
	MATRIX44F m_space, m_spaceinv;
	ABCD m_plane[4];
	double m_fov, m_clipnear, m_aspect, m_factor;
	XYZ m_eye;
	float m_light_size, m_light_area, m_v_grid, m_light_size_1;

	void recursive_lookup(const XYZ& q, const MINMAX* map, int id, short level, float& res);
};
