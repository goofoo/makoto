#include "rmcMesh.h"
#include "fishBone.h"

class CfishSkin
{
public:
	CfishSkin(void);
	~CfishSkin(void);
	void bind(CrmcMesh* mesh, CfishBone* bone);
	void getWeightsById(unsigned int id, int& bone0, float& weight0, int& bone1, float& weight1);

private:	
	unsigned char* m_id;
	float* m_weight;
};
