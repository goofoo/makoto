#ifndef _GPERSPCAMERA_H
#define _GPERSPCAMERA_H

#include "zData.h"

enum G_CAMERA_ACTION {TUMBLE,TRACK,DOLLY};

class gPerspCamera {

public:
	gPerspCamera();
	~gPerspCamera() {}
	gPerspCamera( float fov, int width, int height, float _near, float _far);
	gPerspCamera( float fov, float _near, float _far, XYZ& ori);
	
	void reset();
	void setViewPort( int w, int h );
	void setViewPort(double fov, int w, int h );
	void setSoftViewPort(double fov, int w, int h );
	void setCropViewPort( int wnd_w, int wnd_h, int crop_x, int crop_y );
	void update();
	void update(XYZ& e, XYZ& s, XYZ& u, XYZ& f);
	void setMatrix(XYZ& e, XYZ& s, XYZ& u, XYZ& f);

	void track( float x, float y );
	void tumble( float x, float y );
	void dolly( float z );
	

	void lookAt(float m_eyeX, float m_eyeY, float m_eyeZ,
			 		    float m_viewX,     float m_viewY,     float m_viewZ,
						float upX, float upY, float upZ);
						
	void viewAll( XYZ center, float width );
	
	int getWidth() {return m_width;}
	int getHeight() {return m_height;}
	double getAspectRatio() {return m_aspect;}
	double getFOV() {return m_fov;}
	double getClipNear() {return m_clipnear;}
	double getClipFar() {return m_clipfar;}
	float eyex() {return m_eye.x;}
	float eyey() {return m_eye.y;}
	float eyez() {return m_eye.z;}
	float m_viewx() {return m_view.x;}
	float m_viewy() {return m_view.y;}
	float m_viewz() {return m_view.z;}
	float upx() {return up.x;}
	float upy() {return up.y;}
	float upz() {return up.z;}
	
	void getGLMatrixFull(float* mat);
	void getGLMatrixRotation(float* mat);
	
	void getHUDPosUpLeft(XYZ& pw, float height);
	
	int isInViewFrustum(const qnode& data, float threshold);
	int isInViewFrustum(const qnode& data, float threshold, float left, float right, float bottom, float top);
	void getRasterPosition(XYZ& pw);
	
	XYZ& getEye() { return m_eye; }
	XYZ& getView() { return In;}
	XYZ& getUp() {return up;}
	XYZ& getSide() {return side;}
	
	void resetClips() { m_auto_far = m_clipnear; }
	void calculateClips(const qnode& data);
	void calculateClips(XYZ* bbox);
	
	float getDepth(const XYZ& pw);
	unsigned int depthTest(const qnode& data, float* buffer);
	unsigned int depthTest(int id, qnode* data, int* buffer, float& occ, float& ratio);
	void writeToBuffer(const qnode& data, float* buffer, float value);
	void readFromBuffer(const qnode& data, float* buffer, float& value);
	
	void getCameraP(const qnode& data, float& dist, float& facingforward);

	void setAction(G_CAMERA_ACTION action) {g_action = action;}
	void act(float x, float y, float z)
	{
		switch (g_action)
		{
		case TUMBLE:
			tumble( x, y );
			break;
		case TRACK:
			track( x, y );
			break;
		case DOLLY:
			dolly(z);
			break;
		default:
			return;
		}
	}
	
	void updatePos(int x, int y) { m_pos_x = x; m_pos_y = y;}
	void getDeltaPos(int x, int y, int& dx, int& dy) 
	{
		dx = x - m_pos_x;
		dy = y - m_pos_y;
	}
	
private :
	XYZ m_eye, m_eye_ori, m_view, up, side, I, In, V;
	ABCD m_plane[4];
	
	MATRIX44F m_space, m_spaceinv;
	
	double m_fov, m_clipnear, m_clipfar, m_aspect, m_auto_far;

	int m_width, m_height, m_pos_x, m_pos_y;
	
	void updateSpace();

	G_CAMERA_ACTION g_action;
	
};



#endif
