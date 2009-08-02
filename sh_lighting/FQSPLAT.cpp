#include "FQSPLAT.h"
#include "../shared/zMath.h"
#include "../shared/eigenSystem.h"
#include "../shared/gBase.h"
#define MAX_N_SPLAT 1548576
#define MIN_CONE -0.1
#define POOLSCALE 2.25

FQSPLAT::FQSPLAT():m_root(0),is_null(1),m_col_buf(0),m_coe_buf(0)
{}

FQSPLAT::~FQSPLAT(void)
{
	if(m_root) delete[] m_root;
		if(m_col_buf) delete[] m_col_buf;
			if(m_coe_buf) delete[] m_coe_buf;
}

void FQSPLAT::create(pcdSample* rec, int n_rec)
{
	int m_point_count = n_rec;
	//m_max_level = 13;
	//m_draw_level = 5;
	
	m_root = new HierarchyQSPLATNode();
	int pool_size = n_rec*POOLSCALE;
	m_col_buf = new XYZ[pool_size];
	
	m_num_hipix = 0;
	m_num_leaf = 0;
	createTree(rec, m_root, m_col_buf, m_num_hipix, 0, m_point_count-1);
		
	cout<<" N QSPLAT Node:"<<m_num_hipix<<" N Leaf Node:"<<m_num_leaf;
}

void FQSPLAT::createNoColor(pcdSample* rec, int n_rec)
{
	int m_point_count = n_rec;

	m_root = new HierarchyQSPLATNode();

	m_num_hipix = 0;
	m_num_leaf = 0;
	createTreeNoColor(rec, m_root, m_num_hipix, 0, m_point_count-1);
	/*
	float max_leaf = 0, min_leaf = 10e6;
	for(unsigned i=0; i<m_num_hipix; i++) 
	{
		//objectSpace.transform(m_root[i].mean);
		if(m_root[i].is_leaf && m_root[i].r>max_leaf) max_leaf = m_root[i].r;
		if(m_root[i].is_leaf && m_root[i].r<min_leaf) min_leaf = m_root[i].r;
	}
		
	cout<<" n QSPLAT Node:"<<m_num_hipix<<" n Leaf Node:"<<m_num_leaf<<" min leaf size:"<<min_leaf<<" max leaf size:"<<max_leaf<<endl;
	*/
}

void FQSPLAT::createRT(pcdSample* rec, CoeRec* coe_rec, int n_rec)
{
	int m_point_count = n_rec;
	
	int pool_size = n_rec*POOLSCALE;
	m_root = new HierarchyQSPLATNode();
	m_coe_buf = new CoeRec[pool_size];

	m_num_hipix = 0;
	m_num_leaf = 0;
	createTreeRT(rec, coe_rec, m_root, m_coe_buf, m_num_hipix, 0, m_point_count-1);
	
	/*float max_leaf = 0, min_leaf = 10e6;
	for(unsigned i=0; i<m_num_hipix; i++) 
	{
		//objectSpace.transform(m_root[i].mean);
		if(m_root[i].is_leaf && m_root[i].r>max_leaf) max_leaf = m_root[i].r;
		if(m_root[i].is_leaf && m_root[i].r<min_leaf) min_leaf = m_root[i].r;
	}
		
	cout<<" N QSPLAT Node:"<<m_num_hipix<<" N Leaf Node:"<<m_num_leaf<<" min leaf size:"<<min_leaf<<" max leaf size:"<<max_leaf<<endl;
	*/
}

