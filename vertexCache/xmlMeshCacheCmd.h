/*
 *  xmlMeshCacheCmd.h
 *  vertexCache
 *
 *  Created by zhang on 08-10-9.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MPxDeformerNode.h> 
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MVectorArray.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>

class xmlMeshCache : public MPxCommand
 {
 public:
 				xmlMeshCache();
	virtual		~xmlMeshCache();
 	MStatus doIt( const MArgList& args );
 	static void* creator();
	static MSyntax newSyntax();

	void orderMesh();
	void save(const char* filename, int frameNumber);
  private:

	//FILE *fp;
	MStatus parseArgs ( const MArgList& args );
	
	MDagPathArray m_mesh_list;
	MDagPathArray m_nurbs_list;
	
	float m_space[4][4];
	float m_eye[4][4];
	MDagPath p_eye;
 };

