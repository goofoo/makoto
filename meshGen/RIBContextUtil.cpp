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
* RIBContextUtil.cpp:
*   	utilities for parsing arguments presented by the SetArgs method.
* $Revision: #1 $
*/


#include "RIBContextUtil.h"
#include <string.h>
#include <stdlib.h>

/*-----------------------------------------------------------*/
namespace RIBContextUtil {

int
GetIntValue(RtPointer p, RtInt *i)
{
    *i = *(RtInt *) p;
    return 0;
}

int
GetIntArray(RtToken tok, RtPointer vals, 
    	    	    size_t *len, RtInt **ia)
{
    int err = 0;
    
    // array lengths are embedded in the token:
    //	int[13] myParm
    *len = atol( 1 + strchr(tok, '[') );
    if(*len > 0)
    {
	*ia = new RtInt[*len];
	if(*ia)
	{
	    for(size_t j=0;j < *len;j++)
	    {
		(*ia)[j] = ((RtInt *)vals)[j];
	    }
	}
	else
	{
	    err = -1;
	}
    }
    return err;
}

int
GetFloatValue(RtPointer p, RtFloat *f)
{
    *f = *(RtFloat *) p;
    return 0;
}

int
GetFloatArray(RtToken tok, RtPointer vals, 
    	    	    size_t *len, RtFloat **fa)
{
    int err = 0;
    
    // array lengths are embedded in the token:
    //	float[13] myParm
    *len = atol( 1 + strchr(tok, '[') );
    if(*len > 0)
    {
	*fa = new RtFloat[*len];
	if(*fa)
	{
	    for(size_t j=0;j < *len;j++)
	    {
		(*fa)[j] = ((RtFloat *)vals)[j];
	    }
	}
	else
	{
	    err = -1;
	}
    }
    return err;
}

int
GetTripleValue(RtPointer p, RtFloat *f)
{
    memcpy( f, p, 3 * sizeof(RtFloat) );
    return 0;
}

int
GetMatrixValue(RtPointer p, RtMatrix *f)
{
    memcpy( f, p, sizeof(RtMatrix) );
    return 0;
}

int
GetTupleArray(RtToken tok, RtPointer vals,
    	    	    int tupleSize,
    	    	    size_t *len, RtFloat **fa)
{
    int err = 0;
    
    // array lengths are embedded in the token:
    //	color[13] myParm
    *len = atol( 1 + strchr(tok, '[') );
    if(*len > 0)
    {
	*fa = new RtFloat[*len * tupleSize];
	if(*fa)
	{
	    for(size_t j=0;j < *len*tupleSize;j++)
	    {
		(*fa)[j] = ((RtFloat *)vals)[j];
	    }
	}
	else
	{
	    err = -1;
	}
    }
    return err;
}

int
GetStringValue( RtPointer p, char **strVar )
{
    int err = 0;
    char *s  = *(char **) p;
    if( *strVar )
	delete *strVar;
    if( !s || !*s )
    {
	// set to the NULL string
    	*strVar = 0L;
    }
    else
    {
	*strVar = new char[strlen(s)+1];
	if(*strVar )
	{
	    strcpy(*strVar, s);
	}
	else
	    err = -1;
    }
    return err;
}

char *
CloneString( const char *src )
{
    if(!src)
    	return 0L;
    char *clone = new char [strlen(src) + 1];
    if(clone)
    {
    	strcpy(clone, src);
    }
    return clone;
}

int
GetStringArray( RtToken tok, RtPointer p, 
    	    	    	size_t *len, char ***strArrayVar )
{
    int err = 0;
    // array lengths are embedded in the token:
    //	string[13] myParm
    *len = atol( 1 + strchr(tok, '[') );
    if(*len > 0)
    {
    	*strArrayVar = new char *[ *len ];
	if(*strArrayVar)
	{
	    for(int i=0;i<*len;i++)
	    {
	    	(*strArrayVar)[i] = CloneString( ((char **)p)[i] );
	    }
	}
	else
	    err = -1;
    }
    else
    	err = -1;
    return err;
}

// end of namespace:
}
