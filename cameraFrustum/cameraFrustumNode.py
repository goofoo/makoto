import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx
import maya.OpenMayaRender as OpenMayaRender
import maya.OpenMayaUI as OpenMayaUI

import math
import sys

kPluginNodeTypeName = "cameraFrustumNode"

spCameraFrustumNodeId = OpenMaya.MTypeId(0x000291910)
glRenderer = OpenMayaRender.MHardwareRenderer.theRenderer()
glFT = glRenderer.glFunctionTable()

class spCameraFrustumNode(OpenMayaMPx.MPxLocatorNode):
	
	amatrix = OpenMaya.MObject()
	anear = OpenMaya.MObject()
	afar = OpenMaya.MObject()
	ahaperture = OpenMaya.MObject()
	avaperture = OpenMaya.MObject()
	afocallength = OpenMaya.MObject()
	aorthographic = OpenMaya.MObject()
	aorthographicwidth = OpenMaya.MObject()
	
	def __init__(self):
		OpenMayaMPx.MPxLocatorNode.__init__(self)
		
	def compute(self, plug, dataBlock):
		return OpenMaya.kUnknownParameter

	def draw(self, view, path, style, status):
		thisNode = self.thisMObject()

		plug = OpenMaya.MPlug(thisNode, self.amatrix)
		
		omat = plug.asMObject()
		
		dmat = OpenMaya.MFnMatrixData(omat)
		
		mat = OpenMaya.MMatrix()
		
		mat = dmat.matrix()

		eye = OpenMaya.MPoint()
		
		eye *= mat
		
		plug = OpenMaya.MPlug(thisNode, self.anear)
		
		near_clip = plug.asDouble()
		
		plug = OpenMaya.MPlug(thisNode, self.afar)
		far_clip = plug.asDouble()
		
		plug = OpenMaya.MPlug(thisNode, self.ahaperture)
		h_aperture = plug.asDouble()
		
		plug = OpenMaya.MPlug(thisNode, self.avaperture)
		v_aperture = plug.asDouble()
		
		plug = OpenMaya.MPlug(thisNode, self.afocallength)
		fl = plug.asDouble()

		plug = OpenMaya.MPlug(thisNode, self.aorthographicwidth)
		orthwidth = plug.asDouble()

		plug = OpenMaya.MPlug(thisNode, self.aorthographic)
		orth = plug.asBool()
		
		h_fov = h_aperture * 0.5 / ( fl * 0.03937 );
		v_fov = v_aperture * 0.5 / ( fl * 0.03937 );
	
		fright = far_clip * h_fov;
		ftop = far_clip * v_fov;
	
		nright = near_clip * h_fov;
		ntop = near_clip * v_fov;

		if orth:
                        fright = ftop = nright = ntop = orthwidth/2.0;
	
	
		corner_a = OpenMaya.MPoint(fright,ftop,-far_clip);
		corner_a *= mat;
	
		corner_b = OpenMaya.MPoint(-fright,ftop,-far_clip);
		corner_b *= mat;
		
		corner_c = OpenMaya.MPoint(-fright,-ftop,-far_clip);
		corner_c *= mat;
		
		corner_d = OpenMaya.MPoint(fright,-ftop,-far_clip);
		corner_d *= mat;
		
		corner_e = OpenMaya.MPoint(nright,ntop,-near_clip);
		corner_e *= mat;
		
		corner_f = OpenMaya.MPoint(-nright,ntop,-near_clip);
		corner_f *= mat;
		
		corner_g = OpenMaya.MPoint(-nright,-ntop,-near_clip);
		corner_g *= mat;
		
		corner_h = OpenMaya.MPoint(nright,-ntop,-near_clip);
		corner_h *= mat;
		
		view.beginGL()
		
		glFT.glBegin( OpenMayaRender.MGL_LINES );

		glFT.glVertex3f( corner_a.x , corner_a.y, corner_a.z );
		glFT.glVertex3f( corner_e.x , corner_e.y, corner_e.z );
		
		glFT.glVertex3f( corner_b.x , corner_b.y, corner_b.z );
		glFT.glVertex3f( corner_f.x , corner_f.y, corner_f.z );
		
		glFT.glVertex3f( corner_c.x , corner_c.y, corner_c.z );
		glFT.glVertex3f( corner_g.x , corner_g.y, corner_g.z );
		
		glFT.glVertex3f( corner_d.x , corner_d.y, corner_d.z );
		glFT.glVertex3f( corner_h.x , corner_h.y, corner_h.z );
		
		glFT.glVertex3f( corner_a.x , corner_a.y, corner_a.z );
		glFT.glVertex3f( corner_b.x , corner_b.y, corner_b.z );
		
		glFT.glVertex3f( corner_b.x , corner_b.y, corner_b.z );
		glFT.glVertex3f( corner_c.x , corner_c.y, corner_c.z );
		
		glFT.glVertex3f( corner_c.x , corner_c.y, corner_c.z );
		glFT.glVertex3f( corner_d.x , corner_d.y, corner_d.z );
		
		glFT.glVertex3f( corner_d.x , corner_d.y, corner_d.z );
		glFT.glVertex3f( corner_a.x , corner_a.y, corner_a.z );
		
		glFT.glVertex3f( corner_e.x , corner_e.y, corner_e.z );
		glFT.glVertex3f( corner_f.x , corner_f.y, corner_f.z );
		
		glFT.glVertex3f( corner_f.x , corner_f.y, corner_f.z );
		glFT.glVertex3f( corner_g.x , corner_g.y, corner_g.z );
		
		glFT.glVertex3f( corner_g.x , corner_g.y, corner_g.z );
		glFT.glVertex3f( corner_h.x , corner_h.y, corner_h.z );
		
		glFT.glVertex3f( corner_h.x , corner_h.y, corner_h.z );
		glFT.glVertex3f( corner_e.x , corner_e.y, corner_e.z );
		
		glFT.glEnd();

		if style == OpenMayaUI.M3dView.kFlatShaded or style == OpenMayaUI.M3dView.kGouraudShaded:
			glFT.glPushAttrib(OpenMayaRender.MGL_ALL_ATTRIB_BITS)
			
			glFT.glClearDepth(1.0)
    	
                        glFT.glEnable(OpenMayaRender.MGL_BLEND)

                        glFT.glEnable(OpenMayaRender.MGL_DEPTH_TEST)
                        glFT.glDepthFunc(OpenMayaRender.MGL_LEQUAL)

                        glFT.glShadeModel(OpenMayaRender.MGL_SMOOTH)

                        glFT.glBlendFunc(OpenMayaRender.MGL_SRC_ALPHA, OpenMayaRender.MGL_ONE_MINUS_SRC_ALPHA)

                        glFT.glDepthMask( OpenMayaRender.MGL_FALSE )

                        glFT.glColor4f(0.05, 0.6, 0.2, 0.2)
			
			glFT.glBegin( OpenMayaRender.MGL_QUADS )
			glFT.glVertex3f( corner_e.x , corner_e.y, corner_e.z )
			glFT.glVertex3f( corner_f.x , corner_f.y, corner_f.z )
			glFT.glVertex3f( corner_g.x , corner_g.y, corner_g.z )
			glFT.glVertex3f( corner_h.x , corner_h.y, corner_h.z )
			
			glFT.glVertex3f( corner_a.x , corner_a.y, corner_a.z )
			glFT.glVertex3f( corner_b.x , corner_b.y, corner_b.z )
			glFT.glVertex3f( corner_c.x , corner_c.y, corner_c.z )
			glFT.glVertex3f( corner_d.x , corner_d.y, corner_d.z )
			
			glFT.glVertex3f( corner_a.x , corner_a.y, corner_a.z )
			glFT.glVertex3f( corner_e.x , corner_e.y, corner_e.z )
			glFT.glVertex3f( corner_f.x , corner_f.y, corner_f.z )
			glFT.glVertex3f( corner_b.x , corner_b.y, corner_b.z )
			
			glFT.glVertex3f( corner_d.x , corner_d.y, corner_d.z )
			glFT.glVertex3f( corner_h.x , corner_h.y, corner_h.z )
			glFT.glVertex3f( corner_e.x , corner_e.y, corner_e.z )
			glFT.glVertex3f( corner_a.x , corner_a.y, corner_a.z )
			
			glFT.glVertex3f( corner_c.x , corner_c.y, corner_c.z )
			glFT.glVertex3f( corner_g.x , corner_g.y, corner_g.z )
			glFT.glVertex3f( corner_h.x , corner_h.y, corner_h.z )
			glFT.glVertex3f( corner_d.x , corner_d.y, corner_d.z )
			
			glFT.glVertex3f( corner_b.x , corner_b.y, corner_b.z )
			glFT.glVertex3f( corner_f.x , corner_f.y, corner_f.z )
			glFT.glVertex3f( corner_g.x , corner_g.y, corner_g.z )
			glFT.glVertex3f( corner_c.x , corner_c.y, corner_c.z )
			
			glFT.glEnd()

			glFT.glPopAttrib()

		view.endGL()

	def isBounded(self):
		return False


