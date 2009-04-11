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
#include <maya/MMatrix.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MTime.h>
#include <maya/MDGContext.h>
#include <maya/MItDag.h>
#include <maya/MFnCamera.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnUnitAttribute.h>

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
    
#define zCheckStatusNR(stat, msg)         \
    if ( MS::kSuccess != stat ) {   \
        MGlobal::displayWarning(msg);                \
    }
    
#define zCheckStatusRN(stat, msg)         \
    if ( MS::kSuccess != stat ) {   \
        MGlobal::displayWarning(msg);                \
        return MObject::kNullObj;        \
    }
    
inline MStatus zCreateBoolAttr(MObject& attr, MString& nameLong, MString& nameShort, bool val)
{
	MStatus status;
	MFnNumericAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort,  MFnNumericData::kBoolean, val, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setKeyable(true);
	fAttr.setConnectable(true);
	return status;
}

inline MStatus zCreateTimeAttr(MObject& attr, const MString& nameLong,const MString& nameShort, double val)
{
	MStatus status;
	MFnUnitAttribute unitAttr;
	attr = unitAttr.create(nameLong, nameShort, MFnUnitAttribute::kTime, val, &status );
	unitAttr.setKeyable(true);
	unitAttr.setAffectsWorldSpace(true);
	unitAttr.setStorable(true);
	return status;
}
	
inline MStatus zCreateTypedAttr(MObject& attr, const MString& nameLong, const MString& nameShort, MFnData::Type type)
{
	MStatus status;
	MFnTypedAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, type, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setConnectable(true);
	return status;
}

inline MStatus zCreatePluginAttr(MObject& attr, MString& nameLong, MString& nameShort)
{
	MStatus status;
	
	MFnTypedAttribute fAttr;
	
	attr = fAttr.create(nameLong, nameShort, MFnData::kPlugin, &status);
	fAttr.setStorable(false);
	fAttr.setConnectable(true);
	return status;
}

inline MStatus zCreateMatrixAttr(MObject& attr, const MString& nameLong, const MString& nameShort)
{
	MStatus status;
	MFnMatrixAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, MFnMatrixAttribute::kDouble, &status);
	fAttr.setStorable(false);
	fAttr.setReadable(false);
	fAttr.setConnectable(true);
	return status;
}

inline MStatus zCreateKeyableIntAttr(MObject& attr, MString& nameLong, MString& nameShort, int val)
{
	MStatus status;
	MFnNumericAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort,  MFnNumericData::kInt, val, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setKeyable(true);
	fAttr.setConnectable(true);
	return status;
}


inline MStatus zCreateDoubleAttr(MObject& attr, MString& nameLong, MString& nameShort, double val)
{
	MStatus status;
	MFnNumericAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort,  MFnNumericData::kDouble, val, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setConnectable(true);
	return status;
}

inline MStatus zCreateKeyableDoubleAttr(MObject& attr, const MString& nameLong, const MString& nameShort, double val)
{
	MStatus status;
	MFnNumericAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort,  MFnNumericData::kDouble, val, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setKeyable(true);
	fAttr.setConnectable(true);
	return status;
}

inline MMatrix zGetMatrixAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting matrix data handle\n");
    	return hdata.asMatrix();
}

inline MMatrix zGetMatrixAttr(MObject& node, MObject& attr)
{
	MPlug matplg( node, attr );
	MObject matobj;
	matplg.getValue(matobj);
	MFnMatrixData matdata(matobj);
    	return matdata.matrix();
}

inline MObject zGetMeshAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting mesh data handle\n");
    	return hdata.asMesh();
}

inline bool zGetBoolAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting bool data handle\n");
    	return hdata.asBool();
}

inline int zGetIntAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting int data handle\n");
    	return hdata.asInt();
}

inline double zGetDoubleAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting double data handle\n");
    	return hdata.asDouble();
}

inline MVectorArray zGetVectorArrayAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting vector array data handle\n");
	MFnVectorArrayData farray(hdata.data(), &status);
	zCheckStatusNR(status, "ERROR creating vector array data array\n");
    	return farray.array();
}

inline MDoubleArray zGetDoubleArrayAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNR(status, "ERROR getting double array data handle\n");
	MFnDoubleArrayData farray(hdata.data(), &status);
	zCheckStatusNR(status, "ERROR creating double array data array\n");
    	return farray.array();
}

