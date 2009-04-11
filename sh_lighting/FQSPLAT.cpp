#include "FQSPLAT.h"
#include "../shared/zMath.h"
#include "../shared/eigenSystem.h"
#include "../shared/gBase.h"
#define MAX_N_SPLAT 1548576

FQSPLAT::FQSPLAT():m_himap(0),is_null(1),m_col_buf(0),m_coe_buf(0)
{}

FQSPLAT::~FQSPLAT(void)
{
	if(m_himap) delete[] m_himap;
		if(m_col_buf) delete[] m_col_buf;
			if(m_coe_buf) delete[] m_coe_buf;
}

void FQSPLAT::create(pcdSample* rec, int n_rec)
{
	int m_point_count = n_rec;
	m_max_level = 13;
	m_draw_level = 5;
	m_himap = new HierarchyQSPLATNode[n_rec/2];
	m_col_buf = new XYZ[n_rec/2];
	
		XYZ mean = findMean(rec, 0, m_point_count-1);
		MATRIX44F objectSpace;
		calculateObjectSpace(rec, 0, m_point_count-1, objectSpace);
		
		MATRIX44F objectSpaceInverse = objectSpace;
		objectSpaceInverse.inverse();
	
	for(int i=0; i<m_point_count; i++) objectSpaceInverse.transform(rec[i].pos);

	m_num_hipix = 0;
	m_num_leaf = 0;
	createTree(rec, m_himap, m_col_buf, m_num_hipix, 0, m_point_count-1, 0, -1, 0);
	
	for(int i=0; i<m_num_hipix; i++) objectSpace.transform(m_himap[i].mean);
		
	cout<<" N QSPLAT Node:"<<m_num_hipix<<" N Leaf Node:"<<m_num_leaf;
}

void FQSPLAT::createNoColor(pcdSample* rec, int n_rec)
{
	int m_point_count = n_rec;
	m_max_level = 13;
	m_draw_level = 5;
	m_himap = new HierarchyQSPLATNode[n_rec/2];
	
		XYZ mean = findMean(rec, 0, m_point_count-1);
		MATRIX44F objectSpace;
		calculateObjectSpace(rec, 0, m_point_count-1, objectSpace);
		
		MATRIX44F objectSpaceInverse = objectSpace;
		objectSpaceInverse.inverse();
	
	for(int i=0; i<m_point_count; i++) objectSpaceInverse.transform(rec[i].pos);

	m_num_hipix = 0;
	m_num_leaf = 0;
	createTreeNoColor(rec, m_himap, m_num_hipix, 0, m_point_count-1, 0, -1, 0);
	
	float max_leaf = 0, min_leaf = 10e6;
	for(int i=0; i<m_num_hipix; i++) 
	{
		objectSpace.transform(m_himap[i].mean);
		if(m_himap[i].is_leaf && m_himap[i].r>max_leaf) max_leaf = m_himap[i].r;
		if(m_himap[i].is_leaf && m_himap[i].r<min_leaf) min_leaf = m_himap[i].r;
	}
		
	cout<<" n QSPLAT Node:"<<m_num_hipix<<" n Leaf Node:"<<m_num_leaf<<" min leaf size:"<<min_leaf<<" max leaf size:"<<max_leaf<<endl;
}

void FQSPLAT::create(pcdSample* rec, CoeRec* coe_rec, int n_rec)
{
	int m_point_count = n_rec;
	m_max_level = 13;
	m_draw_level = 5;
	m_himap = new HierarchyQSPLATNode[n_rec/2];
	m_coe_buf = new CoeRec[n_rec/2];
	
		XYZ mean = findMean(rec, 0, m_point_count-1);
		MATRIX44F objectSpace;
		calculateObjectSpace(rec, 0, m_point_count-1, objectSpace);
		
		MATRIX44F objectSpaceInverse = objectSpace;
		objectSpaceInverse.inverse();
	
	for(int i=0; i<m_point_count; i++) objectSpaceInverse.transform(rec[i].pos);

	m_num_hipix = 0;
	m_num_leaf = 0;
	createTree(rec, coe_rec, m_himap, m_coe_buf, m_num_hipix, 0, m_point_count-1, 0, -1, 0);
	
	float max_leaf = 0, min_leaf = 10e6;
	for(int i=0; i<m_num_hipix; i++) 
	{
		objectSpace.transform(m_himap[i].mean);
		if(m_himap[i].is_leaf && m_himap[i].r>max_leaf) max_leaf = m_himap[i].r;
		if(m_himap[i].is_leaf && m_himap[i].r<min_leaf) min_leaf = m_himap[i].r;
	}
		
	cout<<" N QSPLAT Node:"<<m_num_hipix<<" N Leaf Node:"<<m_num_leaf<<" min leaf size:"<<min_leaf<<" max leaf size:"<<max_leaf<<endl;
}

