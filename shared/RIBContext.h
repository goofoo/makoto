#ifndef _H_RIBContext
#define _H_RIBContext
/*-______________________________________________________________________
** 
** Copyright (c) 1996-2002 PIXAR.  All rights reserved.  This program or
** documentation contains proprietary confidential information and trade
** secrets of PIXAR.  Reverse engineering of object code is prohibited.
** Use of copyright notice is precautionary and does not imply
** publication.
** 
**		      RESTRICTED RIGHTS NOTICE
** 
** Use, duplication, or disclosure by the Government is subject to the 
** following restrictions:  For civilian agencies, subparagraphs (a) through
** (d) of the Commercial Computer Software--Restricted Rights clause at
** 52.227-19 of the FAR; and, for units of the Department of Defense, DoD
** Supplement to the FAR, clause 52.227-7013 (c)(1)(ii), Rights in
** Technical Data and Computer Software.
** 
** Pixar
** 1200 Park Ave
** Emeryville, CA  94608
** 
** ______________________________________________________________________
*/

#include <ri.h>
#include <vector>
#include <string>

#if PRMAN_VERSION <= 39
typedef RtVoid (*RtProcSubdivFunc)(RtPointer, RtFloat);
typedef RtVoid (*RtProcFreeFunc)(RtPointer);
#endif

/* vertion 5 added ResourceBegin/End and HSubdivs*/
/* version 6 added IfBegin and friends */
#define RIBCONTEXT_API 6

//
// here's the abstract class definition of the object that
// provides you with access to the current RIB Context. A subset of the
// RenderMan Interface is exposed in the interest of protecting clients
// from disrupting the internal state depended upon by the context
// manager (MTOR).
//
class RIBContextResult;
class RIBContext {
public:
		typedef enum
		{
		  rpInvalid, // during caching
		  rpTraverseOnly,
		  rpFinal,
		  rpShadow,
		  rpReflection,
		  rpEnvironment,
		  rpDepth,
		  rpReference,
		  rpArchive,
		  rpPhoton
		} RenderingPass;

		typedef enum
		{
		  reInfo = RIE_INFO,
		  reWarning = RIE_WARNING,
		  reError = RIE_ERROR,
		  reSevere = RIE_SEVERE
		} RenderingError;

		typedef enum
		{
		    kDefaultBasis,
		    kBezierBasis,
		    kBSplineBasis,
		    kCatmullRomBasis,
		    kHermiteBasis, 
		    kPowerBasis
		} BasisFunction;

		typedef enum
		{
		    kDefaultFilter,
		    kGaussianFilter,
		    kBoxFilter,
		    kTriangleFilter,
		    kCatmullRomFilter,
		    kSincFilter,
		    kDiskFilter,
		    kBesselFilter,
		    kSeparableCatmullRomFilter,
		    kBlackmanHarrisFilter,
		    kMitchellFilter
		} FilterFunction;

		typedef enum
		{
		    kDelayedReadArchive,
		    kRunProgram,
		    kDynamicLoad
		} ProceduralSubdivFunction;

		typedef enum
		{
		    kShutterOpenOnFrame=0,
		    kShutterCenterOnFrame,
		    kShutterCloseOnFrame
		} ShutterTiming;

		typedef enum
		{
		    kShutterMoving,
		    kShutterStationary
		} ShutterConfig;

