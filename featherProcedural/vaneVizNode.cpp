#include "vaneVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/zGlobal.h"
#include "FXMLTriangleMap.h"

MTypeId vaneVizNode::id( 0x00025357 );

MObject vaneVizNode::acachename;
MObject vaneVizNode::aframe;
MObject vaneVizNode::aminframe;
MObject vaneVizNode::amaxframe;
MObject vaneVizNode::aratio;
MObject vaneVizNode::agrowth;
MObject vaneVizNode::acollision;
MObject vaneVizNode::atime;
MObject vaneVizNode::aoutval;
MObject vaneVizNode::awind;
MObject vaneVizNode::outMesh;

vaneVizNode::vaneVizNode():m_scale(1.0f),m_wind(1.f)
{
	m_base = new VVane();
}

vaneVizNode::~vaneVizNode()
{
	delete m_base;
}

MStatus vaneVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutval)
	{
		//double dtime = data.inputValue( aframe ).asDouble();
		MTime currentTime = data.inputValue(atime, &status).asTime();
		
		//MObject ogrow = data.inputValue(agrowth).asMeshTransformed();
		MArrayDataHandle hArray = data.inputArrayValue(agrowth);
		
		int n_obs = hArray.elementCount();
		MObjectArray obslist;
		for(int iter=0; iter<n_obs; iter++)
		{
			obslist.append(hArray.inputValue().asNurbsSurfaceTransformed());
			hArray.next();
		}
		//if(!ogrow.isNull()) m_base->setGrow(ogrow);
		m_base->setNurbs(obslist);
		//m_base->setWind(m_wind);
		
		int frame = (int)currentTime.value();
		i_show_v = 0;
		if(data.inputValue(amaxframe, &status).asInt() == 1)
		{
			i_show_v = 1;
			char file_name[512];
			sprintf( file_name, "%s/%s.%d.xvn", m_cachename.asChar(), MFnDependencyNode(thisMObject()).name().asChar(), frame );
			int start = data.inputValue(aminframe, &status).asInt();
			if(start == frame) 
			{
				m_frame_pre = start;
				//m_base->init();
				m_base->save(file_name);
			}
			else if(m_frame_pre+1 == frame)
			{
				m_frame_pre++;
				//m_base->update();
				m_base->save(file_name);
			}
		}
	}
	
	if(plug== outMesh)
	{
		MArrayDataHandle hArray = data.inputArrayValue(agrowth);
		
		int n_obs = hArray.elementCount();
		MObjectArray obslist;
		for(int iter=0; iter<n_obs; iter++)
		{
			obslist.append(hArray.inputValue().asNurbsSurfaceTransformed());
			hArray.next();
		}
		m_base->setNurbs(obslist);
		
		MObject outMeshData = m_base->createShaft();
		
		MDataHandle meshh = data.outputValue(outMesh, &status);
		meshh.set(outMeshData);
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void vaneVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 
	view.beginGL(); 

	//glPushAttrib( GL_POLYGON_BIT );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	if(i_show_v) m_base->drawVelocity();
	else m_base->draw();
	//glPopAttrib();
	view.endGL();
}

bool vaneVizNode::isBounded() const
{ 
	return true;
}

MBoundingBox vaneVizNode::boundingBox() const
{ 
	MPoint corner1( 0,0,0 );
	MPoint corner2( 1,1,1 );
	
	m_base->getBBox(corner1, corner2);

	return MBoundingBox( corner1, corner2 );
}

void* vaneVizNode::creator()
{
	return new vaneVizNode();
}

MStatus vaneVizNode::initialize()
{ 
	MStatus				stat;
	MFnNumericAttribute numAttr;
	
	/*aframe = numAttr.create( "currentTime", "ct", MFnNumericData::kDouble, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aframe );*/
	
	aminframe = numAttr.create( "startFrame", "sf", MFnNumericData::kInt, 1 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aminframe );
	
	amaxframe = numAttr.create( "saveCache", "saveCache", MFnNumericData::kInt, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( amaxframe );
	
	aratio = numAttr.create( "scale", "sc", MFnNumericData::kFloat, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	numAttr.setMin(0.01f);
	numAttr.setInternal(true);
	addAttribute( aratio );
	
	awind = numAttr.create( "wind", "wnd", MFnNumericData::kFloat, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	numAttr.setMin(0.01f);
	numAttr.setInternal(true);
	addAttribute( awind );
	
	aoutval = numAttr.create( "outval", "ov", MFnNumericData::kInt, 1 );
	numAttr.setStorable(false);
	numAttr.setWritable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutval );
	
	MFnTypedAttribute   stringAttr;
	acachename = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	stringAttr.setInternal(true);
	addAttribute( acachename );
	
	zCheckStatus(zWorks::createTypedArrayAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kNurbsSurface ), "ERROR creating grow mesh");
	zCheckStatus(addAttribute(agrowth), "ERROR adding grow mesh");
	
	zCheckStatus(zWorks::createTypedAttr(acollision, MString("collideMesh"), MString("cm"), MFnData::kMesh), "ERROR creating collide mesh");
	zCheckStatus(addAttribute(acollision), "ERROR adding collide mesh");
	
	zCheckStatus(zWorks::createTimeAttr(atime, MString("currentTime"), MString("ct"), 1.0), "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	zCheckStatus(zWorks::createTypedAttr(outMesh, MString("outMesh"), MString("om"), MFnData::kMesh), "ERROR creating out mesh");
	zCheckStatus(addAttribute(outMesh), "ERROR adding out mesh");

	attributeAffects( atime, aoutval );
	attributeAffects( aratio, aoutval );
	attributeAffects( agrowth, aoutval );
	attributeAffects( acollision, aoutval );
	
	attributeAffects( atime, outMesh );
	attributeAffects( aratio, outMesh );
	attributeAffects( agrowth, outMesh );
	attributeAffects( acollision, outMesh );
    	
	return MS::kSuccess;
}

/* virtual */
bool 
vaneVizNode::setInternalValueInContext( const MPlug& plug,
												  const MDataHandle& handle,
												  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == acachename)
	{
		handledAttribute = true;
		m_cachename = (MString) handle.asString();

	}
	else if(plug == aratio)
	{
		handledAttribute = true;
		m_scale = handle.asFloat();
	}
	else if(plug == awind)
	{
		handledAttribute = true;
		m_wind = handle.asFloat();
	}

	return handledAttribute;
}

/* virtual */
bool
vaneVizNode::getInternalValueInContext( const MPlug& plug,
											  MDataHandle& handle,
											  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == acachename)
	{
		handledAttribute = true;
		handle.set( m_cachename );
	}
	else if(plug == aratio)
	{
		handledAttribute = true;
		handle.set( m_scale );
	}
	else if(plug == awind)
	{
		handledAttribute = true;
		handle.set( m_wind );
	}
	
	return handledAttribute;
}
//:~
