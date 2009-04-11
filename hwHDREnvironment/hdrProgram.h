/*
 *  hdrProgram.h
 *  furOccGen
 *
 *  Created by zhang on 07-12-24.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "../shared/FnCg.h"

class hdrProgram : public FnCg
{
public :
	hdrProgram() {init();}
	void init();
	void voronoiBegin(GLuint tex, float exposure);
	void voronoiEnd();
	
private :
	CGprogram m_vert_voronoi, m_frag_voronoi;
	CGparameter m_voronoi_modelView, m_voronoi_projection;
	CGparameter m_voronoi_tex, m_voronoi_exposure;
};
//:~
