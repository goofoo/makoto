#include "combCurveTool.h"
#include <maya/MFnCamera.h>
#include <maya/MItCurveCV.h>
#include <maya/MFnNurbsCurve.h>
#include "../shared/zWorks.h"

#define kOptFlag "-opt" 
#define kOptFlagLong "-option"
#define kNsegFlag "-nsg" 
#define kNsegFlagLong "-numSegment"
#define kLsegFlag "-lsg" 
#define kLsegFlagLong "-lengthSegment"
#define kNoiseFlag "-noi" 
#define kNoiseFlagLong "-noise"

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
	syntax.addFlag(kNsegFlag, kNsegFlagLong, MSyntax::kUnsigned);
	syntax.addFlag(kLsegFlag, kLsegFlagLong, MSyntax::kDouble );
	
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
			status.perror("opt flag parsing failed");
			return status;
		}
	}
	
	if (argData.isFlagSet(kNsegFlag)) {
		unsigned tmp;
		status = argData.getFlagArgument(kNsegFlag, 0, tmp);
		if (!status) {
			status.perror("numseg flag parsing failed");
			return status;
		}
	}
	
	if (argData.isFlagSet(kLsegFlag)) {
		double tmp;
		status = argData.getFlagArgument(kLsegFlag, 0, tmp);
		if (!status) {
			status.perror("lenseg flag parsing failed");
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
	//command.addArg(MString(kOptFlag));
	//command.addArg((int)opt);
	//command.addArg(MString(kNSegFlag));
	//command.addArg((int)nseg);
	//command.addArg(MString(kLSegFlag));
	//command.addArg((float)lseg);
	return MPxToolCommand::doFinalize( command );
}

combCurveContext::combCurveContext():mOpt(1),m_numSeg(5),m_seg_len(1.f),m_seg_noise(0.f)
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
	curveLen.clear();
	for ( ; !iter.isDone(); iter.next() ) 
	{
		iter.getDagPath( mdagPath, mComponent );
		mdagPath.extendToShape ();
		
		curveList.append(mdagPath);
		
		MFnNurbsCurve fCurve(mdagPath);
		curveLen.append(fCurve.length());
	}
	
	goCollide = 0;
	MItSelectionList meshIter( slist, MFn::kMesh, &stat );
	meshIter.getDagPath( mdagPath, mComponent );
	mdagPath.extendToShape();
	if(fcollide.setObject(mdagPath) == MS::kSuccess) goCollide = 1;

	return MS::kSuccess;		
}


MStatus combCurveContext::doDrag( MEvent & event )
//
// Drag out the combCurve (using OpenGL)
//
{
	event.getPosition( last_x, last_y );
	
	if(listAdjustment != MGlobal::kReplaceList) return MS::kSuccess;
	
	switch (mOpt)
	{
		case 1 :
			moveAll();
			break;
		case 2 :
			dragTip();
			break;
		case 3 :
			scaleAll();
			break;
		case 4 :
			deKink();
			break;
		case 5 :
			dragRoot();
			break;
		default:
			;
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

	// Create the combCurve when you release the mouse button
	if(mOpt==0) grow();


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

void combCurveContext::setNSegment(unsigned val)
{
	m_numSeg = val;
	MToolsInfo::setDirtyFlag(*this);
}

unsigned combCurveContext::getNSegment() const
{
	return m_numSeg;
}

void combCurveContext::setLSegment(float val)
{
	m_seg_len = val;
	MToolsInfo::setDirtyFlag(*this);
}

float combCurveContext::getLSegment() const
{
	return m_seg_len;
}

void combCurveContext::setCreationNoise(float val)
{
	m_seg_noise = val;
	MToolsInfo::setDirtyFlag(*this);
}

float combCurveContext::getCreationNoise() const
{
	return m_seg_noise;
}

void combCurveContext::doCollide(const MPoint& vert, MVector &v)
{
	MPoint moveto = vert + v;
	MPoint closestP;
	MVector closestN;
	fcollide.getClosestPointAndNormal (moveto, closestP, closestN, MSpace::kObject);
	MVector tocloest = closestP - moveto;
	if(tocloest*closestN > 0.f) v = tocloest;
}

void combCurveContext::moveAll()
{
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
						
						if(goCollide) doCollide(vert, disp);
						
						cvFn.translateBy( disp, MSpace::kWorld );
					}
				}
			}
		}
		cvFn.updateCurve();
	}
}

