//-
// ==========================================================================
// Copyright (C) 1995 - 2005 Alias Systems Corp. and/or its licensors.  All 
// rights reserved. 
// 
// The coded instructions, statements, computer programs, and/or related 
// material (collectively the "Data") in these files are provided by Alias 
// Systems Corp. ("Alias") and/or its licensors for the exclusive use of the 
// Customer (as defined in the Alias Software License Agreement that 
// accompanies this Alias software). Such Customer has the right to use, 
// modify, and incorporate the Data into other products and to distribute such 
// products for use by end-users.
//  
// THE DATA IS PROVIDED "AS IS".  ALIAS HEREBY DISCLAIMS ALL WARRANTIES 
// RELATING TO THE DATA, INCLUDING, WITHOUT LIMITATION, ANY AND ALL EXPRESS OR 
// IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE. IN NO EVENT SHALL ALIAS BE LIABLE FOR ANY DAMAGES 
// WHATSOEVER, WHETHER DIRECT, INDIRECT, SPECIAL, OR PUNITIVE, WHETHER IN AN 
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, OR IN EQUITY, 
// ARISING OUT OF ACCESS TO, USE OF, OR RELIANCE UPON THE DATA.
// ==========================================================================
//+

#include "fluidViz.h"
#include "../shared/zAttrWorks.h"
#include "../shared/zMath.h"
#include "../shared/zWorks.h"

MTypeId fluidViz::id( 0x0004132b );
MObject fluidViz::asize;
MObject	fluidViz::resolution;
MObject	fluidViz::resolutionX;
MObject	fluidViz::resolutionY;
MObject	fluidViz::resolutionZ;
MObject	fluidViz::aenable;
MObject	fluidViz::abuoyancy;
MObject	fluidViz::aswirl;
MObject	fluidViz::aconserveVelocity;
MObject fluidViz::atemperature;
MObject	fluidViz::aconserveTemperature;
MObject	fluidViz::aconserveDensity;
MObject	fluidViz::asourceDensity;
MObject	fluidViz::adiffusion;
MObject	fluidViz::awindx;
MObject	fluidViz::awindz;
MObject	fluidViz::asaveCache;
MObject	fluidViz::ashotex;
MObject	fluidViz::atexz;
MObject	fluidViz::aoutDesc;
MObject fluidViz::aoutval;

fluidViz::fluidViz() : m_pDesc(0) 
{
	m_pDesc = new fluidDesc();
	m_pDesc->resolution_x = 64;
	m_pDesc->resolution_y = 64;
	m_pDesc->resolution_z = 64;
	m_pDesc->gridSize = 1.0f;
}
fluidViz::~fluidViz() {if(m_pDesc) delete m_pDesc;}

MStatus fluidViz::compute( const MPlug& plug, MDataBlock& block )
{
	if(plug == aoutDesc) {
	int rx = zGetIntAttr(block, resolutionX);
	int ry = zGetIntAttr(block, resolutionY);
	int rz = zGetIntAttr(block, resolutionZ);
	
	if(isPowerof2(rx) == 1)
	{	
		m_pDesc->resolution_x = rx;
	}
	else MGlobal::displayWarning("WARNING grid resolution ,ust be power of 2");
	
	if(isPowerof2(ry) == 1)
	{
		m_pDesc->resolution_y = ry;
	}
	else MGlobal::displayWarning("WARNING grid resolution ,ust be power of 2");
	
	if(isPowerof2(rz) == 1)
	{
		m_pDesc->resolution_z = rz;
	}
	else MGlobal::displayWarning("WARNING grid resolution ,ust be power of 2");
	
	m_pDesc->gridSize = zGetDoubleAttr(block, asize);
	m_pDesc->enable = zGetIntAttr(block, aenable);
	m_pDesc->save_cache = zGetIntAttr(block, asaveCache);
	m_pDesc->buoyancy = zGetDoubleAttr(block, abuoyancy);
	m_pDesc->swirl = zGetDoubleAttr(block, aswirl);
	m_pDesc->velocity = zGetDoubleAttr(block, aconserveVelocity);
	m_pDesc->temperature = zGetDoubleAttr(block, atemperature);
	m_pDesc->temperatureLoss = zGetDoubleAttr(block, aconserveTemperature);
	m_pDesc->conserveDensity = zGetDoubleAttr(block, aconserveDensity);
	m_pDesc->wind_x = zGetDoubleAttr(block, awindx);
	m_pDesc->wind_z = zGetDoubleAttr(block, awindz);
	m_pDesc->sho_tex = zGetIntAttr(block, ashotex);
	m_pDesc->slice_id = zGetIntAttr(block, atexz);
	m_pDesc->source_density = zGetDoubleAttr(block, asourceDensity);
	m_pDesc->diffusion = zGetDoubleAttr(block, adiffusion);
	
	MStatus status;
	
	
	MFnPluginData fnPluginData;
		MObject newDataObject = fnPluginData.create(zjFluidDescData::id, &status);
		
		zjFluidDescData* pData = (zjFluidDescData*) fnPluginData.data(&status);
		
		if(pData) pData->setDesc(m_pDesc);
	
		MGlobal::displayInfo("update fluid desc");
		
		MDataHandle outDescHandle = block.outputValue(aoutDesc);
		status = outDescHandle.set(pData);
		
		
		block.setClean(aoutDesc);
	
	return MS::kSuccess;
	}
	else return MS::kUnknownParameter;
}