void FQSPLAT::createRTandColor(pcdSample* rec, CoeRec* coe_rec, int n_rec)
{
	int m_point_count = n_rec;
	m_max_level = 13;
	m_draw_level = 5;
	m_himap = new HierarchyQSPLATNode[n_rec/2];
	m_coe_buf = new CoeRec[n_rec/2];
	m_col_buf = new XYZ[n_rec/2];
	
		XYZ mean = findMean(rec, 0, m_point_count-1);
		MATRIX44F objectSpace;
		calculateObjectSpace(rec, 0, m_point_count-1, objectSpace);
		
		MATRIX44F objectSpaceInverse = objectSpace;
		objectSpaceInverse.inverse();
	
	for(int i=0; i<m_point_count; i++) objectSpaceInverse.transform(rec[i].pos);

	m_num_hipix = 0;
	m_num_leaf = 0;
	createTree(rec, coe_rec, m_himap, m_coe_buf, m_col_buf, m_num_hipix, 0, m_point_count-1, 0, -1, 0);
	
	float max_leaf = 0, min_leaf = 10e6;
	for(int i=0; i<m_num_hipix; i++) 
	{
		objectSpace.transform(m_himap[i].mean);
		if(m_himap[i].is_leaf && m_himap[i].r>max_leaf) max_leaf = m_himap[i].r;
		if(m_himap[i].is_leaf && m_himap[i].r<min_leaf) min_leaf = m_himap[i].r;
	}
		
	cout<<" N QSPLAT Node:"<<m_num_hipix<<" N Leaf Node:"<<m_num_leaf<<" min leaf size:"<<min_leaf<<" max leaf size:"<<max_leaf<<endl;
}

void FQSPLAT::combineSamplesNoColor(pcdSample* data, int lo, int hi, HierarchyQSPLATNode& node)
{
	node.mean = XYZ(0.f);
	node.nor = XYZ(0.f);
	//node.kt = node.kr = 0.f;
	float radius = 0.0f;
	
	for(int i = lo; i <=hi; i++)
	{
		node.mean += data[i].pos;
		node.nor += data[i].nor;
		radius += data[i].area;
		//node.kr += data[i].col.x;
		//node.kt += data[i].col.y;
	}
	
	int div = hi-lo+1;
	node.mean /= (float)div;
	//node.kr /= (float)div;
	//node.kt /= (float)div;
	radius /= (float)div;
	
	node.nor.normalize();
	
	node.r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++)
	{
		tocen = data[i].pos - node.mean;
		d2cen = tocen.length();
		if(d2cen>node.r) node.r = d2cen;
	}
	
	node.r += radius;
	
	node.cone = 1.0f;

	for(int i = lo; i <=hi; i++)
	{
		radius = node.nor.dot(data[i].nor);
		if(radius < node.cone) node.cone = radius;
	}
	node.cone -= 1.0f;
}


void FQSPLAT::combineSamples(pcdSample* data, int lo, int hi, HierarchyQSPLATNode& node, XYZ& color)
{
	color.x =color.y = color.z =0.f;
	node.mean = XYZ(0.f);
	node.nor = XYZ(0.f);
	//node.kt = node.kr = 0.f;
	float radius = 0.0f;
	
	for(int i = lo; i <=hi; i++)
	{
		node.mean += data[i].pos;
		node.nor += data[i].nor;
		radius += data[i].area;
		color += data[i].col;
	}
	
	int div = hi-lo+1;
	node.mean /= (float)div;
	color /= (float)div;
	radius /= (float)div;

	node.nor.normalize();
	
	node.r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++)
	{
		tocen = data[i].pos - node.mean;
		d2cen = tocen.length();
		if(d2cen>node.r) node.r = d2cen;
	}
	
	node.r += radius;
	
	node.cone = 1.0f;

	for(int i = lo; i <=hi; i++)
	{
		radius = node.nor.dot(data[i].nor);
		if(radius < node.cone) node.cone = radius;
	}
	node.cone -= 1.0f;
}

