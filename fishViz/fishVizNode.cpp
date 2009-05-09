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

#include "fishVizNode.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zWorks.h"
#include "../shared/pdcFile.h"

MTypeId fishVizNode::id( 0x00027380 );
MObject fishVizNode::aInMesh;
MObject fishVizNode::aOscillate;
MObject fishVizNode::aFlapping;
MObject fishVizNode::aBending;
MObject fishVizNode::aNBone;
MObject fishVizNode::aLength;
MObject fishVizNode::acachename;
MObject fishVizNode::atime;
MObject fishVizNode::aFrequency;

MObject fishVizNode::outMesh;

fishVizNode::fishVizNode():m_num_fish(0), m_num_bone(20)
{
	m_pBone = new CfishBone();
}

fishVizNode::~fishVizNode() 
{
	if(m_pBone) delete m_pBone;
}

MStatus fishVizNode::compute( const MPlug& plug, MDataBlock& data )
{    
    MStatus status;
	
	MObject arbitaryMesh = data.inputValue(aInMesh).asMesh();

	MDoubleArray ptc_time_offset;
	MDoubleArray ptc_amplitude;
	MDoubleArray ptc_bend;
	MDoubleArray ptc_scale;
	MDoubleArray masses;
	
	MString cacheName = data.inputValue(acachename, &status).asString();
	MTime currentTime = data.inputValue(atime, &status).asTime();
	cacheName = cacheName+"."+250*int(currentTime.value())+".pdc";
	
	pdcFile* fpdc = new pdcFile();
	if(fpdc->load(cacheName.asChar())==1)
	{
		//MGlobal::displayInfo(MString("FishViz loaded cache file: ")+cacheName);
		fpdc->readPositions(ptc_positions, ptc_velocities, ptc_ups, ptc_views, ptc_time_offset, ptc_amplitude, ptc_bend, ptc_scale, masses);
	}
	else MGlobal::displayWarning(MString("FishViz cannot open cache file: ")+cacheName);
	
	if(currentTime.value()!=int(currentTime.value())) 
	{
		float delta_t = currentTime.value()-int(currentTime.value());
		
		
		for(int i=0; i<fpdc->getParticleCount(); i++)
		{
			ptc_positions[i] += ptc_velocities[i]*delta_t/24.0f;
		}
	}
	
	delete fpdc;
	
	double flapping = zGetDoubleAttr(data, aFlapping);
	double bending= zGetDoubleAttr(data, aBending);
	double oscillate= zGetDoubleAttr(data, aOscillate);
	double length = zGetDoubleAttr(data, aLength);
	m_fish_length = length;
	double frequency = zGetDoubleAttr(data, aFrequency);
	unsigned num_bones = zGetIntAttr(data, aNBone);
	
	unsigned int nptc = ptc_positions.length();
	MPointArray vertices;
	MATRIX44F mat44, mat_bone;
	XYZ vert, front, up, side;
	MDataHandle outputHandle = data.outputValue(outMesh, &status);
	zCheckStatus(status, "ERROR getting polygon data handle\n");
	if(m_num_fish != nptc || m_num_bone != num_bones)
	{
		m_num_bone = num_bones;
		m_num_fish = nptc;
		unsigned int vertex_count;
		unsigned int face_count;
		MIntArray pcounts;
		MIntArray pconnect;
		
		
		
		unsigned  inmeshnv, inmeshnp;
		MPointArray pinmesh;
		MIntArray count_inmesh;
		MIntArray connect_inmesh;
		zWorks::extractMeshParams(arbitaryMesh, inmeshnv, inmeshnp, pinmesh, count_inmesh, connect_inmesh);
		vertex_count = inmeshnv * nptc;
		face_count = inmeshnp * nptc;
		
		for(unsigned int i=0; i<nptc; i++)
		{
			
			// calculate the bone transformations
			poseBones(length, num_bones, ptc_time_offset[i], frequency, ptc_amplitude[i], ptc_bend[i], flapping, bending, oscillate);
			
			front.x = ptc_views[i].x;
			front.y = ptc_views[i].y;
			front.z = ptc_views[i].z;
			up.x = ptc_ups[i].x;
			up.y = ptc_ups[i].y;
			up.z = ptc_ups[i].z;
			
			side = front.cross(up);
			side.normalize();
			
			up = side.cross(front);
			up.normalize();
			
			mat44.setIdentity();
			mat44.setOrientations(side, up, front);
			mat44.scale(ptc_scale[i]);
			mat44.setTranslation(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
			
			for(unsigned int j=0; j<inmeshnv; j++)
			{
				vert.x = pinmesh[j].x;
				vert.y = pinmesh[j].y;
				vert.z = pinmesh[j].z;
				
				int bone_id;
				if(vert.z>0) bone_id = 0;
				else if(-vert.z>length)  bone_id = num_bones-1;
				else bone_id = int(-vert.z/length*(num_bones-1));
				
				mat_bone = m_pBone->getBoneById(bone_id);
				vert.z -= -length/(num_bones-1)*bone_id;
				
				mat_bone.transform(vert);
				mat44.transform(vert);
				
				vertices.append(MPoint(vert.x, vert.y, vert.z));
			}
			for(unsigned int j=0; j<inmeshnp; j++)
			{
				pcounts.append(count_inmesh[j]);
			}
		}
		
		int acc=0;
		for(unsigned int i=0; i<nptc; i++)
		{
			for(unsigned int j=0; j<connect_inmesh.length(); j++)
			{
				pconnect.append(connect_inmesh[j]+acc);
			}
			acc += inmeshnv;
		}
		
		
		MObject m_mesh = outputHandle.asMesh();
		
		
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&status);
		zCheckStatusNR(status, "ERROR creating outputData");
		
		MFnMesh meshFn;
		m_mesh= meshFn.create(
		  vertex_count,				// number of vertices
		  face_count,		// number of polygons
		  vertices,			// The points
		  pcounts,			// # of vertex for each poly
		  pconnect,			// Vertices index for each poly
		  newOutputData,      // Dependency graph data object
		  &status
		  );
		
		zCheckStatusNR(status, "ERROE creating mesh");
		
		// Update surface 
		//
		outputHandle.set(newOutputData);
		
		
	}
	else
	{
		
		MObject m_mesh = outputHandle.asMesh();
		MFnMesh meshFn(m_mesh);

		unsigned  inmeshnv, inmeshnp;
		MPointArray pinmesh;
		MIntArray count_inmesh;
		MIntArray connect_inmesh;
		zWorks::extractMeshParams(arbitaryMesh, inmeshnv, inmeshnp, pinmesh, count_inmesh, connect_inmesh);
		vertices.setLength(nptc*inmeshnv);
		int acc=0;
		for(unsigned int i=0; i<nptc; i++)
		{
			// calculate the bone transformations
			poseBones(length, num_bones, ptc_time_offset[i], frequency, ptc_amplitude[i], ptc_bend[i], flapping, bending, oscillate);
			
			front.x = ptc_views[i].x;
			front.y = ptc_views[i].y;
			front.z = ptc_views[i].z;
			up.x = ptc_ups[i].x;
			up.y = ptc_ups[i].y;
			up.z = ptc_ups[i].z;
			
			side = front.cross(up);
			side.normalize();
			
			up = side.cross(front);
			up.normalize();
			
			mat44.setIdentity();
			mat44.setOrientations(side, up, front);
			mat44.scale(ptc_scale[i]);
			mat44.setTranslation(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
			
			for(unsigned int j=0; j<inmeshnv; j++)
			{
				vert.x = pinmesh[j].x;
				vert.y = pinmesh[j].y;
				vert.z = pinmesh[j].z;
				
				int bone_id;
				if(vert.z>0) bone_id = 0;
				else if(-vert.z>length)  bone_id = num_bones-1;
				else bone_id = int(-vert.z/length*(num_bones-1));
				
				mat_bone = m_pBone->getBoneById(bone_id);
				vert.z -= -length/(num_bones-1)*bone_id;
				
				mat_bone.transform(vert);
				mat44.transform(vert);
				
				vertices[j+acc] = MPoint(vert.x, vert.y, vert.z);
			}
			acc += inmeshnv;
		}
		
		meshFn.setPoints(vertices);
		outputHandle.set(meshFn.object());
		
		
		
	}
	//delete fmat;
	data.setClean( plug );
	
	
	return MS::kSuccess;
}

void* fishVizNode::creator()
{
	return new fishVizNode();
}

/////////////////////////////////////
// Attribute Setup and Maintenance //
/////////////////////////////////////





MStatus fishVizNode::initialize()
{
	MStatus stat;
	
	stat = zCreateTypedAttr(outMesh, MString("outMesh"), MString("om"), MFnData::kMesh);
	zCheckStatus(stat, "failed to add out mesh attr");
	stat = addAttribute( outMesh );
	
	stat = zCreateTypedAttr(aInMesh, MString("inMesh"), MString("inm"), MFnData::kMesh);
	zCheckStatus(stat, "failed to add in mesh attr");
	stat = addAttribute( aInMesh );
	
	zCheckStatus(zWorks::createIntAttr(aNBone, "boneCount", "bnc", 20, 4), "failed to create n bone attr");
	zCheckStatus(addAttribute(aNBone), "failed to add n bone attr");
	
	stat = zCreateKeyableDoubleAttr(aFrequency, MString("frequency"), MString("feq"), 1.0);
	zCheckStatus(stat, "failed to add frequency attr");
	stat = addAttribute( aFrequency);
	
	stat = zCreateKeyableDoubleAttr(aFlapping, MString("flapping"), MString("fpn"), 1.0);
	zCheckStatus(stat, "failed to add flapping attr");
	stat = addAttribute(aFlapping);
	
	stat = zCreateKeyableDoubleAttr(aBending, MString("bending"), MString("bdn"), 1.0);
	zCheckStatus(stat, "failed to add bending attr");
	stat = addAttribute(aBending);
	
	stat = zCreateKeyableDoubleAttr(aOscillate, MString("oscillate"), MString("osc"), 1.0);
	zCheckStatus(stat, "failed to add oscillate attr");
	stat = addAttribute(aOscillate);
	
	stat = zCreateKeyableDoubleAttr(aLength, MString("length"), MString("l"), 10.0);
	zCheckStatus(stat, "failed to add length attr");
	stat = addAttribute(aLength);
	
	stat = zCreateTypedAttr(acachename, "cacheName", "cchn", MFnData::kString);
	zCheckStatus(stat, "ERROR creating cache name");
	zCheckStatus(addAttribute(acachename), "ERROR adding cache name");

	stat = zCreateTimeAttr(atime, "currentTime", MString("ct"), 1.0);
	zCheckStatus(stat, "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	stat = attributeAffects( aNBone, outMesh );
	stat = attributeAffects( aFrequency, outMesh );
	stat = attributeAffects( aFlapping, outMesh );
	stat = attributeAffects( aBending, outMesh );
	stat = attributeAffects( aOscillate, outMesh );
	stat = attributeAffects( aLength, outMesh );
	stat = attributeAffects( atime, outMesh );
	stat = attributeAffects( aInMesh, outMesh );
	
	return MS::kSuccess;
}


MStatus fishVizNode::connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc )
{
	if(plug==aInMesh)
	{
		m_num_fish = 0;
		MGlobal::displayInfo("update fish mesh");
		return MS::kSuccess;
	}
	
	return MS::kUnknownParameter;
}

void fishVizNode::poseBones(float l, int n, float time, float freq, float ampl, float bend, float kf, float kb, float ko)
{
	m_pBone->initialize(l, n);
	m_pBone->setTime(time/4.0);
	m_pBone->setFrequency(freq);
	m_pBone->setAnglePrim(ampl*600*kf);
	m_pBone->setAngleOffset(bend*1200*kb);
	m_pBone->setOscillation(ampl*90*ko);
	//zDisplayFloat(ptc_bend[i]);
	// calculate the bone transformations
	m_pBone->compose();
}

bool fishVizNode::isBounded() const
{ 
	return false;
}

void fishVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	unsigned num_ptc = ptc_positions.length();

	glBegin(GL_LINES);
	glColor3f(0,0,1);
	for(unsigned i=0; i<num_ptc; i++)
	{
		glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
		glVertex3f(ptc_positions[i].x+ptc_velocities[i].x, ptc_positions[i].y+ptc_velocities[i].y, ptc_positions[i].z+ptc_velocities[i].z);
	}
	//glColor3f(0,1,0);
	//for(unsigned i=0; i<num_ptc; i++)
	//{
	//	glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
	//	glVertex3f(ptc_positions[i].x+ptc_ups[i].x*m_fish_length, ptc_positions[i].y+ptc_ups[i].y*m_fish_length, ptc_positions[i].z+ptc_ups[i].z*m_fish_length);
	//}
	glColor3f(1,0,0);
	for(unsigned i=0; i<num_ptc; i++)
	{
		glVertex3f(ptc_positions[i].x, ptc_positions[i].y, ptc_positions[i].z);
		glVertex3f(ptc_positions[i].x-ptc_views[i].x*m_fish_length, ptc_positions[i].y-ptc_views[i].y*m_fish_length, ptc_positions[i].z-ptc_views[i].z*m_fish_length);
	}
	glEnd();
	glPopAttrib();
	view.endGL();
}
//:~