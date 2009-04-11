#include <gl/gl.h>
#include "zData.h"

inline void zDrawGrid()
{
	glColor3f(0.5f,0.5f,0.5f);
	glBegin( GL_LINES );
	for( int i = -10; i <= 10; i++ ) 
	{		
		glVertex3i(i,0,-10);
		glVertex3i(i,0,10);
		glVertex3i(-10,0,i);
		glVertex3i(10,0,i);		
	}
		
	glEnd();
}

inline void zCoordinateSystem()
{
	glBegin( GL_LINES );
	glColor3f(1,0,0);
	glVertex3i(0,0,0);
	glVertex3i(1,0,0);
	glColor3f(0,1,0);
	glVertex3i(0,0,0);
	glVertex3i(0,1,0);
	glColor3f(0,0,1);
	glVertex3i(0,0,0);
	glVertex3i(0,0,1);
	glEnd();
}

inline void zDrawPoint(const XYZ& p)
{
	glBegin( GL_LINES );
	glColor3f(1,1,1);
	glVertex3f(p.x,p.y,p.z);
	glVertex3f(p.x,p.y+1.0f,p.z);
	glEnd();
}