void fluidViz::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 
	
	float mx = m_pDesc->resolution_x*m_pDesc->gridSize;
	float my = m_pDesc->resolution_y*m_pDesc->gridSize;
	float mz = m_pDesc->resolution_z*m_pDesc->gridSize;

	view.beginGL(); 

	// Draw the outline of the foot
	//
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBegin( GL_LINES );
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(mx,0,0);
		
		glColor3f(0,1,0);
		glVertex3f(0,my,0);
		glVertex3f(0,0,0);
			
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,mz);
	glEnd();
	glPopAttrib();
	glBegin( GL_LINES );

			glVertex3f(mx,0,0);
			glVertex3f(mx,my,0);
			glVertex3f(mx,my,0);
			glVertex3f(0,my,0);

			glVertex3f(0,0,mz);
			glVertex3f(mx,0,mz);
			glVertex3f(mx,0,mz);
			glVertex3f(mx,my,mz);
			glVertex3f(mx,my,mz);
			glVertex3f(0,my,mz);
			glVertex3f(0,my,mz);
			glVertex3f(0,0,mz);

			glVertex3f(mx,0,0);
			glVertex3f(mx,0,mz);
			glVertex3f(0,my,0);
			glVertex3f(0,my,mz);
			glVertex3f(mx,my,0);
			glVertex3f(mx,my,mz);
		
	glEnd();

	view.endGL();
}

bool fluidViz::isBounded() const
{ 
	return false;
}

void* fluidViz::creator()
{
	return new fluidViz();
}