void combCurveContext::dragTip()
{
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
		MFnNurbsCurve fCurve(curveList[i]);
		int numCv = fCurve.numCVs();
		MPointArray cvs;
		fCurve.getCVs ( cvs, MSpace::kObject );
		
		pp = XYZ(cvs[numCv-1].x, cvs[numCv-1].y, cvs[numCv-1].z);
		mat.transform(pp);
		
		if(pp.z > clipNear)
		{
			view.worldToView (cvs[numCv-1], vert_x, vert_y);
					
			XY pscrn(vert_x, vert_y);
			float weight = pscrn.distantTo(cursor);
			weight = 1.f - weight/48.f;
			
			if(weight>0)
			{
				float* lenbuf = new float[numCv-1];
				for(unsigned j=0; j<numCv-1; j++) 
				{
					disp = cvs[j+1] - cvs[j];
					lenbuf[j] = disp.length();
				}
				
				disp = dispNear + (dispFar - dispNear)*(pp.z-clipNear)/(clipFar-clipNear);
				disp *= sqrt(weight);
				
				if(goCollide) doCollide(cvs[numCv-1], disp);
				
				cvs[numCv-1] += disp;
				
				disp = cvs[numCv-1] - cvs[0];
				if(disp.length() > curveLen[i])
				{
					disp.normalize();
					disp *= curveLen[i];
					cvs[numCv-1] = cvs[0] + disp;
				}
				
				for(unsigned j=numCv-2; j>0; j--) 
				{
					disp = cvs[j] - cvs[j+1];
					disp.normalize();
					disp *= lenbuf[j];
					
					cvs[j] = cvs[j+1] + disp;
				}
				
				for(unsigned j=1; j<numCv; j++) {
					disp = cvs[j] - cvs[j-1];
					disp.normalize();
					disp *= lenbuf[j-1];
					
					cvs[j] = cvs[j-1] + disp;
				}
				
				delete[] lenbuf;
			}
			fCurve.setCVs (cvs, MSpace::kObject );
			fCurve.updateCurve();
		}
	}
}

void combCurveContext::scaleAll()
{
	float mag = last_x - start_x - last_y + start_y;
	mag /= 48;
	
	short vert_x, vert_y;
	XY cursor(start_x, start_y);
	
	MStatus stat;
	MVector disp;
	XYZ pp;
	for(unsigned i=0; i<curveList.length(); i++)
	{
		MFnNurbsCurve fCurve(curveList[i]);
		int numCv = fCurve.numCVs();
		MPointArray cvs;
		fCurve.getCVs ( cvs, MSpace::kObject );
		
		pp = XYZ(cvs[0].x, cvs[0].y, cvs[0].z);
		mat.transform(pp);
		
		if(pp.z > clipNear)
		{
			view.worldToView (cvs[0], vert_x, vert_y);
					
			XY pscrn(vert_x, vert_y);
			float weight = pscrn.distantTo(cursor);
			weight = 1.f - weight/64.f;
			
			if(weight>0)
			{
				for(unsigned j=1; j<numCv; j++) 
				{
					disp = cvs[j] - cvs[0];
					disp *= 1.f + mag*weight;
					cvs[j] = cvs[0] + disp;
				}
			}
			fCurve.setCVs (cvs, MSpace::kObject );
			fCurve.updateCurve();
		}
	}
}

void combCurveContext::deKink()
{
	float mag = last_x - start_x - last_y + start_y;
	mag /= 48;
	
	short vert_x, vert_y;
	XY cursor(start_x, start_y);
	
	MStatus stat;
	MVector disp;
	XYZ pp;
	for(unsigned i=0; i<curveList.length(); i++)
	{
		MFnNurbsCurve fCurve(curveList[i]);
		int numCv = fCurve.numCVs();
		MPointArray cvs;
		fCurve.getCVs ( cvs, MSpace::kObject );
		
		pp = XYZ(cvs[0].x, cvs[0].y, cvs[0].z);
		mat.transform(pp);
		if(pp.z > clipNear) {
			view.worldToView (cvs[0], vert_x, vert_y);
						
			XY pscrn(vert_x, vert_y);
			float weight = pscrn.distantTo(cursor);
			weight = 1.f - weight/64.f;
			if(weight>0) {
				MVector totip = cvs[numCv-1] - cvs[0];
				MVector up(0,1,0);
				if(totip.normal()*up > 0.9) up = MVector(1,0,0);
				MVector side = totip.normal()^up;
				up = totip.normal()^side;
				float dseg = 1.f/(numCv-1);
				for(unsigned j=1; j<numCv-1; j++) {

					
					if(mag<0) {
						disp = cvs[j+1] - cvs[j-1];
						float dvl = disp.length();
						disp = up*(random()%71/71.f*2.f - 1.f) + side*(random()%71/71.f*2.f - 1.f);
						disp.normalize();
						disp *= mag*weight*dvl;
					}
					else {
						disp = cvs[0] + totip*j*dseg - cvs[j];
						disp *= mag*weight;
					}
					
					if(goCollide) doCollide(cvs[j], disp);
					
					cvs[j] += disp;
				}
			}
		}
		fCurve.setCVs (cvs, MSpace::kObject );
		fCurve.updateCurve();
	}
}

