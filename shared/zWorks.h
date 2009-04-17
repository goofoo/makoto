#pragma once
#include <maya/MIOStream.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MMatrix.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MTime.h>
#include <maya/MDGContext.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnCamera.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPlugArray.h>

#define zDisplayFloat(x)\
MGlobal::displayInfo(MString("x: ") + x);

#define zDisplayFloat2(x, y)\
MGlobal::displayInfo(MString("xy: ") + x + " " + y);

#define zDisplayFloat3(x, y, z)\
MGlobal::displayInfo(MString("xyz: ") + x + " " + y + " " + z);

#define zCheckStatus(stat, msg)         \
    if ( MS::kSuccess != stat ) {   \
        MGlobal::displayWarning(msg);                \
        return MS::kFailure;        \
    }

class zWorks
{
public:
	zWorks(void);
	~zWorks(void);

	static void getColorAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& r, double& g, double& b);
	static void getNormalAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& r, double& g, double& b);
	static char getDoubleAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& v);
	static char getBoolAttributeByName(const MFnDependencyNode& fnode, const char* attrname, bool& v);
	static char getDoubleAttributeByNameAndTime(const MFnDependencyNode& fnode, const char* attrname, MDGContext & ctx, double& v);
	static char getStringAttributeByName(const MFnDependencyNode& fnode, const char* attrname, MString& v);
	static char getStringAttributeByName(const MObject& node, const char* attrname, MString& v);
	static int getConnectedAttributeByName(const MFnDependencyNode& fnode, const char* attrname, MString& v);
	
	static void getNamedPlug(MPlug& val, const MObject& node, const char* attrname);
	static void getIntFromNamedPlug(int& val, const MObject& node, const char* attrname);
	static void getDoubleFromNamedPlug(double& val, const MObject& node, const char* attrname);
	static void getVectorArrayFromPlug(MVectorArray& array, MPlug& plug);
	static void getDoubleArrayFromPlug(MDoubleArray& array, MPlug& plug);
	static void extractMeshParams(const MObject& mesh, unsigned & numVertex, unsigned & numPolygons, MPointArray& vertices, MIntArray& pcounts, MIntArray& pconnects);
	
	static MStatus createIntAttr(MObject& attr, const char* nameLong, const char* nameShort, int val, int min);
	static MStatus createDoubleAttr(MObject& attr, const MString& nameLong, const MString& nameShort, double val);
	static MStatus createTypedAttr(MObject& attr, const MString& nameLong, const MString& nameShort, MFnData::Type type);
	static MStatus createTypedArrayAttr(MObject& attr, const MString& nameLong, const MString& nameShort, MFnData::Type type);
	static MStatus createMatrixAttr(MObject& attr, const MString& nameLong, const MString& nameShort);
	static MStatus createStringAttr(MObject& attr, const MString& nameLong, const MString& nameShort);
	static MStatus createTimeAttr(MObject& attr, const MString& nameLong,const MString& nameShort, double val);
	static MStatus createTimeAttrInternal(MObject& attr, const MString& nameLong,const MString& nameShort, double val);
	static MStatus createVectorAttr(MObject& attr, MObject& attr0, MObject& attr1, MObject& attr2, const MString& nameLong,const MString& nameShort);
	static MStatus createVectorArrayAttr(MObject& attr, const MString& nameLong,const MString& nameShort);
	
	static MObject getMeshAttr(MDataBlock& data, MObject& attr);
	
	static void getTypedPath(MFn::Type type, const MObject& root, MDagPath& path);
	static void getAllTypedPath(MFn::Type type, MObjectArray& obj_array);
	static int getAllTypedPathByRoot(MFn::Type type, MObject& root, MObjectArray& obj_array);
	static void getTypedPath(MFn::Type type, const MDagPath& root, MDagPath& path);
	static void getTypedPathByName(MFn::Type type, MString& name, MDagPath& path);
	static void getTypedNodeByName(MFn::Type type, MString& name, MObject& node);
	static void getConnectedNode(MObject& val, const MPlug& plg);
	static void getConnectedNodeName(MString& val, const MPlug& plg);
	
	static void getNamedObject(const char* name, MObject& obj);
	
	static MMatrix getMatrixAttr(const MObject& node, MObject& attr);
	static MString getStringAttr(const MObject& node, MObject& attr);
	
	static MVectorArray getVectorArrayAttr(MDataBlock& data, MObject& attr);
	static MDoubleArray getDoubleArrayAttr(MDataBlock& data, MObject& attr);
	static void validateFilePath(MString& name);
	static void noDotDagPath(MString& name);
	static void getProjectDataPath(MString& path);
	static void getWindowsPath(MString& path);
	static void getFileNameFirstDot(MString& name);
	static void cutFileNameLastSlash(MString& name);
	static void changeFileNameExtension(MString& name, const char* ext);
	static int hasNamedAttribute(const MObject& node, const char* attrname);
	static void displayIntParam(const char* str, int val);
	static void displayVectorParam(const char* str, double x, double y, double z);
	
	static void getTransformWorld(const MString& name, float space[4][4]);
	static MVector getTransformWorldNoScale(const MString& name, float space[4][4]);
};