# creator
def nodeCreator():
	return OpenMayaMPx.asMPxPtr( spCameraFrustumNode() )

# initializer
def nodeInitializer():
	
	matAttr = OpenMaya.MFnTypedAttribute()
	spCameraFrustumNode.amatrix = matAttr.create("cameraMatrix", "cm", OpenMaya.MFnData.kMatrix)
	matAttr.setConnectable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.amatrix )
	
	numAttr = OpenMaya.MFnNumericAttribute()
	spCameraFrustumNode.anear = numAttr.create("nearClip", "nc", OpenMaya.MFnNumericData.kDouble)
	numAttr.setConnectable(True)
	numAttr.setKeyable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.anear )
	
	spCameraFrustumNode.afar = numAttr.create("farClip", "fc", OpenMaya.MFnNumericData.kDouble)
	numAttr.setConnectable(True)
	numAttr.setKeyable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.afar )
	
	spCameraFrustumNode.ahaperture = numAttr.create("horizontalFilmAperture", "hfa", OpenMaya.MFnNumericData.kDouble)
	numAttr.setConnectable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.ahaperture )
	
	spCameraFrustumNode.avaperture = numAttr.create("verticalFilmAperture", "vfa", OpenMaya.MFnNumericData.kDouble)
	numAttr.setConnectable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.avaperture )
	
	spCameraFrustumNode.afocallength = numAttr.create("focalLength", "fl", OpenMaya.MFnNumericData.kDouble, 1.0)
	numAttr.setConnectable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.afocallength )

	spCameraFrustumNode.aorthographic = numAttr.create("orthographic", "oth", OpenMaya.MFnNumericData.kBoolean)
	numAttr.setConnectable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.aorthographic )

	spCameraFrustumNode.aorthographicwidth = numAttr.create("orthographicWidth", "ow", OpenMaya.MFnNumericData.kDouble)
	numAttr.setConnectable(True)
	spCameraFrustumNode.addAttribute( spCameraFrustumNode.aorthographicwidth )
	
# initialize the script plug-in
def initializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject, "ZHANG JIAN - Free Downlaod", "3.0", "Any")
	try:
		mplugin.registerNode( kPluginNodeTypeName, spCameraFrustumNodeId, nodeCreator, nodeInitializer, OpenMayaMPx.MPxNode.kLocatorNode )
	except:
		sys.stderr.write( "Failed to register node: %s" % kPluginNodeTypeName )
		raise

# uninitialize the script plug-in
def uninitializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject)
	try:
		mplugin.deregisterNode( spCameraFrustumNodeId )
	except:
		sys.stderr.write( "Failed to deregister node: %s" % kPluginNodeTypeName )
		raise
	
