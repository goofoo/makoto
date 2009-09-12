#include "zWorks.h"

#define zCheckStatusNoReturn(stat, msg)         \
    if ( MS::kSuccess != stat ) {   \
        MGlobal::displayWarning(msg);                \
    }

zWorks::zWorks(void)
{
}

zWorks::~zWorks(void)
{
}

MStatus zWorks::createVectorAttr(MObject& attr, 
					MObject& attr0, MObject& attr1, MObject& attr2, 
					const MString& nameLong,const MString& nameShort)
{
	MStatus status;
	MFnNumericAttribute attrFn;

	attr= attrFn.create(nameLong, nameShort, attr0, attr1, attr2, &status);
	attrFn.setStorable(false);
	attrFn.setKeyable(false);
	attrFn.setReadable(true);
	attrFn.setWritable(false);
	return status;
}

void zWorks::getVectorArrayFromPlug(MVectorArray& array, MPlug& plug)
{
	MObject obj;
	plug.getValue(obj);
	MFnVectorArrayData data(obj);
	array = data.array();
}

void zWorks::getDoubleArrayFromPlug(MDoubleArray& array, MPlug& plug)
{
	MObject obj;
	plug.getValue(obj);
	MFnDoubleArrayData data(obj);
	array = data.array();
}

void zWorks::extractMeshParams(const MObject& mesh, unsigned & numVertex, unsigned & numPolygons, MPointArray& vertices, MIntArray& pcounts, MIntArray& pconnects)
{
	MFnMesh finmesh(mesh);
	numVertex = finmesh.numVertices();
	numPolygons = finmesh.numPolygons();
	finmesh.getPoints(vertices);
	
	pcounts.clear();
	pconnects.clear();
	MItMeshPolygon faceIter(mesh);
	faceIter.reset();
	for( ; !faceIter.isDone(); faceIter.next() ) 
	{
		pcounts.append(faceIter.polygonVertexCount());
		MIntArray  vexlist;
		faceIter.getVertices ( vexlist );
		for( unsigned int i=0; i < vexlist.length(); i++ ) 
		{
			pconnects.append(vexlist[vexlist.length()-1-i]);
		}
	}
}

MStatus zWorks::createIntAttr(MObject& attr, const char* nameLong, const char* nameShort, int val, int min)
{
	MStatus status;
	MFnNumericAttribute fAttr;
	attr = fAttr.create(MString(nameLong), MString(nameShort), MFnNumericData::kInt, val, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setKeyable(true);
	fAttr.setConnectable(true);
	fAttr.setMin(min);
	return status;
}

MStatus zWorks::createMatrixAttr(MObject& attr, const MString& nameLong, const MString& nameShort)
{
	MStatus status;
	MFnMatrixAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, MFnMatrixAttribute::kDouble, &status);
	fAttr.setStorable(false);
	fAttr.setReadable(false);
	fAttr.setConnectable(true);
	return status;
}

MStatus zWorks::createStringAttr(MObject& attr, const MString& nameLong, const MString& nameShort)
{
	MStatus status;
	MFnTypedAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, MFnData::kString, MObject::kNullObj, &status);
	fAttr.setStorable(true);
	fAttr.setReadable(true);
	fAttr.setConnectable(false);
	return status;
}

MStatus zWorks::createVectorArrayAttr(MObject& attr, const MString& nameLong,const MString& nameShort)
{
	MStatus status;
	MFnTypedAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, MFnData::kVectorArray, MObject::kNullObj, &status);
	fAttr.setStorable(false);
	fAttr.setReadable(true);
	fAttr.setConnectable(true);
	return status;
}

void zWorks::getNamedPlug(MPlug& val, const MObject& node, const char* attrname)
{
	MFnDependencyNode fnode(node );
	val = fnode.findPlug(attrname);
}

void zWorks::getIntFromNamedPlug(int& val, const MObject& node, const char* attrname)
{
	MFnDependencyNode fnode(node );
	MPlug plg = fnode.findPlug(attrname);
	plg.getValue(val);
}

void zWorks::getDoubleFromNamedPlug(double& val, const MObject& node, const char* attrname)
{
	MFnDependencyNode fnode(node );
	MPlug plg = fnode.findPlug(attrname);
	plg.getValue(val);
}

