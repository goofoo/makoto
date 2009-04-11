#ifndef _H_RIBGen
#define _H_RIBGen
/*-______________________________________________________________________
** 
** Copyright (c) 1996-2002 PIXAR.  All rights reserved.  This program or
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
** 1200 Park Avenue
** Emeryville, CA  94608
** 
** ______________________________________________________________________
*/

#include "ri.h"
#include "RIBContext.h"

#define RIBGEN_API 4
#define RIBGEN_VERSION ((RIBGEN_API << 8) | RIBCONTEXT_API)
class RIBGen;

// here are the two entrypoints RIB Generators must implement
// and export.  Note that these functions hide construction
// and destruction of your subclass of RIBGen. 
extern "C" RIBGen    *RIBGenCreate();
extern "C" void       RIBGenDestroy( RIBGen * );


// here's the class definition of RIBGen
// RIB Generators must be a subclass of this class.

class RIBGen
{
public:
    // Arguments from the UI are passed in as name/value pairs as
    // in the RenderMan Interface.  We assume that type information
    // is known to the plugin.
    virtual int SetArgs( RIBContext *,
    	int n, RtToken tokens[], RtPointer values[] ) = 0;

    // Optionally cache internal motion-blur state at FrameOpen.
    //  This call allows certain types of RIB Generator to
    //  handle deforming motion blur.  Ostensibly the cached
    //  state will be used to compose the RIB representation
    //  of your deforming object. 
    virtual int CacheFrameOpenState( RIBContext * ) { return 0; }

    // Return a bounding box for your procedural geometry.
    // This may be used to cull your geometry if it's offscreen.
    virtual void Bound( RIBContext *, RtBound b ) = 0;
    
    // Make calls through RenderMan Interface using information
    // and entrypoints provided in the RIBContext object.
    virtual int GenRIB( RIBContext * ) = 0;

    virtual	~RIBGen() {}
    virtual unsigned GetVersion() { return RIBGEN_VERSION; }
};

#endif
