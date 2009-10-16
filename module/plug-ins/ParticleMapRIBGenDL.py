import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx
import sys, math

kPluginCmdName="pmapVizCacheGeo"

def findNodeByName(name):
	it = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst)
	while it.isDone() == 0:
		res = it.item()
		fnode = OpenMaya.MFnDagNode(res)
		if(fnode.fullPathName()==name):
			return res;
		it.next()
		
def findPathByPartialName(name):
	res = OpenMaya.MDagPath()
	it = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst)
	while it.isDone() == 0:
		it.getPath(res)
		fnode = OpenMaya.MFnDagNode(res)
		if(fnode.partialPathName()==name):
			return res;
		it.next()

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
			
			oviz = findNodeByName(spl[0])
			
			fnode = OpenMaya.MFnDependencyNode(oviz)
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
			# get density
			plg = fnode.findPlug('density')
			density = plg.asFloat()
			# get dusty
			plg = fnode.findPlug('dusty')
			dusty = plg.asFloat()
			
			print '  render pass: ',spl[1]
			print '  frame: ',spl[2]
			print '  shutters: ',spl[3],' ',spl[4]
			print '  is shadow pass: ',spl[5]
			print '  fps: ',spl[6]
			print '  camera: ',spl[7]
			
			ocam = findPathByPartialName(spl[7])
			fcam = OpenMaya.MFnCamera(ocam)
			
			viewDir = fcam.viewDirection( OpenMaya.MSpace.kWorld )
			eyePos = fcam.eyePoint( OpenMaya.MSpace.kWorld )
			rightDir = fcam.rightDirection( OpenMaya.MSpace.kWorld);
			upDir = fcam.upDirection( OpenMaya.MSpace.kWorld );
			
			ipersp = 1
			fov = fcam.horizontalFieldOfView()
			fov = fov/3.1415927*180;
			if fcam.isOrtho():
				ipersp = 0
				fov = fcam.orthoWidth()
			
			sp = 'Procedural \\"DynamicLoad\\" [\\"particleCacheProcedural\\" \\"%s.%s.pmap %f %f %s %s 0 %s %f %d %f %f %f 0 %f %f %f 0 %f %f %f 0 %f %f %f 1\\"] [%f %f %f %f %f %f]\\n' % (scache, spl[2], density, dusty, spl[3], spl[4], spl[5], fov, ipersp, -rightDir.x, -rightDir.y, -rightDir.z, upDir.x, upDir.y, upDir.z, viewDir.x, viewDir.y, viewDir.z, eyePos.x, eyePos.y, eyePos.z, bbox[0], bbox[1], bbox[2], bbox[3], bbox[4], bbox[5])
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

	