void FQSPLAT::createRTandColor(pcdSample* rec, CoeRec* coe_rec, int n_rec)
{
	int m_point_count = n_rec;
	
	int pool_size = n_rec*POOLSCALE;
	m_root = new HierarchyQSPLATNode();
	m_coe_buf = new CoeRec[pool_size];
	m_col_buf = new XYZ[pool_size];
	
	m_num_hipix = 0;
	m_num_leaf = 0;
	createTreeRTNColor(rec, coe_rec, m_root, m_coe_buf, m_col_buf, m_num_hipix, 0, m_point_count-1);
	/*
	float max_leaf = 0, min_leaf = 10e6;
	for(unsigned i=0; i<m_num_hipix; i++) 
	{
		//objectSpace.transform(m_root[i].mean);
		if(m_root[i].is_leaf && m_root[i].r>max_leaf) max_leaf = m_root[i].r;
		if(m_root[i].is_leaf && m_root[i].r<min_leaf) min_leaf = m_root[i].r;
	}
		
	cout<<" N QSPLAT Node:"<<m_num_hipix<<" N Leaf Node:"<<m_num_leaf<<" min leaf size:"<<min_leaf<<" max leaf size:"<<max_leaf<<endl;
	*/
}

void FQSPLAT::combineSamplesNoColor(pcdSample* data, int lo, int hi, HierarchyQSPLATNode& node)
{
	node.mean = XYZ(0.f);
	node.nor = XYZ(0.f);
	float radius = 0.0f;
	
	for(int i = lo; i <=hi; i++)
	{
		node.mean += data[i].pos;
		node.nor += data[i].nor;
		radius += data[i].area;
	}
	
	int div = hi-lo+1;
	node.mean /= (float)div;
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

void FQSPLAT::combineSamples(pcdSample* data, int lo, int hi, HierarchyQSPLATNode *node)
{
	XYZ mean(0.f);
	XYZ nor(0.f);

	float radius = 0.0f;
	
	for(int i = lo; i <=hi; i++) {
		mean += data[i].pos;
		nor += data[i].nor;
		radius += data[i].area;
	}
	
	int div = hi-lo+1;
	mean /= (float)div;
	radius /= (float)div;
	nor.normalize();
	
	float r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++) {
		tocen = data[i].pos - mean;
		d2cen = tocen.length();
		if(d2cen > r) r = d2cen;
	}
	
	r += radius;
	
	float cone = 1.0f;
	for(int i = lo; i <=hi; i++) {
		radius = nor.dot(data[i].nor);
		if(radius < cone) cone = radius;
	}
	cone -= 1.0f;
	
	node->mean = mean;
	node->nor = nor;
	node->r = r;
	node->cone = cone;
}

void FQSPLAT::combineSamples(pcdSample* data, int lo, int hi, HierarchyQSPLATNode* node, XYZ& color)
{
	color.x =color.y = color.z =0.f;
	XYZ mean(0.f);
	XYZ nor(0.f);
	float radius = 0.0f;
	
	for(int i = lo; i <=hi; i++) {
		mean += data[i].pos;
		nor += data[i].nor;
		radius += data[i].area;
		color += data[i].col;
	}
	
	int div = hi-lo+1;
	mean /= (float)div;
	color /= (float)div;
	radius /= (float)div;

	nor.normalize();
	
	float r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++) {
		tocen = data[i].pos - mean;
		d2cen = tocen.length();
		if(d2cen > r) r = d2cen;
	}
	
	r += radius;
	
	float cone = 1.0f;

	for(int i = lo; i <=hi; i++) {
		radius = nor.dot(data[i].nor);
		if(radius < cone) cone = radius;
	}
	cone -= 1.0f;
	node->mean = mean;
	node->nor = nor;
	node->r = r;
	node->cone = cone;
}