void FQSPLAT::combineSamples(pcdSample* data, CoeRec* coe_data, int lo, int hi, HierarchyQSPLATNode& node, CoeRec& node_coe, XYZ& node_color)
{
	node_color.x = node_color.y = node_color.z =0.f;
	node.mean = XYZ(0.f);
	node.nor = XYZ(0.f);
	float radius = 0.0f;
	for(int j=0; j<16; j++)
	{
		node_coe.data[j] = 0.f;
	}
	
	for(int i = lo; i <=hi; i++)
	{
		node.mean += data[i].pos;
		node.nor += data[i].nor;
		radius += data[i].area;
		//node.kr += data[i].col.x;
		//node.kt += data[i].col.y;
		for(int j=0; j<16; j++)
		{
			node_coe.data[j] += coe_data[i].data[j];
		}
		node_color += data[i].col;
	}
	
	int div = hi-lo+1;
	node.mean /= (float)div;
	//node.kr /= (float)div;
	radius /= (float)div;
	node_color /= (float)div;
	
	for(int j=0; j<16; j++)
	{
		node_coe.data[j] /= (float)div;
	}
	
	node.nor.normalize();
	
	node.r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++)
	{
		tocen = data[i].pos - node.mean;
		d2cen = tocen.length();
		if(d2cen>node.r) node.r = d2cen;
	}
	
	node.r += radius;
	
	node.cone = 1.0f;

	for(int i = lo; i <=hi; i++)
	{
		radius = node.nor.dot(data[i].nor);
		if(radius < node.cone) node.cone = radius;
	}
	node.cone -= 1.0f;
}

void FQSPLAT::combineSamples(pcdSample* data, CoeRec* coe_data, int lo, int hi, HierarchyQSPLATNode& node, CoeRec& node_coe)
{
	node.mean = XYZ(0.f);
	node.nor = XYZ(0.f);
	//node.kt = node.kr = 0.f;
	float radius = 0.0f;
	for(int j=0; j<16; j++)
	{
		node_coe.data[j] = 0.f;
	}
	
	for(int i = lo; i <=hi; i++)
	{
		node.mean += data[i].pos;
		node.nor += data[i].nor;
		radius += data[i].area;
		//node.kr += data[i].col.x;
		//node.kt += data[i].col.y;
		for(int j=0; j<16; j++)
		{
			node_coe.data[j] += coe_data[i].data[j];
		}
	}
	
	int div = hi-lo+1;
	node.mean /= (float)div;
	//node.kr /= (float)div;
	//node.kt /= (float)div;
	radius = radius/(float)div;
	
	for(int j=0; j<16; j++)
	{
		node_coe.data[j] /= (float)div;
	}
	
	node.nor.normalize();
	
	node.r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++)
	{
		tocen = data[i].pos - node.mean;
		d2cen = tocen.length();
		if(d2cen>node.r) node.r = d2cen;
	}
	
	node.r += radius;
	
	node.cone = 1.0f;

	for(int i = lo; i <=hi; i++)
	{
		radius = node.nor.dot(data[i].nor);
		if(radius < node.cone) node.cone = radius;
	}
	node.cone -= 1.0f;
}

