// fish bone and skining visualization

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MAngle.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MMatrix.h>

#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshVertex.h>
#include <maya/MString.h> 
#include <maya/MItMeshPolygon.h>
#include <maya/MFnNurbsSurfaceData.h>
#include "noiseVolumeNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include "../shared/FNoise.h"

/* noise functions over 1, 2, and 3 dimensions */

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
//static float g3[B + B + 2][3];
//static float g2[B + B + 2][2];
static float g1[B + B + 2];
static int start = 1;

static void init(void);

#define s_curve(t) ( t * t * (3. - 2. * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;
	
float noise1(float arg);
float noise2(float vec[]);
	
float noise1(float arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0); sx = 0.5;

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v)*2;
}
/*
float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}
*/
static void normalize2(float v[2])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(float v[3])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

#define RANDMASK  0x7fffffff
#define RANDNBR   ((random() & RANDMASK)/(double) RANDMASK)

static void init(void)
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((random() % (B + B)) - B) / B;
		
/*
		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((random() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((random() % (B + B)) - B) / B;
		normalize3(g3[i]);*/
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = random() % B];
		p[j] = k;
	}
/*
	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}*/
}
#define TABSIZE          256
#define TABMASK          (TABSIZE-1)
#define PERM(x)          perm[(x)&TABMASK]

#define INDEX(ix,iy,iz)  PERM((ix)+PERM((iy)+PERM(iz)))
#define FLOOR(x) ((int)(x) - ((x) < 0 && (x) != (int)(x)))
#define PERM(x)          perm[(x)&TABMASK]
static float valueTab[TABSIZE];

unsigned char perm[TABSIZE] = {
        225,155,210,108,175,199,221,144,203,116, 70,213, 69,158, 33,252,
          5, 82,173,133,222,139,174, 27,  9, 71, 90,246, 75,130, 91,191,
        169,138,  2,151,194,235, 81,  7, 25,113,228,159,205,253,134,142,
        248, 65,224,217, 22,121,229, 63, 89,103, 96,104,156, 17,201,129,
         36,  8,165,110,237,117,231, 56,132,211,152, 20,181,111,239,218,
        170,163, 51,172,157, 47, 80,212,176,250, 87, 49, 99,242,136,189,
        162,115, 44, 43,124, 94,150, 16,141,247, 32, 10,198,223,255, 72,
         53,131, 84, 57,220,197, 58, 50,208, 11,241, 28,  3,192, 62,202,
         18,215,153, 24, 76, 41, 15,179, 39, 46, 55,  6,128,167, 23,188,
        106, 34,187,140,164, 73,112,182,244,195,227, 13, 35, 77,196,185,
         26,200,226,119, 31,123,168,125,249, 68,183,230,177,135,160,180,
         12,  1,243,148,102,166, 38,238,251, 37,240,126, 64, 74,161, 40,
        184,149,171,178,101, 66, 29, 59,146, 61,254,107, 42, 86,154,  4,
        236,232,120, 21,233,209, 45, 98,193,114, 78, 19,206, 14,118,127,
         48, 79,147, 85, 30,207,219, 54, 88,234,190,122, 95, 67,143,109,
        137,214,145, 93, 92,100,245,  0,216,186, 60, 83,105, 97,204, 52
};

static void valueTabInit(int seed);
static float vlattice(int ix, int iy, int iz);

float
clamp(float x, float a, float b)
{
    return (x < a ? a : (x > b ? b : x));
}

/* Coefficients of basis matrix. */
#define CR00     -0.5
#define CR01      1.5
#define CR02     -1.5
#define CR03      0.5
#define CR10      1.0
#define CR11     -2.5
#define CR12      2.0
#define CR13     -0.5
#define CR20     -0.5
#define CR21      0.0
#define CR22      0.5
#define CR23      0.0
#define CR30      0.0
#define CR31      1.0
#define CR32      0.0
#define CR33      0.0

