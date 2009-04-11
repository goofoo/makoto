#include "zData.h"

class CfishBone
{
public:
	CfishBone(void);
	~CfishBone(void);
	
	void initialize(float length, int a);
	void compose();
	void setTime(float t) { m_time = t; }
	void setFrequency(float f) { m_frequency = f; }
	void setSpacing(float s);
	void setLength(float l);
	void setAnglePrim(float a) { m_angle_prim = a; }
	void setAngleOffset(float a) { m_angle_offset = a; }
	void setOscillation(float a) { m_angle_oscillate = a; }
	float getSpacing() { return m_spacing; }
	int getBoneCount() { return m_n_bones; }
	
	const MATRIX44F& getBoneById(int id);
	void getBoneById(int id, MATRIX44F& mat, XYZ& pos);
	void getBoneById(int id, MATRIX44F& mat);

private:
	int m_n_bones;
	float m_time;
	float m_frequency;
	float m_spacing;
	float m_angle_prim;
	float m_angle_offset;
	float m_angle_oscillate;
	MATRIX44F* m_skeleton;
	XYZ* m_pos_bind;
};
