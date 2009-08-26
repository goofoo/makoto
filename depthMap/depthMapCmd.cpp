/*
 *  depthMapCmd.cpp
 *  vertexCache
 *
 *  Created by zhang on 08-10-9.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "depthMapCmd.h"
#include "../shared/zWorks.h"
#include <maya/MAnimControl.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnTransform.h>
#include "../shared/zData.h"
#include "../shared/zGlobal.h"
#include "../shared/FXMLScene.h"
#include "../shared/zFnEXR.h"
 
depthMap::depthMap() {}

depthMap::~depthMap() {}

MSyntax depthMap::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-n", "-name", MSyntax::kString);
	syntax.addFlag("-sc", "-scenepath", MSyntax::kString);
	syntax.addFlag("-ca", "-camera", MSyntax::kString);
	
	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus depthMap::doIt( const MArgList& args ) 
{

	MStatus status = parseArgs( args );
	
	if( status != MS::kSuccess ) return status;
	
	MArgDatabase argData(syntax(), args);
	
	MAnimControl timeControl;
	MTime time = timeControl.currentTime();
	int frame =int(time.value());
	
	MString scene_name, camera_name, title;
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, title);
	else return MS::kFailure;
	if (argData.isFlagSet("-sc")) argData.getFlagArgument("-sc", 0, scene_name);
	else return MS::kFailure;
	if (argData.isFlagSet("-ca")) argData.getFlagArgument("-ca", 0, camera_name);
	else return MS::kFailure;
	
	m_eye[0][0]=1; m_eye[0][1]=0; m_eye[0][2]=0; m_eye[0][3]=0;
	m_eye[1][0]=0; m_eye[1][1]=1; m_eye[1][2]=0; m_eye[1][3]=0;
	m_eye[2][0]=0; m_eye[2][1]=0; m_eye[2][2]=1; m_eye[2][3]=0;
	m_eye[3][0]=0; m_eye[3][1]=0; m_eye[3][2]=0; m_eye[3][3]=1;
	
// get eye space
	zWorks::getTypedPathByName(MFn::kTransform, camera_name, p_eye);
	MObject o_eye = p_eye.transform();
	if(o_eye.isNull()) MGlobal::displayWarning("Cannot find eye camera, use default space.");
	else zWorks::getTransformWorldNoScale(p_eye.partialPathName(), m_eye);
	
	m_eye[0][0] *=-1; m_eye[0][1] *=-1; m_eye[0][2] *=-1;
	m_eye[2][0] *=-1; m_eye[2][1] *=-1; m_eye[2][2] *=-1;
	
	p_eye.extendToShape();
	MFnCamera feye(p_eye);
	double fov = feye.horizontalFieldOfView();
	int map_w = 1024, map_h = 1024;
	float* data = new float[map_w * map_h];
	
	for(int i=0; i<map_w * map_h; i++) data[i] = 10e6;

	string sscene = scene_name.asChar();
	zGlobal::changeFrameNumber(sscene, frame);
	MGlobal::displayInfo ( MString(" calculating ") + sscene.c_str());
	
	FXMLScene* fscene = new FXMLScene();
	if(fscene->load(sscene.c_str()) != 1) {
		MGlobal::displayWarning(" cannot open scene, do nothing.");
		return MS::kFailure;
	}
	
	fscene->depthMap(data, map_w, map_h, fov, m_eye);
	
	zGlobal::cutByLastSlash(sscene);
	sscene = sscene + "/" + title.asChar() + ".1.exr";
	zGlobal::changeFrameNumber(sscene, frame);
	//zGlobal::changeFilenameExtension(sscene, "exr");
	MGlobal::displayInfo ( MString(" saving ") + sscene.c_str());
	
	M44f amat(m_eye[0][0], m_eye[0][1], m_eye[0][2], m_eye[0][3],
			m_eye[1][0], m_eye[1][1], m_eye[1][2], m_eye[1][3],
			m_eye[2][0], m_eye[2][1], m_eye[2][2], m_eye[2][3],
			m_eye[3][0], m_eye[3][1], m_eye[3][2], m_eye[3][3] );
	ZFnEXR::saveCameraNZ(data, amat, fov, sscene.c_str(), map_w, map_h);
	delete[] data;
	delete fscene;

 return MS::kSuccess;
 }

void* depthMap::creator() {
 return new depthMap;
 }
 
MStatus depthMap::parseArgs( const MArgList& args )
{
	// Parse the arguments.
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}
//:~