MObject zWorks::getMeshAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNoReturn(status, "ERROR getting mesh data handle\n");
    	return hdata.asMesh();
}

void zWorks::getTypedPath(MFn::Type type, const MObject& root, MDagPath& path)
{	
	MItDag itdag;
	itdag.reset(root, MItDag::kDepthFirst, type);
	        
        for(; !itdag.isDone(); itdag.next())
        {
        	if(itdag.item().hasFn(type))
        	{
        		itdag.getPath(path);
        		return;
        	}
        }
}

void zWorks::getTypedPath(MFn::Type type, const MDagPath& root, MDagPath& path)
{
	MItDag itdag;
	itdag.reset(root, MItDag::kDepthFirst, type);
	        
        for(; !itdag.isDone(); itdag.next())
        {
        	if(itdag.item().hasFn(type))
        	{
        		itdag.getPath(path);
        		return;
        	}
        }
}

void zWorks::getAllTypedPath(MFn::Type type, MObjectArray& obj_array)
{
	obj_array.clear();
	
	MStatus stat;
	MItDependencyNodes itdag(type, &stat);
	
	if(!stat) MGlobal::displayInfo("Error creating iterator");
	
	for(; !itdag.isDone(); itdag.next())
        {
		MObject aobj =itdag.thisNode();
		obj_array.append(aobj);
        }
}

int zWorks::getAllTypedPathByRoot(MFn::Type type, MObject& root, MObjectArray& obj_array)
{
	obj_array.clear();
	
	MStatus stat;
	MItDependencyGraph itdag(root, type, MItDependencyGraph::kUpstream, MItDependencyGraph::kDepthFirst, MItDependencyGraph::kNodeLevel, &stat);
	
	if(!stat) return 0; //MGlobal::displayInfo("Error creating iterator");
	
	for(; !itdag.isDone(); itdag.next())
        {
		MObject aobj =itdag.thisNode();
		obj_array.append(aobj);
        }
	
	return 1;
}

void zWorks::getTypedPathByName(MFn::Type type, MString& name, MDagPath& path)
{	
	MItDag itdag(MItDag::kDepthFirst, type);
	        
        for(; !itdag.isDone(); itdag.next())
        {
        	if(itdag.item().hasFn(type))
        	{
        		itdag.getPath(path);
				
        		MFnDagNode pf(path);
				if(pf.name()==name) return;
        	}
        }
}

void zWorks::getTypedNodeByName(MFn::Type type, MString& name, MObject& node)
{	
	MItDag itdag(MItDag::kDepthFirst, type);
	        
        for(; !itdag.isDone(); itdag.next())
        {
        	if(itdag.item().hasFn(type))
        	{
        		node = itdag.item();
				
        		MFnDagNode pf(node);
				//MGlobal::displayInfo(pf.fullPathName());
				if(pf.fullPathName()==name) return;
        	}
        }
}

void zWorks::getConnectedNode(MObject& val, const MPlug& plg)
{
	MPlugArray conns;
	if(!plg.connectedTo (conns, true, true )) val = MObject::kNullObj;
	else val = conns[0].node();
}

void zWorks::getConnectedNodeName(MString& val, const MPlug& plg)
{
	MObject obj;
	MPlugArray conns;
	if(!plg.connectedTo (conns, true, true ))  obj = MObject::kNullObj;
	else  obj = conns[0].node();
	
	if(obj != MObject::kNullObj) val = MFnDagNode(obj).fullPathName();
}

MStatus zWorks::createTypedArrayAttr(MObject& attr, const MString& nameLong, const MString& nameShort, MFnData::Type type)
{
	MStatus status;
	MFnTypedAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, type, &status);
	fAttr.setStorable(false);
	fAttr.setReadable(true);
	fAttr.setConnectable(true);
	fAttr.setArray(true);
	return status;
}

void zWorks::getNamedObject(const char* name, MObject& obj) 
{
	MGlobal::selectByName(MString(name), MGlobal::kReplaceList);
	
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

MVectorArray zWorks::getVectorArrayAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNoReturn(status, "ERROR getting vector array data handle\n");
	MFnVectorArrayData farray(hdata.data(), &status);
	zCheckStatusNoReturn(status, "ERROR creating vector array data array\n");
    	return farray.array();
}

