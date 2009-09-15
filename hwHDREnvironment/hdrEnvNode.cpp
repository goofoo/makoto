#include <maya/MMatrix.h>
#include <maya/MVectorArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include "hdrEnvNode.h"
#include "../shared/zWorks.h"
#include "../shared/gBase.h"
#include "../shared/hdrtexture.h"
#include "../shared/zMath.h"
#include "../sh_lighting/zSphericalHarmonics.h"

MTypeId hdrEnvNode::id( 0x0005907 );

MObject	hdrEnvNode::aExposure;
MObject	hdrEnvNode::aSize;
MObject	hdrEnvNode::aHDRName;
MObject hdrEnvNode::aworldSpace;
MObject hdrEnvNode::acullFace;
MObject hdrEnvNode::aoutput;

#define IMG_W 64
#define IMG_H 32
#define IMG_WMINUSONE 63
#define IMG_HMINUSONE 31

struct HDRLight : public SHFunction
{
	HDRLight(const HDRtexture* _img): img( _img) {}
	float getFloat(const SHSample& s) const
	{
		return 128.0f;
	}
	
	XYZ getColor(const SHSample& s) const
	{
		float r, g, b;
		img->getRGBByVector(s.vec[0], s.vec[1], s.vec[2], r,g,b);
		
		return XYZ(r,g,b);
	}
	
	const HDRtexture* img;
};

void hdrEnvNode::loadHDR(MString& hdrname)
{
	if(!m_program) delete m_program;
	m_program = new hdrProgram();
	if(hdrname=="") return;

	if(f_hdr)
	{
		delete f_hdr;
		f_hdr = NULL;
	}
	
	f_hdr = new HDRtexture(hdrname.asChar());
	if(f_hdr->isValid()) 
	{
		gBase::genTexture(m_tex, GL_TEXTURE_RECTANGLE_ARB, f_hdr->getWidth(), f_hdr->getHeight(), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT, f_hdr->getTexturePointer());

//				gBase::genTexture(m_tex, GL_TEXTURE_RECTANGLE_NV, f_hdr->getWidth(), f_hdr->getHeight(), GL_FLOAT_RGBA16_NV, GL_RGBA, GL_FLOAT, f_hdr->getTexturePointer());
//
		m_width = f_hdr->getWidth();
		m_height = f_hdr->getHeight();
		
		float grid_x = (float)m_width/(float)IMG_WMINUSONE;
		float grid_y = (float)m_height/(float)IMG_HMINUSONE;
		
		for(int j=0; j<IMG_H; j++)
		{
			for(int i=0; i<IMG_W; i++)
			{
				m_pST[j*IMG_W+i].x = i*grid_x;
				m_pST[j*IMG_W+i].y = j*grid_y;
			}
		}
		MGlobal::displayInfo(MString("HDR texture loaded: ")+ hdrname);
	}
	else
	{
		MGlobal::displayWarning(MString("Failed to load: ")+ hdrname);
	}
}

hdrEnvNode::hdrEnvNode() : m_tex(0),f_hdr(0),m_program(0)
{
	m_width = m_height = 1;
	
	m_pPos = new XYZ[IMG_W*IMG_H];
	m_pST = new XY[IMG_W*IMG_H];
	
	for(int j=0; j<IMG_H; j++)
	{
		for(int i=0; i<IMG_W; i++)
		{
			float x = (float)i/(float)IMG_WMINUSONE;
			float y = (float)j/(float)IMG_HMINUSONE;
			float theta = 2.0f *acos(sqrt(1.0f - y));
			float phi = 2.0f *PIF* x;
			
			sph2vec(theta, phi, m_pPos[j*IMG_W+i].x, m_pPos[j*IMG_W+i].y, m_pPos[j*IMG_W+i].z);
			
		}
	}

	m_size = 100;
	m_exposure = 0.0f;
	f_sh = new sphericalHarmonics();
}

hdrEnvNode::~hdrEnvNode() 
{
	if(m_tex>0) glDeleteTextures(1, &m_tex );
	delete[] m_pPos;
	delete[] m_pST;
	delete f_sh;
	if(f_hdr) delete f_hdr;
}

MStatus hdrEnvNode::compute( const MPlug& plug, MDataBlock& data )
{ 
	MStatus status;
	if(plug == aoutput)
	{
		MMatrix mat = data.inputValue(aworldSpace).asMatrix();
		
		MVectorArray outv;
		outv.setLength(16);

		if(f_hdr)
		{
			if(f_hdr->isValid())
			{
				XYZ side((float)mat[0][0], (float)mat[0][1], (float)mat[0][2]);
				XYZ up((float)mat[1][0], (float)mat[1][1], (float)mat[1][2]);
				XYZ front((float)mat[2][0], (float)mat[2][1], (float)mat[2][2]);
				
				side.normalize();
				up.normalize();
				front.normalize();
				MATRIX44F space;
				space.setOrientations(side, up, front);
				space.inverse();
				f_hdr->setExposure(m_exposure);
				f_hdr->setSpace(space);
				
				HDRLight hl(f_hdr);
				f_sh->projectFunction(hl, hdrCoeff);
				
				for(unsigned i=0; i<16; i++) outv[i] = MVector(hdrCoeff[i].x, hdrCoeff[i].y, hdrCoeff[i].z);
			}
		}
		
		MDataHandle outh = data.outputValue(aoutput, &status);
		
		MFnVectorArrayData outd;
		outh.set(outd.create(outv));
	    
		data.setClean(plug);
	}
	return MS::kUnknownParameter;
}