MStatus fluidViz::initialize()
{ 
	MFnUnitAttribute unitFn;
	MStatus			 status;
	MFnNumericAttribute		numAttr;
	MFnCompoundAttribute		compFn;

	zCheckStatus(zCreateKeyableDoubleAttr(asize, MString("gridSize"), MString("gridSize"), 1.0), "ERROR creating size"); 
	zCheckStatus(addAttribute(asize), "ERROR adding size");

	
	status = zCreateKeyableIntAttr(aenable, MString("enable"), MString("enb"), 1);
	zCheckStatus(status, "ERROR creating enable");
	status = addAttribute(aenable);
	
	resolution = compFn.create( "resolution", "rez", &status);
	zCheckStatus(status, "ERROR creating resolution");
	
	status = zCreateKeyableIntAttr(resolutionX, MString("resolutionX"), MString("rezx"), 64);
	zCheckStatus(status, "ERROR creating resolution x");
	status = addAttribute(resolutionX);
	
	status = zCreateKeyableIntAttr(resolutionY, MString("resolutionY"), MString("rezy"), 64);
	zCheckStatus(status, "ERROR creating resolution y");
	status = addAttribute(resolutionY);
	
	status = zCreateKeyableIntAttr(resolutionZ, MString("resolutionZ"), MString("rezz"), 64);
	zCheckStatus(status, "ERROR creating resolution z");
	status = addAttribute(resolutionZ);
	
	status = compFn.addChild( resolutionX );
	CHECK_MSTATUS( status );
	status = compFn.addChild( resolutionY );
	CHECK_MSTATUS( status );
	status = compFn.addChild( resolutionZ );
	CHECK_MSTATUS( status );
	status = addAttribute(resolution);
	zCheckStatus(status, "ERROR creating resolution");
	
	status = zCreateKeyableDoubleAttr(abuoyancy, MString("buoyancy"), MString("buo"), 2.0);
	zCheckStatus(status, "ERROR creating buoyancy");
	zCheckStatus(addAttribute(abuoyancy), "ERROR adding buoyancy");
	
	status = zCreateKeyableDoubleAttr(aswirl, MString("swirl"), MString("swl"), 0.5);
	zCheckStatus(status, "ERROR creating swirl");
	zCheckStatus(addAttribute(aswirl), "ERROR adding swirl");
	
	status = zCreateKeyableDoubleAttr(aconserveVelocity, MString("conserveVelocity"), MString("csvy"), 0.99);
	zCheckStatus(status, "ERROR creating conserveVelocity");
	zCheckStatus(addAttribute(aconserveVelocity), "ERROR adding conserveVelocity");
	
	status = zCreateKeyableDoubleAttr(atemperature, MString("temperature"), MString("tpt"), 2.0);
	zCheckStatus(status, "ERROR creating temperature");
	zCheckStatus(addAttribute(atemperature), "ERROR adding temperature");
	
	status = zCreateKeyableDoubleAttr(aconserveTemperature, MString("temperatureLoss"), MString("tls"), 0.0);
	zCheckStatus(status, "ERROR creating conserveTemperature");
	zCheckStatus(addAttribute(aconserveTemperature), "ERROR adding conserveTemperature");
	
	status = zCreateKeyableDoubleAttr(aconserveDensity, MString("conserveDenisty"), MString("csd"), 1.0);
	zCheckStatus(status, "ERROR creating conserveDensity");
	zCheckStatus(addAttribute(aconserveDensity), "ERROR adding conserveDensity");
	
	status = zCreateKeyableDoubleAttr(asourceDensity, MString("sourceDenisty"), MString("scd"), 1.0);
	zCheckStatus(status, "ERROR creating sourceDensity");
	zCheckStatus(addAttribute(asourceDensity), "ERROR adding source density");
	
	adiffusion = numAttr.create( "diffusion", "dfs", MFnNumericData::kDouble, 0.1 );
	numAttr.setMin(0);
	numAttr.setMax(1);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( adiffusion );
	
	status = zCreateKeyableDoubleAttr(awindx, MString("windX"), MString("wdx"), 0.0);
	zCheckStatus(status, "ERROR creating wind x");
	zCheckStatus(addAttribute(awindx), "ERROR adding wind y");
	
	status = zCreateKeyableDoubleAttr(awindz, MString("windZ"), MString("wdz"), 0.0);
	zCheckStatus(status, "ERROR creating wind z");
	zCheckStatus(addAttribute(awindz), "ERROR adding wind z");
	
	ashotex = numAttr.create( "shoTex", "sot", MFnNumericData::kInt, 0 );
	numAttr.setMin(0);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( ashotex );
	
	atexz = numAttr.create( "sliceZ", "slz", MFnNumericData::kInt, 32 );
	numAttr.setMin(1);
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute( atexz );
	
	status = zCreateKeyableIntAttr(asaveCache, MString("saveCache"), MString("saveCache"), 0);
	zCheckStatus(status, "ERROR creating save cache");
	status = addAttribute(asaveCache);
	
	status = zCreatePluginAttr(aoutDesc, MString("outDesc"), MString("outDesc"));
	zCheckStatus(status, "ERROR creating output fluid desc");
	status = addAttribute(aoutDesc);
	
	aoutval = numAttr.create( "outVal", "otv", MFnNumericData::kDouble, 0.0 );
	numAttr.setStorable(false);
	numAttr.setKeyable(false);
	addAttribute( aoutval );
	
	attributeAffects(aenable, aoutDesc);
	attributeAffects(asaveCache, aoutDesc);
	attributeAffects(ashotex, aoutDesc);
	attributeAffects(asize, aoutDesc);
	attributeAffects(resolutionX, aoutDesc);
	attributeAffects(resolutionY, aoutDesc);
	attributeAffects(resolutionZ, aoutDesc);
	attributeAffects(abuoyancy, aoutDesc);
	attributeAffects(aswirl, aoutDesc);
	attributeAffects(aconserveVelocity, aoutDesc);
	attributeAffects(atemperature, aoutDesc);
	attributeAffects(aconserveTemperature, aoutDesc);
	attributeAffects(aconserveDensity, aoutDesc);
	attributeAffects(asourceDensity, aoutDesc);
	attributeAffects(awindx, aoutDesc);
	attributeAffects(awindz, aoutDesc);
	attributeAffects(atexz, aoutDesc);
	attributeAffects(adiffusion, aoutDesc);
	
	attributeAffects(aenable, aoutval);
	attributeAffects(asaveCache, aoutval);
	attributeAffects(ashotex, aoutval);
	attributeAffects(asize, aoutval);
	attributeAffects(resolutionX, aoutval);
	attributeAffects(resolutionY, aoutval);
	attributeAffects(resolutionZ, aoutval);
	attributeAffects(abuoyancy, aoutval);
	attributeAffects(aswirl, aoutval);
	attributeAffects(aconserveVelocity, aoutval);
	attributeAffects(atemperature, aoutval);
	attributeAffects(aconserveTemperature, aoutval);
	attributeAffects(aconserveDensity, aoutval);
	attributeAffects(asourceDensity, aoutval);
	attributeAffects(awindx, aoutval);
	attributeAffects(awindz, aoutval);
	attributeAffects(atexz, aoutval);
	attributeAffects(adiffusion, aoutval);
	
	return MS::kSuccess;
}

