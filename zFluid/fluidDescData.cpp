#include "fluidDescData.h"

void* zjFluidDescData::creator()
{
    return new zjFluidDescData;
}


void zjFluidDescData::copy ( const MPxData& other )
{
	_pDesc = ((const zjFluidDescData&)other)._pDesc;
}

MTypeId zjFluidDescData::typeId() const
{
	return zjFluidDescData::id;
}

MString zjFluidDescData::name() const
{ 
	return zjFluidDescData::typeName;
}

MStatus zjFluidDescData::readASCII(  const MArgList& args,
                                unsigned& lastParsedElement )
{
    return MS::kSuccess;
}

MStatus zjFluidDescData::writeASCII( ostream& out )
{
    //out << fValue << " ";
    return MS::kSuccess;
}

MStatus zjFluidDescData::readBinary( istream& in, unsigned )
{
    //in.read( (char*) &fValue, sizeof( fValue ));
    //return in.fail() ? MS::kFailure : MS::kSuccess;
    return MS::kSuccess;
}

MStatus zjFluidDescData::writeBinary( ostream& out )
{
    //out.write( (char*) &fValue, sizeof( fValue));
    //return out.fail() ? MS::kFailure : MS::kSuccess;
    return MS::kSuccess;;
}

//
// this is the unique type id used to identify the new user-defined type.
//
const MTypeId zjFluidDescData::id( 0x0004138c );
const MString zjFluidDescData::typeName( "fluidDescData" );
