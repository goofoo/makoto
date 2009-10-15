#include "PTCMapCmd.h"
#include <assert.h>
#include <maya/MGlobal.h>
#include <string>
#include "../3dtexture/Z3DTexture.h"
#include "../shared/zWorks.h"

PTCMapCmd::PTCMapCmd()
{
}

PTCMapCmd::~PTCMapCmd()
{
}

MSyntax PTCMapCmd::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-n", "-name", MSyntax::kString);
	syntax.addFlag("-a","-attrib",MSyntax::kString);
	syntax.addFlag("-mnd","-mindist",MSyntax::kDouble);
	syntax.addFlag("-mxd","-maxdist",MSyntax::kDouble);
	syntax.addFlag("-dfd","-defaultdist",MSyntax::kDouble);
	syntax.addFlag("-t","-trans",MSyntax::kString);
	syntax.addFlag("-tk","-transkey",MSyntax::kString);
	syntax.addFlag("-te","-transeye",MSyntax::kString);
	syntax.addFlag("-ml","-maxlevel",MSyntax::kLong);
	syntax.addFlag("-rs","-rootsize",MSyntax::kDouble);
	syntax.addFlag("-o","-opacity",MSyntax::kDouble);

	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus PTCMapCmd::doIt( const MArgList& args)
{
	MStatus status = parseArgs( args );
	
	if( status != MS::kSuccess ) return status;
	
	MArgDatabase argData(syntax(), args);
	
	MAnimControl timeControl;
	MTime time = timeControl.currentTime();
	int frame =int(time.value());
	MString proj;
	MGlobal::executeCommand( MString ("string $p = `workspace -q -fn`"), proj );

    MString cache_path = proj + "/data";
	MString cache_name = "foo";
	MString cache_attrib;
	double cache_mindist = 0.1;
	double cache_maxdist = 4.1;
	int max_level = 3;
	double root_size = 32;
	MString dem_trans = "nil";
	double cloud_os = 0.05;
	MString key_trans = "nil";
	MString eye_trans = "nil";

	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);
	if (argData.isFlagSet("-a")) argData.getFlagArgument("-a", 0, cache_attrib);
	if (argData.isFlagSet("-mnd")) argData.getFlagArgument("-mnd", 0, cache_mindist);
	if (argData.isFlagSet("-mxd")) argData.getFlagArgument("-mxd", 0, cache_maxdist);
	if (argData.isFlagSet("-ml")) argData.getFlagArgument("-ml", 0, max_level);
	if (argData.isFlagSet("-rs")) argData.getFlagArgument("-rs", 0, root_size);
	if (argData.isFlagSet("-t")) argData.getFlagArgument("-t", 0, dem_trans);
	if(argData.isFlagSet("-o")) argData.getFlagArgument("-o", 0, cloud_os);
	if(argData.isFlagSet("-tk")) argData.getFlagArgument("-tk", 0, key_trans);
	if(argData.isFlagSet("-te")) argData.getFlagArgument("-te", 0, eye_trans);
	
	float div = 1;
	int last = max_level;
	while(last >=0) {
		div *=2;
		last--;
	}
	
	float def_area = root_size/div;
	def_area *= def_area;
	
// get bounding box center
	MDagPath p_bbox;
		
	zWorks::getTypedPathByName(MFn::kTransform, dem_trans, p_bbox);
	MObject o_bbox = p_bbox.transform();
	
	float m_space[4][4];
	m_space[0][0]=1; m_space[0][1]=0; m_space[0][2]=0;
	m_space[1][0]=0; m_space[1][1]=1; m_space[1][2]=0;
	m_space[2][0]=0; m_space[2][1]=0; m_space[2][2]=1;
	m_space[3][0]=0; m_space[3][1]=0; m_space[3][2]=0;
	
	if(o_bbox.isNull()) MGlobal::displayWarning("Cannot find pmap dimension, use default space.");
	else zWorks::getTransformWorldNoScale(p_bbox.partialPathName(), m_space);
	
	XYZ root_center(m_space[3][0], m_space[3][1], m_space[3][2]);
	
// get key light dir
	MDagPath p_key;
		
	zWorks::getTypedPathByName(MFn::kTransform, key_trans, p_key);
	MObject o_key = p_key.transform();
	
	m_space[0][0]=1; m_space[0][1]=0; m_space[0][2]=0;
	m_space[1][0]=0; m_space[1][1]=1; m_space[1][2]=0;
	m_space[2][0]=0; m_space[2][1]=0; m_space[2][2]=1;
	m_space[3][0]=0; m_space[3][1]=0; m_space[3][2]=0;
	
	if(o_key.isNull()) MGlobal::displayWarning("Cannot find key camera, use default space.");
	else zWorks::getTransformWorldNoScale(p_key.partialPathName(), m_space);
	
	XYZ key_dir(m_space[2][0], m_space[2][1], m_space[2][2]); key_dir.normalize();
	
// get view dir
	MDagPath p_eye;
		
	zWorks::getTypedPathByName(MFn::kTransform, eye_trans, p_eye);
	MObject o_eye = p_eye.transform();
	
	m_space[0][0]=1; m_space[0][1]=0; m_space[0][2]=0;
	m_space[1][0]=0; m_space[1][1]=1; m_space[1][2]=0;
	m_space[2][0]=0; m_space[2][1]=0; m_space[2][2]=1;
	m_space[3][0]=0; m_space[3][1]=0; m_space[3][2]=0;
	
	if(o_eye.isNull()) MGlobal::displayWarning("Cannot find render camera, use default space.");
	else zWorks::getTransformWorldNoScale(p_eye.partialPathName(), m_space);
	
	XYZ view_dir(-m_space[2][0], -m_space[2][1], -m_space[2][2]); view_dir.normalize();
	