MStatus fluidViz::connectionMade ( const  MPlug & plug, const  MPlug & otherPlug, bool asSrc )
{
	if(plug==aoutDesc)
	{
		int rx,ry,rz,enable, save_cache;
		double gsize, buoy, swir, velo, temp, temploss, wind_x, wind_z;
		MObject node = thisMObject();
		zWorks::getIntFromNamedPlug(rx, node, "rezx");
		zWorks::getIntFromNamedPlug(ry, node, "rezy");
		zWorks::getIntFromNamedPlug(rz, node, "rezz");
		zWorks::getDoubleFromNamedPlug(gsize, node, "gridSize");
		zWorks::getIntFromNamedPlug(enable, node, "enb");
		zWorks::getIntFromNamedPlug(save_cache, node, "saveCache");
		zWorks::getDoubleFromNamedPlug(buoy, node, "buo");
		zWorks::getDoubleFromNamedPlug(swir, node, "swl");
		zWorks::getDoubleFromNamedPlug(velo, node, "csvy");
		zWorks::getDoubleFromNamedPlug(temp, node, "tpt");
		zWorks::getDoubleFromNamedPlug(temploss, node, "tls");
		zWorks::getDoubleFromNamedPlug(wind_x, node, "wdx");
		zWorks::getDoubleFromNamedPlug(wind_z, node, "wdz");
	
		if(isPowerof2(rx) == 1)
		{	
			m_pDesc->resolution_x = rx;
		}
		else MGlobal::displayWarning("WARNING grid resolution ,ust be power of 2");
		
		if(isPowerof2(ry) == 1)
		{
			m_pDesc->resolution_y = ry;
		}
		else MGlobal::displayWarning("WARNING grid resolution ,ust be power of 2");
		
		if(isPowerof2(rz) == 1)
		{
			m_pDesc->resolution_z = rz;
		}
		else MGlobal::displayWarning("WARNING grid resolution ,ust be power of 2");
	
		m_pDesc->gridSize = gsize;
		m_pDesc->enable = enable;
		m_pDesc->buoyancy = buoy;
		m_pDesc->swirl = swir;
		m_pDesc->velocity = velo;
		m_pDesc->temperature = temp;
		m_pDesc->temperatureLoss = temploss;
		m_pDesc->save_cache = save_cache;
		m_pDesc->wind_x = wind_x;
		m_pDesc->wind_z = wind_z;
			
		MGlobal::displayInfo("connect fluid desc");
		return MS::kSuccess;
	}
	
	return MS::kUnknownParameter;
}