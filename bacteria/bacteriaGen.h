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
* bacteriaGen.h
*  	header for bacteriaGen - methods to obtain data during SetArgs
*   	from a myriad of RIB datatypes.
*
*   	you can subclass this to obtain access to various helper functions.
*
* $Revision: #1 $
*/

#ifndef _h_bacteriaGen
#define _h_bacteriaGen

#include "RIBGen.h"
#include "RIBContext.h"

class bacteriaGen : public RIBGen
{
public:
		    bacteriaGen();
		    ~bacteriaGen();

    virtual int	    SetArgs( RIBContext *c,
    	    	    	int n, RtToken tokens[], RtPointer vals[] );
    virtual void    Bound( RIBContext *c, RtBound b );
    virtual int	    GenRIB( RIBContext *c );

private:
	char* m_image_name;
	char* m_image_file1;
	char* m_image_file2;
	char* m_image_file3;
	char* m_image_file4;
	char* m_image_file5;
	float maxframe1, maxframe2, maxframe3, maxframe4, maxframe5;
	//RtFloat m_i_hdr_shadowed, m_i_hdr_interreflection, m_i_hdr_subsurfacescat, m_i_hdr_backscat;
	//RtFloat m_i_lightsrc_shadowed, m_i_lightsrc_interreflection, m_i_lightsrc_subsurfacescat, m_i_double_sided;
};


#endif
