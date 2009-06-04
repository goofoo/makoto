#include "combCurveTool.h"
#include <maya/MFnCamera.h>
#include <maya/MItCurveCV.h>
#include "../shared/zWorks.h"

#define kOptFlag "-opt" 
#define kOptFlagLong "-option"

combCurveTool::~combCurveTool() {}

combCurveTool::combCurveTool()
{
	setCommandString("combCurveToolCmd");
}

void* combCurveTool::creator()
{
	return new combCurveTool;
}

MSyntax combCurveTool::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(kOptFlag, kOptFlagLong, MSyntax::kUnsigned);
	
	return syntax;
}

MStatus combCurveTool::doIt(const MArgList &args)
//
// Description
//     Sets up the helix parameters from arguments passed to the
//     MEL command.
//
{
	MStatus status;

	status = parseArgs(args);

	return MS::kSuccess;
}

MStatus combCurveTool::parseArgs(const MArgList &args)
{
	MStatus status;
	MArgDatabase argData(syntax(), args);
	
	if (argData.isFlagSet(kOptFlag)) {
		unsigned tmp;
		status = argData.getFlagArgument(kOptFlag, 0, tmp);
		if (!status) {
			status.perror("numCVs flag parsing failed");
			return status;
		}
	}
	
	return MS::kSuccess;
}

MStatus combCurveTool::finalize()
//
// Description
//     Command is finished, construct a string for the command
//     for journalling.
//
{
	MArgList command;
	command.addArg(commandString());
	command.addArg(MString(kOptFlag));
	command.addArg((int)opt);
	return MPxToolCommand::doFinalize( command );
}

combCurveContext::combCurveContext():mOpt(0)
{
	setTitleString ( "combCurve Tool" );

	// Tell the context which XPM to use so the tool can properly
	// be a candidate for the 6th position on the mini-bar.
	setImage("combCurveTool.xpm", MPxContext::kImage1 );
	
}

void combCurveContext::toolOnSetup ( MEvent & )
{
	setHelpString( helpString );
}

MStatus combCurveContext::doPress( MEvent & event )
//
// Begin combCurve drawing (using OpenGL)
// Get the start position of the combCurve 
//
{
	//combCurveTool * cmd = (combCurveTool*)newToolCommand();
	//cmd->finalize();
	
	listAdjustment = MGlobal::kReplaceList;
		// Figure out which modifier keys were pressed, and set up the
	// listAdjustment parameter to reflect what to do with the selected points.
	if (event.isModifierShift() || event.isModifierControl() ) {
		if ( event.isModifierShift() ) 
		{
			// shift only, xor new selections with previous ones
			listAdjustment = MGlobal::kXORWithList;
			
		} 
		else if ( event.isModifierControl() ) 
		{
			// control only, remove new selections from the previous list
			listAdjustment = MGlobal::kRemoveFromList; 
		}
	}

	// Extract the event information
	//
	event.getPosition( start_x, start_y );
	
	view = M3dView::active3dView();
	
// get camera matrix
	MDagPath camera;
	view.getCamera( camera );
	
	MFnCamera fnCamera( camera );
	MVector upDir = fnCamera.upDirection( MSpace::kWorld );
	MVector rightDir = fnCamera.rightDirection( MSpace::kWorld );
	MVector viewDir = fnCamera.viewDirection( MSpace::kWorld );
	MPoint eyePos = fnCamera.eyePoint ( MSpace::kWorld );
	
	clipNear = fnCamera.nearClippingPlane();
	clipFar = fnCamera.farClippingPlane();
	
	zDisplayFloat(mOpt);
	
	mat.setIdentity ();
	mat.v[0][0] = -rightDir.x;
	mat.v[0][1] = -rightDir.y;
	mat.v[0][2] = -rightDir.z;
	mat.v[1][0] = upDir.x;
	mat.v[1][1] = upDir.y;
	mat.v[1][2] = upDir.z;
	mat.v[2][0] = viewDir.x;
	mat.v[2][1] = viewDir.y;
	mat.v[2][2] = viewDir.z;
	mat.v[3][0] = eyePos.x;
	mat.v[3][1] = eyePos.y;
	mat.v[3][2] = eyePos.z;
	mat.inverse();
	
	// Create a selection list iterator
	//
	MSelectionList slist;
 	MGlobal::getActiveSelectionList( slist );
	MStatus stat;
	MItSelectionList iter( slist, MFn::kNurbsCurve, &stat );
	
	MDagPath 	mdagPath;
	MObject 	mComponent;
	
	curveList.clear();
	for ( ; !iter.isDone(); iter.next() ) 
	{
		iter.getDagPath( mdagPath, mComponent );
		mdagPath.extendToShape ();
		
		curveList.append(mdagPath);
	}

	return MS::kSuccess;		
}