void FQSPLAT::createTreeNoColor(pcdSample* data, HierarchyQSPLATNode* res, unsigned int& count, int lo, int hi, short level, int parent_id, short child_id)
{
	if(hi<lo) return;
	HierarchyQSPLATNode ap;
	ap.level = level;
	ap.is_leaf = 0;
	unsigned int nodeId = count;
	
	combineSamplesNoColor(data, lo, hi, ap);
	
	if(hi==lo) 
	{
		ap.is_leaf = 1;
		res[nodeId] = ap;
	
		if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		count++;
		return;
	}

	res[nodeId] = ap;
	
	count++;
	
	if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		
	if(level>11 || (hi-lo < 8 && ap.cone > -0.1))
	{
		res[nodeId].is_leaf = 1;
		m_num_leaf++;
		return;
	}
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	//float dx = xmax - cx;
	//float dy = ymax - cy;
	//float dz = zmax - cz;
	//
	////float bound = dx + dy +dz;
	//float bound = dx;
	//if(dy > bound) bound = dy;
	//if(dz > bound) bound = dz; bound *= 1.733f;
	//
	//if(bound > res[nodeId].r) res[nodeId].r = bound;
	
	level++;
	
	DivideBy<XYZ> adivide;
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamplesNoColor(data, lo, a, ta);
	combineSamplesNoColor(data, b, hi, tb);
	
	divideByAngle(adivide, data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	createTreeNoColor(data, res, count, lo, 		aa, 	 level, nodeId, 0);
	createTreeNoColor(data, res, count, ab, 		a, 	 level, nodeId, 1);
	createTreeNoColor(data, res, count, b, 		ba, 	 level, nodeId, 2);
	createTreeNoColor(data, res, count, bb, 		hi, 	level, nodeId, 3);
}

void FQSPLAT::createTree(pcdSample* data, HierarchyQSPLATNode* res, XYZ* res_color, unsigned int& count, int lo, int hi, short level, int parent_id, short child_id)
{
	if(hi<lo) return;
	HierarchyQSPLATNode ap;
	XYZ ac;
	ap.level = level;
	ap.is_leaf = 0;
	unsigned int nodeId = count;
	
	combineSamples(data, lo, hi, ap, ac);//cout<<" "<<ac.x;
	
	if(hi==lo) 
	{
		ap.is_leaf = 1;
		res[nodeId] = ap;
		res_color[nodeId] = ac;
	
		if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		count++;
		return;
	}

	res[nodeId] = ap;
	res_color[nodeId] = ac;
	
	count++;
	
	if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		
	if(level>11 || (hi-lo < 8 && ap.cone > -0.1))
	{
		res[nodeId].is_leaf = 1;
		m_num_leaf++;
		return;
	}
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	//float dx = xmax - cx;
	//float dy = ymax - cy;
	//float dz = zmax - cz;
	//
	////float bound = dx + dy +dz;
	//float bound = dx;
	//if(dy > bound) bound = dy;
	//if(dz > bound) bound = dz; bound *= 1.733f;
	//
	//if(bound > res[nodeId].r) res[nodeId].r = bound;
	
	level++;
	
	DivideBy<XYZ> adivide;
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamples(data, lo, a, ta, ac);
	combineSamples(data, b, hi, tb, ac);
	
	divideByAngle(adivide, data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	createTree(data, res, res_color, count, lo, 		aa, 	 level, nodeId, 0);
	createTree(data, res, res_color, count, ab, 		a, 	 level, nodeId, 1);
	createTree(data, res, res_color, count, b, 		ba, 	 level, nodeId, 2);
	createTree(data, res, res_color, count, bb, 		hi, 	level, nodeId, 3);
}

void FQSPLAT::createTree(pcdSample* data, CoeRec* coe_data, HierarchyQSPLATNode* res, CoeRec* coe_res, unsigned int& count, int lo, int hi, short level, int parent_id, short child_id)
{
	if(hi<lo) return;
	HierarchyQSPLATNode ap;
	CoeRec acoe;
	ap.level = level;
	ap.is_leaf = 0;
	unsigned int nodeId = count;

	combineSamples(data, coe_data, lo, hi, ap, acoe);
	
	if(hi==lo) 
	{
		ap.is_leaf = 1;
		res[nodeId] = ap;
		coe_res[nodeId] = acoe;
		m_num_leaf++;
		if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		count++;
		return;
	}

	res[nodeId] = ap;
	coe_res[nodeId] = acoe;
	
	count++;
	
	if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		
	if(level>11 || (hi-lo < 8 && ap.cone > -0.1))
	{
		res[nodeId].is_leaf = 1;
		m_num_leaf++;
		return;
	}
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	//float dx = xmax - cx;
	//float dy = ymax - cy;
	//float dz = zmax - cz;
	//
	////float bound = dx + dy +dz;
	//float bound = dx;
	//if(dy > bound) bound = dy;
	//if(dz > bound) bound = dz; bound *= 1.733f;
	//
	//if(bound > res[nodeId].r) res[nodeId].r = bound;
	
	level++;
	
	DivideBy<CoeRec> adivide;
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, coe_data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	//CoeRec acoe;
	combineSamples(data, coe_data, lo, a, ta, acoe);
	combineSamples(data, coe_data, b, hi, tb, acoe);
	
	divideByAngle(adivide, data, coe_data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, coe_data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	createTree(data, coe_data, res, coe_res, count, lo, 		aa, 	 level, nodeId, 0);
	createTree(data, coe_data, res, coe_res, count, ab, 		a, 	 level, nodeId, 1);
	createTree(data, coe_data, res, coe_res, count, b, 		ba, 	 level, nodeId, 2);
	createTree(data, coe_data, res, coe_res, count, bb, 		hi, 	level, nodeId, 3);
}

void FQSPLAT::createTree(pcdSample* data, CoeRec* coe_data, HierarchyQSPLATNode* res, CoeRec* coe_res, XYZ* color_res, unsigned int& count, int lo, int hi, short level, int parent_id, short child_id)
{
	if(hi<lo) return;
	HierarchyQSPLATNode ap;
	CoeRec acoe;
	XYZ ac;
	ap.level = level;
	ap.is_leaf = 0;
	unsigned int nodeId = count;

	combineSamples(data, coe_data, lo, hi, ap, acoe, ac);
	
	if(hi==lo) 
	{
		ap.is_leaf = 1;
		res[nodeId] = ap;
		coe_res[nodeId] = acoe;
		color_res[nodeId] = ac;
		m_num_leaf++;
		if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		count++;
		return;
	}

	res[nodeId] = ap;
	coe_res[nodeId] = acoe;
	color_res[nodeId] = ac;
	
	count++;
	
	if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		
	if(level>11 || (hi-lo < 8 && ap.cone > -0.1))
	{
		res[nodeId].is_leaf = 1;
		m_num_leaf++;
		return;
	}
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	level++;
	
	DivideBy<CoeRec> adivide;
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, coe_data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	//CoeRec acoe;
	combineSamples(data, coe_data, lo, a, ta, acoe);
	combineSamples(data, coe_data, b, hi, tb, acoe);
	
	divideByAngle(adivide, data, coe_data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, coe_data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	createTree(data, coe_data, res, coe_res, color_res, count, lo, 		aa, 	 level, nodeId, 0);
	createTree(data, coe_data, res, coe_res, color_res, count, ab, 		a, 	 level, nodeId, 1);
	createTree(data, coe_data, res, coe_res, color_res, count, b, 		ba, 	 level, nodeId, 2);
	createTree(data, coe_data, res, coe_res, color_res, count, bb, 		hi, 	level, nodeId, 3);
}
/*
void FQSPLAT::createTree(pcdSample* data, HierarchyQSPLATNode* res, XYZ* res_color, unsigned int& count, int lo, int hi, short level, int parent_id, short child_id)
{
	HierarchyQSPLATNode ap;
	ap.level = level;
	ap.is_leaf = 0;
	unsigned int nodeId = count;
	
	if(hi==lo) 
	{
		ap.is_leaf = 1;
		res[nodeId] = ap;
			
		if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
	
		count++;
		return;
	}

	combineSamples(data, lo, hi, ap, res_color[nodeId]);
	
	res[nodeId] = ap;
	
	count++;
	
	if(parent_id>-1) res[parent_id].child[child_id] = nodeId;
		
	if(level>16 || (hi-lo < 9 && ap.cone > 0.91f))
	{
		res[nodeId].is_leaf = 1;
		m_num_leaf++;
		return;
	}
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	float dx = xmax - cx;
	float dy = ymax - cy;
	float dz = zmax - cz;
	
	//float bound = dx + dy +dz;
	float bound = dx;
	if(dy > bound) bound = dy;
	if(dz > bound) bound = dz; bound *= 1.733f;
	
	if(bound > res[nodeId].r) res[nodeId].r = bound;
	
	level++;
	
	DivideBy<XYZ> adivide;
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamples(data, lo, a, ta);
	combineSamples(data, b, hi, tb);
	
	divideByAngle(adivide, data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	createTree(data, res, res_color, count, lo, 		aa, 	 level, nodeId, 0);
	createTree(data, res, res_color, count, ab, 		a, 	 level, nodeId, 1);
	createTree(data, res, res_color, count, b, 		ba, 	 level, nodeId, 2);
	createTree(data, res, res_color, count, bb, 		hi, 	level, nodeId, 3);

}
*/
void FQSPLAT::lookup(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookup(origin, ray, 0, visibility, bias);
}

void FQSPLAT::recursive_lookup(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility, const float bias) const
{
	HierarchyQSPLATNode node = m_himap[id];
		
	float z = visibility->getDepth(node.mean);

	if(z+node.r < 0) return;

	XYZ tosamp = node.mean - origin;
	float dist = tosamp.length();
	
	tosamp.normalize();

	float solid_angle = node.r/dist;
	
	float end_threshold = VIS_DELTA_THETA + (1.0f-tosamp.dot(ray))*VIS_DELTA_THETA;
	
	if(solid_angle < end_threshold || node.is_leaf)
	{
		if(dist < bias) return;
		float tsdotn = tosamp.dot(node.nor);
		if( tsdotn< 0) return;

		int n_grd = 1 + solid_angle*2/VIS_DELTA_THETA*tsdotn;
		visibility->write(node.mean, dist, n_grd);
	}
	else
	{
		recursive_lookup(origin, ray, node.child[0], visibility, bias);
		recursive_lookup(origin, ray, node.child[1], visibility, bias);
		recursive_lookup(origin, ray, node.child[2], visibility, bias);
		recursive_lookup(origin, ray, node.child[3], visibility, bias);
	}
}

void FQSPLAT::lookupNoDist(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupNoDist(origin, ray, 0, visibility, bias);
}

void FQSPLAT::recursive_lookupNoDist(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility, const float bias) const
{
	HierarchyQSPLATNode node = m_himap[id];
		
	float z = visibility->getDepth(node.mean);

	if(z+node.r < 0) return;

	XYZ tosamp = node.mean - origin;
	float dist = tosamp.length();
	
	tosamp.normalize();
	
	float tsdotn = tosamp.dot(node.nor);
	
	if(tsdotn+node.r*node.r/(dist*dist+node.r*node.r) < node.cone) return;
	
	float solid_angle = node.r/dist;

	if(solid_angle < VIS_DELTA_THETA || node.is_leaf)
	{
		if(dist < bias) return;
		//float tsdotn = tosamp.dot(node.nor);
		if( tsdotn< 0) return;

		int n_grd = 1 + solid_angle/VIS_DELTA_THETA;
		visibility->write(node.mean, n_grd);
	}
	else
	{
		recursive_lookupNoDist(origin, ray, node.child[0], visibility, bias);
		recursive_lookupNoDist(origin, ray, node.child[1], visibility, bias);
		recursive_lookupNoDist(origin, ray, node.child[2], visibility, bias);
		recursive_lookupNoDist(origin, ray, node.child[3], visibility, bias);
	}
}

void FQSPLAT::lookupInterreflect(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility) const
{
	recursive_lookupInterreflect(origin, ray, 0, visibility);
}

void FQSPLAT::recursive_lookupInterreflect(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility) const
{
	HierarchyQSPLATNode node = m_himap[id];
		
	float z = visibility->getDepth(node.mean);

	if(z+node.r < 0) return;

	XYZ toori = origin - node.mean;
	float dist = toori.length();
	
	toori.normalize();
	
	float toroidotn = toori.dot(node.nor);
	
	if(toroidotn+node.r*node.r/(dist*dist+node.r*node.r) < node.cone) return;

	float solid_angle = node.r/dist;

	if(solid_angle < VIS_DELTA_THETA || node.is_leaf)
	{
		//if(node.nor.dot(tosamp) > 0) return;
		if(toroidotn < 0) return;

		int n_grd = 1 + solid_angle/VIS_DELTA_THETA;
		visibility->write(node.mean, dist, (unsigned int)id, 1.0f, n_grd);
	}
	else
	{
		recursive_lookupInterreflect(origin, ray, node.child[0], visibility);
		recursive_lookupInterreflect(origin, ray, node.child[1], visibility);
		recursive_lookupInterreflect(origin, ray, node.child[2], visibility);
		recursive_lookupInterreflect(origin, ray, node.child[3], visibility);
	}
}

void FQSPLAT::lookupSubsurface(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupSubsurface(origin, ray, 0, visibility, bias);
}

void FQSPLAT::recursive_lookupSubsurface(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility, const float bias) const
{
	HierarchyQSPLATNode node = m_himap[id];
	node.mean += node.nor*bias*.23f;
		
	float z = visibility->getDepth(node.mean);

	if(z+node.r < 0) return;

	XYZ tosamp = node.mean - origin;
	float dist = tosamp.length();
	if(dist-node.r > bias*5) return;
	
	tosamp.normalize();
	float tsdotn = tosamp.dot(node.nor);
	if(tsdotn+node.r*node.r/(dist*dist+node.r*node.r) < node.cone) return;
	
	float solid_angle = node.r/dist;
	
	if( solid_angle<  VIS_DELTA_THETA || node.is_leaf)
	{
		//if(node.nor.dot(ray)<0) return;
		if(tsdotn<0) return;

		int n_grd = 1+solid_angle/VIS_DELTA_THETA;
		visibility->write(node.mean, dist, (unsigned int)id, 1.0f, n_grd);
	}
	else
	{
		recursive_lookupSubsurface(origin, ray, node.child[0], visibility, bias);
		recursive_lookupSubsurface(origin, ray, node.child[1], visibility, bias);
		recursive_lookupSubsurface(origin, ray, node.child[2], visibility, bias);
		recursive_lookupSubsurface(origin, ray, node.child[3], visibility, bias);
	}
}

void FQSPLAT::lookupSubsurfaceB(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupSubsurfaceB(origin, ray, 0, visibility, bias);
}

void FQSPLAT::recursive_lookupSubsurfaceB(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility, const float bias) const
{
	HierarchyQSPLATNode node = m_himap[id];
	node.mean += node.nor*bias*1.3f;
		
	float z = visibility->getDepth(node.mean);

	if(z+node.r < 0) return;

	XYZ tosamp = node.mean - origin;
	float dist = tosamp.length();
	if(dist-node.r > bias*31) return;
	
	tosamp.normalize();
	float tsdotn = tosamp.dot(node.nor);
	if(tsdotn+node.r*node.r/(dist*dist+node.r*node.r) < node.cone) return;
	
	float solid_angle = node.r/dist;
	
	if( solid_angle<  VIS_DELTA_THETA || node.is_leaf)
	{
		if(node.nor.dot(ray)<0) return;

		int n_grd = 1+solid_angle/VIS_DELTA_THETA; 
		visibility->write(node.mean, dist, (unsigned int)id, 1.0f, n_grd);

	}
	else
	{
		recursive_lookupSubsurfaceB(origin, ray, node.child[0], visibility, bias);
		recursive_lookupSubsurfaceB(origin, ray, node.child[1], visibility, bias);
		recursive_lookupSubsurfaceB(origin, ray, node.child[2], visibility, bias);
		recursive_lookupSubsurfaceB(origin, ray, node.child[3], visibility, bias);
	}
}
/*
void FQSPLAT::lookupBone(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupBone(origin, ray, 0, visibility, bias);
}

void FQSPLAT::recursive_lookupBone(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility, const float bias) const
{
	HierarchyQSPLATNode node = m_himap[id];
		
	float z = visibility->getDepth(node.mean);
	if(z+node.r < 0) return;

	XYZ tosamp = node.mean - origin;
	float dist = tosamp.length();
	if(dist-node.r > bias*128) return;
	tosamp.normalize();
	
	float solid_angle = node.r/dist;

	if(solid_angle < VIS_DELTA_THETA || node.is_leaf)
	{
		if(node.nor.dot(ray)<0) return;
		if(dist<bias) return;
			int n_grd = 1+solid_angle/VIS_DELTA_THETA;
			visibility->write(node.mean, dist, (unsigned int)id, 1.0, n_grd);

	}
	else
	{
		recursive_lookupBone(origin, ray, node.child[0], visibility, bias);
		recursive_lookupBone(origin, ray, node.child[1], visibility, bias);
		recursive_lookupBone(origin, ray, node.child[2], visibility, bias);
		recursive_lookupBone(origin, ray, node.child[3], visibility, bias);
	}
}
*/
/*Lightsource lookup*/
void FQSPLAT::lookupLight(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility) const
{
	recursive_lookupLight(origin, ray, 0, visibility);
}

void FQSPLAT::recursive_lookupLight(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility) const
{
	HierarchyQSPLATNode node = m_himap[id];
		
	float z = visibility->getDepth(node.mean);
	if(z+node.r < 0) return;

	XYZ tosamp = node.mean - origin;
	float dist = tosamp.length();
	
	tosamp.normalize();
	
	float solid_angle = node.r/dist;
	
	float end_threshold = VIS_DELTA_THETA + (1.0f-tosamp.dot(ray))*VIS_DELTA_THETA;

	if(solid_angle < end_threshold || node.is_leaf)
	{
		float facing = tosamp.reversed().dot(node.nor);
		if(facing<0) return;

		int n_grd = 1 + solid_angle*2/VIS_DELTA_THETA*facing;
		visibility->write(node.mean, dist, (unsigned int)id, facing, n_grd);
	}
	else
	{
		recursive_lookupLight(origin, ray, node.child[0], visibility);
		recursive_lookupLight(origin, ray, node.child[1], visibility);
		recursive_lookupLight(origin, ray, node.child[2], visibility);
		recursive_lookupLight(origin, ray, node.child[3], visibility);
	}
}

///*look for color buffer file*/
//int FQSPLAT::checkExistingColorFile() const
//{
//	string rtName = filename + ".col";
//	ifstream ifile;
//	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
//	
//	if(ifile.is_open())
//	{
//		ifstream::pos_type size = ifile.tellg();
//		ifile.close();
//		if((int)size == m_num_hipix*sizeof(XYZ)) return 1;
//	}
//	
//	return 0;
//}
///*read color buffer file*/
//int FQSPLAT::readExistingColorFile(XYZ* data) const
//{
//	string rtName = filename + ".col";
//	ifstream ifile;
//	ifile.open(rtName.c_str(), ios::in | ios::binary | ios::ate);
//	
//	if(ifile.is_open())
//	{
//		ifstream::pos_type size = ifile.tellg();
//		
//		if((int)size == m_num_hipix*3*sizeof(float))
//		{
//			ifile.seekg(0, ios::beg);
//			ifile.read((char*)data, m_num_hipix*3*sizeof(float));
//			ifile.close();
//		}
//		else
//		{
//			ifile.close();
//			cout<<" ERROR: existing col file "<<rtName<<" is not usable! ";
//			return 0;
//		}
//		
//	}
//	else
//	{
//		cout<<" ERROR: failed to open "<<rtName<<" to read! ";
//	}
//	
//	return 1;
//}

void FQSPLAT::divideByAngle(DivideBy<XYZ>adivide, pcdSample* data, float cx, float cy, float cz, int lo, int hi, int& a, int& b)
{
	//DivideBy<XYZ> adivide;
	
	//HierarchyQSPLATNode t, ta, tb;
	
	//combineSamplesNoColor(data, lo, hi, t);
	
	/*if(t.cone<0.f)
	{
		adivide.X(data, lo, hi, cx, a, b);
		combineSamples(data, lo, a, ta);
		combineSamples(data, b, hi, tb);
		float conex = ta.cone + tb.cone;
		
		adivide.Y(data, lo, hi, cy, a, b);
		combineSamples(data, lo, a, ta);
		combineSamples(data, b, hi, tb);
		float coney = ta.cone + tb.cone;
		
		adivide.Z(data, lo, hi, cz, a, b);
		combineSamples(data, lo, a, ta);
		combineSamples(data, b, hi, tb);
		float conez = ta.cone + tb.cone;
		
		if(conex>coney && conex>conez)
		{
			adivide.X(data, lo, hi, cx, a, b);
			
		}
		else if(coney>conex && coney>conez)
		{
			adivide.Y(data, lo, hi, cy, a, b);
		}
		else
		{
			adivide.Z(data, lo, hi, cz, a, b);
		}
	}
	else*/
	//{
		float xmin, xmax, ymin, ymax, zmin, zmax;
		findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
		
		float dx = xmax - cx;
		float dy = ymax - cy;
		float dz = zmax - cz;
		
		if(dx>dy && dx>dz)
		{
			adivide.X(data, lo, hi, cx, a, b);
			
		}
		else if(dy>dx && dy>dz)
		{
			adivide.Y(data, lo, hi, cy, a, b);
		}
		else
		{
			adivide.Z(data, lo, hi, cz, a, b);
		}
	//}
}

void FQSPLAT::divideByAngle(DivideBy<CoeRec>adivide, pcdSample* data, CoeRec* coe_data, float cx, float cy, float cz, int lo, int hi, int& a, int& b)
{
	//HierarchyQSPLATNode t, ta, tb;
	//CoeRec acoe;
	//combineSamples(data, coe_data, lo, hi, t, acoe);
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
		findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
		
	float dx = xmax - cx;
	float dy = ymax - cy;
	float dz = zmax - cz;
	
	if(dx>dy && dx>dz)
	{
		adivide.X(data, coe_data, lo, hi, cx, a, b);
		
	}
	else if(dy>dx && dy>dz)
	{
		adivide.Y(data, coe_data, lo, hi, cy, a, b);
	}
	else
	{
		adivide.Z(data, coe_data, lo, hi, cz, a, b);
	}
}
//:~
