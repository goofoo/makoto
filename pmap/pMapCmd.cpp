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
	//syntax.addFlag("-c","-color",MSyntax::kBoolean);
	//syntax.addFlag("-v","-velocity",MSyntax::kBoolean);
	syntax.addFlag("-a","-attrib",MSyntax::kString);
	//syntax.addFlag("-l","-lifespan",MSyntax::kBoolean); 
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
	MString cache_name = "foo";;
	MString cache_attrib;
	//double mindist;

	if (argData.isFlagSet("-p")) argData.getFlagArgument("-p", 0, cache_path);
	if (argData.isFlagSet("-n")) argData.getFlagArgument("-n", 0, cache_name);
	//if (argData.isFlagSet("-v")) argData.getFlagArgument("-v", 0, cache_velocity);
	//if (argData.isFlagSet("-c")) argData.getFlagArgument("-c", 0, cache_color);
	if (argData.isFlagSet("-a")) argData.getFlagArgument("-a", 0, cache_attrib);
	//if (argData.isFlagSet("-l")) argData.getFlagArgument("-l", 0, cache_lifespan);
	//if (argData.isFlagSet("-md")) argData.getFlagArgument("-a", 0, mindist);

	MStringArray attribArray;
	cache_attrib.split('.',attribArray);
	//}
	//for(int i = 0;i<attribArray.length();i++ ){
		//cache_attrib.split(' ',attribArray);
	//	MGlobal::displayInfo(MString("caoniama   ") + attribArray[i]);
	//}
    
	//MGlobal::displayInfo(MString("cache_attrib: ")+cache_attrib);

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
	
	list.reset();
	for(;!list.isDone();list.next()) {
		list.getDagPath (fDagPath, component);
	    MFnParticleSystem ps( fDagPath );
 
		const unsigned int count = ps.count();
	   // MIntArray ids;
	    //ps.particleIds( ids );
	    MVectorArray positions;
	    ps.position( positions );
		assert( positions.length() == count);
       
		for(unsigned i=0; i<positions.length(); i++,acc++ ) {
			MVector p = positions[i];
			buf[acc].pos.x = p[0];
			buf[acc].pos.y = p[1];
			buf[acc].pos.z = p[2];
		    buf[acc].idx = acc;
		}
		
/*
		if (cache_velocity){
			acc = add;
			MVectorArray velocity;
			ps.velocity(velocity);
			for(unsigned i=0; i<positions.length(); i++,acc++ ) {
				MVector v = velocity[i];
			    pve[acc].x = v[0];
			    pve[acc].y = v[1];
			    pve[acc].z = v[2];
			}
		}

		if (cache_color) {
			acc = add;
			//if(ps.hasRgb()){
				MVectorArray color;
				ps.rgb(color);
				for(unsigned i=0; i<positions.length(); i++,acc++ ){	
					MVector c = color[i];
			        pco[acc].x = c[0];
			        pco[acc].y = c[1];
			        pco[acc].z = c[2];
				}
			//}
		}

		if (cache_acceleration) {
			acc = add;
			MVectorArray acceleration;
			ps.acceleration(acceleration);
			for(unsigned i=0; i<positions.length(); i++,acc++ ) {
				MVector a = acceleration[i];
			    pac[acc].x = a[0];
			    pac[acc].y = a[1];
			    pac[acc].z = a[2];
			}
		}

		if (cache_lifespan) {
			acc = add;
			MDoubleArray lifespan;
			ps.lifespan(lifespan);
			for(unsigned i=0; i<positions.length(); i++,acc++ ) {
				plf[acc] = lifespan[i];
			}
		}

		add += count;*/
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
	
	for(unsigned i=0; i<attribArray.length(); i++)
	{
		XYZ* attrArray = new XYZ[npt];
	    float* attr = new float[npt];
        
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
							attr[j] = attribute[j]; 
					}
					tree->addSingle(attr,temp.asChar(),buf);
					//MGlobal::displayInfo(MString("")+temp.asChar());
				}
			}
			delete[] attrArray;
			delete attrArray;
	}

	//if (cache_velocity)
	//	tree->addThree(pve, "velocity", buf);
	//if (cache_color)
	//	tree->addThree(pco, "color", buf);
	//if (cache_acceleration)
	//	tree->addThree(pac, "acceleration", buf);
	//if (cache_lifespan)
	//	tree->addSingle(plf, "lifespan", buf);
	tree->save(filename);
	
	//if(tree->hasColor()) MGlobal::displayInfo("check color");
	
	delete[] buf;
	//delete[] pco;
	//delete[] pve;
	//delete[] pac;
	//delete[] plf;
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

