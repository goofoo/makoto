#ifndef __ZJ_FLUID_DESC_DATA_INCLUDED__
#define __ZJ_FLUID_DESC_DATA_INCLUDED__

#include <maya/MPxData.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>

struct fluidDesc
{
	float gridSize, buoyancy, swirl, velocity, temperatureLoss, temperature, wind_x, wind_z, conserveDensity;
	int enable, save_cache, resolution_x, resolution_y, resolution_z, sho_tex;
};

class zjFluidDescData : public MPxData
{
public:
						zjFluidDescData() {}
    virtual					~zjFluidDescData() {}

    virtual MStatus         readASCII( const MArgList&, unsigned& lastElement );
    virtual MStatus         readBinary( istream& in, unsigned length );
    virtual MStatus         writeASCII( ostream& out );
    virtual MStatus         writeBinary( ostream& out );

	virtual void			copy( const MPxData& );
	MTypeId                 typeId() const; 
	MString                 name() const;

    	fluidDesc* getDesc() const { return _pDesc; }
    	void setDesc(fluidDesc* pDesc) { _pDesc = pDesc; }


	static const MString	typeName;
    static const MTypeId    id;
	static void*            creator();

private:
    fluidDesc* _pDesc; 
};
#endif