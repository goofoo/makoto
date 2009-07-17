///:
// cameraFrustumNode.cpp
// Zhang Jian
// 07/12/05

#include <math.h>
#include <maya/MIOStream.h>
#include <maya/MPxLocatorNode.h> 
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MFnPlugin.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MMatrix.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPoint.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>
#include <maya/MFnCamera.h>

 

class cameraFrustumNode : public MPxLocatorNode
{
public:
	cameraFrustumNode();
	virtual ~cameraFrustumNode(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;

	static  void *          creator();
	static  MStatus         initialize();

	static  MObject         amatrix;
	static  MObject         anear;
	static  MObject         afar;
	static	MObject		ahapeture;
	static	MObject		avapeture;
	static	MObject		afocallength;
	static	MObject		aorthographic;
	static	MObject		aorthographicwidth;

public: 
	static	MTypeId		id;
};

MTypeId cameraFrustumNode::id( 0x00029191 );

MObject cameraFrustumNode::amatrix;
MObject cameraFrustumNode::anear;
MObject cameraFrustumNode::afar;
MObject cameraFrustumNode::ahapeture;
MObject cameraFrustumNode::avapeture;
MObject cameraFrustumNode::afocallength;
MObject cameraFrustumNode::aorthographic;
MObject cameraFrustumNode::aorthographicwidth;

cameraFrustumNode::cameraFrustumNode() {}
cameraFrustumNode::~cameraFrustumNode() {}

MStatus cameraFrustumNode::compute( const MPlug& /*plug*/, MDataBlock& /*data*/ )
{ 
	return MS::kUnknownParameter;
}

void cameraFrustumNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 

	MObject thisNode = thisMObject();
	MFnDependencyNode nodeFn(thisNode);

	MPlug matPlug = nodeFn.findPlug( "cameraMatrix" );
	
	MObject mat_obj;
	matPlug.getValue( mat_obj );
	MFnMatrixData mat_data( mat_obj );
	MMatrix cmat = mat_data.matrix();
	
	MPoint eye;
	eye *= cmat;
	
	double near_clip, far_clip;
	
	MPlug nearPlug = nodeFn.findPlug( "nearClip" );
	nearPlug.getValue( near_clip );
	near_clip -= 0.001;
	
	MPlug farPlug = nodeFn.findPlug( "farClip" );
	farPlug.getValue( far_clip );
	far_clip += 0.001;
	
	double h_apeture, v_apeture;
	MPlug hfaPlug = nodeFn.findPlug( "horizontalFilmAperture" );
	hfaPlug.getValue( h_apeture );
	
	MPlug vfaPlug = nodeFn.findPlug( "verticalFilmAperture" );
	vfaPlug.getValue( v_apeture );
	
	double fl;
	MPlug flPlug = nodeFn.findPlug( "focalLength" );
	flPlug.getValue( fl );
	
	double orthwidth;
	MPlug owPlug = nodeFn.findPlug( "orthographicWidth" );
	owPlug.getValue( orthwidth );
	
	bool orth;
	MPlug orthPlug = nodeFn.findPlug( "orthographic" );
	orthPlug.getValue( orth );

	double h_fov = h_apeture * 0.5 / ( fl * 0.03937 );
	double v_fov = v_apeture * 0.5 / ( fl * 0.03937 );
	
	float fright = far_clip * h_fov;
	float ftop = far_clip * v_fov;
	
	float nright = near_clip * h_fov;
	float ntop = near_clip * v_fov;
	
	if(orth) fright = ftop = nright = ntop = orthwidth/2.0;
	
	
	MPoint corner_a(fright,ftop,-far_clip);
	corner_a *= cmat;
	
	MPoint corner_b(-fright,ftop,-far_clip);
	corner_b *= cmat;
	
	MPoint corner_c(-fright,-ftop,-far_clip);
	corner_c *= cmat;
	
