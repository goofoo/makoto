/*
 *  zSphere.cpp
 *  runtime_ogl_vertex_fragment
 *
 *  Created by zhang on 07-3-11.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "zSphere.h"
#include "zMath.h"

zSphere::zSphere(float radius, int gridX, int gridY):
//m_pVertex(0), 
//m_pTexcoord(0),
m_samples(0)
{
	m_radius = radius;
	m_grid_x = gridX+1;
	m_grid_y = gridY+1;
	//m_count = (m_grid_x+1)*(m_grid_y+1)*6;
	//m_pVertex = new float[(m_grid_x+1)*(m_grid_y+1)*6*3];
	//m_pTexcoord = new float[(m_grid_x+1)*(m_grid_y+1)*6*2];

	m_samples = new sphereSample[m_grid_x*m_grid_y];
	float u, v;
	for(int j=0; j<m_grid_y; j++)
	{
		for(int i=0; i<m_grid_x; i++)
		{
			u = (float)i/(float)gridX;
			v = (float)j/(float)gridY;
			
			param(u, v, m_samples[m_grid_x*j+i].pos.x, m_samples[m_grid_x*j+i].pos.y, m_samples[m_grid_x*j+i].pos.z);
			
			m_samples[m_grid_x*j+i].col = XYZ(1,0,0);
			
			m_samples[m_grid_x*j+i].texcoord.x = u;
			m_samples[m_grid_x*j+i].texcoord.y = v;
			/*
			u = (float)i/(float)m_grid_x;
			v = float(j+1)/(float)m_grid_y;
			
			param(u, v, m_pVertex[(((m_grid_x+1)*j+i)*6+1)*3], m_pVertex[(((m_grid_x+1)*j+i)*6+1)*3+1], m_pVertex[(((m_grid_x+1)*j+i)*6+1)*3+2]);
			
			
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+1)*2] = u;
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+1)*2+1] = v;
			
			u = float(i+1)/(float)m_grid_x;
			v = float(j+1)/(float)m_grid_y;
			
			param(u, v, m_pVertex[(((m_grid_x+1)*j+i)*6+2)*3], m_pVertex[(((m_grid_x+1)*j+i)*6+2)*3+1], m_pVertex[(((m_grid_x+1)*j+i)*6+2)*3+2]);
			
			
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+2)*2] = u;
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+2)*2+1] = v;
			
			m_pVertex[(((m_grid_x+1)*j+i)*6+3)*3] = m_pVertex[(((m_grid_x+1)*j+i)*6+2)*3];
			m_pVertex[(((m_grid_x+1)*j+i)*6+3)*3+1] = m_pVertex[(((m_grid_x+1)*j+i)*6+2)*3+1];
			m_pVertex[(((m_grid_x+1)*j+i)*6+3)*3+2] = m_pVertex[(((m_grid_x+1)*j+i)*6+2)*3+2];
			
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+3)*2] = u;
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+3)*2+1] = v;
			
			u = float(i+1)/(float)m_grid_x;
			v = float(j)/(float)m_grid_y;
			
			param(u, v, m_pVertex[(((m_grid_x+1)*j+i)*6+4)*3], m_pVertex[(((m_grid_x+1)*j+i)*6+4)*3+1], m_pVertex[(((m_grid_x+1)*j+i)*6+4)*3+2]);
			
			
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+4)*2] = u;
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+4)*2+1] = v;
			
			m_pVertex[(((m_grid_x+1)*j+i)*6+5)*3] = m_pVertex[(((m_grid_x+1)*j+i)*6)*3];
			m_pVertex[(((m_grid_x+1)*j+i)*6+5)*3+1] = m_pVertex[(((m_grid_x+1)*j+i)*6)*3+1];
			m_pVertex[(((m_grid_x+1)*j+i)*6+5)*3+2] = m_pVertex[(((m_grid_x+1)*j+i)*6)*3+2];
			
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+5)*2] = m_pTexcoord[(((m_grid_x+1)*j+i)*6)*2];
			m_pTexcoord[(((m_grid_x+1)*j+i)*6+5)*2+1] = m_pTexcoord[(((m_grid_x+1)*j+i)*6)*2+1];
			*/
		}
	}
}

zSphere::~zSphere()
{
	//if(m_pVertex) delete[] m_pVertex;
	//if(m_pTexcoord) delete[] m_pTexcoord;
	if(m_samples) delete[] m_samples;
}

void zSphere::draw()
{
/*
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (float*)m_pVertex);
		glTexCoordPointer(2, GL_FLOAT, 0, (float*)m_pTexcoord);
		glDrawArrays(GL_TRIANGLES, 0, m_count);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		*/
		glBegin(GL_QUADS);
		sphereSample t_sample;
	for(int j=0; j<m_grid_y; j++)
	{
		for(int i=0; i<m_grid_x; i++)
		{
			t_sample = m_samples[j*m_grid_x+i];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.pos.x, t_sample.pos.y, t_sample.pos.z);
			
			t_sample = m_samples[j*m_grid_x+i+1];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.pos.x, t_sample.pos.y, t_sample.pos.z);

			t_sample = m_samples[(j+1)*m_grid_x+i+1];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.pos.x, t_sample.pos.y, t_sample.pos.z);
			
			t_sample = m_samples[(j+1)*m_grid_x+i];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.pos.x, t_sample.pos.y, t_sample.pos.z);

		}
	}
	glEnd();

}

void zSphere::drawUV()
{
/*
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (float*)m_pVertex);
		glTexCoordPointer(2, GL_FLOAT, 0, (float*)m_pTexcoord);
		glDrawArrays(GL_TRIANGLES, 0, m_count);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		*/
		glBegin(GL_QUADS);
		sphereSample t_sample;
	for(int j=0; j<m_grid_y-1; j++)
	{
		for(int i=0; i<m_grid_x-1; i++)
		{
			t_sample = m_samples[j*m_grid_x+i];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.texcoord.x, t_sample.texcoord.y, 0);
			
			t_sample = m_samples[j*m_grid_x+i+1];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.texcoord.x, t_sample.texcoord.y, 0);

			t_sample = m_samples[(j+1)*m_grid_x+i+1];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.texcoord.x, t_sample.texcoord.y, 0);
			
			t_sample = m_samples[(j+1)*m_grid_x+i];
			glTexCoord2f(t_sample.texcoord.x, t_sample.texcoord.y);
			glColor3f(t_sample.col.x, t_sample.col.y, t_sample.col.z);
			glVertex3f(t_sample.texcoord.x, t_sample.texcoord.y, 0);

		}
	}
	glEnd();

}

void zSphere::draw(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x,y,z);
	draw();
	glPopMatrix();
}

void zSphere::param(float u, float v, float& x, float& y, float& z)
{
	float theta = PI*v, phi = 2.0f * PI * u;
			
    x = m_radius*sin(theta) * cos(phi);
    y = -m_radius*cos(theta);
    z = -m_radius*sin(theta) * sin(phi);
}

void zSphere::setColorAt(int& x, int& y, float& r, float& g, float& b)
{
	m_samples[y*m_grid_x+x].col.x = r;
	m_samples[y*m_grid_x+x].col.y = g;
	m_samples[y*m_grid_x+x].col.z = b;
}

void zSphere::getUVAt(int& x, int& y, float& u, float& v)
{
	u = m_samples[y*m_grid_x+x].texcoord.x;
	v = m_samples[y*m_grid_x+x].texcoord.y;
}