		typedef enum
		{
		    kRib,
		    kImage,
		    kTmp,
		    kShader,
		    kTexture
		} OutDirCategory;
		
virtual     	  ~RIBContext() {};
		
virtual void      GetRenderingPass(RenderingPass *p,
			 RtBoolean *declarePhase) = 0;
virtual RtBoolean GetMotionBlur() = 0;
virtual int       GetFrame() = 0;
virtual void      GetShutter(RtFloat *open, RtFloat *close) = 0;
virtual const char * GetJobName() = 0;
virtual const char * GetObjName() = 0;
virtual void      GetCamBasis( RtMatrix m, int inverse = 0 ) = 0;
virtual void      ReportError( RenderingError e, const char *fmt, ... ) = 0;
virtual void      GetMotionInfo( RtFloat *shutterAngle,
			RtFloat *fps,
			RtBoolean *subframeMotion,
			RtBoolean *blurCamera=0L,
			ShutterTiming *shutterTiming=0L,
			ShutterConfig *shutterConfig=0L
			) = 0;
virtual const RIBContextResult *
		  ExecuteHostCmd( const char *cmd,
			std::string &errstr ) = 0;
virtual const RIBContextResult *
		  ExecuteHostCmd( const char *cmd,
			char** errstr ) = 0; // Caller must free errstr.
virtual RtBasis	* GetBasis( BasisFunction ) = 0;
virtual void	  SetDefaultFilterFunction( FilterFunction ) = 0;
virtual RtFilterFunc GetFilterFunction( FilterFunction ) = 0;
virtual RtFilterFunc GetFilterFunction( const char *nm ) = 0;
virtual RtProcSubdivFunc GetProcSubdivFunc( ProceduralSubdivFunction ) = 0;
virtual RtProcFreeFunc GetProcFreeFunc() = 0;
virtual const char *GetOutputDir(OutDirCategory) = 0;

public:
// These routines are exposed for the purposes of writing
// RIB archives, use with caution.
virtual RtVoid	    Begin( RtToken name ) = 0;
virtual RtVoid	    End() = 0;
virtual RtContextHandle GetContext() = 0;
virtual RtVoid	    Context( RtContextHandle ) = 0;

virtual RtVoid	    RIBBox(const RtString) = 0;
virtual RtToken	    Declare(char *, char *) = 0;
virtual RtVoid	    AttributeBegin() = 0;
virtual RtVoid	    AttributeEnd() = 0;
virtual RtVoid	    Color(RtColor) = 0;
virtual RtVoid	    Opacity(RtColor) = 0;
virtual RtVoid	    TextureCoordinates(RtFloat s1, RtFloat t1,
			RtFloat s2, RtFloat t2,
			RtFloat s3, RtFloat t3,
			RtFloat s4, RtFloat t4) = 0;
virtual RtLightHandle LightSource(RtToken name, ...) = 0;
virtual RtLightHandle LightSourceV(RtToken name,
			RtInt n, 
			RtToken tokens[], 
			RtPointer parms[]) = 0;
virtual RtLightHandle AreaLightSource(RtToken name, ...) = 0;
virtual RtLightHandle AreaLightSourceV(RtToken name,
			RtInt n, 
			RtToken tokens[], 
			RtPointer parms[]) = 0;
virtual RtVoid	    Illuminate(RtLightHandle light,
			RtBoolean onoff) = 0;
virtual RtVoid	    Surface(RtToken name, ...) = 0;
virtual RtVoid	    SurfaceV(RtToken name, RtInt n,
			RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Atmosphere(RtToken name, ...) = 0;
virtual RtVoid	    AtmosphereV(RtToken name, RtInt n, 
			RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Interior(RtToken name, ...) = 0;
virtual RtVoid	    InteriorV(RtToken name, RtInt n, 
			RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Exterior(RtToken name, ...) = 0;
virtual RtVoid	    ExteriorV(RtToken name, RtInt n, 
			RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    ShadingRate(RtFloat) = 0;
virtual RtVoid	    ShadingInterpolation(RtToken type) = 0;
virtual RtVoid	    Matte(RtBoolean onoff) = 0;
virtual RtVoid	    Bound(RtBound bound) = 0;
virtual RtVoid	    Detail(RtBound bound) = 0;
virtual RtVoid	    DetailRange(RtFloat minvis, RtFloat lowtran,
			RtFloat uptran, RtFloat maxvis) = 0;
virtual RtVoid	    RelativeDetail(RtFloat) = 0;
virtual RtVoid	    GeometricApproximation(RtToken type, 
			RtFloat value) = 0;
virtual RtVoid	    Orientation(RtToken orientation) = 0;
virtual RtVoid	    ReverseOrientation(void) = 0;
virtual RtVoid	    Sides(RtInt sides) = 0;
virtual RtVoid	    Identity(void) = 0;
virtual RtVoid	    Transform(RtMatrix transform) = 0;
virtual RtVoid	    ConcatTransform(RtMatrix transform) = 0;
virtual RtVoid	    Perspective(RtFloat fov) = 0;
virtual RtVoid	    Translate(RtFloat dx, RtFloat dy, RtFloat dz) = 0;
virtual RtVoid	    Rotate(RtFloat angle, RtFloat dx, RtFloat dy,
			RtFloat dz) = 0;
virtual RtVoid	    Scale(RtFloat sx, RtFloat sy, RtFloat sz) = 0;
virtual RtVoid	    Skew(RtFloat angle, RtFloat dx1, RtFloat dy1,
			RtFloat dz1, RtFloat dx2, 
			RtFloat dy2, RtFloat dz2) = 0;
virtual RtVoid	    Displacement(RtToken name, ...) = 0;
virtual RtVoid	    DisplacementV(RtToken name, RtInt n, 
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    CoordinateSystem(RtToken space) = 0;
virtual RtVoid	    ScopedCoordinateSystem(RtToken space) = 0;
virtual RtVoid	    CoordSysTransform(RtToken space) = 0;
virtual RtVoid	    TransformBegin(void) = 0;
virtual RtVoid	    TransformEnd(void) = 0;
virtual RtVoid	    Attribute(RtToken name, ...) = 0;
virtual RtVoid	    AttributeV(RtToken name, RtInt n, 
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    Polygon(RtInt nverts, ...) = 0;
virtual RtVoid	    PolygonV(RtInt nverts, RtInt n, 
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    GeneralPolygon(RtInt nloops, 
			RtInt nverts[], ...) = 0;
virtual RtVoid	    GeneralPolygonV(RtInt nloops,
			RtInt nverts[],
			RtInt n,
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    PointsPolygons(RtInt npolys,
			RtInt nverts[], RtInt verts[], ...) = 0;
virtual RtVoid	    PointsPolygonsV(RtInt npolys,
			RtInt nverts[], RtInt verts[],
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    PointsGeneralPolygons(RtInt npolys,
			RtInt nloops[], RtInt nverts[],
			RtInt verts[], ...) = 0;
virtual RtVoid	    PointsGeneralPolygonsV(RtInt npolys,
			RtInt nloops[], RtInt nverts[],
			RtInt verts[], RtInt n,
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    Basis(RtBasis ubasis, RtInt ustep,
			RtBasis vbasis, RtInt vstep) = 0;
virtual RtVoid	    Patch(RtToken type, ...) = 0;
virtual RtVoid	    PatchV(RtToken type, RtInt n,
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    PatchMesh(RtToken type,
			RtInt nu, RtToken uwrap,
			RtInt nv, RtToken vwrap, ...) = 0;
virtual RtVoid	    PatchMeshV(RtToken type,
			RtInt nu, RtToken uwrap,
			RtInt nv, RtToken vwrap,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    NuPatch(RtInt nu,
			RtInt uorder, RtFloat uknot[],
			RtFloat umin, RtFloat umax,
			RtInt nv, RtInt vorder,
			RtFloat vknot[], RtFloat vmin,
			RtFloat vmax, ...) = 0;
virtual RtVoid	    NuPatchV(RtInt nu,
			RtInt uorder, RtFloat uknot[],
			RtFloat umin, RtFloat umax,
			RtInt nv, RtInt vorder, RtFloat vknot[],
			RtFloat vmin, RtFloat vmax,
			RtInt n,
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    TrimCurve(RtInt nloops, RtInt ncurves[],
			RtInt order[], RtFloat knot[],
			RtFloat min[], RtFloat max[], RtInt n[],
			RtFloat u[], RtFloat v[], RtFloat w[]) = 0;
virtual RtVoid	    Sphere(RtFloat radius, RtFloat zmin,
			RtFloat zmax, RtFloat tmax, ...) = 0;
virtual RtVoid	    SphereV(RtFloat radius, RtFloat zmin,
			RtFloat zmax, RtFloat tmax,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Cone(RtFloat height, RtFloat radius,
			RtFloat tmax, ...) = 0;
virtual RtVoid	    ConeV(RtFloat height, RtFloat radius,
			RtFloat tmax,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Cylinder(RtFloat radius, RtFloat zmin,
			RtFloat zmax, RtFloat tmax, ...) = 0;
virtual RtVoid	    CylinderV(RtFloat radius, RtFloat zmin,
			RtFloat zmax, RtFloat tmax,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Hyperboloid(RtPoint point1, RtPoint point2,
			RtFloat tmax, ...) = 0;
virtual RtVoid	    HyperboloidV(RtPoint point1, RtPoint point2,
			RtFloat tmax,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Paraboloid(RtFloat rmax, RtFloat zmin,
			RtFloat zmax, RtFloat tmax, ...) = 0;
virtual RtVoid	    ParaboloidV(RtFloat rmax, RtFloat zmin,
			RtFloat zmax, RtFloat tmax,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Disk(RtFloat height, RtFloat radius,
			RtFloat tmax, ...) = 0;
virtual RtVoid	    DiskV(RtFloat height, RtFloat radius,
			RtFloat tmax,
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Torus(RtFloat majrad, RtFloat minrad,
			RtFloat phimin, RtFloat phimax,
			RtFloat tmax, ...) = 0;
virtual RtVoid	    TorusV(RtFloat majrad, RtFloat minrad,
			RtFloat phimin, RtFloat phimax,
			RtFloat tmax, RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Geometry(RtToken type, ...) = 0;
virtual RtVoid	    GeometryV(RtToken type, RtInt n,
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    SolidBegin(RtToken operation) = 0;
virtual RtVoid	    SolidEnd(void) = 0;
virtual RtVoid	    MotionBegin(RtInt n, ...) = 0;
virtual RtVoid	    MotionBeginV(RtInt n, RtFloat times[]) = 0;
virtual RtVoid	    MotionEnd(void) = 0;
virtual RtVoid	    ArchiveRecord(RtToken type,
			char *format, ...) = 0;
virtual RtVoid	    ReadArchive(RtToken name,
			RtVoid (*callback)(RtToken,char*,...),
			...) = 0;
virtual RtVoid	    ReadArchiveV(RtToken name,
			RtVoid (*callback)(RtToken,char*,...),
			RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    Procedural(RtPointer data, RtBound bound,
			RtVoid (*subdivfunc)(RtPointer, RtFloat),
			RtVoid (*freefunc)(RtPointer)) = 0;
virtual RtVoid	    Curves(RtToken type, RtInt ncurves,
			RtInt nvertices[], RtToken wrap, ...) = 0;
virtual RtVoid	    CurvesV(RtToken type, RtInt ncurves,
			RtInt nvertices[], RtToken wrap,
			RtInt n, RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    Points(RtInt nverts,...) = 0;
virtual RtVoid	    PointsV(RtInt nverts, RtInt nargs, 
			RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    SubdivisionMesh(RtToken scheme, RtInt nf, 
    	    	    	RtInt nverts[], RtInt verts[],
			RtInt nt, RtToken tags[], 
    	    	    	RtInt numargs[], RtInt intargs[],
			RtFloat floatargs[], ...) = 0;
virtual RtVoid	    SubdivisionMeshV(RtToken scheme, RtInt nf, 
    	    	    	RtInt nverts[], RtInt verts[],
			RtInt nt, RtToken tags[], RtInt numargs[],
			RtInt intargs[],
			RtFloat floatargs[], RtInt n, RtToken tokens[],
			RtPointer parms[]) = 0;
virtual RtVoid	    HierarchicalSubdivisionMesh(RtToken scheme, RtInt nf, 
    	    	    	RtInt nverts[], RtInt verts[],
			RtInt ntags, RtToken tags[], 
    	    	    	RtInt numargs[], RtInt intargs[],
			RtFloat floatargs[], RtToken stringargs[], ...) = 0;
virtual RtVoid	    HierarchicalSubdivisionMeshV(RtToken scheme, RtInt nf, 
    	    	    	RtInt nverts[], RtInt verts[],
			RtInt nt, RtToken tags[], 
			RtInt numargs[], RtInt intargs[], 
			RtFloat floatargs[], RtToken stringargs[], 
			RtInt n, RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid	    Blobby(RtInt, RtInt, RtInt [], 
    	    	    	RtInt, RtFloat [],
			RtInt, RtToken [], ...) = 0;
virtual RtVoid	    BlobbyV(RtInt, RtInt, RtInt [], 
    	    	    	RtInt, RtFloat [],
			RtInt, RtToken [],
			RtInt, RtToken [], RtPointer []) = 0;

virtual RtVoid	    ClippingPlane( RtFloat x, RtFloat y, RtFloat z,
			RtFloat nx, RtFloat ny, RtFloat nz) = 0;
		    // handle: "foo", type: "attributes",
		    //	"operation" "load|save"
		    //	"subset" "all|shading"
virtual RtVoid	    Resource(RtToken handle, RtToken type, ...) = 0;
virtual RtVoid      ResourceV(RtToken handle, RtToken type,
	                RtInt n, RtToken tokens[], RtPointer parms[]) = 0;
virtual RtVoid      ResourceBegin() = 0;
virtual RtVoid	    ResourceEnd() = 0;
virtual RtVoid	    System(char *) = 0;
virtual RtVoid	    IfBegin(char *) = 0;
virtual RtVoid	    Else() = 0;
virtual RtVoid	    ElseIf(char *) = 0;
virtual RtVoid	    IfEnd() = 0;

};

// Here's a class which encapsulates data returned from
// the host application.
typedef RtFloat	RtHPoint[4];
class RIBContextResult
{
public:
		    enum Type
		    {
			kInvalid,
			kInt,
			kIntArray,
			kFloat,
			kFloatArray,
			kString,
			kStringArray,
			kPoint,
			kPointArray,
			kHPoint,
			kHPointArray,
			kMatrix
		    };
    virtual Type    ResultType() const = 0;

    virtual int     GetResult( RtInt &, std::string *err=NULL ) const = 0;
    virtual int	    GetResult( std::vector<RtInt> &,
    	    	    	std::string *err=NULL ) const = 0;
    virtual int	    GetResult( RtFloat &, std::string *err=NULL ) const = 0;
    virtual int	    GetResult( std::vector<RtFloat> &,
    	    	    	std::string *err=NULL) const = 0;
    virtual int	    GetResult( std::string &, std::string *err=NULL ) const = 0;
    virtual int	    GetResult( std::vector<std::string> &,
    	    	    	std::string *err=NULL ) const = 0;
    virtual int	    GetResult( RtPoint &, std::string *err=NULL ) const = 0;
    virtual int     GetResult( std::vector< std::vector<RtFloat> > &,
    	    	    	std::string *err=NULL ) const = 0;
    virtual int	    GetResult( RtMatrix &, std::string *err=NULL ) const = 0;

    // The following methods allocate memory for strings and arrays 
    // without predefined length.  The caller must delete [].
    virtual int     GetResult( RtInt*, 
				char** err=NULL ) const = 0;
    virtual int	    GetResult( RtInt**,
				unsigned *len,
				char** err=NULL ) const = 0;
    virtual int	    GetResult( RtFloat*, 
				char** err=NULL ) const = 0;
    virtual int	    GetResult( RtFloat**,
				unsigned *len,
				char** err=NULL ) const = 0;
    virtual int	    GetResult( char**,
				char** err=NULL ) const = 0;
    virtual int	    GetResult( char***,
				unsigned *len,
				char** err=NULL ) const = 0;
    virtual int	    GetResult( RtPoint*, 
				char** err=NULL ) const = 0;
    virtual int	    GetResult( RtMatrix*, 
				char** err=NULL ) const = 0;
    
};

#endif
