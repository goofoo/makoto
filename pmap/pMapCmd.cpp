#include "pMapCmd.h"
#include <assert.h>
#include <maya/MGlobal.h>
#include <string>
#include "../shared/OcTree.h"

MSyntax pMapCmd::newSyntax() 
{
	MSyntax syntax;

	syntax.addFlag("-p", "-path", MSyntax::kString);
	syntax.addFlag("-n", "-name", MSyntax::kString);
	
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

	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);

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
	
	PosAndId *buf = new PosAndId[npt];
	XYZ *pco = new XYZ[npt];
	XYZ *pve = new XYZ[npt];
	
	list.reset();
	for(;!list.isDone();list.next()) {
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
 
		const unsigned int count = ps.count();
	    MIntArray ids;
	    ps.particleIds( ids );
	    MVectorArray positions;
		MVectorArray color;
		MVectorArray velocity;
	    ps.position( positions );
		assert( positions.length() == count);
		ps.velocity(velocity);
		//if(ps.hasRgb()) {
			//ps.rgb(color);
			for(unsigned i=0; i<positions.length(); i++,acc++ ) {
			
				MVector p = positions[i];
			    buf[acc].pos.x = p[0];
			    buf[acc].pos.y = p[1];
			    buf[acc].pos.z = p[2];
				buf[acc].idx = acc;
				
				MVector v = velocity[i];
				pve[acc].x = v[0];
				pve[acc].y = v[1];
				pve[acc].z = v[2];
				
				//MVector c = color[i];
				//pco[acc].x = c[0];
				//pco[acc].y = c[1];
				//pco[acc].z = c[2];
			    
			}
		/*}
		else {
			for(unsigned i=0; i<positions.length(); i++,acc++ )
			{
				MVector p = positions[i];
			    buf[acc].pos.x = p[0];
			    buf[acc].pos.y = p[1];
			    buf[acc].pos.z = p[2];
				MVector v = velocity[i];
				pve[acc].x = v[0];
				pve[acc].y = v[1];
				pve[acc].z = v[2];
				if(pve[acc].x == 0 && pve[acc].y == 0 && pve[acc].z == 0 ) {
					pco[acc].x = 1.;
					pco[acc].y = 0.;
					pco[acc].z = 0.;
				}
				else {
					float vo = 2*sqrt(pve[acc].x*pve[acc].x + pve[acc].y*pve[acc].y + pve[acc].z*pve[acc].z);
					pco[acc].x = 0.5 + pve[acc].x/vo; 
					pco[acc].y = 0.5 + pve[acc].y/vo;
					pco[acc].z = 0.5 + pve[acc].z/vo;	 
				}
			    buf[acc].idx = acc;
			}
		}*/
	}

	XYZ rootCenter;
	float rootSize;
    OcTree::getBBox(buf, npt, rootCenter, rootSize);
	float mindist = 0.1;
	short maxlevel = 0;
	while(mindist<rootSize) {
		maxlevel++;
		mindist*=2;
	}
	MGlobal::displayInfo(MString("  max leval: ")+ maxlevel);
	
	OcTree* tree = new OcTree();
	tree->construct(buf, npt, rootCenter, rootSize, maxlevel);
	tree->addThree(pve, "velocity", buf);
	tree->save(filename);
	
	//if(tree->hasColor()) MGlobal::displayInfo("check color");
	
	delete[] buf;
	delete[] pco;
	delete[] pve;
	delete tree;
	return MS::kSuccess;
}

void* pMapCmd::creator()
{
	return new pMapCmd();
}

pMapCmd::pMapCmd()
{
}

pMapCmd::~pMapCmd()
{
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