	MPoint corner_d(fright,-ftop,-far_clip);
	corner_d *= cmat;
	
	MPoint corner_e(nright,ntop,-near_clip);
	corner_e *= cmat;
	
	MPoint corner_f(-nright,ntop,-near_clip);
	corner_f *= cmat;
	
	MPoint corner_g(-nright,-ntop,-near_clip);
	corner_g *= cmat;
	
	MPoint corner_h(nright,-ntop,-near_clip);
	corner_h *= cmat;

	
	view.beginGL(); 
	
			
	glBegin( GL_LINES );

		glVertex3f( corner_a.x , corner_a.y, corner_a.z );
		glVertex3f( corner_e.x , corner_e.y, corner_e.z );
		
		glVertex3f( corner_b.x , corner_b.y, corner_b.z );
		glVertex3f( corner_f.x , corner_f.y, corner_f.z );
		
		glVertex3f( corner_c.x , corner_c.y, corner_c.z );
		glVertex3f( corner_g.x , corner_g.y, corner_g.z );
		
		glVertex3f( corner_d.x , corner_d.y, corner_d.z );
		glVertex3f( corner_h.x , corner_h.y, corner_h.z );
		
		glVertex3f( corner_a.x , corner_a.y, corner_a.z );
		glVertex3f( corner_b.x , corner_b.y, corner_b.z );
		
		glVertex3f( corner_b.x , corner_b.y, corner_b.z );
		glVertex3f( corner_c.x , corner_c.y, corner_c.z );
		
		glVertex3f( corner_c.x , corner_c.y, corner_c.z );
		glVertex3f( corner_d.x , corner_d.y, corner_d.z );
		
		glVertex3f( corner_d.x , corner_d.y, corner_d.z );
		glVertex3f( corner_a.x , corner_a.y, corner_a.z );
		
		glVertex3f( corner_e.x , corner_e.y, corner_e.z );
		glVertex3f( corner_f.x , corner_f.y, corner_f.z );
		
		glVertex3f( corner_f.x , corner_f.y, corner_f.z );
		glVertex3f( corner_g.x , corner_g.y, corner_g.z );
		
		glVertex3f( corner_g.x , corner_g.y, corner_g.z );
		glVertex3f( corner_h.x , corner_h.y, corner_h.z );
		
		glVertex3f( corner_h.x , corner_h.y, corner_h.z );
		glVertex3f( corner_e.x , corner_e.y, corner_e.z );
		

	glEnd();
	
	if ( ( style == M3dView::kFlatShaded  ||  style == M3dView::kGouraudShaded )  ) {

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		
		glClearDepth(1.0);
    	
	    	glEnable(GL_BLEND);
	    	
	    	glEnable(GL_DEPTH_TEST);
	    	glDepthFunc(GL_LEQUAL);
	    	
	    	glShadeModel(GL_SMOOTH);
	    
	    	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    	
	    	glDepthMask( GL_FALSE );
    	
		glColor4f(0.05f, 0.6f, 0.2f, 0.2f);

		glBegin( GL_QUADS );
			glVertex3f( corner_e.x , corner_e.y, corner_e.z );
			glVertex3f( corner_f.x , corner_f.y, corner_f.z );
			glVertex3f( corner_g.x , corner_g.y, corner_g.z );
			glVertex3f( corner_h.x , corner_h.y, corner_h.z );
			
			glVertex3f( corner_a.x , corner_a.y, corner_a.z );
			glVertex3f( corner_b.x , corner_b.y, corner_b.z );
			glVertex3f( corner_c.x , corner_c.y, corner_c.z );
			glVertex3f( corner_d.x , corner_d.y, corner_d.z );
			
			glVertex3f( corner_a.x , corner_a.y, corner_a.z );
			glVertex3f( corner_e.x , corner_e.y, corner_e.z );
			glVertex3f( corner_f.x , corner_f.y, corner_f.z );
			glVertex3f( corner_b.x , corner_b.y, corner_b.z );
			
			glVertex3f( corner_d.x , corner_d.y, corner_d.z );
			glVertex3f( corner_h.x , corner_h.y, corner_h.z );
			glVertex3f( corner_e.x , corner_e.y, corner_e.z );
			glVertex3f( corner_a.x , corner_a.y, corner_a.z );
			
			glVertex3f( corner_c.x , corner_c.y, corner_c.z );
			glVertex3f( corner_g.x , corner_g.y, corner_g.z );
			glVertex3f( corner_h.x , corner_h.y, corner_h.z );
			glVertex3f( corner_d.x , corner_d.y, corner_d.z );
			
			glVertex3f( corner_b.x , corner_b.y, corner_b.z );
			glVertex3f( corner_f.x , corner_f.y, corner_f.z );
			glVertex3f( corner_g.x , corner_g.y, corner_g.z );
			glVertex3f( corner_c.x , corner_c.y, corner_c.z );
			
		glEnd();
		
		glPopAttrib();
		
	}

