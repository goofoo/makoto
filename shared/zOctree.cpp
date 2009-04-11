#include "zOctree.h"
#include <stdio.h>
#include <vector>

zOctree::zOctree():
m_data(0),
m_num_record(0),
m_level(0),
m_children(0),
m_is_leaf(0),
m_color(0,0,0),
m_median(0,0,0),
m_normal(0,0,0)
{
}

zOctree::~zOctree()
{
	if(m_data) delete[] m_data;
	if(m_children) delete[] m_children;
}

void zOctree::create(const rpcSample* data, int count, const XYZ& box_min, const XYZ& box_max, int level, std::vector<rpcSample>& cachelist)
{
	m_level = level;
	m_min = box_min;
	m_max = box_max;
	
	m_center.x = (box_min.x + box_max.x)/2;
	m_center.y = (box_min.y + box_max.y)/2;
	m_center.z = (box_min.z + box_max.z)/2;
	
	m_num_record = count;
	
	for(int i=0; i<m_num_record; i++)
	{
		m_median += data[i].pos;
		m_normal += data[i].nor;
		m_color += data[i].col;
	}
	
	m_normal /= m_num_record;
	m_median /= m_num_record;
	m_color /= m_num_record;

	if(m_level == OCTREE_N_MAX_LEVEL || (count < OCTREE_N_MAX_COUNT && length(m_normal)>0.9))// no subdivide
	{
		m_is_leaf = 1;
		
		m_data = new rpcSample[m_num_record];
		for(int i=0; i<m_num_record; i++)
		{
			m_data[i] = data[i];
		}
		normalize(m_normal);
		m_sample.pos = m_median;
		m_sample.nor = m_normal;
		m_sample.col = m_color;
		cachelist.push_back(m_sample);
		
		return;
	}
	
		m_children = new zOctree[8];
		std::vector<rpcSample>p000d;
		std::vector<rpcSample>p100d;
		std::vector<rpcSample>p010d;
		std::vector<rpcSample>p110d;
		std::vector<rpcSample>p001d;
		std::vector<rpcSample>p101d;
		std::vector<rpcSample>p011d;
		std::vector<rpcSample>p111d;
		
		for(int i=0; i<count; i++)
		{
			if(data[i].pos.x <= m_center.x && data[i].pos.y <= m_center.y && data[i].pos.z <= m_center.z)
			{
				p000d.push_back(data[i]);
			}
			else if(data[i].pos.x>m_center.x && data[i].pos.y<m_center.y && data[i].pos.z<m_center.z)
			{
				p100d.push_back(data[i]);
			}
			else if(data[i].pos.x<m_center.x && data[i].pos.y>m_center.y && data[i].pos.z<m_center.z)
			{
				p010d.push_back(data[i]);
			}
			else if(data[i].pos.x>m_center.x && data[i].pos.y>m_center.y && data[i].pos.z<m_center.z)
			{
				p110d.push_back(data[i]);
			}
			else if(data[i].pos.x<m_center.x && data[i].pos.y<m_center.y && data[i].pos.z>m_center.z)
			{
				p001d.push_back(data[i]);
			}
			else if(data[i].pos.x>m_center.x && data[i].pos.y<m_center.y && data[i].pos.z>m_center.z)
			{
				p101d.push_back(data[i]);
			}
			else if(data[i].pos.x<m_center.x && data[i].pos.y>m_center.y && data[i].pos.z>m_center.z)
			{
				p011d.push_back(data[i]);
			}
			else if(data[i].pos.x>m_center.x && data[i].pos.y>m_center.y && data[i].pos.z>m_center.z)
			{
				p111d.push_back(data[i]);
			}
		}

		int new_level = m_level+1;
		XYZ new_min, new_max;
		
		if(p000d.size() == 0) m_children[0].setEmpty();
		else
		{
			new_min.x = m_min.x;
			new_min.y = m_min.y;
			new_min.z = m_min.z;
			new_max.x = m_center.x;
			new_max.y = m_center.y;
			new_max.z = m_center.z;
			rpcSample* buf = new rpcSample[p000d.size()];
			for(int i=0; i<p000d.size(); i++) buf[i]= p000d[i];
			m_children[0].create(buf, p000d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p100d.size() == 0) m_children[1].setEmpty();
		else
		{
			new_min.x = m_center.x;
			new_min.y = m_min.y;
			new_min.z = m_min.z;
			new_max.x = m_max.x;
			new_max.y = m_center.y;
			new_max.z = m_center.z;
			rpcSample* buf = new rpcSample[p100d.size()];
			for(int i=0; i<p100d.size(); i++) buf[i]= p100d[i];
			m_children[1].create(buf, p100d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p010d.size() == 0) m_children[2].setEmpty();
		else
		{
			new_min.x = m_min.x;
			new_min.y = m_center.y;
			new_min.z = m_min.z;
			new_max.x = m_center.x;
			new_max.y = m_max.y;
			new_max.z = m_center.z;
			rpcSample* buf = new rpcSample[p010d.size()];
			for(int i=0; i<p010d.size(); i++) buf[i]= p010d[i];
			m_children[2].create(buf, p010d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p110d.size() == 0) m_children[3].setEmpty();
		else
		{
			new_min.x = m_center.x;
			new_min.y = m_center.y;
			new_min.z = m_min.z;
			new_max.x = m_max.x;
			new_max.y = m_max.y;
			new_max.z = m_center.z;
			rpcSample* buf = new rpcSample[p110d.size()];
			for(int i=0; i<p110d.size(); i++) buf[i]= p110d[i];
			m_children[3].create(buf, p110d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p001d.size() == 0) m_children[4].setEmpty();
		else
		{
			new_min.x = m_min.x;
			new_min.y = m_min.y;
			new_min.z = m_center.z;
			new_max.x = m_center.x;
			new_max.y = m_center.y;
			new_max.z = m_max.z;
			rpcSample* buf = new rpcSample[p001d.size()];
			for(int i=0; i<p001d.size(); i++) buf[i]= p001d[i];
			m_children[4].create(buf, p001d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p101d.size() == 0) m_children[5].setEmpty();
		else
		{
			new_min.x = m_center.x;
			new_min.y = m_min.y;
			new_min.z = m_center.z;
			new_max.x = m_max.x;
			new_max.y = m_center.y;
			new_max.z = m_max.z;
			rpcSample* buf = new rpcSample[p101d.size()];
			for(int i=0; i<p101d.size(); i++) buf[i]= p101d[i];
			m_children[5].create(buf, p101d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p011d.size() == 0) m_children[6].setEmpty();
		else
		{
			new_min.x = m_min.x;
			new_min.y = m_center.y;
			new_min.z = m_center.z;
			new_max.x = m_center.x;
			new_max.y = m_max.y;
			new_max.z = m_max.z;
			rpcSample* buf = new rpcSample[p011d.size()];
			for(int i=0; i<p011d.size(); i++) buf[i]= p011d[i];
			m_children[6].create(buf, p011d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		if(p111d.size() == 0) m_children[7].setEmpty();
		else
		{
			new_min.x = m_center.x;
			new_min.y = m_center.y;
			new_min.z = m_center.z;
			new_max.x = m_max.x;
			new_max.y = m_max.y;
			new_max.z = m_max.z;
			rpcSample* buf = new rpcSample[p111d.size()];
			for(int i=0; i<p111d.size(); i++) buf[i]= p111d[i];
			m_children[7].create(buf, p111d.size(), new_min, new_max, new_level, cachelist);
			delete[] buf;
		}
		
		p000d.clear();
		p100d.clear();
		p010d.clear();
		p110d.clear();
		p001d.clear();
		p101d.clear();
		p011d.clear();
		p111d.clear();
	
}
/*
void zOctree::create(const rpcSample* data, int count, const XYZ& boxcenter, float boxsize, int level)
{
	m_center = boxcenter;
	m_size = boxsize;
	m_level = level;
	m_min.x = boxcenter.x - boxsize/2;
	m_min.y = boxcenter.y - boxsize/2;
	m_min.z = boxcenter.z - boxsize/2;
	m_max.x = boxcenter.x + boxsize/2;
	m_max.y = boxcenter.y + boxsize/2;
	m_max.z = boxcenter.z + boxsize/2;
	
	m_num_record = count;
	
	for(int i=0; i<m_num_record; i++)
	{
		m_median.x += data[i].pos.x;
		m_median.y += data[i].pos.y;
		m_median.z += data[i].pos.z;
		m_view.x += data[i].nor.x;
		m_view.y += data[i].nor.y;
		m_view.z += data[i].nor.z;
	}
	
	m_view.x /= m_num_record;
	m_view.y /= m_num_record;
	m_view.z /= m_num_record;
	
	m_median.x /= m_num_record;
	m_median.y /= m_num_record;
	m_median.z /= m_num_record;
	
//	printf("%f ==", length(m_view));

	if(m_level == OCTREE_N_MAX_LEVEL || (count < OCTREE_N_MAX_COUNT && length(m_view)>0.8))// no subdivide
	{
		m_is_leaf = 1;
		
		normalize(m_view);
		
		return;
	}
	
		m_children = new zOctree[8];
		std::vector<rpcSample>p000d;
		std::vector<rpcSample>p100d;
		std::vector<rpcSample>p010d;
		std::vector<rpcSample>p110d;
		std::vector<rpcSample>p001d;
		std::vector<rpcSample>p101d;
		std::vector<rpcSample>p011d;
		std::vector<rpcSample>p111d;
		
		for(int i=0; i<count; i++)
		{
			if(data[i].pos.x <= m_center.x && data[i].pos.y <= m_center.y && data[i].pos.z <= m_center.z)
			{
				p000d.push_back(data[i]);
			}
			if(data[i].pos.x>=m_center.x && data[i].pos.y<=m_center.y && data[i].pos.z<=m_center.z)
			{
				p100d.push_back(data[i]);
			}
			if(data[i].pos.x<=m_center.x && data[i].pos.y>=m_center.y && data[i].pos.z<=m_center.z)
			{
				p010d.push_back(data[i]);
			}
			if(data[i].pos.x>=m_center.x && data[i].pos.y>=m_center.y && data[i].pos.z<=m_center.z)
			{
				p110d.push_back(data[i]);
			}
			if(data[i].pos.x<=m_center.x && data[i].pos.y<=m_center.y && data[i].pos.z>=m_center.z)
			{
				p001d.push_back(data[i]);
			}
			if(data[i].pos.x>=m_center.x && data[i].pos.y<=m_center.y && data[i].pos.z>=m_center.z)
			{
				p101d.push_back(data[i]);
			}
			if(data[i].pos.x<=m_center.x && data[i].pos.y>=m_center.y && data[i].pos.z>=m_center.z)
			{
				p011d.push_back(data[i]);
			}
			if(data[i].pos.x>=m_center.x && data[i].pos.y>=m_center.y && data[i].pos.z>=m_center.z)
			{
				p111d.push_back(data[i]);
			}
		}

		float new_size = m_size/2;
		int new_level = m_level+1;
		XYZ new_center;
		
		if(p000d.size() == 0) m_children[0].setEmpty();
		else
		{
			new_center.x = m_center.x - new_size/2;
			new_center.y = m_center.y - new_size/2;
			new_center.z = m_center.z - new_size/2;
			rpcSample* buf = new rpcSample[p000d.size()];
			for(int i=0; i<p000d.size(); i++) buf[i]= p000d[i];
			m_children[0].create(buf, p000d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p100d.size() == 0) m_children[1].setEmpty();
		else
		{
			new_center.x = m_center.x + new_size/2;
			new_center.y = m_center.y - new_size/2;
			new_center.z = m_center.z - new_size/2;
			rpcSample* buf = new rpcSample[p100d.size()];
			for(int i=0; i<p100d.size(); i++) buf[i]= p100d[i];
			m_children[1].create(buf, p100d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p010d.size() == 0) m_children[2].setEmpty();
		else
		{
			new_center.x = m_center.x - new_size/2;
			new_center.y = m_center.y + new_size/2;
			new_center.z = m_center.z - new_size/2;
			rpcSample* buf = new rpcSample[p010d.size()];
			for(int i=0; i<p010d.size(); i++) buf[i]= p010d[i];
			m_children[2].create(buf, p010d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p110d.size() == 0) m_children[3].setEmpty();
		else
		{
			new_center.x = m_center.x + new_size/2;
			new_center.y = m_center.y + new_size/2;
			new_center.z = m_center.z - new_size/2;
			rpcSample* buf = new rpcSample[p110d.size()];
			for(int i=0; i<p110d.size(); i++) buf[i]= p110d[i];
			m_children[3].create(buf, p110d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p001d.size() == 0) m_children[4].setEmpty();
		else
		{
			new_center.x = m_center.x - new_size/2;
			new_center.y = m_center.y - new_size/2;
			new_center.z = m_center.z + new_size/2;
			rpcSample* buf = new rpcSample[p001d.size()];
			for(int i=0; i<p001d.size(); i++) buf[i]= p001d[i];
			m_children[4].create(buf, p001d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p101d.size() == 0) m_children[5].setEmpty();
		else
		{
			new_center.x = m_center.x + new_size/2;
			new_center.y = m_center.y - new_size/2;
			new_center.z = m_center.z + new_size/2;
			rpcSample* buf = new rpcSample[p101d.size()];
			for(int i=0; i<p101d.size(); i++) buf[i]= p101d[i];
			m_children[5].create(buf, p101d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p011d.size() == 0) m_children[6].setEmpty();
		else
		{
			new_center.x = m_center.x - new_size/2;
			new_center.y = m_center.y + new_size/2;
			new_center.z = m_center.z + new_size/2;
			rpcSample* buf = new rpcSample[p011d.size()];
			for(int i=0; i<p011d.size(); i++) buf[i]= p011d[i];
			m_children[6].create(buf, p011d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		if(p111d.size() == 0) m_children[7].setEmpty();
		else
		{
			new_center.x = m_center.x + new_size/2;
			new_center.y = m_center.y + new_size/2;
			new_center.z = m_center.z + new_size/2;
			rpcSample* buf = new rpcSample[p111d.size()];
			for(int i=0; i<p111d.size(); i++) buf[i]= p111d[i];
			m_children[7].create(buf, p111d.size(), new_center, new_size, new_level);
			delete[] buf;
		}
		
		p000d.clear();
		p100d.clear();
		p010d.clear();
		p110d.clear();
		p001d.clear();
		p101d.clear();
		p011d.clear();
		p111d.clear();
	
}
*/
void zOctree::displayCube()
{
	
	glBegin(GL_QUADS);
		
		glVertex3f(m_min.x, m_min.y, m_min.z);
		glVertex3f(m_max.x, m_min.y, m_min.z);
		glVertex3f(m_max.x, m_max.y, m_min.z);
		glVertex3f(m_min.x, m_max.y, m_min.z);

		glVertex3f(m_min.x, m_min.y, m_max.z);
		glVertex3f(m_max.x, m_min.y, m_max.z);
		glVertex3f(m_max.x, m_max.y, m_max.z);
		glVertex3f(m_min.x, m_max.y, m_max.z);

		glVertex3f(m_min.x, m_min.y, m_min.z);
		glVertex3f(m_min.x, m_min.y, m_max.z);
		glVertex3f(m_max.x, m_min.y, m_max.z);
		glVertex3f(m_max.x, m_min.y, m_min.z);

		glVertex3f(m_min.x, m_max.y, m_min.z);
		glVertex3f(m_min.x, m_max.y, m_max.z);
		glVertex3f(m_max.x, m_max.y, m_max.z);
		glVertex3f(m_max.x, m_max.y, m_min.z);

		glVertex3f(m_min.x, m_min.y, m_min.z);
		glVertex3f(m_min.x, m_min.y, m_max.z);
		glVertex3f(m_min.x, m_max.y, m_max.z);
		glVertex3f(m_min.x, m_max.y, m_min.z);

		glVertex3f(m_max.x, m_min.y, m_min.z);
		glVertex3f(m_max.x, m_min.y, m_max.z);
		glVertex3f(m_max.x, m_max.y, m_max.z);
		glVertex3f(m_max.x, m_max.y, m_min.z);
	glEnd();
	/*
	glBegin(GL_POINTS);
		glVertex3f(m_median.x, m_median.y, m_median.z);
	glEnd();
	*/	
	
}

void zOctree::displayPoints()
{
	glBegin(GL_POINTS);
	for(int i=0; i<m_num_record; i++) glVertex3f(m_data[i].pos.x, m_data[i].pos.y, m_data[i].pos.z);
	glEnd();
}

void zOctree::draw(const XYZ& P, const XYZ& N)
{
	/*
	if(m_is_leaf==1)
	{
		if(m_num_record == 0) return;
		if(distance_between(P, m_median)>(m_size*16)) 
		{
			displayCube();
			return;
		}
		displayPoints();
	}
	else
	{
		if(plane_box_intersection(P, N, m_min, m_max)==0) return;
		else
		{
			m_children[0].draw(P, N);
			m_children[1].draw(P, N);
			m_children[2].draw(P, N);
			m_children[3].draw(P, N);
			m_children[4].draw(P, N);
			m_children[5].draw(P, N);
			m_children[6].draw(P, N);
			m_children[7].draw(P, N);

		}
		
	}
	*/
}

void zOctree::draw()
{
	if(m_is_leaf==1)
	{
		if(m_num_record == 0) return;
		displayCube();
		//displayPoints();
	}
	else
	{
				m_children[0].draw();
				m_children[1].draw();
				m_children[2].draw();
				m_children[3].draw();
				m_children[4].draw();
				m_children[5].draw();
				m_children[6].draw();
				m_children[7].draw();
	}
}

int zOctree::rayHitTest(const XYZ& origin, const XYZ& ray)
{
	if(ray_box_intersection(origin, ray, m_min, m_max)==0) return 0;
	
	if(m_is_leaf==1)
	{
		if(m_num_record == 0) return 0;
		
		for(int i=0; i<m_num_record; i++)
		{
			XYZ to_origin(m_data[i].pos.x - origin.x, m_data[i].pos.y - origin.y, m_data[i].pos.z - origin.z);
			normalize(to_origin);
			if(dot(to_origin, ray)>0.95) 
			{
				//glVertex3f(m_data[i].x, m_data[i].y, m_data[i].z);
				return 1;
			}
		}
		
		return 0;
	}
	else
	{
		if(m_children[0].rayHitTest(origin, ray) == 1) return 1;
		if(m_children[1].rayHitTest(origin, ray) == 1) return 1;
		if(m_children[2].rayHitTest(origin, ray) == 1) return 1;
		if(m_children[3].rayHitTest(origin, ray) == 1) return 1;
		if(m_children[4].rayHitTest(origin, ray) == 1) return 1;
		if(m_children[5].rayHitTest(origin, ray) == 1) return 1;
		if(m_children[6].rayHitTest(origin, ray) == 1) return 1;
		return m_children[7].rayHitTest(origin, ray);
	}
}
/*
void zOctree::setRecordToCell(const rpcRecord& rec)
{
	if(m_is_leaf==1) m_record = rec;
	else
	{
		if(rec.pos.x <= m_center.x && rec.pos.y <= m_center.y && rec.pos.z <= m_center.z)
		{
			m_children[0].setRecordToCell(rec);
		}
		else if(rec.pos.x>=m_center.x && rec.pos.y<=m_center.y && rec.pos.z<=m_center.z)
		{
			m_children[1].setRecordToCell(rec);
		}
		else if(rec.pos.x<=m_center.x && rec.pos.y>=m_center.y && rec.pos.z<=m_center.z)
		{
			m_children[2].setRecordToCell(rec);
		}
		else if(rec.pos.x>=m_center.x && rec.pos.y>=m_center.y && rec.pos.z<=m_center.z)
		{
			m_children[3].setRecordToCell(rec);
		}
		else if(rec.pos.x<=m_center.x && rec.pos.y<=m_center.y && rec.pos.z>=m_center.z)
		{
			m_children[4].setRecordToCell(rec);
		}
		else if(rec.pos.x>=m_center.x && rec.pos.y<=m_center.y && rec.pos.z>=m_center.z)
		{
			m_children[5].setRecordToCell(rec);
		}
		else if(rec.pos.x<=m_center.x && rec.pos.y>=m_center.y && rec.pos.z>=m_center.z)
		{
			m_children[6].setRecordToCell(rec);
		}
		else if(rec.pos.x>=m_center.x && rec.pos.y>=m_center.y && rec.pos.z>=m_center.z)
		{
			m_children[7].setRecordToCell(rec);
		}
	}
}

const XYZW& zOctree::getTableFromCell(const XYZ& p)
{
	if(m_is_leaf==1) return m_record.table;
	else
	{
		if(p.x <= m_center.x && p.y <= m_center.y && p.z <= m_center.z)
		{
			return m_children[0].getTableFromCell(p);
		}
		else if(p.x>=m_center.x && p.y<=m_center.y && p.z<=m_center.z)
		{
			return m_children[1].getTableFromCell(p);
		}
		else if(p.x<=m_center.x && p.y>=m_center.y && p.z<=m_center.z)
		{
			return m_children[2].getTableFromCell(p);
		}
		else if(p.x>=m_center.x && p.y>=m_center.y && p.z<=m_center.z)
		{
			return m_children[3].getTableFromCell(p);
		}
		else if(p.x<=m_center.x && p.y<=m_center.y && p.z>=m_center.z)
		{
			return m_children[4].getTableFromCell(p);
		}
		else if(p.x>=m_center.x && p.y<=m_center.y && p.z>=m_center.z)
		{
			return m_children[5].getTableFromCell(p);
		}
		else if(p.x<=m_center.x && p.y>=m_center.y && p.z>=m_center.z)
		{
			return m_children[6].getTableFromCell(p);
		}
		else if(p.x>=m_center.x && p.y>=m_center.y && p.z>=m_center.z)
		{
			return m_children[7].getTableFromCell(p);
		}
	}
}*/