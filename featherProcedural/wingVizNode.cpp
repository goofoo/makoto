/*
 *  wingVizNode.cpp
 *  
 *
 *  Created by jian zhang on 6/15/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "wingVizNode.h"
#include "../shared/zWorks.h"
#include "../shared/zGlobal.h"
#include "FXMLTriangleMap.h"

MTypeId wingVizNode::id( 0x00025358 );

MObject wingVizNode::acachename;
MObject wingVizNode::aframe;
MObject wingVizNode::aminframe;
MObject wingVizNode::amaxframe;
MObject wingVizNode::aratio;
MObject wingVizNode::agrowth;
MObject wingVizNode::acollision;
MObject wingVizNode::atime;
MObject wingVizNode::aoutval;
MObject wingVizNode::awind;
MObject wingVizNode::outMesh;
MObject wingVizNode::ashaft0;
MObject wingVizNode::ashaft1;
MObject wingVizNode::awidth0;
MObject wingVizNode::awidth1;
MObject wingVizNode::aedge0;
MObject wingVizNode::aedge1;
MObject wingVizNode::atwist0;
MObject wingVizNode::atwist1;
MObject wingVizNode::areverse;
MObject wingVizNode::astep;
MObject wingVizNode::anoisize;
MObject wingVizNode::anoifreq;
MObject wingVizNode::anoiseed;
MObject wingVizNode::aouttexcoordoffsetpp;

wingVizNode::wingVizNode():m_scale(1.0f),m_wind(1.f)
{
       m_base = new VWing();
}

wingVizNode::~wingVizNode()
{
       delete m_base;
}

MStatus wingVizNode::compute( const MPlug& plug, MDataBlock& data )
{ 
       MStatus status;
       if(plug == aoutval)
       {
               //double dtime = data.inputValue( aframe ).asDouble();
               MTime currentTime = data.inputValue(atime, &status).asTime();
               float ns = data.inputValue(anoisize).asFloat();
               float nf = data.inputValue(anoifreq).asFloat();
               int nd = data.inputValue(anoiseed).asInt();
               
               MObject ogrow = data.inputValue(agrowth).asNurbsSurfaceTransformed();
               //MArrayDataHandle hArray = data.inputArrayValue(agrowth);
               
               //int n_obs = hArray.elementCount();
               //MObjectArray obslist;
               //for(int iter=0; iter<n_obs; iter++)
               //{
               //      obslist.append(hArray.inputValue().asNurbsSurfaceTransformed()
               //      hArray.next();
               //}
               //if(!ogrow.isNull()) m_base->setGrow(ogrow);
               m_base->setNoise(ns, nf, nd);
               m_base->setTime(currentTime.value());
               m_base->setNurbs(ogrow);
               //m_base->setWind(m_wind);
               
               /*int frame = (int)currentTime.value();
               i_show_v = 0;
               if(data.inputValue(amaxframe, &status).asInt() == 1)
               {
                       i_show_v = 1;
                       char file_name[512];
                       sprintf( file_name, "%s/%s.%d.xvn", m_cachename.asChar(), MFnD
                       int start = data.inputValue(aminframe, &status).asInt();
                       if(start == frame) 
                       {
                               m_frame_pre = start;
//m_base->init();
                               m_base->save(file_name);
                       }
                       else if(m_frame_pre+1 == frame)
                       {
                               m_frame_pre++;
                               //m_base->update();
                               m_base->save(file_name);
                       }
               }*/
       }
       
       if(plug== outMesh)
       {
               MObject ogrow = data.inputValue(agrowth).asNurbsSurfaceTransformed();
               m_base->setNurbs(ogrow);
               
               float w0 = data.inputValue(ashaft0).asFloat();
               float w1 = data.inputValue(ashaft1).asFloat();
               
               float fw0 = data.inputValue(awidth0).asFloat();
               float fw1 = data.inputValue(awidth1).asFloat();
               
               float e0 = data.inputValue(aedge0).asFloat();
               float e1 = data.inputValue(aedge1).asFloat();
               
               float t0 = data.inputValue(atwist0).asFloat();
               float t1 = data.inputValue(atwist1).asFloat();
               
               int r = data.inputValue(areverse).asInt();
               int s = data.inputValue(astep).asInt();
               m_base->setReverse(r);
               m_base->setFeatherWidth(fw0, fw1);
               m_base->setFeatherEdge(e0, e1);
               m_base->setShaftWidth(w0, w1);
               m_base->setFeatherTwist(t0, t1);
               m_base->setStep(s);

               MObject outMeshData = m_base->createFeather();
               
               //zDisplayFloat3(m_base->getSeed(), m_base->getSeed1(), m_base->getFra
               
               MDataHandle meshh = data.outputValue(outMesh, &status);
			   meshh.set(outMeshData);
           
               data.setClean(plug);
       }
       
       if(plug== aouttexcoordoffsetpp)
       {
               MDoubleArray coeff_array;
               
               m_base->getTexcoordOffset(coeff_array);
               
               MFnDoubleArrayData coeffFn;
               MObject coeffOutputData = coeffFn.create( coeff_array, &status );
               MDataHandle coeffh = data.outputValue( plug, &status);
               coeffh.set(coeffOutputData);
           
               data.setClean(plug);
       }
       return MS::kUnknownParameter;
}

void wingVizNode::draw( M3dView & view, const MDagPath & /*path*/, 
                                                        M3dView::DisplayStyle style,
                                                        M3dView::DisplayStatus status
{ 
       view.beginGL(); 

       //glPushAttrib( GL_POLYGON_BIT );
       //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
       //glFrontFace(GL_CW);
       //glEnable(GL_CULL_FACE);
       //if(i_show_v) m_base->drawVelocity();
       m_base->draw();
       //glPopAttrib();
       view.endGL();
}

bool wingVizNode::isBounded() const
{ 
       return true;
}

MBoundingBox wingVizNode::boundingBox() const