MDoubleArray zWorks::getDoubleArrayAttr(MDataBlock& data, MObject& attr)
{
	MStatus status;
	MDataHandle hdata = data.inputValue(attr, &status);
	zCheckStatusNoReturn(status, "ERROR getting double array data handle\n");
	MFnDoubleArrayData farray(hdata.data(), &status);
	zCheckStatusNoReturn(status, "ERROR creating double array data array\n");
    	return farray.array();
}

MStatus zWorks::createTypedAttr(MObject& attr, const MString& nameLong, const MString& nameShort, MFnData::Type type)
{
	MStatus status;
	MFnTypedAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort, type, &status);
	fAttr.setStorable(false);
	fAttr.setReadable(true);
	fAttr.setConnectable(true);
	return status;
}

MStatus zWorks::createTimeAttr(MObject& attr, const MString& nameLong,const MString& nameShort, double val)
{
	MStatus status;
	MFnUnitAttribute unitAttr;
	attr = unitAttr.create(nameLong, nameShort, MFnUnitAttribute::kTime, val, &status );
	unitAttr.setKeyable(true);
	unitAttr.setAffectsWorldSpace(true);
	unitAttr.setStorable(true);
	return status;
}

MStatus zWorks::createTimeAttrInternal(MObject& attr, const MString& nameLong,const MString& nameShort, double val)
{
	MStatus status;
	MFnUnitAttribute unitAttr;
	attr = unitAttr.create(nameLong, nameShort, MFnUnitAttribute::kTime, val, &status );
	unitAttr.setKeyable(true);
	unitAttr.setAffectsWorldSpace(true);
	unitAttr.setStorable(true);
	unitAttr.setInternal(true);
	return status;
}

MMatrix zWorks::getMatrixAttr(const MObject& node, MObject& attr)
{
	MPlug matplg( node, attr );
	MObject matobj;
	matplg.getValue(matobj);
	MFnMatrixData matdata(matobj);
    	return matdata.matrix();
}

MString zWorks::getStringAttr(const MObject& node, MObject& attr)
{
	MPlug strplg( node, attr );
	MString res;
	strplg.getValue(res);
    	return res;
}

MStatus zWorks::createDoubleAttr(MObject& attr, const MString& nameLong, const MString& nameShort, double val)
{
	MStatus status;
	MFnNumericAttribute fAttr;
	attr = fAttr.create(nameLong, nameShort,  MFnNumericData::kDouble, val, &status);
	fAttr.setStorable(true);
	fAttr.setKeyable(true);
	fAttr.setReadable(true);
	fAttr.setConnectable(true);
	return status;
}

#include <string>

void zWorks::validateFilePath(MString& name)
{
	std::string str(name.asChar());

	int found = str.find('|', 0);
	
	while(found>-1)
	{
		str[found] = '_';
		found = str.find('|', found);
	}
	
	found = str.find(':', 0);
	
	while(found>-1)
	{
		str[found] = '_';
		found = str.find(':', found);
	}
		
		name = MString(str.c_str());
}

void zWorks::noDotDagPath(MString& name)
{
	std::string str(name.asChar());

	int found = str.find('.', 0);
	
	while(found>-1)
	{
		str[found] = '_';
		found = str.find('.', found);
	}
		
		name = MString(str.c_str());
}

void zWorks::getWindowsPath(MString& name)
{
	std::string str(name.asChar());

	int found = str.find('/', 0);
	
	while(found>-1)
	{
		str[found] = '\\';
		found = str.find('/', found);
	}
		
		name = MString(str.c_str());
}

void zWorks::getFileNameFirstDot(MString& name)
{
	std::string str(name.asChar());

	int found = str.find('.', 0);
	if(found>-1)str.erase(found);
		
	name = MString(str.c_str());
}

void zWorks::cutFileNameLastSlash(MString& name)
{
	std::string str(name.asChar());

	int found = str.find_last_of('/', str.size()-1);
	if(found>-1)str.erase(0, found+1);
		
	name = MString(str.c_str());
}

void zWorks::changeFileNameExtension(MString& name, const char* ext)
{
	std::string str(name.asChar());

	int found = str.find_last_of('.', str.size()-1);
	if(found>-1)str.erase(found);
		
	name = MString(str.c_str());

	name += MString(ext);
}

