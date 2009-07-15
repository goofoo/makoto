#include "zHairNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"

MTypeId HairNode::id( 0x0002518 );

MObject HairNode::afuzz;
MObject HairNode::astartframe;
MObject HairNode::acurrenttime;
MObject	HairNode::aSize;
MObject	HairNode::aHDRName;
MObject HairNode::aworldSpace;
MObject HairNode::aoutput;
MObject HairNode::agrowth;
MObject HairNode::aguide;
MObject HairNode::alengthnoise;
MObject HairNode::asavemap;
MObject HairNode::astep;
MObject HairNode::aalternativepatch;
MObject HairNode::ainterpolate;

HairNode::HairNode()
{
	m_base = new hairMap();
}

HairNode::~HairNode() 
{
	delete m_base;
}

MStatus HairNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput)
	{
		MObject ogrow = data.inputValue(agrowth).asMesh();
		if(!ogrow.isNull()) m_base->setBase(ogrow);
		
		MArrayDataHandle hArray = data.inputArrayValue(aguide);
		int n_guide = hArray.elementCount();
		MObjectArray guidelist;
		for(int i=0; i<n_guide; i++)
		{
			guidelist.append(hArray.inputValue().asMesh());
			hArray.next();
		}
		
		m_base->setGuide(guidelist);
		m_base->updateBase();
		
		//MMatrix mat = data.inputValue(aworldSpace).asMatrix();
		MTime currentTime = data.inputValue(acurrenttime, &status).asTime();
		int frame = (int)currentTime.value();
		
		int startFrame = data.inputValue(astartframe, &status).asInt();
		
		MString cache_path, cache_start, proj_path;
		MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), proj_path);
		
		MString spath = data.inputValue(aHDRName, &status).asString();
		if(spath.length() <1)
		{
			cache_path = proj_path + "/data/" + MFnDependencyNode(thisMObject()).name() + "."+frame+".hair";
			cache_start = proj_path + "/data/" + MFnDependencyNode(thisMObject()).name() + ".hairstart";
		}
		else
		{
			cache_path = spath + "/" + MFnDependencyNode(thisMObject()).name() + "."+frame+".hair";
			cache_start = spath + "/" + MFnDependencyNode(thisMObject()).name() + ".hairstart";
		}
		
		int isave = data.inputValue(asavemap, &status).asInt();
		
		m_base->setPatchOrder(data.inputValue(aalternativepatch).asInt());
		m_base->setInterpolate(data.inputValue(ainterpolate).asInt());
		
		if(startFrame == frame)
		{
			int npt = m_base->dice();
			MGlobal::displayInfo(MString("ZHair diced ") + npt + " samples");
			m_base->initGuide();
			//npt = m_base->saveDguide( );
			//if(npt == 1)
			//	MGlobal::displayInfo("Save Dguide data successed");
			//else
			//	MGlobal::displayInfo("Save Dguide data failed");
			m_base->bind();
			if(isave==1) 
			{
				MGlobal::displayInfo(MString("Saving ")+cache_start);
				m_base->saveStart(cache_start.asChar());
				MGlobal::displayInfo(MString("Saving ")+cache_path);
				m_base->save(cache_path.asChar());
			}
		}
		else 
		{
			m_base->updateGuide();
			if(isave==1) 
			{
				MGlobal::displayInfo(MString("Saving ")+cache_path);
				m_base->save(cache_path.asChar());
			}
		}
		
		m_base->setClumping(data.inputValue(aSize).asFloat());
		m_base->setFuzz(data.inputValue(afuzz).asFloat());
		m_base->setKink(data.inputValue(alengthnoise).asFloat());
		m_base->setDrawAccuracy(data.inputValue(astep).asInt());
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void HairNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	m_base->draw();
	//m_base->drawUV();
	//m_base->drawGuideUV();
	glPopAttrib();
	view.endGL();
}

bool HairNode::isBounded() const
{ 
	return false;
}

MBoundingBox HairNode::boundingBox() const
{ 
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* HairNode::creator()
{
	return new HairNode();
}

MStatus HairNode::initialize()
{ 
	MStatus			 status;
	MFnNumericAttribute numAttr;
    MFnTypedAttribute tAttr;
	
	afuzz = numAttr.create( "fuzz", "fuzz", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(afuzz);
	
	alengthnoise = numAttr.create("kink", "kink",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
	addAttribute(alengthnoise);
	
	aSize = numAttr.create("clumping", "clp",
						  MFnNumericData::kFloat, 0.f, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( numAttr.setStorable(true));
    CHECK_MSTATUS( numAttr.setKeyable(true));
    CHECK_MSTATUS( numAttr.setDefault(0.f));
	numAttr.setCached( true );
	addAttribute(aSize);
	
	astep = numAttr.create( "drawStep", "dsp", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(astep);
	
	aHDRName = tAttr.create("cachePath", "cp",
	MFnData::kString, MObject::kNullObj, &status);
    CHECK_MSTATUS( status );
    CHECK_MSTATUS( tAttr.setStorable(true));
    CHECK_MSTATUS( tAttr.setKeyable(false));
	tAttr.setCached( true );
	
	zWorks::createMatrixAttr(aworldSpace, "worldSpace", "ws");
	
	aoutput = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutput );
	
	zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kMesh);
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zWorks::createTypedArrayAttr(aguide, MString("guideMesh"), MString("gdm"), MFnData::kMesh);
	zCheckStatus(addAttribute(aguide), "ERROR adding guide mesh");
	
	astartframe = numAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( astartframe );
	
	zWorks::createTimeAttr(acurrenttime, MString("currentTime"), MString("ct"), 1.0);
	zCheckStatus(addAttribute(acurrenttime), "ERROR adding time");
	
	ainterpolate = numAttr.create( "interpolate", "ipl", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(ainterpolate);
	
	asavemap = numAttr.create( "saveMap", "sm", MFnNumericData::kInt, 0);
	numAttr.setStorable(false);
	numAttr.setKeyable(true);
	addAttribute(asavemap);
	
	aalternativepatch = numAttr.create( "patchOrder", "pod", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(aalternativepatch);
	
	CHECK_MSTATUS( addAttribute(aHDRName));
	addAttribute(aworldSpace);
	attributeAffects( alengthnoise, aoutput );
	attributeAffects( aSize, aoutput );
	attributeAffects( aworldSpace, aoutput );
	attributeAffects( agrowth, aoutput );
	attributeAffects( aguide, aoutput );
	attributeAffects( astartframe, aoutput );
	attributeAffects( acurrenttime, aoutput );
	attributeAffects( asavemap, aoutput );
	attributeAffects( afuzz, aoutput );
	attributeAffects( astep, aoutput );
	attributeAffects( aalternativepatch, aoutput );
	attributeAffects( ainterpolate, aoutput );
	
	return MS::kSuccess;
}
//:~