inline MVectorArray zGetVectorArrayData(MObject& attr)
{
	MStatus status;
	MFnVectorArrayData farray(attr, &status);
	zCheckStatusNR(status, "ERROR creating vector array data array\n");
    	return farray.array();
}

inline MDoubleArray zGetDoubleArrayData(MObject& attr)
{
	MStatus status;
	MFnDoubleArrayData farray(attr, &status);
	zCheckStatusNR(status, "ERROR creating double array data array\n");
    	return farray.array();
}

inline MObject zGetSelectedShape(MFn::Type type)
{
	MSelectionList activeList;
	MGlobal::getActiveSelectionList(activeList);
	if(activeList.length()<1)
	{
		return MObject::kNullObj;
	}
	
	MItSelectionList iter(activeList);

	for ( ; !iter.isDone(); iter.next() )
	{								
	        MDagPath item;			
	        MObject component;		
	        iter.getDagPath( item, component );
	        item.extendToShape();
	        
	        if(item.node().hasFn(type)) 
		{
			return item.node();
		}	
	}
	
	return MObject::kNullObj;		
}

inline void zGetSelectedPath(MFn::Type type, MDagPath& path)
{
	MSelectionList activeList;
	MGlobal::getActiveSelectionList(activeList);
	if(activeList.length()<1)
	{
		return;
	}
	
	MItSelectionList iter(activeList);
	MItDag itdag;
	MStatus stat;
	for ( ; !iter.isDone(); iter.next() )
	{								
	        MDagPath item;	
	        MObject component;		
	        iter.getDagPath( item, component );
	        //item.extendToShape();
	        //MGlobal::displayInfo(item.fullPathName());

	        itdag.reset(item, MItDag::kDepthFirst);
	        
	        for(; !itdag.isDone(); itdag.next())
	        {
	        	if(itdag.item().hasFn(type))
	        	{
	        	//MGlobal::displayInfo(itdag.fullPathName());
	        		itdag.getPath (path);
	        		return;
	        	}
	        }

	}		
}

inline void zGetTypedPath(MFn::Type type, const MDagPath& root, MDagPath& path)
{
	MItDag itdag;
	itdag.reset(root, MItDag::kDepthFirst);
	        
        for(; !itdag.isDone(); itdag.next())
        {
        	if(itdag.item().hasFn(type))
        	{
        	//MGlobal::displayInfo(itdag.fullPathName());
        		itdag.getPath(path);
        		return;
        	}
        }
}

inline void zGetTypedPath(MFn::Type type, const MObject& root, MDagPath& path)
{
	MItDag itdag;
	itdag.reset(root, MItDag::kDepthFirst);
	        
        for(; !itdag.isDone(); itdag.next())
        {
        	if(itdag.item().hasFn(type))
        	{
        	//MGlobal::displayInfo(itdag.fullPathName());
        		itdag.getPath(path);
        		return;
        	}
        }
}

inline MObject zGetShape(const MString & name, MFn::Type type) 
{
	MGlobal::selectByName(name, MGlobal::kReplaceList);
	return zGetSelectedShape(type);
}

inline void zGetNamedAttr(const MString & name, const MObject& node, MObject& val) 
{
	MStatus status;
	MFnDependencyNode fnode(node, &status);
	zCheckStatusNR(status, "ERROR getting dependency node");
	MPlug plg = fnode.findPlug (name, &status);
	zCheckStatusNR(status, "ERROR getting attribute plug");
	plg.getValue(val);
}

inline void zGetNamedAttr(const MString & name, const MObject& node, MObject& val, MDGContext& ctx)
{
	MStatus status;
	MFnDependencyNode fnode(node, &status);
	zCheckStatusNR(status, "ERROR getting dependency node");
	MPlug plg = fnode.findPlug (name, &status);
	zCheckStatusNR(status, "ERROR getting attribute plug");
	plg.getValue(val, ctx);
}

inline void zGetNamedObject(const MString & name, MObject& obj) 
{
	MGlobal::selectByName(name, MGlobal::kReplaceList);
	
	MSelectionList activeList;
	MGlobal::getActiveSelectionList(activeList);
	if(activeList.length()<1)
	{
		obj = MObject::kNullObj;
	}
	
	MItSelectionList iter(activeList);
	iter.getDependNode(obj);
	
	MGlobal::unselectByName(name);
}
