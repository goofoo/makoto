#ifndef _PMAPNODE_H
#define _PMAPNODE_H

#include <maya/MPxLocatorNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MPoint.h>
#include <maya/MFnDagNode.h>
#include <iostream>
#include <fstream>
#include <string>
#include <maya/MFnCamera.h>
#include <maya/MDagPath.h>
#include "../zFluid/FluidOctree.h"
 
class PMapNode : public MPxLocatorNode
{
public:
						  PMapNode();
	virtual				  ~PMapNode(); 

	virtual MStatus		  compute( const MPlug& plug, MDataBlock& data );
	virtual void          draw( M3dView & view, const MDagPath & path, 
                                M3dView::DisplayStyle style,M3dView::DisplayStatus status );
    virtual bool          isBounded() const;
    virtual MBoundingBox  boundingBox() const; 

	static  void*		creator();
	static  MStatus		initialize();

public:

	static  	MTypeId		id;
	static  	MObject		input;
	static  	MObject		frame;
	static  	MObject 	aminframe;
	static  	MObject 	amaxframe;
	static      MObject     adensity;
	static      MObject     anoise;
	static      MObject     adiffuse;
	static      MObject     alightposx;
	static      MObject     alightposy;
	static      MObject     alightposz;
	static  	MObject 	aviewattrib;
	static      MObject     adrawtype;
	static      MObject     ameanradius;
	static      MObject     acloudr;
	static      MObject     acloudg;
	static      MObject     acloudb;
	static      MObject     akkey;
	static      MObject     akback;
	static      MObject     alacunarity;
	static      MObject     adimension;
	static MObject aincoe;
	static MObject asaveimage;
	static MObject aresolutionx;
	static MObject aresolutiony;
	static MObject acameraname;
	static      MObject     aoutval;
private:
	FluidOctree *fData;
	int f_type, fSaveImage;
	
	void parseCamera(const MDagPath& camera, MATRIX44F& mat, double& clipnear, double& clipfar, double& fov, int& ispersp);
	MString fEyeCamera;
	char fSupported, fValid;
};

#endif
