#include "wingVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/zGlobal.h"
#include "FXMLTriangleMap.h"

MTypeId wingVizNode::id( 0x00025358 );

MObject wingVizNode::acachename;
MObject wingVizNode::aframe;
MObject wingVizNode::aminframe;
MObject wingVizNode::amaxframe;
MObject wingVizNode::aratio;
MObject wingVizNode::agrowth;
MObject wingVizNode::acollision;
MObject wingVizNode::atime;
MObject wingVizNode::aoutval;
MObject wingVizNode::awind;
MObject wingVizNode::outMesh;
MObject wingVizNode::ashaft0;
MObject wingVizNode::ashaft1;
MObject wingVizNode::awidth0;
MObject wingVizNode::awidth1;
MObject wingVizNode::aedge0;
MObject wingVizNode::aedge1;
MObject wingVizNode::atwist0;
MObject wingVizNode::atwist1;
MObject wingVizNode::areverse;
MObject wingVizNode::astep;
MObject wingVizNode::anoisize;
MObject wingVizNode::anoifreq;
MObject wingVizNode::anoiseed;
MObject wingVizNode::aouttexcoordoffsetpp;

wingVizNode::wingVizNode():m_scale(1.0f),m_wind(1.f)
{
	m_base = new VWing();
}

wingVizNode::~wingVizNode()
{
	delete m_base;
}

MStatus wingVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutval)
	{
		//double dtime = data.inputValue( aframe ).asDouble();
		MTime currentTime = data.inputValue(atime, &status).asTime();
		float ns = data.inputValue(anoisize).asFloat();
		float nf = data.inputValue(anoifreq).asFloat();
		int nd = data.inputValue(anoiseed).asInt();
		
		MObject ogrow = data.inputValue(agrowth).asNurbsSurfaceTransformed();
		//MArrayDataHandle hArray = data.inputArrayValue(agrowth);
		
		//int n_obs = hArray.elementCount();
		//MObjectArray obslist;
		//for(int iter=0; iter<n_obs; iter++)
		//{
		//	obslist.append(hArray.inputValue().asNurbsSurfaceTransformed());
		//	hArray.next();
		//}
		//if(!ogrow.isNull()) m_base->setGrow(ogrow);
		m_base->setNoise(ns, nf, nd);
		m_base->setTime(currentTime.value());
		m_base->setNurbs(ogrow);
		//m_base->setWind(m_wind);
		
		/*int frame = (int)currentTime.value();
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
		}*/
	}
	
	if(plug== outMesh)
	{
		MObject ogrow = data.inputValue(agrowth).asNurbsSurfaceTransformed();
		m_base->setNurbs(ogrow);
		
		float w0 = data.inputValue(ashaft0).asFloat();
		float w1 = data.inputValue(ashaft1).asFloat();
		
		float fw0 = data.inputValue(awidth0).asFloat();
		float fw1 = data.inputValue(awidth1).asFloat();
		
		float e0 = data.inputValue(aedge0).asFloat();
		float e1 = data.inputValue(aedge1).asFloat();
		
		float t0 = data.inputValue(atwist0).asFloat();
		float t1 = data.inputValue(atwist1).asFloat();
		
		int r = data.inputValue(areverse).asInt();
		int s = data.inputValue(astep).asInt();
		m_base->setReverse(r);
		m_base->setFeatherWidth(fw0, fw1);
		m_base->setFeatherEdge(e0, e1);
		m_base->setShaftWidth(w0, w1);
		m_base->setFeatherTwist(t0, t1);
		m_base->setStep(s);

		MObject outMeshData = m_base->createFeather();
		
		//zDisplayFloat3(m_base->getSeed(), m_base->getSeed1(), m_base->getFract());
		
		MDataHandle meshh = data.outputValue(outMesh, &status);
		meshh.set(outMeshData);
	    
		data.setClean(plug);
	}
	
	if(plug== aouttexcoordoffsetpp)
	{
		MDoubleArray coeff_array;
		
		m_base->getTexcoordOffset(coeff_array);
		
		MFnDoubleArrayData coeffFn;
		MObject coeffOutputData = coeffFn.create( coeff_array, &status );
		MDataHandle coeffh = data.outputValue( plug, &status);
		coeffh.set(coeffOutputData);
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void wingVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 
	view.beginGL(); 

	//glPushAttrib( GL_POLYGON_BIT );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	//if(i_show_v) m_base->drawVelocity();
	m_base->draw();
	//glPopAttrib();
	view.endGL();
}

bool wingVizNode::isBounded() const
{ 
	return true;
}

MBoundingBox wingVizNode::boundingBox() const
{ 
	MPoint corner1( 0,0,0 );
	MPoint corner2( 1,1,1 );
	
	m_base->getBBox(corner1, corner2);

	return MBoundingBox( corner1, corner2 );
}

void* wingVizNode::creator()
{
	return new wingVizNode();
}