void hdrEnvNode::draw( M3dView & view, const MDagPath & /*path*/, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	MObject thisNode = thisMObject();
	MPlug plug( thisNode, acullFace );
	int ireverse;
	plug.getValue( ireverse );

	if(!m_program) return;
	view.beginGL(); 

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable(GL_CULL_FACE);
	if(ireverse==1) glFrontFace(GL_CW);
	else glFrontFace(GL_CCW);

	m_program->voronoiBegin(m_tex, pow(2.0f, m_exposure));
		glBegin(GL_QUADS);
		int i1, j1;
		for(int j=0; j<IMG_HMINUSONE; j++)
		{
			j1 = j+1;
			for(int i=0; i<IMG_WMINUSONE; i++)
			{
				i1 = i+1;

				glTexCoord4f(m_pST[j1*IMG_W+i].x, m_pST[j1*IMG_W+i].y, 0, 0);
				glVertex3f(m_pPos[j1*IMG_W+i].x*m_size, m_pPos[j1*IMG_W+i].y*m_size, m_pPos[j1*IMG_W+i].z*m_size);
				
				glTexCoord4f(m_pST[j*IMG_W+i].x, m_pST[j*IMG_W+i].y, 0, 0);
				glVertex3f(m_pPos[j*IMG_W+i].x*m_size, m_pPos[j*IMG_W+i].y*m_size, m_pPos[j*IMG_W+i].z*m_size);
				
				glTexCoord4f(m_pST[j*IMG_W+i1].x, m_pST[j*IMG_W+i1].y, 0, 0);
				glVertex3f(m_pPos[j*IMG_W+i1].x*m_size, m_pPos[j*IMG_W+i1].y*m_size, m_pPos[j*IMG_W+i1].z*m_size);

				glTexCoord4f(m_pST[j1*IMG_W+i1].x, m_pST[j1*IMG_W+i1].y, 0, 0);
				glVertex3f(m_pPos[j1*IMG_W+i1].x*m_size, m_pPos[j1*IMG_W+i1].y*m_size, m_pPos[j1*IMG_W+i1].z*m_size);
			}
		}
		
		glEnd();
	m_program->voronoiEnd();
	glPopAttrib();
	view.endGL();
}

bool hdrEnvNode::isBounded() const
{ 
	return false;
}

MBoundingBox hdrEnvNode::boundingBox() const
{ 
	MPoint corner1( -1.-1,-1 );
	MPoint corner2( 1,1,1 );

	return MBoundingBox( corner1, corner2 );
}


void* hdrEnvNode::creator()
{
	return new hdrEnvNode();
}

MStatus hdrEnvNode::initialize()
{ 
	MStatus			 status;
	    MFnNumericAttribute nAttr; 
    MFnTypedAttribute tAttr;
	
	aExposure = nAttr.create("exposure", "exposure",
						  MFnNumericData::kFloat, 1.f, &status);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setDefault(1.f);
    nAttr.setSoftMin(-10.f);
    nAttr.setSoftMax(10.f);
	nAttr.setCached( true );
	nAttr.setInternal( true );
	
	aSize = nAttr.create("size", "size",
						  MFnNumericData::kFloat, 900.f, &status);
    nAttr.setStorable(true);
    nAttr.setKeyable(true);
    nAttr.setDefault(900.f);
    nAttr.setMin(1.f);
	nAttr.setCached( true );
	nAttr.setInternal( true );
	
	aHDRName = tAttr.create("hdrName", "hdrname",
	MFnData::kString, MObject::kNullObj, &status);

    tAttr.setStorable(true);
    tAttr.setKeyable(false);
	tAttr.setCached( true );
	tAttr.setInternal( true );
	
	zWorks::createMatrixAttr(aworldSpace, "worldSpace", "ws");
	
	zWorks::createVectorArrayAttr(aoutput, "output", "output");
	zWorks::createIntAttr(acullFace, "backSide", "bks", 1, 0);
	
	addAttribute(aSize);
	addAttribute(aExposure);
	addAttribute(aHDRName);
	addAttribute(aoutput);
	addAttribute(aworldSpace);
	addAttribute(acullFace);
	attributeAffects( aExposure, aoutput );
	attributeAffects( aworldSpace, aoutput );
	attributeAffects( acullFace, aoutput );
	
	return MS::kSuccess;
}

/* virtual */
bool 
hdrEnvNode::setInternalValueInContext( const MPlug& plug,
												  const MDataHandle& handle,
												  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == aHDRName)
	{
		handledAttribute = true;
		m_hdrname = (MString) handle.asString();
		loadHDR(m_hdrname);
	}

	else if (plug == aExposure)
	{
		handledAttribute = true;
		float val = handle.asFloat();
		if (val != m_exposure)
		{
			m_exposure = val;
			mAttributesChanged = true;
		}
	}
	else if (plug == aSize)
	{
		handledAttribute = true;
		float val = handle.asFloat();
		if (val != m_size)
		{
			m_size = val;
			mAttributesChanged = true;
		}
	}

	return handledAttribute;
}

/* virtual */
bool
hdrEnvNode::getInternalValueInContext( const MPlug& plug,
											  MDataHandle& handle,
											  MDGContext&)
{
	bool handledAttribute = false;
	if (plug == aExposure)
	{
		handledAttribute = true;
		handle.set( m_exposure );
	}
	else if (plug == aHDRName)
	{
		handledAttribute = true;
		handle.set( m_hdrname );
	}
	else if (plug == aSize)
	{
		handledAttribute = true;
		handle.set( m_size );
	}

	return handledAttribute;
}

//:~
