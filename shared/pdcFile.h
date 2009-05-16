#include <maya/MGlobal.h>
#include <maya/MVectorArray.h>

struct pdcHeader
{
char format[4];

	int formatVersion;

	int byteOrder;

	int extra1;

	int extra2;

	int numParticles;

	int numAttributes;
};

class pdcFile
{
public:
	pdcFile();
	~pdcFile(void);
	
	int load(const char* filename);
	int readPositions(MVectorArray& positions, MVectorArray& velocities, MVectorArray& ups, MVectorArray& fronts, MDoubleArray& offsets, MDoubleArray& amplitudes, MDoubleArray& bends, MDoubleArray& scales, MDoubleArray& masses);
	int readPositionAndVelocity(MVectorArray& positions, MVectorArray& velocities);
	int getParticleCount() {return m_num_of_ptc;}
	

private:
	char* data;
	pdcHeader* pheader;
	int m_num_of_ptc;
};
