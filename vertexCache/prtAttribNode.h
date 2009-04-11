#include <maya/MPxNode.h> 


 
class prtAttribNode : public MPxNode
{
public:
						prtAttribNode();
	virtual				~prtAttribNode(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:
	static  MObject		askipidir;
	static  MObject		askipscat;
	static  MObject		askipepid;
	static  MObject		aaslight;
	static  MObject		aklight;
	static  MObject		anoshadow;
	static  MObject		aasghost;
	static  MObject		output;        // The output value.
	static	MTypeId		id;
};