void zWorks::getProjectDataPath(MString& path)
{
	MGlobal::executeCommand(MString ("string $p = `workspace -q -fn`"), path);
	path = path+"/data/";
}

int zWorks::hasNamedAttribute(const MObject& node, const char* attrname)
{
	MFnDependencyNode fnode(node);
			
	MStatus res;
	fnode.attribute(attrname, &res);
	if(res == MS::kFailure) return 0;
		
	return 1;
}

void zWorks::displayIntParam(const char* str, int val)
{
	MGlobal::displayInfo(MString(str) + ": " + val);
}

void zWorks::displayVectorParam(const char* str, double x, double y, double z)
{
	MGlobal::displayInfo(MString(str) + ": " + x + " " + y + " " + z);
}

void zWorks::getColorAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& r, double& g, double& b)
{
	MPlug plgR = fnode.findPlug(MString(attrname)+"R");
	plgR.getValue(r);
	
	MPlug plgG = fnode.findPlug(MString(attrname)+"G");
	plgG.getValue(g);
	
	MPlug plgB = fnode.findPlug(MString(attrname)+"B");
	plgB.getValue(b);
}

void zWorks::getNormalAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& r, double& g, double& b)
{
	MPlug plgR = fnode.findPlug(MString(attrname)+"X");
	plgR.getValue(r);
	
	MPlug plgG = fnode.findPlug(MString(attrname)+"Y");
	plgG.getValue(g);
	
	MPlug plgB = fnode.findPlug(MString(attrname)+"Z");
	plgB.getValue(b);
}

char zWorks::getBoolAttributeByName(const MFnDependencyNode& fnode, const char* attrname, bool& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char zWorks::getDoubleAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char zWorks::getDoubleAttributeByNameAndTime(const MFnDependencyNode& fnode, const char* attrname, MDGContext & ctx, double& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v, ctx);
	return 1;
}

char zWorks::getStringAttributeByName(const MFnDependencyNode& fnode, const char* attrname, MString& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char zWorks::getStringAttributeByName(const MObject& node, const char* attrname, MString& v)
{
	MFnDependencyNode fnode(node);
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

int zWorks::getConnectedAttributeByName(const MFnDependencyNode& fnode, const char* attrname, MString& v)
{
	MPlug plgTo = fnode.findPlug(MString(attrname));
	
	if(!plgTo.isConnected()) return 0;
	
	MPlugArray conns;
	
	plgTo.connectedTo (conns, true, false);
	
	v = conns[0].name();
	
	return 1;
}

void zWorks::getTransformWorld(const MString& name, float space[4][4])
{
	MDoubleArray fm;
	MGlobal::executeCommand(MString("xform -q -m -ws ")+name,fm );
	space[0][0]=fm[0]; space[0][1]=fm[1]; space[0][2]=fm[2];
	space[1][0]=fm[4]; space[1][1]=fm[5]; space[1][2]=fm[6];
	space[2][0]=fm[8]; space[2][1]=fm[9]; space[2][2]=fm[10];
	space[3][0]=fm[12]; space[3][1]=fm[13]; space[3][2]=fm[14];
	fm.clear();
}

MVector zWorks::getTransformWorldNoScale(const MString& name, float space[4][4])
{
	MDoubleArray fm;
	MGlobal::executeCommand(MString("xform -q -m -ws ")+name,fm );
	MVector scale;
	MVector vx(fm[0], fm[1], fm[2]); scale.x = vx.length(); vx.normalize();
	MVector vy(fm[4], fm[5], fm[6]); scale.y = vy.length(); vy.normalize();
	MVector vz(fm[8], fm[9], fm[10]); scale.z = vz.length(); vz.normalize();
	space[0][0]=vx.x; space[0][1]=vx.y; space[0][2]=vx.z;
	space[1][0]=vy.x; space[1][1]=vy.y; space[1][2]=vy.z;
	space[2][0]=vz.x; space[2][1]=vz.y; space[2][2]=vz.z;
	space[3][0]=fm[12]; space[3][1]=fm[13]; space[3][2]=fm[14];
	fm.clear();
	return scale;
}
//:~
