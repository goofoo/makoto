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

class xmlMeshCache : public MPxCommand
 {
 public:
 				xmlMeshCache();
	virtual		~xmlMeshCache();
 	MStatus doIt( const MArgList& args );
 	static void* creator();
	static MSyntax newSyntax();
	
 /*add triangles*/
	void appendTriangles(MDagPath meshDag);
 /*traingle output*/
	void saveTriangles(const char* filename, int frame);

  private:

	//FILE *fp;
	MStatus parseArgs ( const MArgList& args );
	bool worldSpace, i_export_scene;
	int frame_reference;
  
	int m_n_triangle, m_n_vertex;
	double m_area;
	MFloatArray m_face_area;
	MIntArray m_vertex_id;
	MPointArray m_vertex_p;
	MVectorArray m_vertex_n, m_vertex_t;
 };

