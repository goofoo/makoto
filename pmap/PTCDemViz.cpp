#include "PTCDemViz.h"
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>

#include <maya/MGlobal.h>

MTypeId     PTCDemViz::id( 0x0004902 );
MObject     PTCDemViz::asize;
MObject     PTCDemViz::alevel;

PTCDemViz::PTCDemViz(): f_size(32.f), f_level(5)
{
}

PTCDemViz::~PTCDemViz() 
{
}

MStatus PTCDemViz::compute( const MPlug& plug, MDataBlock& data )
{
		MStatus stat;
		f_size =  data.inputValue(asize).asFloat();
	    f_level = data.inputValue( alevel ).asInt();
	
	return MS::kSuccess;
}

void* PTCDemViz::creator()
{
	return new PTCDemViz();
}

bool PTCDemViz::isBounded() const
{ 
    return true;
}

MBoundingBox PTCDemViz::boundingBox() const
{
	MPoint corner1( -f_size,-f_size,-f_size );
	MPoint corner2(f_size,f_size,f_size);
	
	return MBoundingBox( corner1, corner2 ); 
} 

void PTCDemViz::draw( M3dView & view, const MDagPath & path, 
                                 M3dView::DisplayStyle style,M3dView::DisplayStatus status )
{ 	
	view.beginGL();
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glBegin(GL_LINES);
	glVertex3f(-f_size, -f_size, -f_size);
	glVertex3f(f_size, -f_size, -f_size);
	glVertex3f(-f_size, f_size, -f_size);
	glVertex3f(f_size, f_size, -f_size);
	glVertex3f(-f_size, -f_size, f_size);
	glVertex3f(f_size, -f_size, f_size);
	glVertex3f(-f_size, f_size, f_size);
	glVertex3f(f_size, f_size, f_size);
	
	glVertex3f(-f_size, -f_size, -f_size);
	glVertex3f(-f_size, f_size, -f_size);
	glVertex3f(f_size, -f_size, -f_size);
	glVertex3f(f_size, f_size, -f_size);
	glVertex3f(-f_size, -f_size, f_size);
	glVertex3f(-f_size, f_size, f_size);
	glVertex3f(f_size, -f_size, f_size);
	glVertex3f(f_size, f_size, f_size);
	
	glVertex3f(-f_size, -f_size, -f_size);
	glVertex3f(-f_size, -f_size, f_size);
	glVertex3f(-f_size, f_size, -f_size);
	glVertex3f(-f_size, f_size, f_size);
	glVertex3f(f_size, -f_size, -f_size);
	glVertex3f(f_size, -f_size, f_size);
	glVertex3f(f_size, f_size, -f_size);
	glVertex3f(f_size, f_size, f_size);
	
	MPoint cen(0, -f_size, 0);
	float sz = f_size;
	drawCross(f_level, cen, sz);
	glEnd();

	glPopAttrib();
	view.endGL();
}

MStatus PTCDemViz::initialize()	
{
    MStatus stat;
	MFnNumericAttribute nAttr;
	
	alevel = nAttr.create( "maxLevel", "mxl", MFnNumericData::kInt, 5 );
	nAttr.setMin(3);
	nAttr.setMax(8);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( alevel );
	
	asize = nAttr.create( "boxSize", "bsz", MFnNumericData::kFloat, 32);
	nAttr.setMin(32);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute( asize );
	
	return MS::kSuccess;
}

void PTCDemViz::drawCross(int last, MPoint center, float size)
{
	glVertex3f(center.x-size, center.y, center.z);
	glVertex3f(center.x+size, center.y, center.z);
	glVertex3f(center.x, center.y, center.z-size);
	glVertex3f(center.x, center.y, center.z+size);
	last--;
	if(last>0) {
		size /= 2;
		MPoint scen;
		scen.y = center.y;
		scen.x = center.x - size;
		scen.z = center.z - size;
		drawCross(last, scen, size);
		scen.x = center.x + size;
		scen.z = center.z - size;
		drawCross(last, scen, size);
		scen.x = center.x + size;
		scen.z = center.z + size;
		drawCross(last, scen, size);
		scen.x = center.x - size;
		scen.z = center.z + size;
		drawCross(last, scen, size);
	}
}