float
spline(float x, int nknots, float *knot)
{
    int span;
    int nspans = nknots - 3;
    float c0, c1, c2, c3;	/* coefficients of the cubic.*/

    if (nspans < 1) {  /* illegal */
        fprintf(stderr, "Spline has too few knots.\n");
        return 0;
    }

    /* Find the appropriate 4-point span of the spline. */
    x = clamp(x, 0, 1) * nspans;
    span = (int) x;
    if (span >= nknots - 3)
        span = nknots - 3;
    x -= span;
    knot += span;

    /* Evaluate the span cubic at x using Horner's rule. */
    c3 = CR00*knot[0] + CR01*knot[1]
       + CR02*knot[2] + CR03*knot[3];
    c2 = CR10*knot[0] + CR11*knot[1]
       + CR12*knot[2] + CR13*knot[3];
    c1 = CR20*knot[0] + CR21*knot[1]
       + CR22*knot[2] + CR23*knot[3];
    c0 = CR30*knot[0] + CR31*knot[1]
       + CR32*knot[2] + CR33*knot[3];

    return ((c3*x + c2)*x + c1)*x + c0;
}

float
vnoise(float x, float y, float z)
{
    int ix, iy, iz;
    int i, j, k;
    float fx, fy, fz;
    float xknots[4], yknots[4], zknots[4];
    static int initialized = 0;

    if (!initialized) {
        valueTabInit(665);
        initialized = 1;
    }

    ix = FLOOR(x);
    fx = x - ix;

    iy = FLOOR(y);
    fy = y - iy;

    iz = FLOOR(z);
    fz = z - iz;

    for (k = -1; k <= 2; k++) {
        for (j = -1; j <= 2; j++) {
            for (i = -1; i <= 2; i++)
                xknots[i+1] = vlattice(ix+i,iy+j,iz+k);
            yknots[j+1] = spline(fx, 4, xknots);
        }
        zknots[k+1] = spline(fy, 4, yknots);
    }
    return spline(fz, 4, zknots);
}

float
catrom2(float d)
{
#define SAMPRATE 100  /* table entries per unit distance */
#define NENTRIES (4*SAMPRATE+1)
    float x;
    int i;
    static float table[NENTRIES];
    static int initialized = 0;

    if (d >= 4)
        return 0;

    if (!initialized) {
        for (i = 0; i < NENTRIES; i++) {
            x = i/(float) SAMPRATE;
            x = sqrtf(x);
            if (x < 1)
                table[i] = 0.5 * (2+x*x*(-5+x*3));
            else
                table[i] = 0.5 * (4+x*(-8+x*(5-x)));
        }
        initialized = 1;
    }

    d = d*SAMPRATE + 0.5;
    i = FLOOR(d);
    if (i >= NENTRIES)
        return 0;
    return table[i];
}

float
vcnoise(float x, float y, float z)
{
    int ix, iy, iz;
    int i, j, k;
    float fx, fy, fz;
    float dx, dy, dz;
    float sum = 0;
    static int initialized = 0;

    if (!initialized) {
        valueTabInit(665);
        initialized = 1;
    }

    ix = FLOOR(x);
    fx = x - ix;

    iy = FLOOR(y);
    fy = y - iy;

    iz = FLOOR(z);
    fz = z - iz;

    for (k = -1; k <= 2; k++) {
        dz = k - fz;
        dz = dz*dz;
        for (j = -1; j <= 2; j++) {
            dy = j - fy;
            dy = dy*dy;
            for (i = -1; i <= 2; i++){
                dx = i - fx;
                dx = dx*dx;
                sum += vlattice(ix+i,iy+j,iz+k)
                    * catrom2(dx + dy + dz);
            }
        }
    }
    return sum;
}

static void
valueTabInit(int seed)
{
    float *table = valueTab;
    int i;

    srandom(seed);
    for(i = 0; i < TABSIZE; i++)
        *table++ = 1. - 2.*RANDNBR;
}

static float
vlattice(int ix, int iy, int iz)
{
    return valueTab[INDEX(ix,iy,iz)];
}


MTypeId noiseVolumeNode::id( 0x0003018 );


MObject noiseVolumeNode::acachename;
MObject noiseVolumeNode::atime;
MObject noiseVolumeNode::aFrequency;
MObject noiseVolumeNode::aoutput;
MObject noiseVolumeNode::amatrix;
MObject noiseVolumeNode::anear;
MObject noiseVolumeNode::afar;
MObject noiseVolumeNode::ahapeture;
MObject noiseVolumeNode::avapeture;
MObject noiseVolumeNode::afocallength;
MObject noiseVolumeNode::amaxage;

noiseVolumeNode::noiseVolumeNode():m_num_fish(0), m_num_bone(20),m_offset(0)
{
}

noiseVolumeNode::~noiseVolumeNode() 
{
}

