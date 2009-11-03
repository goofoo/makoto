#include "viewVolumeTool.h"
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

viewVolumeTool::~viewVolumeTool() {}

viewVolumeTool::viewVolumeTool()
{
	setCommandString("viewVolumeToolCmd");
}

void* viewVolumeTool::creator()
{
	return new viewVolumeTool;
}

MSyntax viewVolumeTool::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(kOptFlag, kOptFlagLong, MSyntax::kUnsigned);
	syntax.addFlag(kNsegFlag, kNsegFlagLong, MSyntax::kUnsigned);
	syntax.addFlag(kLsegFlag, kLsegFlagLong, MSyntax::kDouble );
	
	return syntax;
}

MStatus viewVolumeTool::doIt(const MArgList &args)
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

MStatus viewVolumeTool::parseArgs(const MArgList &args)
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

MStatus viewVolumeTool::finalize()
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

viewVolumeContext::viewVolumeContext()
{
	setTitleString ( "viewVolume Tool" );

	// Tell the context which XPM to use so the tool can properly
	// be a candidate for the 6th position on the mini-bar.
	setImage("viewVolumeTool.xpm", MPxContext::kImage1 );
	
}

void viewVolumeContext::toolOnSetup ( MEvent & )
{
	setHelpString( helpString );
}

MStatus viewVolumeContext::doPress( MEvent & event )
//
// Begin viewVolume drawing (using OpenGL)
// Get the start position of the viewVolume 
//
{
// list all pmap node
	MStatus stat;
	MItDependencyNodes itdag(MFn::kPluginLocatorNode, &stat);
		
	if( event.mouseButton() == MEvent::kLeftMouse && event.isModifierControl() ) {
		MGlobal::displayInfo("view high volume");
		for(; !itdag.isDone(); itdag.next())
		{
			MObject aobj =itdag.thisNode();
			MFnDependencyNode fnode(aobj);
			if(fnode.typeName() == "pmapViz") {
				fnode.findPlug("drawType").setValue(0);
			}
		}
	}
	else {
		MGlobal::displayInfo("view low volume");
		for(; !itdag.isDone(); itdag.next())
		{
			MObject aobj =itdag.thisNode();
			MFnDependencyNode fnode(aobj);
			if(fnode.typeName() == "pmapViz") {
				fnode.findPlug("drawType").setValue(1);
			}
		}
	}

	return MS::kSuccess;		
}

MStatus viewVolumeContext::doEnterRegion( MEvent & )
{
	return setHelpString( helpString );
}

void viewVolumeContext::getClassName( MString & name ) const
{
	name.set("viewVolume");
}

viewVolumeContextCmd::viewVolumeContextCmd() {}

MPxContext* viewVolumeContextCmd::makeObj()
{
	fContext = new viewVolumeContext();
	return fContext;
}

void* viewVolumeContextCmd::creator()
{
	return new viewVolumeContextCmd;
}

MStatus viewVolumeContextCmd::doEditFlags()
{
	MStatus status = MS::kSuccess;
	
	MArgParser argData = parser();
	/*
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
*/
	return MS::kSuccess;
}

MStatus viewVolumeContextCmd::doQueryFlags()
{
	MArgParser argData = parser();
	/*
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
	*/
	return MS::kSuccess;
}

MStatus viewVolumeContextCmd::appendSyntax()
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