void FQSPLAT::combineSamples(pcdSample* data, CoeRec* coe_data, int lo, int hi, HierarchyQSPLATNode* node, CoeRec& node_coe, XYZ& node_color)
{
	node_color.x = node_color.y = node_color.z =0.f;
	XYZ mean(0.f);
	XYZ nor(0.f);
	float radius = 0.0f;
	for(int j=0; j<16; j++) {
		node_coe.data[j] = 0.f;
	}
	
	for(int i = lo; i <=hi; i++) {
		mean += data[i].pos;
		nor += data[i].nor;
		radius += data[i].area;
		for(int j=0; j<16; j++) {
			node_coe.data[j] += coe_data[i].data[j];
		}
		node_color += data[i].col;
	}
	
	int div = hi-lo+1;
	mean /= (float)div;
	radius /= (float)div;
	node_color /= (float)div;
	
	for(int j=0; j<16; j++) {
		node_coe.data[j] /= (float)div;
	}
	
	nor.normalize();
	
	float r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++) {
		tocen = data[i].pos - mean;
		d2cen = tocen.length();
		if(d2cen > r) r = d2cen;
	}
	
	r += radius;
	
	float cone = 1.0f;

	for(int i = lo; i <=hi; i++) {
		radius = nor.dot(data[i].nor);
		if(radius < cone) cone = radius;
	}
	cone -= 1.0f;
	node->mean = mean;
	node->nor = nor;
	node->r = r;
	node->cone = cone;
}

void FQSPLAT::combineSamples(pcdSample* data, CoeRec* coe_data, int lo, int hi, HierarchyQSPLATNode* node, CoeRec& node_coe)
{
	XYZ mean(0.f);
	XYZ nor(0.f);
	float radius = 0.0f;
	for(int j=0; j<16; j++) {
		node_coe.data[j] = 0.f;
	}
	
	for(int i = lo; i <=hi; i++) {
		mean += data[i].pos;
		nor += data[i].nor;
		radius += data[i].area;
		for(int j=0; j<16; j++)
		{
			node_coe.data[j] += coe_data[i].data[j];
		}
	}
	
	int div = hi-lo+1;
	mean /= (float)div;
	radius /= (float)div;
	
	for(int j=0; j<16; j++) {
		node_coe.data[j] /= (float)div;
	}
	
	nor.normalize();
	
	float r = 0;
	XYZ tocen;
	float d2cen;
	
	for(int i = lo; i <=hi; i++) {
		tocen = data[i].pos - mean;
		d2cen = tocen.length();
		if(d2cen > r) r = d2cen;
	}
	
	r += radius;
	
	float cone = 1.0f;

	for(int i = lo; i <=hi; i++) {
		radius = nor.dot(data[i].nor);
		if(radius < cone) cone = radius;
	}
	cone -= 1.0f;
	node->mean = mean;
	node->nor = nor;
	node->r = r;
	node->cone = cone;
}