MStatus wingVizNode::initialize()
{ 
	MStatus				stat;
	MFnNumericAttribute numAttr;
	MFnTypedAttribute   meshAttr;
	
	awidth0 = numAttr.create( "width0", "w0", MFnNumericData::kFloat, 0.5 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( awidth0 );
	
	awidth1 = numAttr.create( "width1", "w1", MFnNumericData::kFloat, 0.2 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( awidth1 );
	
	aedge0 = numAttr.create( "edge0", "e0", MFnNumericData::kFloat, 0.9 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aedge0 );
	
	aedge1 = numAttr.create( "edge1", "e1", MFnNumericData::kFloat, 0.2 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( aedge1 );
	
	atwist0 = numAttr.create( "twist0", "tw0", MFnNumericData::kFloat, 0.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( atwist0 );
	
	atwist1 = numAttr.create( "twist1", "tw1", MFnNumericData::kFloat, 60.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( atwist1 );
	
	areverse = numAttr.create( "reverse", "re", MFnNumericData::kInt, 0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( areverse );
	
	astep = numAttr.create( "step", "st", MFnNumericData::kInt, 4 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	numAttr.setMin(1);
	addAttribute( astep );
	
	ashaft0 = numAttr.create( "shaftWidth0", "shw0", MFnNumericData::kFloat, 0.25 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( ashaft0 );
	
	ashaft1 = numAttr.create( "shaftWidth1", "shw1", MFnNumericData::kFloat, 0.20 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( ashaft1 );
	
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
	
	zCheckStatus(zWorks::createTypedAttr(agrowth, MString("growMesh"), MString("gm"), MFnData::kNurbsSurface ), "ERROR creating grow mesh");
	zCheckStatus(addAttribute(agrowth), "ERROR adding ing base");
	
	zCheckStatus(zWorks::createTypedAttr(acollision, MString("collideMesh"), MString("cm"), MFnData::kMesh), "ERROR creating collide mesh");
	zCheckStatus(addAttribute(acollision), "ERROR adding collide mesh");
	
	zCheckStatus(zWorks::createTimeAttr(atime, MString("currentTime"), MString("ct"), 1.0), "ERROR creating time");
	zCheckStatus(addAttribute(atime), "ERROR adding time");
	
	zCheckStatus(zWorks::createTypedAttr(outMesh, MString("outMesh"), MString("om"), MFnData::kMesh), "ERROR creating out mesh");
	zCheckStatus(addAttribute(outMesh), "ERROR adding out mesh");
	
	aouttexcoordoffsetpp = meshAttr.create( "outTexcoordOffset", "oto", MFnData::kDoubleArray ); 
	meshAttr.setStorable(false);
	meshAttr.setWritable(false);
	addAttribute( aouttexcoordoffsetpp );
	
	anoisize = numAttr.create( "noiseSize", "nsz", MFnNumericData::kFloat, 1.0 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( anoisize );
	
	anoifreq = numAttr.create( "noiseFrequency", "nfq", MFnNumericData::kFloat, 8.0 );
	numAttr.setStorable(true);
	numAttr.setMin(0.01);
	numAttr.setKeyable(true);
	addAttribute( anoifreq );
	
	anoiseed = numAttr.create( "noiseSeed", "nsd", MFnNumericData::kInt, 19 );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	numAttr.setMin(10);
	addAttribute( anoiseed );
	
	attributeAffects( areverse, aoutval );
	attributeAffects( awidth0, aoutval );
	attributeAffects( awidth1, aoutval );
	attributeAffects( aedge0, aoutval );
	attributeAffects( aedge1, aoutval );
	attributeAffects( atwist0, aoutval );
	attributeAffects( atwist1, aoutval );
	attributeAffects( ashaft0, aoutval );
	attributeAffects( ashaft1, aoutval );
	attributeAffects( atime, aoutval );
	attributeAffects( aratio, aoutval );
	attributeAffects( agrowth, aoutval );
	attributeAffects( acollision, aoutval );
	attributeAffects( astep, aoutval );
	
	attributeAffects( areverse, outMesh );
	attributeAffects( awidth0, outMesh );
	attributeAffects( awidth1, outMesh );
	attributeAffects( aedge0, outMesh );
	attributeAffects( aedge1, outMesh );
	attributeAffects( atwist0, outMesh );
	attributeAffects( atwist1, outMesh );
	attributeAffects( ashaft0, outMesh );
	attributeAffects( ashaft1, outMesh );
	attributeAffects( atime, outMesh );
	attributeAffects( aratio, outMesh );
	attributeAffects( agrowth, outMesh );
	attributeAffects( acollision, outMesh );
	attributeAffects( astep, outMesh );
	attributeAffects( anoisize, outMesh );
	attributeAffects( anoifreq, outMesh );
	attributeAffects( anoiseed, outMesh );
	
	attributeAffects( areverse, aouttexcoordoffsetpp );
	attributeAffects( agrowth, aouttexcoordoffsetpp );
	attributeAffects( astep, aouttexcoordoffsetpp );
    	
	return MS::kSuccess;
}

/* virtual */
bool 
wingVizNode::setInternalValueInContext( const MPlug& plug,
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
wingVizNode::getInternalValueInContext( const MPlug& plug,
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