	view.endGL();
	
}

bool cameraFrustumNode::isBounded() const
{ 
	return false;
}


void* cameraFrustumNode::creator()
{
	return new cameraFrustumNode();
}

MStatus cameraFrustumNode::initialize()
{ 
	MFnTypedAttribute matAttr;
	MFnNumericAttribute numAttr;
	MStatus			 stat;
	
	amatrix = matAttr.create( "cameraMatrix", "cm", MFnData::kMatrix );
	matAttr.setStorable(false);
	matAttr.setConnectable(true);
	addAttribute( amatrix );
	
	anear = numAttr.create( "nearClip", "nc", MFnNumericData::kDouble, 0.1 );
	numAttr.setStorable(true);
	numAttr.setConnectable(true);
	numAttr.setKeyable(true);
	addAttribute( anear );
	
	afar = numAttr.create( "farClip", "fc", MFnNumericData::kDouble, 1000.0 );
	numAttr.setStorable(true);
	numAttr.setConnectable(true);
	numAttr.setKeyable(true);
	addAttribute( afar );
	
	ahapeture = numAttr.create( "horizontalFilmAperture", "hfa", MFnNumericData::kDouble );
	numAttr.setStorable(false);
	numAttr.setConnectable(true);
	addAttribute( ahapeture );
	
	avapeture = numAttr.create( "verticalFilmAperture", "vfa", MFnNumericData::kDouble );
	numAttr.setStorable(false);
	numAttr.setConnectable(true);
	addAttribute( avapeture );
	
	afocallength = numAttr.create( "focalLength", "fl", MFnNumericData::kDouble );
	numAttr.setStorable(false);
	numAttr.setConnectable(true);
	addAttribute( afocallength );
	
	aorthographicwidth = numAttr.create( "orthographicWidth", "ow", MFnNumericData::kDouble );
	numAttr.setStorable(false);
	numAttr.setConnectable(true);
	addAttribute( aorthographicwidth );
	
	aorthographic = numAttr.create( "orthographic", "oth", MFnNumericData::kBoolean );
	numAttr.setStorable(true);
	addAttribute( aorthographic );
	
	return MS::kSuccess;
}

MStatus initializePlugin( MObject obj )
{ 
	MStatus   stat;
	MFnPlugin plugin( obj, "ZHANG JIAN - Free Downlaod", "3.0", "Any");

	stat = plugin.registerNode( "cameraFrustumNode", cameraFrustumNode::id, 
						 &cameraFrustumNode::creator, &cameraFrustumNode::initialize,
						 MPxNode::kLocatorNode );
	if (!stat) {
		stat.perror("registerNode");
		return stat;
	}
	return stat;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   stat;
	MFnPlugin plugin( obj );

	stat = plugin.deregisterNode( cameraFrustumNode::id );
	if (!stat) {
		stat.perror("deregisterNode");
		return stat;
	}

	return stat;
}
