#include "pMapCmd.h"
#include <assert.h>
#include <maya/MGlobal.h>
#include <string>
#include "../3dtexture/Z3DTexture.h"

pMapCmd::pMapCmd()
{
}

pMapCmd::~pMapCmd()
{
}

MSyntax pMapCmd::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-n", "-name", MSyntax::kString);
	syntax.addFlag("-a","-attrib",MSyntax::kString);
	syntax.addFlag("-md","-mindist",MSyntax::kDouble);

	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

MStatus pMapCmd::doIt( const MArgList& args)
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
	double cache_mindist = 1.0;

	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);
	if (argData.isFlagSet("-a")) argData.getFlagArgument("-a", 0, cache_attrib);
	if (argData.isFlagSet("-md")) argData.getFlagArgument("-md", 0, cache_mindist);

	MStringArray attribArray;
	cache_attrib.split('.', attribArray);

    MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList list( slist, MFn::kParticle, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return status;
	}

    char filename[512];
	sprintf( filename, "%s/%s.%d.pmap", cache_path.asChar(), cache_name.asChar(), frame );
	MGlobal::displayInfo(MString("PMap saving ") + filename);
	MObject component;

	if (list.isDone()) {
		displayError( "No particles selected" );
		return MS::kFailure;
	}

	MDagPath fDagPath;
	
	unsigned npt = 0,acc = 0;
	for(;!list.isDone();list.next()) {
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
		npt += ps.count();
	}
	
	if(npt < 1) {
		MGlobal::displayInfo(" zero particle: do nothing ");
	}
	
	RGRID *buf = new RGRID[npt];
	
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
       
		for(unsigned i=0; i<positions.length(); i++,acc++ ) {
			MVector p = positions[i];
			buf[acc].pos.x = p[0];
			buf[acc].pos.y = p[1];
			buf[acc].pos.z = p[2];
		    buf[acc].nor.x = velarr[i].x;
			buf[acc].nor.y = velarr[i].y;
			buf[acc].nor.z = velarr[i].z;
			buf[acc].area = 4;
			buf[acc].col = XYZ(1,1,0);
		}
	}

	Z3DTexture* tree = new Z3DTexture();
	tree->setGrid(buf, npt);
	tree->constructTree();
	MGlobal::displayInfo(MString(" num grid ")+ tree->getNumGrid());
	MGlobal::displayInfo(MString(" num voxel ")+ tree->getNumVoxel());
	MGlobal::displayInfo(MString(" max level ")+ tree->getMaxLevel());
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
	tree->save(filename);
	
	delete tree;
	return MS::kSuccess;
}

void* pMapCmd::creator()
{
	return new pMapCmd();
}

bool pMapCmd::isUndoable() const
{
	return false;
}

MStatus pMapCmd::parseArgs( const MArgList& args )
{
	MStatus stat = MS::kSuccess;
	return MS::kSuccess;
}