void combCurveContext::dragRoot()
{
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
		MFnNurbsCurve fCurve(curveList[i]);
		int numCv = fCurve.numCVs();
		MPointArray cvs;
		fCurve.getCVs ( cvs, MSpace::kObject );
		
		pp = XYZ(cvs[1].x, cvs[1].y, cvs[1].z);
		mat.transform(pp);
		
		if(pp.z > clipNear)
		{
			view.worldToView (cvs[1], vert_x, vert_y);
					
			XY pscrn(vert_x, vert_y);
			float weight = pscrn.distantTo(cursor);
			weight = 1.f - weight/48.f;
			
			if(weight>0)
			{
				float* lenbuf = new float[numCv-1];
				for(unsigned j=0; j<numCv-1; j++) 
				{
					disp = cvs[j+1] - cvs[j];
					lenbuf[j] = disp.length();
				}
				
				disp = dispNear + (dispFar - dispNear)*(pp.z-clipNear)/(clipFar-clipNear);
				disp *= sqrt(weight);
				
				if(goCollide) doCollide(cvs[1], disp);
				
				cvs[1] += disp;
				
				for(unsigned j=2; j<numCv; j++) {
					disp = cvs[j] - cvs[j-1];
					disp.normalize();
					disp *= lenbuf[j-1];
					
					cvs[j] = cvs[j-1] + disp;
				}
				
				delete[] lenbuf;
			}
			fCurve.setCVs (cvs, MSpace::kObject );
			fCurve.updateCurve();
		}
	}
}

void combCurveContext::grow()
{
	MPoint fromNear, fromFar;
	view.viewToWorld ( start_x, start_y, fromNear, fromFar );
	
	MVector ray = fromFar - fromNear;
	
	MSelectionList slist;
 	MGlobal::getActiveSelectionList( slist );
	MStatus stat;
	MItSelectionList iter( slist, MFn::kMesh, &stat );
	
	MDagPath 	mdagPath;
	MObject 	mComponent;
	
	iter.getDagPath( mdagPath, mComponent );
	mdagPath.extendToShape ();
	
	MFnMesh meshFn(mdagPath, &stat );
	if(stat)
	{
		MPointArray Aphit;
		MIntArray Aihit;
		if(meshFn.intersect (fromNear, ray, Aphit, 0, MSpace::kObject, &Aihit, &stat)) 
		{
			MVector hitnormal;
			meshFn.getPolygonNormal ( Aihit[0], hitnormal,  MSpace::kObject  );
			hitnormal.normalize();
				
			MFnNurbsCurve fcurve;
			MDoubleArray knots;
			for(unsigned j = 0;j <= m_numSeg;j++) knots.append(j);
			
			MPointArray cvs;
			for(unsigned j = 0;j <= m_numSeg;j++)
			{
				MPoint vert;
				MVector dispnor = hitnormal + MVector(rand( )%31/31.f-0.5, rand( )%71/71.f-0.5, rand( )%131/131.f-0.5)*m_seg_noise;
				dispnor.normalize();
				vert =  Aphit[0] + dispnor*m_seg_len*j;
				cvs.append(vert);
			}
			fcurve.create ( cvs, knots, 1, MFnNurbsCurve::kOpen , 0, 0, MObject::kNullObj, &stat );
		}
	}
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
	
	if (argData.isFlagSet(kNsegFlag)) 
	{
		unsigned nseg;
		status = argData.getFlagArgument(kNsegFlag, 0, nseg);
		if (!status) {
			status.perror("nseg flag parsing failed.");
			return status;
		}
		fContext->setNSegment(nseg);
	}
	
	if (argData.isFlagSet(kLsegFlag)) 
	{
		double lseg;
		status = argData.getFlagArgument(kLsegFlag, 0, lseg);
		if (!status) {
			status.perror("lseg flag parsing failed.");
			return status;
		}
		fContext->setLSegment(lseg);
	}
	
	if (argData.isFlagSet(kNoiseFlag)) 
	{
		double noi;
		status = argData.getFlagArgument(kNoiseFlag, 0, noi);
		if (!status) {
			status.perror("lseg flag parsing failed.");
			return status;
		}
		fContext->setCreationNoise(noi);
	}

	return MS::kSuccess;
}

MStatus combCurveContextCmd::doQueryFlags()
{
	MArgParser argData = parser();
	
	if (argData.isFlagSet(kOptFlag)) {
		setResult((int)fContext->getOperation());
	}
	
	if (argData.isFlagSet(kNsegFlag)) {
		setResult((int)fContext->getNSegment());
	}
	
	if (argData.isFlagSet(kLsegFlag)) {
		setResult((float)fContext->getLSegment());
	}
	
	if (argData.isFlagSet(kNoiseFlag)) {
		setResult((float)fContext->getCreationNoise());
	}
	
	return MS::kSuccess;
}

MStatus combCurveContextCmd::appendSyntax()
{
	MSyntax mySyntax = syntax();
	
	MStatus stat;
	stat = mySyntax.addFlag(kOptFlag, kOptFlagLong, MSyntax::kUnsigned);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add option arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kNsegFlag, kNsegFlagLong, MSyntax::kUnsigned);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add numseg arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kLsegFlag, kLsegFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add lenseg arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kNoiseFlag, kNoiseFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add noise arg");
		return MS::kFailure;
	}
	
	return MS::kSuccess;
}

