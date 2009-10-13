import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx
import sys, math

kPluginCmdName="pmapVizCacheGeo"

# command
class scriptedCommand(OpenMayaMPx.MPxCommand):
	def __init__(self):
		OpenMayaMPx.MPxCommand.__init__(self)
	
	def doIt(self, args):
		
		# Parse the arguments.
		argData = OpenMaya.MArgDatabase(self.syntax(), args)		
		
		if argData.isFlagSet("-e"):
			oname = []
			argData.getObjects( oname )
			spl = oname[0].split(' ',8)
			print kPluginCmdName,' emits ',spl[0]
			print '  render pass: ',spl[1]
			print '  frame: ',spl[2]
			print '  shutters: ',spl[3],' ',spl[4]
			print '  is shadow pass: ',spl[5]
			print '  fps: ',spl[6]
			print '  camera: ',spl[7]
			sp = 'Procedural \\"DynamicLoad\\" [\\"spheres\\" \\"21\\"] [-3 3 -3 3 -3 3]\\n'
			asp = 'RiArchiveRecord -m "verbatim" -t "'+sp+'"'
			OpenMaya.MGlobal.executeCommand(asp)
			
# Creator
def cmdCreator():
	# Create the command
	return OpenMayaMPx.asMPxPtr( scriptedCommand() )

# Syntax creator
def syntaxCreator():
	syntax = OpenMaya.MSyntax()
	syntax.addFlag("-st", "-sampleTime", OpenMaya.MSyntax.kDouble);

	syntax.addFlag("-a", "-addstep");
	syntax.addFlag("-r", "-remove");
	syntax.addFlag("-e", "-emit");
	syntax.addFlag("-f", "-flush");
	
	syntax.addFlag("-c", "-contains");
	syntax.addFlag("-l", "-list");
	
	syntax.setObjectType(OpenMaya.MSyntax.kStringObjects, 0, 1);
  
	return syntax

# Initialize the script plug-in
def initializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject, "ZHANG JIAN", "1.0.0 Tue Oct 13 2009", "Any")
	try:
		mplugin.registerCommand( kPluginCmdName, cmdCreator, syntaxCreator )
	except:
		sys.stderr.write( "Failed to register command: %s\n" % kPluginCmdName )
		raise

# Uninitialize the script plug-in
def uninitializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject)
	try:
		mplugin.deregisterCommand( kPluginCmdName )
	except:
		sys.stderr.write( "Failed to unregister command: %s\n" % kPluginCmdName )
		raise

	
