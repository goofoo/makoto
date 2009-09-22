#ifndef _H_RIBContextUtil
#define _H_RIBContextUtil

//
// $Revision: #1 $
//
#include <string.h>
#include <stdlib.h>
#include "RIBContext.h"

namespace RIBContextUtil
{
    int	    GetIntValue( RtPointer p, RtInt * );
    int	    GetIntArray( RtToken t, RtPointer p, 
    	    	    	    size_t *len, RtInt ** );
    int	    GetFloatValue( RtPointer p, RtFloat * );
    int	    GetFloatArray( RtToken t, RtPointer p, 
    	    	    	    size_t *len, RtFloat ** );
    int	    GetTripleValue( RtPointer p, RtFloat * );
    int	    GetMatrixValue( RtPointer p, RtMatrix * );
    int	    GetTupleArray( RtToken t, RtPointer p, 
    	    	    	    int tupleSize, /* float,color,..:3, matrix: 16 */
    	    	    	    size_t *len, RtFloat ** );
    int	    GetStringValue( RtPointer p, char ** );
    int	    GetStringArray( RtToken t, RtPointer p, 
    	    	    	    size_t *len, char *** );
    char *  CloneString( const char *src );

}


#endif