// additional attribs
	MStringArray attribArray;
	cache_attrib.split('.', attribArray);

    MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList list( slist, MFn::kParticle, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return status;
	}

	if (list.isDone()) {
		displayError( "No particles selected" );
		return MS::kSuccess;
	}

	MDagPath fDagPath;
	MObject component;
	unsigned npt = 0,acc = 0;
	for(;!list.isDone();list.next()) {
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
		npt += ps.count();
	}
	
	if(npt < 1) {
		MGlobal::displayInfo(" zero particle: do nothing ");
		return MS::kSuccess;
	}
	
	RGRID *buf = new RGRID[npt];
	unsigned *idxb = new unsigned[npt];
	
	list.reset();
	for(;!list.isDone();list.next()) {
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
 
		const unsigned int count = ps.count();
	    MVectorArray positions;
	    ps.position( positions );
		assert( positions.length() == count);
		
		MVectorArray velarr;
		ps.velocity( velarr );
		
		MIntArray ids;
		ps.particleIds(ids);
       
		for(unsigned i=0; i<positions.length(); i++,acc++ ) {
			buf[acc].pos.x = positions[i].x;
			buf[acc].pos.y = positions[i].y;
			buf[acc].pos.z = positions[i].z;
		    buf[acc].nor.x = velarr[i].x;
			buf[acc].nor.y = velarr[i].y;
			buf[acc].nor.z = velarr[i].z;
			buf[acc].area = def_area;
			buf[acc].col = XYZ(1,1,0);
			idxb[acc] = ids[i];
		}
	}

	Z3DTexture* tree = new Z3DTexture();
	tree->setGrid(buf, npt);
	tree->constructTree(root_center, root_size, max_level);
	tree->orderGridData(idxb, npt);
	MGlobal::displayInfo(MString(" num grid ")+ tree->getNumGrid());
	MGlobal::displayInfo(MString(" num voxel ")+ tree->getNumVoxel());
	MGlobal::displayInfo(MString(" max level ")+ tree->getMaxLevel());
	MGlobal::displayInfo(" calculating voxel volume occlusion...");
	tree->occlusionVolume(cloud_os, key_dir, view_dir);
	MGlobal::displayInfo(" done");
	MGlobal::displayInfo(" updating grid distance to neighbour...");
	tree->distanceToNeighbour(cache_mindist, cache_maxdist);
	MGlobal::displayInfo(" done");
/*
	XYZ* attrArray = new XYZ[npt];
	float* attr = new float[npt];
	for(unsigned i=0; i<attribArray.length(); i++)
	{ 
		acc = 0;
		MStringArray tokenizeAttr;
		MString attribute;
		attribute = attribArray[i];
		attribute.split(':',tokenizeAttr);

		MString temp = tokenizeAttr[0];
		//char* attriName = temp.asChar();
		list.reset();
		list.getDagPath (fDagPath, component);
		MFnParticleSystem ps( fDagPath );
			
		if(tokenizeAttr[1] == "vectorArray")
			{
				if(ps.hasAttribute(tokenizeAttr[0]))
				{
					list.reset();
					for(;!list.isDone();list.next()) {
		                list.getDagPath (fDagPath, component);
		                MFnParticleSystem ps( fDagPath );

						MVectorArray attribute;
						ps.getPerParticleAttribute(tokenizeAttr[0],attribute);
						for(unsigned j=0; j<attribute.length(); j++,acc++ )
						{
							MVector p = attribute[j];
							attrArray[acc].x = p[0];
							attrArray[acc].y = p[1];
							attrArray[acc].z = p[2];
						}
					}
					tree->addThree(attrArray,temp.asChar(),buf);
					//MGlobal::displayInfo(MString("")+temp.asChar());
				}
				else MGlobal::displayWarning(MString("This object has't ")+tokenizeAttr[0]+MString(" attribute"));
			}
			
			if(tokenizeAttr[1] == "floatArray")
			{
				if(ps.hasAttribute(tokenizeAttr[0]))
				{
					list.reset();
					for(;!list.isDone();list.next()) {
		                list.getDagPath (fDagPath, component);
		                MFnParticleSystem ps( fDagPath );

					    MDoubleArray attribute;
					    ps.getPerParticleAttribute(tokenizeAttr[0],attribute);
					    for(unsigned j=0; j<attribute.length(); j++,acc++ )
							attr[acc] = attribute[j]; 
					}
					tree->addSingle(attr,temp.asChar(),buf);
					//MGlobal::displayInfo(MString("")+temp.asChar());
				}
				else MGlobal::displayWarning(MString("This object has't ")+tokenizeAttr[0]+MString(" attribute"));
			}
			//delete[] attrArray;
			//delete attrArray;
	}
*/
	char filename[512];
	sprintf( filename, "%s/%s.%d.pmap", cache_path.asChar(), cache_name.asChar(), frame );
	MGlobal::displayInfo(MString("PTCMap saved ") + filename);
	tree->save(filename);
	
	delete tree;
	return MS::kSuccess;
}

void* PTCMapCmd::creator()
{
	return new PTCMapCmd();
}

bool PTCMapCmd::isUndoable() const
{
	return false;
}

MStatus PTCMapCmd::parseArgs( const MArgList& args )
{
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}

