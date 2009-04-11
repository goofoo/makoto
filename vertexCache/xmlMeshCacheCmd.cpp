/*
 *  xmlMeshCacheCmd.cpp
 *  vertexCache
 *
 *  Created by zhang on 08-10-9.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "xmlMeshCacheCmd.h"
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
////////////////////////////////////////
//
// cache file generation Command Class
//
////////////////////////////////////////
 
  
xmlMeshCache::xmlMeshCache() {}

xmlMeshCache::~xmlMeshCache() {}

MSyntax xmlMeshCache::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-t", "-title", MSyntax::kString);
	syntax.addFlag("-bc", "-backscatcamera", MSyntax::kString);
	syntax.addFlag("-ec", "-eyecamera", MSyntax::kString);
	
	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus xmlMeshCache::doIt( const MArgList& args ) 
{

	MStatus status = parseArgs( args );
	
	if( status != MS::kSuccess ) return status;
	
	MArgDatabase argData(syntax(), args);
	
	MAnimControl timeControl;
	MTime time = timeControl.currentTime();
	int frame =int(time.value());

	MString proj;
	MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), proj );
	
	MSelectionList selList;
	MGlobal::getActiveSelectionList ( selList );

	if ( selList.length() == 0 )
	{
		MGlobal:: displayError ( "Nothing is selected!" );
		return MS::kSuccess;
	}
	
	MItSelectionList iter( selList );

	MString cache_path = proj + "/data/";
	MString cache_name;
	MString scene_name = "untitled";
	MString sscat("spaceBS");
	MString seye("spaceBS");
	
	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-t")) argData.getFlagArgument("-t", 0, scene_name);
	
	m_space[0][0]=1; m_space[0][1]=0; m_space[0][2]=0;
	m_space[1][0]=0; m_space[1][1]=1; m_space[1][2]=0;
	m_space[2][0]=0; m_space[2][1]=0; m_space[2][2]=1;
	m_space[3][0]=0; m_space[3][1]=0; m_space[3][2]=0;
	
	m_eye[0][0]=1; m_eye[0][1]=0; m_eye[0][2]=0;
	m_eye[1][0]=0; m_eye[1][1]=1; m_eye[1][2]=0;
	m_eye[2][0]=0; m_eye[2][1]=0; m_eye[2][2]=1;
	m_eye[3][0]=0; m_eye[3][1]=0; m_eye[3][2]=0;
	
	if (argData.isFlagSet("-bc")) 
	{
		argData.getFlagArgument("-bc", 0, sscat);
	
// get back light space
		MDagPath p_backscat;
		
		zWorks::getTypedPathByName(MFn::kTransform, sscat, p_backscat);
		MObject o_backscat = p_backscat.transform();
		if(o_backscat.isNull()) MGlobal::displayWarning("Cannot find backscat camera, use default space.");
		else zWorks::getTransformWorldNoScale(p_backscat.partialPathName(), m_space);
	}
	
	if (argData.isFlagSet("-ec")) 
	{
		argData.getFlagArgument("-ec", 0, seye);
	
// get back light space
		MDagPath p_eye;
		
		zWorks::getTypedPathByName(MFn::kTransform, seye, p_eye);
		MObject o_eye = p_eye.transform();
		if(o_eye.isNull()) MGlobal::displayWarning("Cannot find eye camera, use default space.");
		else zWorks::getTransformWorldNoScale(p_eye.partialPathName(), m_eye);
	}
	
	m_mesh_list.clear();
	m_nurbs_list.clear();

	for ( ; !iter.isDone(); iter.next() )
	{								
		MDagPath meshPath;		
		iter.getDagPath( meshPath );
		
		if( meshPath.hasFn(MFn::kMesh))  m_mesh_list.append(meshPath);
		if( meshPath.hasFn(MFn::kNurbsSurface)) m_nurbs_list.append(meshPath);
		
	}	

	char file_name[512];
	    
	sprintf( file_name, "%s/%s.%d.scene", cache_path.asChar(), scene_name.asChar(), frame );

	orderMesh();
	save(file_name, frame);
	MGlobal::displayInfo ( MString("Vertex Cache writes ") + file_name);

 return MS::kSuccess;
 }

void* xmlMeshCache::creator() {
 return new xmlMeshCache;
 }
 
MStatus xmlMeshCache::parseArgs( const MArgList& args )
{
	// Parse the arguments.
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}

void xmlMeshCache::orderMesh()
{
	MIntArray nvertarray;
	unsigned n_mesh = m_mesh_list.length();
	nvertarray.setLength(n_mesh);
	MStatus status;
	for(unsigned i=0; i<n_mesh; i++) 
	{
		MFnMesh meshFn(m_mesh_list[i], &status );
		nvertarray[i] = meshFn.numVertices();
	}
	
	int ntmp;
	MDagPath ptmp;
	for(unsigned j=0; j<n_mesh; j++) 
	{
		for(unsigned i=0; i<n_mesh-1-j; i++) 
		{
			if(nvertarray[i] < nvertarray[i+1])
			{
				ntmp = nvertarray[i];
				nvertarray[i] = nvertarray[i+1];
				nvertarray[i+1] = ntmp;
				
				ptmp = m_mesh_list[i];
				m_mesh_list[i] = m_mesh_list[i+1];
				m_mesh_list[i+1] = ptmp;
			}
		}
	}
}

void xmlMeshCache::save(const char* filename, int frameNumber)
{
	MStatus status;
	FXMLScene xml_f;
	xml_f.begin(filename, frameNumber);
	for(int i=0; i<m_mesh_list.length(); i++) 
	{
		m_mesh_list[i].extendToShape();
		
		MString surface = m_mesh_list[i].fullPathName();
	
		zWorks::validateFilePath(surface);

		xml_f.meshBegin(surface.asChar());
	
	MFnMesh meshFn(m_mesh_list[i], &status );
	MItMeshPolygon faceIter(m_mesh_list[i], MObject::kNullObj, &status );
	MItMeshVertex vertIter(m_mesh_list[i], MObject::kNullObj, &status);
	MItMeshEdge edgeIter(m_mesh_list[i], MObject::kNullObj, &status);
	
	//zWorks::displayIntParam("N Face", meshFn.numPolygons());
	//zWorks::displayIntParam("N Vertex", meshFn.numVertices());
	//zWorks::displayIntParam("N Facevertex", meshFn.numFaceVertices());

	int n_tri = 0;
	float f_area = 0;
	double area;
	
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray vexlist;
		faceIter.getVertices ( vexlist );
		n_tri += vexlist.length() - 2;
		
		faceIter.getArea( area,  MSpace::kWorld );
		f_area += (float)area;
	}
	
	xml_f.triangleInfo(n_tri, f_area);
	
	MFnDependencyNode fnode(m_mesh_list[i].node());
	MString smsg("prtMsg");
	MPlug pmsg = fnode.findPlug( smsg, 1,  &status );
	
	double light_intensity = 1.0;
	
	if(status)
	{
		MObject oattrib;
		zWorks::getConnectedNode(oattrib, pmsg);
		fnode.setObject(oattrib);

		bool iattr = 0;
		zWorks::getBoolAttributeByName(fnode, "skipIndirect", iattr);
		if(iattr) xml_f.addAttribute("skipIndirect", 1);
		
		iattr = 0;
		zWorks::getBoolAttributeByName(fnode, "skipScatter", iattr);
		if(iattr) xml_f.addAttribute("skipScatter", 1);
		
		iattr = 0;
		zWorks::getBoolAttributeByName(fnode, "skipBackscatter", iattr);
		if(iattr) xml_f.addAttribute("skipBackscatter", 1);
		
		iattr = 0;
		zWorks::getBoolAttributeByName(fnode, "asLightsource", iattr);
		if(iattr) xml_f.addAttribute("asLightsource", 1);
		
		iattr = 0;
		zWorks::getBoolAttributeByName(fnode, "asGhost", iattr);
		if(iattr) xml_f.addAttribute("invisible", 1);
		
		iattr = 0;
		zWorks::getBoolAttributeByName(fnode, "castNoShadow", iattr);
		if(iattr) xml_f.addAttribute("noShadow", 1);
		
		double td;
		if(zWorks::getDoubleAttributeByName(fnode, "lightIntensity", td)) light_intensity = td;
		
		fnode.setObject(m_mesh_list[i].node());
	}

	xml_f.staticBegin();
	
	int n_poly = meshFn.numPolygons();
	int n_vert = meshFn.numVertices();
	int* polycount = new int[n_poly];
	
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) polycount[ faceIter.index() ] = faceIter.polygonVertexCount();
	
	xml_f.addFaceCount(n_poly, polycount);
	delete[] polycount;
	
	int n_facevertex = meshFn.numFaceVertices();
	int* polyconnect = new int[n_facevertex];
	
	int acc = 0;
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		for( int i=vexlist.length()-1; i >=0; i-- ) 
		{
			polyconnect[acc] = vexlist[i];
			acc++;
		}
	}
	
	xml_f.addFaceConnection(n_facevertex, polyconnect);
	delete[] polyconnect;
	
	int* triconnect = new int[3*n_tri];
	acc = 0;
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		for( int i=vexlist.length()-2; i >0; i-- ) 
		{
			triconnect[acc] = vexlist[vexlist.length()-1];
			acc++;
			triconnect[acc] = vexlist[i];
			acc++;
			triconnect[acc] = vexlist[i-1];
			acc++;
		}
	}
	
	xml_f.addTriangleConnection(3*n_tri, triconnect);
	delete[] triconnect;

	if(meshFn.numUVSets() > 0)
	{
		MStringArray setNames;
		meshFn.getUVSetNames(setNames);
		for(int i=0; i< setNames.length(); i++)
		{
			float* scoord = new float[n_facevertex];
			float* tcoord = new float[n_facevertex];
			
			acc = 0;
			faceIter.reset();
			for( ; !faceIter.isDone(); faceIter.next() ) 
			{
				MFloatArray uarray, varray;
				faceIter.getUVs ( uarray, varray, &setNames[i] );
				for( int j=uarray.length()-1; j >=0 ; j-- ) 
				{
					scoord[acc] = uarray[j];
					tcoord[acc] = 1.0 - varray[j];
					acc++;
				}
			}
			
			
			if(setNames[i] == "map1")
			{
				xml_f.uvSetBegin(setNames[i].asChar());
				xml_f.addS("facevarying float s", meshFn.numFaceVertices(), scoord);
				xml_f.addT("facevarying float t", meshFn.numFaceVertices(), tcoord);
				xml_f.uvSetEnd();
			}
			else
			{
				xml_f.uvSetBegin(setNames[i].asChar());
				string paramname("facevarying float u_");
				paramname.append(setNames[i].asChar());
				xml_f.addS(paramname.c_str(), meshFn.numFaceVertices(), scoord);
				
				paramname = "facevarying float v_";
				paramname.append(setNames[i].asChar());
				xml_f.addT(paramname.c_str(), meshFn.numFaceVertices(), tcoord);
				xml_f.uvSetEnd();
			}
			
			delete[] scoord;
			delete[] tcoord;
		}
	}
	
	MStringArray colorSetNames;
	meshFn.getColorSetNames (colorSetNames);
	
	for(unsigned int i=0; i<colorSetNames.length(); i++)
	{
		MStatus hasColor;
		
		XYZ *colors = new XYZ[n_vert];
		vertIter.reset();
		MString aset = colorSetNames[i];
		MColor col;
		for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
		{
				MIntArray conn_face;
				vertIter.getConnectedFaces(conn_face);
				vertIter.getColor(col, conn_face[0], &aset);
				colors[i].x = col.r*light_intensity;
				colors[i].y = col.g*light_intensity;
				colors[i].z = col.b*light_intensity;
		}
			
		xml_f.addVertexColor(aset.asChar(), n_vert, colors);
		delete[] colors;
	}

	xml_f.staticEnd();
	

	
	xml_f.dynamicBegin();
	
	MPointArray p_vert;
	
	meshFn.getPoints ( p_vert, MSpace::kWorld );
	
	MPoint corner_l(10e6, 10e6, 10e6);
	MPoint corner_h(-10e6, -10e6, -10e6);
	
	for( unsigned int i=0; i<p_vert.length(); i++) 
	{
		if( p_vert[i].x < corner_l.x ) corner_l.x = p_vert[i].x;
		if( p_vert[i].y < corner_l.y ) corner_l.y = p_vert[i].y;
		if( p_vert[i].z < corner_l.z ) corner_l.z = p_vert[i].z;
		if( p_vert[i].x > corner_h.x ) corner_h.x = p_vert[i].x;
		if( p_vert[i].y > corner_h.y ) corner_h.y = p_vert[i].y;
		if( p_vert[i].z > corner_h.z ) corner_h.z = p_vert[i].z;
	}
	
	
	
	XYZ *cv = new XYZ[n_vert];
	
	for( unsigned int i=0; i<p_vert.length(); i++) 
	{
		cv[i].x = p_vert[i].x;
		cv[i].y = p_vert[i].y;
		cv[i].z= p_vert[i].z;
	}
	
	xml_f.addP(n_vert, cv);
	delete[] cv;
	
	XYZ *nor = new XYZ[n_vert];
	XYZ *tang = new XYZ[n_vert];
	
	vertIter.reset();
	MVector vnor;
	
	for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		vertIter.getNormal(vnor, MSpace::kWorld);
		vnor.normalize();
		nor[i].x = vnor.x;
		nor[i].y = vnor.y;
		nor[i].z = vnor.z;
	}
	
	MString uvset("map1");
	
	vertIter.reset();
	for( unsigned int i=0; !vertIter.isDone(); vertIter.next(), i++ )
	{
		MIntArray conn_face;
		vertIter.getConnectedFaces(conn_face);
		
		MVector ctang(0,0,0);
		MVector ttang;
		for(int j = 0; j<conn_face.length(); j++) 
		{
			meshFn.getFaceVertexTangent (conn_face[j], i,  ttang,  MSpace::kWorld, &uvset);
			ttang.normalize();
			ctang += ttang;
		}
		ctang.normalize();
		tang[i].x = ctang.x;
		tang[i].y = ctang.y;
		tang[i].z = ctang.z;
		
		tang[i] = nor[i].cross(tang[i]);
		tang[i].normalize();
	}

	xml_f.addN(n_vert, nor);
	xml_f.addTangent(n_vert, tang);

	delete[] tang;
	delete[] nor;
	
	xml_f.dynamicEnd();
	xml_f.addBBox(corner_l.x, corner_l.y, corner_l.z, corner_h.x, corner_h.y, corner_h.z);

		xml_f.meshEnd();
	}
	
	float aspace[4][4];
	for(int i=0; i<m_nurbs_list.length(); i++) 
	{
		zWorks::getTransformWorld(m_nurbs_list[i].partialPathName(), aspace);
		xml_f.addTransform(m_nurbs_list[i].partialPathName().asChar(), aspace);
	}
	xml_f.addCamera("backscat_camera", m_space);
	xml_f.addCamera("eye_camera", m_eye);
	xml_f.end(filename);
}
//:~