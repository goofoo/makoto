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
MObject HairNode::adraw;
MObject HairNode::aoffset;
MObject HairNode::adensitymap;
MObject HairNode::aenable;

HairNode::HairNode(): idraw(0), ienable(1)
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
		ienable = data.inputValue(aenable, &status).asInt();
		if(ienable == 0) return MS::kUnknownParameter;
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
		
		int startFrame = data.inputValue(astartframe, &status).asInt();
		MTime currentTime = data.inputValue(acurrenttime, &status).asTime();
		int frame = (int)currentTime.value();
		
		if(startFrame == frame) m_base->setOffset(0.f);
		else m_base->setOffset(data.inputValue(aoffset).asFloat());
		m_base->updateBase();
		
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
		
		if(startFrame == frame)
		{
			int npt = m_base->pushFaceVertice();
			MGlobal::displayInfo(MString("ZHair diced ") + npt + " samples");
			m_base->initGuide();
			
			MString dnmpath = data.inputValue(adensitymap, &status).asString();
			if(dnmpath.length() > 0) m_base->setDensityMap(dnmpath.asChar());
			
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
		
		idraw = data.inputValue(adraw).asInt();
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void HairNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	if(ienable == 0) return;
	view.beginGL(); 
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glShadeModel(GL_SMOOTH);
	if(idraw ==1) m_base->draw();
	else if(idraw ==0) m_base->drawGuide();
	else if(idraw == 2){
		m_base->drawUV();
		m_base->drawGuideUV();
	}
	else m_base->drawBind();
	glPopAttrib();
	m_base->drawBBox();
	view.endGL();
}

bool HairNode::isBounded() const
{ 
	return true;
}

MBoundingBox HairNode::boundingBox() const
{ 
	MPoint corner1( -1,-1,-1 );
	MPoint corner2( 1,1,1 );

	if(m_base) {
		corner1.x = m_base->getBBox0X();
		corner1.y = m_base->getBBox0Y();
		corner1.z = m_base->getBBox0Z();
		corner2.x = m_base->getBBox1X();
		corner2.y = m_base->getBBox1Y();
		corner2.z = m_base->getBBox1Z();
	}

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

    numAttr.setStorable(true);
    numAttr.setKeyable(true);
	addAttribute(alengthnoise);
	
	aSize = numAttr.create("clumping", "clp",
						  MFnNumericData::kFloat, 0.f, &status);

    numAttr.setStorable(true);
    numAttr.setKeyable(true);
    numAttr.setDefault(0.f);
	numAttr.setCached( true );
	addAttribute(aSize);
	
	aoffset = numAttr.create("offset", "ofs",
						  MFnNumericData::kFloat, 0.f, &status);

    numAttr.setStorable(true);
    numAttr.setKeyable(true);
    numAttr.setDefault(0.f);
	numAttr.setCached( true );
	addAttribute(aoffset);
	
	aHDRName = tAttr.create("cachePath", "cp",
	MFnData::kString, MObject::kNullObj, &status);

    tAttr.setStorable(true);
    tAttr.setKeyable(false);
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
	
	asavemap = numAttr.create( "saveMap", "sm", MFnNumericData::kInt, 0);
	numAttr.setStorable(false);
	numAttr.setKeyable(true);
	addAttribute(asavemap);
	
	adraw = numAttr.create( "drawType", "drt", MFnNumericData::kInt, 0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(adraw);
	
	zWorks::createStringAttr(adensitymap, "densityMap", "dnm");
	addAttribute(adensitymap);
	
	aenable = numAttr.create( "enable", "en", MFnNumericData::kInt, 1);
	numAttr.setStorable(false);
	numAttr.setKeyable(true);
	addAttribute(aenable);
	
	addAttribute(aHDRName);
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
	attributeAffects( adraw, aoutput );
	attributeAffects( aoffset, aoutput );
	attributeAffects( adensitymap, aoutput );
	attributeAffects( aenable, aoutput );
	
	return MS::kSuccess;
}
//:~
