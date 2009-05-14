//
// Copyright (C) YiLi
// 
// File: guideCmd.cpp
//
// MEL Command: guide
//
// Author: Maya Plug-in Wizard 2.0
//

#include "guideCommand.h"

#include <maya/MGlobal.h>
#include <math.h>


MStatus guide::doIt( const MArgList& )
//
//	Description:
//		implements the MEL guide command.
//
//	Arguments:
//		args - the argument list that was passes to the command from MEL
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - command failed (returning this value will cause the 
//                     MEL script that is being run to terminate unless the
//                     error is caught using a "catch" statement.
//
{
	MGlobal::displayInfo("guide::doIt()");
	MStatus status;
    MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	
	MItSelectionList list( slist, MFn::kComponent, &status );
	if (MS::kSuccess != status) {
		displayError( "Could not create selection list iterator");
		return status;
	}

	MObject component;

	if (list.isDone()) {
		displayError( "No components selected" );
		return MS::kFailure;
	}
	pointArray.clear();
	normalArray.clear();
	tangentArray.clear();

	list.getDagPath (fDagPath, component);
	//switch(component.apiType())
	//{
		//case MFn::kMeshVertComponent:
		//{
	        MIntArray vertlist;
			MIntArray facelist;
			int index;
			MItMeshVertex vertexIter( fDagPath, component );
            MFnMesh meshFn( fDagPath );
			vertexIter.reset();
			if(vertexIter.isDone())
				MGlobal::displayError("No vertex or faceVertex selected");
			else
			for( int i = 0;!vertexIter.isDone();vertexIter.next() ,i++)
			{
				pointArray.append(vertexIter.position(MSpace::kWorld ));
				vertexIter.getConnectedFaces(facelist);
				index = vertexIter.index();
				meshFn.getFaceVertexTangent(facelist[0],index,tang,MSpace::kObject);
				
				vertexIter.getNormal(nor,MSpace::kObject);
				normalArray.append(nor);	
				tang = nor^tang;
				tang = tang^nor;
				tangentArray.append( tang );
			}
			//break;
		//}
	//}
 return redoIt();
}

MStatus guide::redoIt()
//
//	Description:
//		implements redo for the MEL guide command. 
//
//		This method is called when the user has undone a command of this type
//		and then redoes it.  No arguments are passed in as all of the necessary
//		information is cached by the doIt method.
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - redoIt failed.  this is a serious problem that will
//                     likely cause the undo queue to be purged
//
{
	MFnMesh meshFn;
	for(unsigned int i = 0;i < pointArray.length();i++)
	{
		MPoint mPoint;
	    MPointArray vertexArray;
	    MIntArray polygonCounts;
	    MIntArray polygonConnects;
		vertexArray.clear();
		polygonCounts.clear();
		polygonConnects.clear();
		for(int j = 0;j < num_seg;j++)
		{
			polygonCounts.append(4);
			polygonConnects.append(2*j);
			polygonConnects.append(2*j+1);
			polygonConnects.append(2*j+2);
			polygonConnects.append(2*j+3);
			
		}

		for(int j = 0;j < 2*num_seg+2;j++)
		{
			int ratio;
			if(j < 4)
				ratio = 0;
			else{
				ratio = (j-2)/2;
			    mPoint.x = pointArray[i].x + seglength*ratio*normalArray[i].x;
			    mPoint.y = pointArray[i].y + seglength*ratio*normalArray[i].y;
			    mPoint.z = pointArray[i].z + seglength*ratio*normalArray[i].z;}

			int modulus = j%4;
			if( j < 4 )
			{
				switch(j)
				{
				case 0:
					{
						vertexArray.append(MPoint(pointArray[i].x-0.5*seglength*normalArray[i].x+0.5*seglength*tangentArray[i].x,
							                      pointArray[i].y-0.5*seglength*normalArray[i].y+0.5*seglength*tangentArray[i].y,
							                      pointArray[i].z-0.5*seglength*normalArray[i].z+0.5*seglength*tangentArray[i].z));
						break;
					}
				case 1:
					{
						vertexArray.append(MPoint(pointArray[i].x-0.5*seglength*normalArray[i].x-0.5*seglength*tangentArray[i].x,
			                                      pointArray[i].y-0.5*seglength*normalArray[i].y-0.5*seglength*tangentArray[i].y,
				                                  pointArray[i].z-0.5*seglength*normalArray[i].z-0.5*seglength*tangentArray[i].z));
						break;
					}
				case 2:
					{
						vertexArray.append(MPoint(pointArray[i].x+0.5*seglength*normalArray[i].x-0.5*seglength*tangentArray[i].x,
			                                      pointArray[i].y+0.5*seglength*normalArray[i].y-0.5*seglength*tangentArray[i].y,
				                                  pointArray[i].z+0.5*seglength*normalArray[i].z-0.5*seglength*tangentArray[i].z));
						break;
					}
				case 3:
					{
						vertexArray.append(MPoint(pointArray[i].x+0.5*seglength*normalArray[i].x+0.5*seglength*tangentArray[i].x,
			                                      pointArray[i].y+0.5*seglength*normalArray[i].y+0.5*seglength*tangentArray[i].y,
				                                  pointArray[i].z+0.5*seglength*normalArray[i].z+0.5*seglength*tangentArray[i].z));
						break;
					}
				}
			}

			else
			{
				if(((j+ratio)%2 )== 0)
					vertexArray.append(MPoint(mPoint.x+0.5*seglength*normalArray[i].x-0.5*seglength*tangentArray[i].x,
			                                  mPoint.y+0.5*seglength*normalArray[i].y-0.5*seglength*tangentArray[i].y,
				                              mPoint.z+0.5*seglength*normalArray[i].z-0.5*seglength*tangentArray[i].z));
				else
					vertexArray.append(MPoint(mPoint.x+0.5*seglength*normalArray[i].x+0.5*seglength*tangentArray[i].x,
			                                  mPoint.y+0.5*seglength*normalArray[i].y+0.5*seglength*tangentArray[i].y,
				                              mPoint.z+0.5*seglength*normalArray[i].z+0.5*seglength*tangentArray[i].z));
			}

			
		}
		
		MObject newMesh = meshFn.create(2*num_seg+2, num_seg,vertexArray,polygonCounts,polygonConnects,MObject::kNullObj );
	}
	return MS::kSuccess;	
}



void* guide::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new guide();
}

guide::guide()
//
//	Description:
//		guide constructor
//
{
}

guide::~guide()
//
//	Description:
//		guide destructor
//
{
}


