#include "../shared/RIBGen.h"
#include "../shared/RIBContext.h"
#include "../shared/RIBContextUtil.h"

class fishGen : public RIBGen
{
public:
		    fishGen();
		    ~fishGen();

    virtual int	    SetArgs( RIBContext *c,
    	    	    	int n, RtToken tokens[], RtPointer vals[] );
    virtual void    Bound( RIBContext *c, RtBound b );
    virtual int	    GenRIB( RIBContext *c );

private: // see .sli file for details on variables

    
    RtFloat m_xmin, m_xmax, m_ymin, m_ymax, m_zmin, m_zmax, m_k_flap, m_k_bend, m_k_oscillate, m_length, m_frequency, m_displacementBound, m_bonecount;
    
	char* m_project_path;
    char* m_cache_body_0;
    char* m_cache_body_1;
    char* m_cache_body_2;
    char* m_cache_body_3;
    char* m_cache_teeth_top_0;
    char* m_cache_teeth_top_1;
    char* m_cache_teeth_top_2;
    char* m_cache_teeth_top_3;
    char* m_cache_teeth_bottom_0;
    char* m_cache_teeth_bottom_1;
    char* m_cache_teeth_bottom_2;
    char* m_cache_teeth_bottom_3;
    char* m_cache_eye_0;
    char* m_cache_eye_1;
    char* m_cache_eye_2;
    char* m_cache_eye_3;
    
    char* m_body_surf;
    char* m_body_disp;
    char* m_teeth_surf;
    char* m_teeth_disp;
    char* m_eye_surf;
    char* m_eye_disp;
    
    char hostname[64];
    
    void setupShaders(RIBContext *c, const char* surface, const char* displacement);
};