MStatus combCurveContext::doDrag( MEvent & event )
//
// Drag out the combCurve (using OpenGL)
//
{
	event.getPosition( last_x, last_y );
	
	if(listAdjustment != MGlobal::kReplaceList) return MS::kSuccess;
	
	MPoint toNear, toFar;
	view.viewToWorld ( last_x, last_y, toNear, toFar );
	
	MPoint fromNear, fromFar;
	view.viewToWorld ( start_x, start_y, fromNear, fromFar );
	
	MVector dispNear = toNear - fromNear;
	MVector dispFar = toFar - fromFar;
	
	short vert_x, vert_y;
	XY cursor(start_x, start_y);
	
	MStatus stat;
	MVector disp;
	XYZ pp;
	for(unsigned i=0; i<curveList.length(); i++)
	{
		MItCurveCV cvFn( curveList[i], MObject::kNullObj, &stat );
		for ( ; !cvFn.isDone(); cvFn.next() ) 
		{
			if(cvFn.index() !=0)
			{
				MPoint vert = cvFn.position ( MSpace::kWorld);
				pp = XYZ(vert.x, vert.y, vert.z);
				mat.transform(pp);
				
				if(pp.z >clipNear)
				{
					view.worldToView ( vert, vert_x, vert_y);
					
					XY pscrn(vert_x, vert_y);
					float weight = pscrn.distantTo(cursor);
					weight = 1.f - weight/48.f;
					
					if(weight>0)
					{
						disp = dispNear + (dispFar - dispNear)*(pp.z-clipNear)/(clipFar-clipNear);
						disp *= sqrt(weight);
						cvFn.translateBy( disp, MSpace::kWorld );
					}
				}
			}
		}
		cvFn.updateCurve();
	}

	start_x = last_x;
	start_y = last_y;
	view.refresh( true );
	return MS::kSuccess;		
}

MStatus combCurveContext::doRelease( MEvent & event )
//
// Selects objects within the combCurve box.
{

	MSelectionList			incomingList, combCurveList;

	// Clear the combCurve when you release the mouse button



	// If HW overlays enabled, then clear the overlay plane
	// such that the combCurve is no longer drawn on screen.

	// Get the end position of the combCurve
	event.getPosition( last_x, last_y );
		
	if(listAdjustment != MGlobal::kReplaceList)
	{
	// Save the state of the current selections.  The "selectFromSceen"
	// below will alter the active list, and we have to be able to put
	// it back.
		MGlobal::getActiveSelectionList(incomingList);

		// If we have a zero dimension box, just do a point pick
		//
		if ( abs(start_x - last_x) < 2 && abs(start_y - last_y) < 2 ) {
			MGlobal::selectFromScreen( start_x, start_y, MGlobal::kReplaceList );
		} else {
			// Select all the objects or components within the combCurve.
			MGlobal::selectFromScreen( start_x, start_y, last_x, last_y,
									   MGlobal::kReplaceList );
		}

		// Get the list of selected items
		MGlobal::getActiveSelectionList(combCurveList);

		// Restore the active selection list to what it was before
		// the "selectFromScreen"
		MGlobal::setActiveSelectionList(incomingList, MGlobal::kReplaceList);

		// Update the selection list as indicated by the modifier keys.
		MGlobal::selectCommand(combCurveList, listAdjustment);
	}
	return MS::kSuccess;		
}

MStatus combCurveContext::doEnterRegion( MEvent & )
{
	return setHelpString( helpString );
}

void combCurveContext::getClassName( MString & name ) const
{
	name.set("combCurve");
}

void combCurveContext::setOperation(unsigned val)
{
	mOpt = val;
	MToolsInfo::setDirtyFlag(*this);
}

unsigned combCurveContext::getOperation() const
{
	return mOpt;
}

combCurveContextCmd::combCurveContextCmd() {}

MPxContext* combCurveContextCmd::makeObj()
{
	fContext = new combCurveContext();
	return fContext;
}

void* combCurveContextCmd::creator()
{
	return new combCurveContextCmd;
}

MStatus combCurveContextCmd::doEditFlags()
{
	MStatus status = MS::kSuccess;
	
	MArgParser argData = parser();
	
	if (argData.isFlagSet(kOptFlag)) 
	{
		unsigned mode;
		status = argData.getFlagArgument(kOptFlag, 0, mode);
		if (!status) {
			status.perror("mode flag parsing failed.");
			return status;
		}
		fContext->setOperation(mode);
	}
	
	return MS::kSuccess;
}

MStatus combCurveContextCmd::doQueryFlags()
{
	MArgParser argData = parser();
	
	if (argData.isFlagSet(kOptFlag)) {
		setResult((int)fContext->getOperation());
	}
	
	return MS::kSuccess;
}

MStatus combCurveContextCmd::appendSyntax()
{
	MSyntax mySyntax = syntax();
	
	if (MS::kSuccess != mySyntax.addFlag(kOptFlag, kOptFlagLong, MSyntax::kUnsigned)) return MS::kFailure;

	return MS::kSuccess;
}