MStatus noiseVolumeNode::compute( const MPlug& plug, MDataBlock& data )
{    
    MStatus status;
	if(plug == aoutput)
	{
		double time = data.inputValue(atime).asTime().value();
		m_offset = time;
		m_num_bone = data.inputValue(aFrequency).asDouble();
		data.setClean( plug );
	}
	
	return MS::kSuccess;
}

void* noiseVolumeNode::creator()
{
	return new noiseVolumeNode();
}

/////////////////////////////////////
// Attribute Setup and Maintenance //
/////////////////////////////////////

MStatus noiseVolumeNode::initialize()
{
	MStatus stat;
	
	MFnNumericAttribute nAttr; 
	aoutput = nAttr.create( "output", "output", MFnNumericData::kInt, 1 );
	nAttr.setStorable(false);
	nAttr.setWritable(false);
	nAttr.setKeyable(false);
	addAttribute( aoutput );
	
	stat = zCreateKeyableDoubleAttr(aFrequency, MString("size"), MString("sz"), 1.0);
	zCheckStatus(stat, "failed to add size attr");
	stat = addAttribute( aFrequency);
	
	stat = zCreateTypedAttr(acachename, "cacheName", "cchn", MFnData::kString);
	zCheckStatus(stat, "ERROR creating cache name");
	zCheckStatus(addAttribute(acachename), "ERROR adding cache name");

	stat = zCreateTimeAttr(atime, "currentTime", MString("ct"), 1.0);
	zCheckStatus(stat, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	MFnTypedAttribute matAttr;
	amatrix = matAttr.create( "cameraMatrix", "cm", MFnData::kMatrix );
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	addAttribute( amatrix );
	
	anear = nAttr.create( "nearClip", "nc", MFnNumericData::kFloat, 0.1 );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( anear );
	
	afar = nAttr.create( "farClip", "fc", MFnNumericData::kFloat, 1000.0 );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( afar );
	
	ahapeture = nAttr.create( "horizontalFilmAperture", "hfa", MFnNumericData::kFloat );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( ahapeture );
	
	avapeture = nAttr.create( "verticalFilmAperture", "vfa", MFnNumericData::kFloat );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( avapeture );
	
	afocallength = nAttr.create( "focalLength", "fl", MFnNumericData::kFloat );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( afocallength );

	amaxage= nAttr.create( "maxage", "ma", MFnNumericData::kFloat, 10.0 );
	nAttr.setStorable(false);
	nAttr.setConnectable(true);
	addAttribute( amaxage );
	
	attributeAffects( amatrix, aoutput );
	attributeAffects( anear, aoutput );
	attributeAffects( afar, aoutput );
	attributeAffects( ahapeture, aoutput );
	attributeAffects( avapeture, aoutput );
	attributeAffects( afocallength, aoutput );
	attributeAffects( atime, aoutput );
	attributeAffects( aFrequency, aoutput );
	attributeAffects( amaxage, aoutput );
	
	return MS::kSuccess;
}


MStatus noiseVolumeNode::connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc )
{
	return MS::kUnknownParameter;
}

bool noiseVolumeNode::isBounded() const
{ 
	return false;
}

void sphereRand(XYZ& q, int seed, int idx)
{	
	FNoise fnoi;
	float noi = fnoi.randfint( seed*29 + idx );
	float v = noi;
// 0 <= theta <= 2PI
	float theta = v*2 * PI;

// -1 <= u <= 1
	noi = fnoi.randfint( seed*11 + idx);
	v = (noi -0.5)*2;
		 
	q.x = cos(theta) * sqrt(1.0-(v*v));
	q.y = sin(theta) * sqrt(1.0-(v*v));
	q.z = v;
}

void noiseVolumeNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	//
	float noi;
	XYZ p;
	FNoise fnoi;
	for(int i=0; i< m_num_bone; i++) {
		sphereRand(p, i, m_offset);
		
		float dis = fnoi.randfint(i*73 +m_offset);
		
		//dis = sqrt((dis + 1)*0.5);
		//dis = 0.1 + 0.9*dis;
		p *= sqrt(dis);
		glVertex3f(p.x, p.y, p.z);

	}
	
	glEnd();
	
	glBegin(GL_LINES);
	for(int i=0; i< m_num_bone; i++) {
		noi = fnoi.randfint(i*5 +m_offset);
		glVertex3f(0.06*i,0,0);
		glVertex3f(0.06*i, noi,0);
	}
	glEnd();

	glPopAttrib();
	view.endGL();
}
//:~