import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx
import sys, math

kPluginCmdName="pmapVizCacheGeo"

# command
class scriptedCommand(OpenMayaMPx.MPxCommand):
	def __init__(self):
		self.onode = OpenMaya.MObject()
		OpenMayaMPx.MPxCommand.__init__(self)
	
	def doIt(self, args):
		
		# Parse the arguments.
		argData = OpenMaya.MArgDatabase(self.syntax(), args)		
		
		if argData.isFlagSet("-e"):
			oname = []
			argData.getObjects( oname )
			spl = oname[0].split(' ',8)
			print kPluginCmdName,' emits ',spl[0]
			self.findNode(spl[0])
			fnode = OpenMaya.MFnDependencyNode(self.onode)
			# get bounding box
			bbox = [-1,1,-1,1,-1,1]
			plg = fnode.findPlug('boundingBoxMinX')
			bbox[0] = plg.asFloat()
			plg = fnode.findPlug('boundingBoxMaxX')
			bbox[1] = plg.asFloat()
			plg = fnode.findPlug('boundingBoxMinY')
			bbox[2] = plg.asFloat()
			plg = fnode.findPlug('boundingBoxMaxY')
			bbox[3] = plg.asFloat()
			plg = fnode.findPlug('boundingBoxMinZ')
			bbox[4] = plg.asFloat()
			plg = fnode.findPlug('boundingBoxMaxZ')
			bbox[5] = plg.asFloat()
			# get cache path
			plg = fnode.findPlug('cachePath')
			scache = plg.asString()
			print '  render pass: ',spl[1]
			print '  frame: ',spl[2]
			print '  shutters: ',spl[3],' ',spl[4]
			print '  is shadow pass: ',spl[5]
			print '  fps: ',spl[6]
			print '  camera: ',spl[7]
			sp = 'Procedural \\"DynamicLoad\\" [\\"particleCacheProcedural.so\\" \\"%s.%s.pmap\\"] [%f %f %f %f %f %f]\\n' % (scache, spl[2], bbox[0], bbox[1], bbox[2], bbox[3], bbox[4], bbox[5])
			asp = 'RiArchiveRecord -m "verbatim" -t "'+sp+'"'
			OpenMaya.MGlobal.executeCommand(asp)
	
	def findNode(self, name):
		it = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst)
		while it.isDone() == 0:
			self.onode = it.item()
        		fnode = OpenMaya.MFnDagNode(self.onode)
			if(fnode.fullPathName()==name):
				return;
			it.next()

			
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

	