void FQSPLAT::createTreeNoColor(pcdSample* data, HierarchyQSPLATNode* res, unsigned int& count, int lo, int hi)
{
	if(hi<lo) return;

	res->is_leaf = 1;
	res->index = count; count++;
	combineSamples(data, lo, hi, res);
	
	if(hi==lo) {
		m_num_leaf++;
		return;
	}
		
	if(hi-lo < 4 && res->cone > MIN_CONE) {
		m_num_leaf++;
		return;
	}
	
	res->is_leaf = 0;
	
	
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	//level++;
	
	DivideBy<XYZ> adivide;
	
	MATRIX44F objectSpace;
	if(hi-lo>32) {
			
			calculateObjectSpace(data, lo, hi, objectSpace);
			
			MATRIX44F objectSpaceInverse = objectSpace;
			objectSpaceInverse.inverse();
		
			for(int i=lo; i<=hi; i++) objectSpaceInverse.transform(data[i].pos);
	}
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamplesNoColor(data, lo, a, ta);
	combineSamplesNoColor(data, b, hi, tb);
	
	divideByAngle(adivide, data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	if(hi-lo>32) {
			for(int i=lo; i<=hi; i++) objectSpace.transform(data[i].pos);
	}
	
	res->child0 = new HierarchyQSPLATNode();
	res->child1 = new HierarchyQSPLATNode();
	res->child2 = new HierarchyQSPLATNode();
	res->child3 = new HierarchyQSPLATNode();
	
	createTreeNoColor(data, res->child0, count, lo, 		aa);
	createTreeNoColor(data, res->child1, count, ab,			a);
	createTreeNoColor(data, res->child2, count, b,			ba);
	createTreeNoColor(data, res->child3, count, bb, 		hi);
}

void FQSPLAT::createTree(pcdSample* data, HierarchyQSPLATNode* res, XYZ* res_color, unsigned int& count, int lo, int hi)
{
	if(hi<lo) return;
	
	XYZ ac;
	res->is_leaf = 1;
	res->index = count; count++;
	
	combineSamples(data, lo, hi, res, ac);
	
	res_color[res->index] = ac;
	
	if(hi==lo) {
		m_num_leaf++;
		return;
	}
	
	if(hi-lo < 4 && res->cone > MIN_CONE) {
		m_num_leaf++;
		return;
	}
	
	res->is_leaf = 0;
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	DivideBy<XYZ> adivide;
	
	MATRIX44F objectSpace;
	if(hi-lo>32) {
			
			calculateObjectSpace(data, lo, hi, objectSpace);
			
			MATRIX44F objectSpaceInverse = objectSpace;
			objectSpaceInverse.inverse();
		
			for(int i=lo; i<=hi; i++) objectSpaceInverse.transform(data[i].pos);
	}
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamplesNoColor(data, lo, a, ta);
	combineSamplesNoColor(data, b, hi, tb);
	
	divideByAngle(adivide, data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	if(hi-lo>32) {
			for(int i=lo; i<=hi; i++) objectSpace.transform(data[i].pos);
	}
	
	res->child0 = new HierarchyQSPLATNode();
	res->child1 = new HierarchyQSPLATNode();
	res->child2 = new HierarchyQSPLATNode();
	res->child3 = new HierarchyQSPLATNode();
	
	createTree(data, res->child0, res_color, count, lo, 		aa);
	createTree(data, res->child1, res_color, count, ab, 		a);
	createTree(data, res->child2, res_color, count, b, 		ba);
	createTree(data, res->child3, res_color, count, bb, 		hi);
}

void FQSPLAT::createTreeRT(pcdSample* data, CoeRec* coe_data, HierarchyQSPLATNode* res, CoeRec* coe_res, unsigned int& count, int lo, int hi)
{
	if(hi<lo) return;
	
	CoeRec acoe;
	res->is_leaf = 1;
	res->index = count; count++;

	combineSamples(data, coe_data, lo, hi, res, acoe);
	
	coe_res[res->index] = acoe;
	
	if(hi==lo) {
		m_num_leaf++;
		return;
	}
		
	if(hi-lo < 4 && res->cone > MIN_CONE)
	{
		m_num_leaf++;
		return;
	}
	
	res->is_leaf = 0;
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	DivideBy<CoeRec> adivide;
	
	MATRIX44F objectSpace;
	if(hi-lo>32) {
			
			calculateObjectSpace(data, lo, hi, objectSpace);
			
			MATRIX44F objectSpaceInverse = objectSpace;
			objectSpaceInverse.inverse();
		
			for(int i=lo; i<=hi; i++) objectSpaceInverse.transform(data[i].pos);
	}
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, coe_data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamplesNoColor(data, lo, a, ta);
	combineSamplesNoColor(data, b, hi, tb);
	
	divideByAngle(adivide, data, coe_data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, coe_data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	if(hi-lo>32) {
			for(int i=lo; i<=hi; i++) objectSpace.transform(data[i].pos);
	}
	
	res->child0 = new HierarchyQSPLATNode();
	res->child1 = new HierarchyQSPLATNode();
	res->child2 = new HierarchyQSPLATNode();
	res->child3 = new HierarchyQSPLATNode();
	
	createTreeRT(data, coe_data, res->child0, coe_res, count, lo, 		aa);
	createTreeRT(data, coe_data, res->child1, coe_res, count, ab, 		a);
	createTreeRT(data, coe_data, res->child2, coe_res, count, b, 		ba);
	createTreeRT(data, coe_data, res->child3, coe_res, count, bb, 		hi);
}

void FQSPLAT::createTreeRTNColor(pcdSample* data, CoeRec* coe_data, HierarchyQSPLATNode* res, CoeRec* coe_res, XYZ* color_res, unsigned int& count, int lo, int hi)
{
	if(hi<lo) return;
	CoeRec acoe;
	XYZ ac;
	
	res->is_leaf = 1;
	res->index = count; count++;

	combineSamples(data, coe_data, lo, hi, res, acoe, ac);
	
	coe_res[res->index] = acoe;
	color_res[res->index] = ac;
	
	if(hi==lo) {
		m_num_leaf++;
		return;
	}
	
	if(hi-lo < 4 && res->cone > MIN_CONE)
	{
		m_num_leaf++;
		return;
	}
	
	res->is_leaf = 0;
	
	float xmin, xmax, ymin, ymax, zmin, zmax;
	findCube1(data, lo, hi, xmin, xmax, ymin, ymax, zmin, zmax);
	
	float cx = (xmin + xmax)/2;
	float cy = (ymin + ymax)/2;
	float cz = (zmin + zmax)/2;
	
	DivideBy<CoeRec> adivide;
	
	MATRIX44F objectSpace;
	if(hi-lo>32) {
			
			calculateObjectSpace(data, lo, hi, objectSpace);
			
			MATRIX44F objectSpaceInverse = objectSpace;
			objectSpaceInverse.inverse();
		
			for(int i=lo; i<=hi; i++) objectSpaceInverse.transform(data[i].pos);
	}
	
	int aa, ab, a, b, ba, bb;
	divideByAngle(adivide, data, coe_data, cx, cy, cz, lo, hi, a, b);
	
	HierarchyQSPLATNode ta, tb;
	combineSamplesNoColor(data, lo, a, ta);
	combineSamplesNoColor(data, b, hi, tb);
	
	divideByAngle(adivide, data, coe_data, ta.mean.x, ta.mean.y, ta.mean.z, lo, a, aa, ab);
	divideByAngle(adivide, data, coe_data, tb.mean.x, tb.mean.y, tb.mean.z, b, hi, ba, bb);
	
	if(hi-lo>32) {
			for(int i=lo; i<=hi; i++) objectSpace.transform(data[i].pos);
	}
	
	res->child0 = new HierarchyQSPLATNode();
	res->child1 = new HierarchyQSPLATNode();
	res->child2 = new HierarchyQSPLATNode();
	res->child3 = new HierarchyQSPLATNode();
	
	createTreeRTNColor(data, coe_data, res->child0, coe_res, color_res, count, lo, 		aa);
	createTreeRTNColor(data, coe_data, res->child0, coe_res, color_res, count, ab, 		a);
	createTreeRTNColor(data, coe_data, res->child0, coe_res, color_res, count, b, 		ba);
	createTreeRTNColor(data, coe_data, res->child0, coe_res, color_res, count, bb, 		hi);
}

void FQSPLAT::lookup(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookup(origin, ray, m_root, visibility, bias);
}

void FQSPLAT::recursive_lookup(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const
{
	float z = visibility->getDepth(node->mean);

	if(z+node->r < 0) return;

	XYZ tosamp = node->mean - origin;
	float dist = tosamp.length();
	
	tosamp.normalize();

	float solid_angle = node->r/dist;
	
	float end_threshold = VIS_DELTA_THETA + (1.0f-tosamp.dot(ray))*VIS_DELTA_THETA;
	
	if(solid_angle < end_threshold || node->is_leaf) {
		if(dist < bias) return;
		float tsdotn = tosamp.dot(node->nor);
		if( tsdotn< 0) return;

		int n_grd = 1 + solid_angle*2/VIS_DELTA_THETA*tsdotn;
		visibility->write(node->mean, dist, n_grd);
	}
	else {
		recursive_lookup(origin, ray, node->child0, visibility, bias);
		recursive_lookup(origin, ray, node->child1, visibility, bias);
		recursive_lookup(origin, ray, node->child2, visibility, bias);
		recursive_lookup(origin, ray, node->child3, visibility, bias);
	}
}

void FQSPLAT::lookupNoDist(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupNoDist(origin, ray, m_root, visibility, bias);
}

void FQSPLAT::recursive_lookupNoDist(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const
{
	float z = visibility->getDepth(node->mean);

	if(z+node->r < 0) return;

	XYZ tosamp = node->mean - origin;
	float dist = tosamp.length();
	
	tosamp.normalize();
	
	float tsdotn = tosamp.dot(node->nor);
	
	if(tsdotn+node->r*node->r/(dist*dist+node->r*node->r) < node->cone) return;
	
	float solid_angle = node->r/dist;

	if(solid_angle < VIS_DELTA_THETA || node->is_leaf) {
		if(dist < bias) return;
		if( tsdotn< 0) return;

		int n_grd = 1 + solid_angle/VIS_DELTA_THETA;
		visibility->write(node->mean, n_grd);
	}
	else {
		recursive_lookupNoDist(origin, ray, node->child0, visibility, bias);
		recursive_lookupNoDist(origin, ray, node->child1, visibility, bias);
		recursive_lookupNoDist(origin, ray, node->child2, visibility, bias);
		recursive_lookupNoDist(origin, ray, node->child3, visibility, bias);
	}
}

void FQSPLAT::lookupInterreflect(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility) const
{
	recursive_lookupInterreflect(origin, ray, m_root, visibility);
}

void FQSPLAT::recursive_lookupInterreflect(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility) const
{	
	float z = visibility->getDepth(node->mean);

	if(z+node->r < 0) return;

	XYZ toori = origin - node->mean;
	float dist = toori.length();
	
	toori.normalize();
	
	float toroidotn = toori.dot(node->nor);
	
	if(toroidotn+node->r*node->r/(dist*dist+node->r*node->r) < node->cone) return;

	float solid_angle = node->r/dist;

	if(solid_angle < VIS_DELTA_THETA || node->is_leaf) {
		if(toroidotn < 0) return;

		int n_grd = 1 + solid_angle/VIS_DELTA_THETA;
		visibility->write(node->mean, dist, node->index, 1.0f, n_grd);
	}
	else
	{
		recursive_lookupInterreflect(origin, ray, node->child0, visibility);
		recursive_lookupInterreflect(origin, ray, node->child1, visibility);
		recursive_lookupInterreflect(origin, ray, node->child2, visibility);
		recursive_lookupInterreflect(origin, ray, node->child3, visibility);
	}
}

void FQSPLAT::lookupSubsurface(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupSubsurface(origin, ray, m_root, visibility, bias);
}

void FQSPLAT::recursive_lookupSubsurface(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const
{
	XYZ mean = node->mean + node->nor*bias*.23f;
		
	float z = visibility->getDepth(mean);

	if(z+node->r < 0) return;

	XYZ tosamp = mean - origin;
	float dist = tosamp.length();
	if(dist-node->r > bias*5) return;
	
	tosamp.normalize();
	float tsdotn = tosamp.dot(node->nor);
	if(tsdotn+node->r*node->r/(dist*dist+node->r*node->r) < node->cone) return;
	
	float solid_angle = node->r/dist;
	
	if( solid_angle<  VIS_DELTA_THETA || node->is_leaf) {
		if(tsdotn<0) return;

		int n_grd = 1+solid_angle/VIS_DELTA_THETA;
		visibility->write(mean, dist, node->index, 1.0f, n_grd);
	}
	else {
		recursive_lookupSubsurface(origin, ray, node->child0, visibility, bias);
		recursive_lookupSubsurface(origin, ray, node->child1, visibility, bias);
		recursive_lookupSubsurface(origin, ray, node->child2, visibility, bias);
		recursive_lookupSubsurface(origin, ray, node->child3, visibility, bias);
	}
}

void FQSPLAT::lookupSubsurfaceB(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupSubsurfaceB(origin, ray, m_root, visibility, bias);
}

void FQSPLAT::recursive_lookupSubsurfaceB(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility, const float bias) const
{
	XYZ mean = node->mean + node->nor*bias*1.3f;
		
	float z = visibility->getDepth(mean);

	if(z+node->r < 0) return;

	XYZ tosamp = mean - origin;
	float dist = tosamp.length();
	if(dist-node->r > bias*31) return;
	
	tosamp.normalize();
	float tsdotn = tosamp.dot(node->nor);
	if(tsdotn+node->r*node->r/(dist*dist+node->r*node->r) < node->cone) return;
	
	float solid_angle = node->r/dist;
	
	if( solid_angle<  VIS_DELTA_THETA || node->is_leaf) {
		if(node->nor.dot(ray)<0) return;

		int n_grd = 1+solid_angle/VIS_DELTA_THETA; 
		visibility->write(mean, dist, node->index, 1.0f, n_grd);

	}
	else {
		recursive_lookupSubsurfaceB(origin, ray, node->child0, visibility, bias);
		recursive_lookupSubsurfaceB(origin, ray, node->child1, visibility, bias);
		recursive_lookupSubsurfaceB(origin, ray, node->child2, visibility, bias);
		recursive_lookupSubsurfaceB(origin, ray, node->child3, visibility, bias);
	}
}

/*
void FQSPLAT::lookupBone(const XYZ& origin, const XYZ& ray,  CVisibilityBuffer* visibility, const float bias) const
{
	recursive_lookupBone(origin, ray, 0, visibility, bias);
}

void FQSPLAT::recursive_lookupBone(const XYZ& origin, const XYZ& ray, int id,  CVisibilityBuffer* visibility, const float bias) const
{
	HierarchyQSPLATNode node = m_root[id];
		
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
	recursive_lookupLight(origin, ray, m_root, visibility);
}

void FQSPLAT::recursive_lookupLight(const XYZ& origin, const XYZ& ray, HierarchyQSPLATNode* node,  CVisibilityBuffer* visibility) const
{
	float z = visibility->getDepth(node->mean);
	if(z+node->r < 0) return;

	XYZ tosamp = node->mean - origin;
	float dist = tosamp.length();
	
	tosamp.normalize();
	
	float solid_angle = node->r/dist;
	
	float end_threshold = VIS_DELTA_THETA + (1.0f-tosamp.dot(ray))*VIS_DELTA_THETA;

	if(solid_angle < end_threshold || node->is_leaf) {
		float facing = tosamp.reversed().dot(node->nor);
		if(facing<0) return;

		int n_grd = 1 + solid_angle*2/VIS_DELTA_THETA*facing;
		visibility->write(node->mean, dist, node->index, facing, n_grd);
	}
	else {
		recursive_lookupLight(origin, ray, node->child0, visibility);
		recursive_lookupLight(origin, ray, node->child1, visibility);
		recursive_lookupLight(origin, ray, node->child2, visibility);
		recursive_lookupLight(origin, ray, node->child3, visibility);
	}
}

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

void FQSPLAT::draw(const float& eta) const
{
	recursive_draw(m_root,  eta);
}

void FQSPLAT::recursive_draw(const HierarchyQSPLATNode* node,  const float& eta) const
{
	if(node->r < eta || node->is_leaf) {
		gBase::drawSplatAt(node->mean, node->nor, node->r);
	}
	else {
		recursive_draw(node->child0, eta);
		recursive_draw(node->child1, eta);
		recursive_draw(node->child2, eta);
		recursive_draw(node->child3, eta);
	}
}
//:~
