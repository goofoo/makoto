/*
 *  obj2mcf.cpp
 *  
 *
 *  Created by zhang on 08-1-12.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "obj2mcf.h"
#include "../shared/zData.h"
#include "../shared/FMCFMesh.h"
obj2mcf::obj2mcf() {}

obj2mcf::~obj2mcf() {}

MStatus obj2mcf::doIt( const MArgList& args ) 
{
	MString objname, mcfname;
	MArgDatabase argData(syntax(), args);
		
	if ( !argData.isFlagSet("-i") || !argData.isFlagSet("-o") ) {
		MGlobal::displayError( "No .obj or .mcf file specified to translate! Example: obj2mcf -i filename.obj -o filename.mcf" );
		return MS::kSuccess;
	}
	else {
		argData.getFlagArgument("-i", 0, objname);
		argData.getFlagArgument("-o", 0, mcfname);
		
		char str_buf[512];
		FILE *fp;
		
		fp = fopen( objname.asChar(), "r");
		
		if( fp == NULL ) {
			sprintf( str_buf, "Failed to open file %s", objname.asChar() );
			MGlobal::displayError( str_buf );
			return MS::kFailure;
		}
		
		
		MIntArray polygonCounts;
		MIntArray polygonConnects;
		MPointArray vertexArray;
		MFloatArray uArray;
		MFloatArray vArray;
		MIntArray uvIds;
		
		int hasNormal =0;
		double x, y, z;
		
		while ( fgets( str_buf, 512, fp ) ) {
			// vertices
			if( 3 == sscanf( str_buf, "v %lg %lg %lg", &x, &y, &z ) ) {
				vertexArray.append( MPoint( x, y, z ) );
			}
			// uvs
			else if ( 2 == sscanf( str_buf, "vt %lg %lg", &x, &y ) ){
				uArray.append( x );
				vArray.append( y );
			}
			//normals
			else if( 3 == sscanf( str_buf, "vn %lg %lg %lg", &x, &y, &z ) ) 
			{
				hasNormal = 1;
			}
			// faces
			else if (strstr( str_buf, "f ")) {
			
				int count = 0;
				
				unsigned int len = strlen(str_buf);
				
				// append polygon count
				//
				for(unsigned int  i = 1; i < len; i++ ) {
	 				if( str_buf[i] == ' ' ) {
						count++;
					}
				}
				polygonCounts.append( count );
				
				// append polygon connect
				//
				char istr[32];

				MIntArray ilist;
				unsigned int j = 0;
				
				for(unsigned int i = 1; i < len + 1; i++ ) {
	 				if( str_buf[i] != ' ' && str_buf[i] != '/' && str_buf[i] != '\0' ) {
						istr[j] = str_buf[i];
						j++;
							
					}
					else {
						istr[j] = '\0';
						int ii = atoi(istr);
						ilist.append(ii);
						j = 0;
						
					}
				}
				
				for(unsigned int  i = 1; i < ilist.length(); i++ ) 
				{
					if(hasNormal==0)
					{
						if( 1 == i%2 )
							polygonConnects.append( ilist[i] - 1 );
						else
							uvIds.append( ilist[i] - 1 );
					}
					else
					{
						if( 1 == i%3 )
							polygonConnects.append( ilist[i] - 1 );
						else if( 2 == i%3)
							uvIds.append( ilist[i] - 1 );
					}
				}
			}
			
			
			
			//
		}
		
		fclose(fp);
		
		int *fcbuf = new int[polygonCounts.length()];
		int *vertex_id = new int[polygonConnects.length()];
		int *uv_id = new int[polygonConnects.length()];
		
		cout<<" N face: "<<polygonCounts.length()<<endl;
		cout<<" N face vertex: "<<polygonConnects.length()<<endl;
		cout<<" N vertex: "<<vertexArray.length()<<endl;
		cout<<" N uv: "<<uArray.length()<<endl;

		for(unsigned int  i = 0; i < polygonCounts.length(); i++ ) fcbuf[i] = polygonCounts[i];
		
		for( unsigned int i = 0; i < polygonConnects.length(); i++ ) 
		{
			vertex_id[i] = polygonConnects[i];
			uv_id[i] = uvIds[i];
		}

		XYZ *pbuf = new XYZ[vertexArray.length()];
		
		for(unsigned int  i=0; i < vertexArray.length(); i++ ) 
		{
			pbuf[i].x = vertexArray[i].x;
			pbuf[i].y = vertexArray[i].y;
			pbuf[i].z = vertexArray[i].z;
		}
		
		double *ubuf = new double[uArray.length()];
		double *vbuf = new double[vArray.length()];
		
		for(unsigned int  i=0; i<uArray.length(); i++) 
		{
			ubuf[i] = uArray[i];
			vbuf[i] = vArray[i];
		}
		
		FMCFMesh fmesh;
		fmesh.save(vertexArray.length(), 
				polygonConnects.length(), 
				polygonCounts.length(),
				uArray.length(), 
				0,
				0,
				fcbuf, 
				vertex_id, 
				uv_id,
				pbuf,
				pbuf,
				pbuf,
				pbuf,
				pbuf,
				ubuf,
				vbuf,
				mcfname.asChar());
		
		sprintf( str_buf, "obj2mcf writes %s", mcfname.asChar() );
		MGlobal::displayInfo( str_buf );
		

		polygonCounts.clear();
		polygonConnects.clear();
		vertexArray.clear();
		uArray.clear();
		vArray.clear();
		uvIds.clear();
		
		delete[] fcbuf;
		delete[] vertex_id;
		delete[] uv_id;
		delete[] pbuf;
		delete[] ubuf;
		delete[] vbuf;
		
		return MS::kSuccess;
	}

}

void* obj2mcf::creator()
{
 return new obj2mcf;
}

MSyntax obj2mcf::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag("-i", "-input", MSyntax::kString);
	syntax.addFlag("-o", "-output", MSyntax::kString);

	syntax.enableQuery(false);
	syntax.enableEdit(false);

   return syntax;
}