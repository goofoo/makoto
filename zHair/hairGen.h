/*
** Copyright (c) 1999 PIXAR.  All rights reserved.  This program or
** documentation contains proprietary confidential information and trade
** secrets of PIXAR.  Reverse engineering of object code is prohibited.
** Use of copyright notice is precautionary and does not imply
** publication.
**
**                      RESTRICTED RIGHTS NOTICE
**
** Use, duplication, or disclosure by the Government is subject to the
** following restrictions:  For civilian agencies, subparagraphs (a) through
** (d) of the Commercial Computer Software--Restricted Rights clause at
** 52.227-19 of the FAR; and, for units of the Department of Defense, DoD
** Supplement to the FAR, clause 52.227-7013 (c)(1)(ii), Rights in
** Technical Data and Computer Software.
**
** Pixar
** 1001 West Cutting Blvd.
** Richmond, CA  94804
**
** ----------------------------------------------------------------------------
*
* hairGen.h
*  	header for hairGen - methods to obtain data during SetArgs
*   	from a myriad of RIB datatypes.
*
*   	you can subclass this to obtain access to various helper functions.
*
* $Revision: #1 $
*/

#ifndef _h_hairGen
#define _h_hairGen

#include "RIBGen.h"
#include "RIBContext.h"

class hairGen : public RIBGen
{
public:
		    hairGen();
		    ~hairGen();

    virtual int	    SetArgs( RIBContext *c,
    	    	    	int n, RtToken tokens[], RtPointer vals[] );
    virtual void    Bound( RIBContext *c, RtBound b );
    virtual int	    GenRIB( RIBContext *c );

private:
	float ndice, rootwidth, tipwidth, dof_min, dof_max, fov, orthow;
	char* m_depth_file;
};


#endif
