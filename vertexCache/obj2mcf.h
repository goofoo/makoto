/*
 *  obj2mcf.h
 *  
 *
 *  Created by zhang on 08-1-12.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MPxDeformerNode.h> 
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

//#include "meshInfo.h"

class obj2mcf : public MPxCommand
{
public :
			obj2mcf();
	virtual		~obj2mcf();
 	MStatus doIt( const MArgList& args );
 	static void* creator();
	static MSyntax newSyntax();


  private:

};