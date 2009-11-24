#include "fluidProgram.h"

static const char *programSource =
" half EPSILON = 2.4414e-4; \n"
"\n"
"half3 packToWorld2(half2 coord, half4 dimensions)"
"{"
"	half col = floor(coord.x/dimensions.x);"
"	half row = floor(coord.y/dimensions.y);"
"	half3 pw = half3(floor(coord.x)-col*dimensions.x+0.5, floor(coord.y)-row*dimensions.y+0.5, row*dimensions.w + col+0.5);"
"	return pw;"
"}"
"\n"
"half3 packToWorld(half2 coord)"
"{"
"	half col = floor(coord.x/64);"
"	half row = floor(coord.y/64);"
"	half3 pw = half3(floor(coord.x)-col*64+0.5, floor(coord.y)-row*64+0.5, row*8 + col+0.5);"
"	return pw;"
"}"
"\n"
"half2 flatTo2D2(half3 world, half4 dimensions)"
"{"
"	half x = clamp(world.x,0,dimensions.x);"
"	half y = clamp(world.y,0,dimensions.y);"
"	half z = clamp(world.z,0,dimensions.z);"
"	int row = z/dimensions.w;"
"	int col = z-row*dimensions.w;"
"	return half2(col*dimensions.x+x, row*dimensions.y+y);"
"}"
"\n"
"half4 h4trilerp2(samplerRECT x, half3 pos, half4 dimensions)"
"{"
"	half3 p = floor(pos-0.5)+0.5;"
"	half4 v000 = h4texRECT(x, flatTo2D2(p, dimensions));"
"	half4 v100 = h4texRECT(x, flatTo2D2(p+half3(1,0,0), dimensions));"
"	half4 v010 = h4texRECT(x, flatTo2D2(p+half3(0,1,0), dimensions));"
"	half4 v001 = h4texRECT(x, flatTo2D2(p+half3(0,0,1), dimensions));"
"	half4 v101 = h4texRECT(x, flatTo2D2(p+half3(1,0,1), dimensions));"
"	half4 v011 = h4texRECT(x, flatTo2D2(p+half3(0,1,1), dimensions));"
"	half4 v110 = h4texRECT(x, flatTo2D2(p+half3(1,1,0), dimensions));"
"	half4 v111 = h4texRECT(x, flatTo2D2(p+half3(1,1,1), dimensions));"
"	half3 beta = pos-p;"
"	return v000*(1-beta.x)*(1-beta.y)*(1-beta.z) + v100*beta.x*(1-beta.y)*(1-beta.z) + v010*(1-beta.x)*beta.y*(1-beta.z) + v001*(1-beta.x)*(1-beta.y)*beta.z + v101*beta.x*(1-beta.y)*beta.z + v011*(1-beta.x)*beta.y*beta.z + v110*beta.x*beta.y*(1-beta.z) + v111*beta.x*beta.y*beta.z;"
"}"
"\n"
"void h1gridneighbourst(samplerRECT x, half2 st, half4 minmax, out half left, out half right, out half bottom, out half top, out half back, out half front)"
"{"
"	left = h1texRECT(x, st - half2(1,0));"
"	right = h1texRECT(x, st + half2(1,0));"
"	bottom = h1texRECT(x, st - half2(0,1));"
"	top = h1texRECT(x, st + half2(0,1));"
"	back = h1texRECT(x, st + minmax.xy);"
"	front = h1texRECT(x, st + minmax.zw);"
"}"
"\n"
"void h4gridneighbourst(samplerRECT x, half2 st, half4 minmax, out half4 left, out half4 right, out half4 bottom, out half4 top, out half4 back, out half4 front)"
"{"
"	left = h4texRECT(x, st - half2(1,0));"
"	right = h4texRECT(x, st + half2(1,0));"
"	bottom = h4texRECT(x, st - half2(0,1));"
"	top = h4texRECT(x, st + half2(0,1));"
"	back = h4texRECT(x, st + minmax.xy);"
"	front = h4texRECT(x, st + minmax.zw);"
"}"
"\n"
"void h4gridneighbours(samplerRECT x, half2 st, half4 zminmax, out half4 left, out half4 right, out half4 bottom, out half4 top, out half4 back, out half4 front)"
"{"
"	left = h4texRECT(x, st - half2(1,0));"
"	right = h4texRECT(x, st + half2(1,0));"
"	bottom = h4texRECT(x, st - half2(0,1));"
"	top = h4texRECT(x, st + half2(0,1));"
"	back = h4texRECT(x, st + zminmax.xy);"
"	front = h4texRECT(x, st + zminmax.zw);"
"}"
"\n"
"void h1gridneighbours(samplerRECT x, half2 st, half4 zminmax, out half left, out half right, out half bottom, out half top, out half back, out half front)"
"{"
"	left = h1texRECT(x, st - half2(1,0));"
"	right = h1texRECT(x, st + half2(1,0));"
"	bottom = h1texRECT(x, st - half2(0,1));"
"	top = h1texRECT(x, st + half2(0,1));"
"	back = h1texRECT(x, st + zminmax.xy);"
"	front = h1texRECT(x, st + zminmax.zw);"
"}"
"\n"
"struct appdata	\n"
"{ \n"
"	float4 position : POSITION; \n"
"	float2 texcoord	: TEXCOORD0; \n"
"	half3 coord3D	: TEXCOORD1; \n"
"	half4 coordZ	: TEXCOORD2; \n"
"	half4 color	: COLOR; \n"
"}; \n"
" \n"
"struct ptcdata	\n"
"{ \n"
"	float4 position : POSITION; \n"
"	float4 texcoord	: TEXCOORD0; \n"
"}; \n"
"struct ptcconn	\n"
"{ \n"
"	float4 HPos : POSITION; \n"
"	float4 Tex	: TEXCOORD0; \n"
"}; \n"
"struct vfconn\n"
"{ \n"
"	float4 HPos	: POSITION; \n"
"	half2 TexCoord : TEXCOORD0; \n"
"	half3 coord3D  : TEXCOORD1; \n"
"	half4 coordZ  : TEXCOORD2; \n"
"	half4 color  : COLOR; \n"
"}; \n"
" \n"
"vfconn vertexMain(appdata IN, uniform float4x4 ModelViewProj) \n"
"{"
"	vfconn OUT; \n"
"	OUT.HPos = mul(ModelViewProj, IN.position); \n"
"	OUT.TexCoord = IN.texcoord.xy; \n"
"	OUT.coord3D = IN.coord3D; \n"
"	OUT.coordZ = IN.coordZ; \n"
"	OUT.color = IN.color; \n"
"	return OUT; \n"
"}"
"\n"
"ptcconn ptcMain(ptcdata IN, uniform float4x4 ModelViewProj) \n"
"{"
"	ptcconn OUT; \n"
"	OUT.HPos = mul(ModelViewProj, IN.position); \n"
"	OUT.Tex = IN.texcoord; \n"
"	return OUT; \n"
"}"
"\n"
"half4 advect(vfconn IN, uniform float timeStep, uniform samplerRECT u, uniform samplerRECT q, uniform half4 dimensions, uniform half alpha) : COLOR \n"
"{"
"	half4 velocity = h4texRECT(u, IN.TexCoord);"
"	half3 trace_back = IN.coord3D - timeStep * velocity.xyz;"
"	return alpha*h4trilerp2(q, trace_back, dimensions);"
"}"
"\n"
"half4 divergence(vfconn IN, uniform samplerRECT q) : COLOR \n"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(q, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	return 0.5 * (right.x - left.x + top.y - bottom.y + front.z - back.z);"
"}"
"\n"
"half4 jacobi(vfconn IN, uniform samplerRECT x, uniform samplerRECT b) : COLOR \n"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(x, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	half4 bC = h4texRECT(b, IN.TexCoord);"
"	return (left.r+right.r+bottom.r+top.r+back.r+front.r - bC.r)/6;"
"}"
"\n"
"half4 displayVector(vfconn IN, uniform samplerRECT x) : COLOR \n"
"{"
"	return 0.5+0.5*h4texRECT(x, IN.TexCoord);"
"}"
"\n"
"half4 gradient(vfconn IN, uniform samplerRECT p, uniform samplerRECT u) : COLOR \n"
"{"
"	half left, right, bottom, top, back, front;"
"	h1gridneighbourst(p, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	return h4texRECT(u, IN.TexCoord)-0.5*half4(right-left,top-bottom,front-back,0);"
"}"
"\n"
"half4 impulse(vfconn IN, uniform samplerRECT u, uniform half radius, uniform half3 c, uniform half3 p, uniform half spread) : COLOR \n"
"{"
"	half3 to = IN.coord3D-p;"
"	half magSqr = max(EPSILON, dot(to, to));"
"	half dens = exp(-dot(to, to)/radius);"
//"	return h4texRECT(u, IN.TexCoord) + dens*half4(to*rsqrt(magSqr)+c,0);"
"	return h4texRECT(u, IN.TexCoord) + dens*half4(c,0);"
//"	return h4texRECT(u, IN.TexCoord) + (1-step(radius, dot(to, to)))*half4(c+to*rsqrt(magSqr)*spread,0);"
"}"
"\n"
/*
"half4 addTemperature(vfconn IN, uniform samplerRECT u, uniform half radius, uniform half T, uniform half3 p) : COLOR \n"
"{"
"	half3 to = p-IN.coord3D;"
"	half dens = exp(-dot(to, to)/radius);"
"	return h4texRECT(u, IN.TexCoord) + dens*half4(T,T,T,0);"
"}"
"\n"
*/
"half4 buoyancy(vfconn IN, uniform samplerRECT u, uniform samplerRECT t, uniform half Kb) : COLOR \n"
"{"
"	return h4texRECT(u, IN.TexCoord) + half4(0,Kb,0,0)*h4texRECT(t, IN.TexCoord).r;"
"}"
"\n"
"half4 vorticity(vfconn IN, uniform samplerRECT u) : COLOR \n"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(u, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	return 0.5 * half4((top.z - bottom.z) - (front.y - back.y), ((front.x - back.x) - (right.z - left.z)), ((right.y - left.y) - (top.x - bottom.x)), 1);"
"}"
"\n"
"half4 swirl(vfconn IN, uniform samplerRECT v, uniform samplerRECT u, uniform half Ks) : COLOR \n"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(v, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	half3 force = 0.5 * half3(abs(right.x) - abs(left.x), abs(top.y) - abs(bottom.y), abs(front.z) - abs(back.z));\n" 
"	half magSqr = max(EPSILON, dot(force, force));"
"	force = force / sqrt(magSqr);" 
"	half3 vC = h4texRECT(v, IN.TexCoord).xyz;" 
"	force = Ks * cross(force, vC);"
"	return h4texRECT(u, IN.TexCoord) + half4(force,1);"
"}"
"\n"
"half4 boundary(vfconn IN, uniform samplerRECT u, uniform float alpha) : COLOR"
"{"
"	return alpha*h4texRECT(u, IN.TexCoord);"
"}"
"\n"
"half4 offset(vfconn IN, uniform samplerRECT o) : COLOR"
"{"
"	half left, right, bottom, top, back, front;"
"	h1gridneighbourst(o, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	half center = h1texRECT(o, IN.TexCoord);"
//"	return 0.5 * (right.x - left.x + top.y - bottom.y + front.z - back.z);"

//"	half oL = h1texRECT(o, IN.TexCoord - half2(1,0)); \n"
//"	half oR = h1texRECT(o, IN.TexCoord + half2(1,0)); \n"
//"	half oB = h1texRECT(o, IN.TexCoord - half2(0,1)); \n"
//"	half oT = h1texRECT(o, IN.TexCoord + half2(0,1)); \n"
//"	half oC = h1texRECT(o, IN.TexCoord); \n"
"	return half4(left-right, bottom-top, back-front, center);"
"}"
"\n"
"half4 abc(vfconn IN, uniform samplerRECT u, uniform samplerRECT o, uniform half4 dimensions) : COLOR"
"{"
"	half4 v0 = h4texRECT(u, IN.TexCoord);"
"	half4 offset = h4texRECT(o, IN.TexCoord);"
"	half3 offset_pos = IN.coord3D + offset.xyz;"
"	half4 v1 = -h4texRECT(u, flatTo2D2(offset_pos, dimensions));"
"	return v0 + (v1 - v0) * offset.w;"
"}"
"\n"
"half4 flat(vfconn IN) : COLOR"
"{"
"	return IN.color; \n"
"}"
"\n"
"half4 convex(vfconn IN, uniform samplerRECT x, uniform samplerRECT y, uniform samplerRECT z) : COLOR"
"{"
"	return h4texRECT(x, IN.coord3D.zy) * h4texRECT(y, IN.coord3D.xz) * h4texRECT(z, IN.coord3D.xy);"
//"	return h4texRECT(y, IN.coord3D.xz);"
"}"
"\n"
"half4 addTemperature(vfconn IN, uniform samplerRECT u, uniform samplerRECT impulse) : COLOR"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(impulse, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
//"	return h4texRECT(u, IN.TexCoord) + (left.a+right.a+bottom.a+top.a+back.a+front.a)/120;"
"return h4texRECT(u, IN.TexCoord) + half4(left.a +right.a+ bottom.a + top.a + back.a + front.a)/120;"
"}"
"\n"
"half4 addVelocity(vfconn IN, uniform samplerRECT u, uniform samplerRECT impulse) : COLOR"
"{"
//"	half4 left, right, bottom, top, back, front;"
//"	h4gridneighbourst(impulse, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	return h4texRECT(u, IN.TexCoord) + h4texRECT(impulse, IN.TexCoord)/24;"
"}"
"half4 particle(ptcconn IN) : COLOR"
"{"
"	return IN.Tex;"
"}"
"\n";

CFluidProgram::CFluidProgram(void):
m_context(0),
m_vert_program(0),
m_frag_advect(0)
{
	errorlog = "Cg Error Log: ";
	m_context = cgCreateContext();
	
	if(m_context == NULL) cgCheckError("Failed To Create Cg Context");
	
	if (cgGLIsProfileSupported(CG_PROFILE_VP30))
		m_vert_profile = CG_PROFILE_VP30;
	else if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
        m_vert_profile = CG_PROFILE_ARBVP1;

	if(m_vert_profile == CG_PROFILE_UNKNOWN) cgCheckError("Invalid profile type");
	
	m_vert_program = cgCreateProgram(m_context, CG_SOURCE, programSource, m_vert_profile, "vertexMain", 0);
	
	// Validate Success
	if(m_vert_program == NULL) cgCheckError("ERROR create vertex program");

	cgGLLoadProgram(m_vert_program);
	modelViewMatrix	= cgGetNamedParameter(m_vert_program, "ModelViewProj");
	
	m_vert_particle = cgCreateProgram(m_context, CG_SOURCE, programSource, m_vert_profile, "ptcMain", 0);
	cgGLLoadProgram(m_vert_particle);
	particle_modelViewMatrix	= cgGetNamedParameter(m_vert_particle, "ModelViewProj");
	
	if (cgGLIsProfileSupported(CG_PROFILE_FP30))
		m_frag_profile = CG_PROFILE_FP30;
	else if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
        	m_frag_profile = CG_PROFILE_ARBFP1;
	
	if (m_frag_profile == CG_PROFILE_UNKNOWN) cgCheckError("ERROR create fragment profile");
	
	m_frag_advect = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "advect", 0);
	
	if (m_frag_advect == NULL) cgCheckError("ERROR create advect");
	
	cgGLLoadProgram(m_frag_advect);
	advect_timeStep = cgGetNamedParameter(m_frag_advect, "timeStep");
	advect_u = cgGetNamedParameter(m_frag_advect, "u");
	advect_q = cgGetNamedParameter(m_frag_advect, "q");
	advect_dimensions = cgGetNamedParameter(m_frag_advect, "dimensions");
	advect_alpha = cgGetNamedParameter(m_frag_advect, "alpha");
	
	m_frag_divergence = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "divergence", 0);
	
	if (m_frag_divergence == NULL) cgCheckError("ERROR create divergence");
	
	cgGLLoadProgram(m_frag_divergence);
	divergence_q = cgGetNamedParameter(m_frag_divergence, "q");
	
	m_frag_jacobi = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "jacobi", 0);
	
	if (m_frag_jacobi == NULL) cgCheckError("ERROR create jacobi");
	
	cgGLLoadProgram(m_frag_jacobi);
	jacobi_x = cgGetNamedParameter(m_frag_jacobi, "x");
	jacobi_b = cgGetNamedParameter(m_frag_jacobi, "b");
	
	m_frag_displayVector = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "displayVector", 0);
	
	if (m_frag_displayVector == NULL) cgCheckError("ERROR create display");
	
	cgGLLoadProgram(m_frag_displayVector);
	displayVector_x = cgGetNamedParameter(m_frag_displayVector, "x");
	
	m_frag_gradient = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "gradient", 0);
	
	if (m_frag_gradient == NULL) cgCheckError("ERROR create gradient");
	
	cgGLLoadProgram(m_frag_gradient);
	gradient_p = cgGetNamedParameter(m_frag_gradient, "p");
	gradient_u = cgGetNamedParameter(m_frag_gradient, "u");
	
	m_frag_impulse = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "impulse", 0);
	
	if (m_frag_impulse == NULL) cgCheckError("ERROR create impulse");
	
	cgGLLoadProgram(m_frag_impulse);
	impulse_c = cgGetNamedParameter(m_frag_impulse, "c");
	impulse_u = cgGetNamedParameter(m_frag_impulse, "u");
	impulse_p = cgGetNamedParameter(m_frag_impulse, "p");
	impulse_radius = cgGetNamedParameter(m_frag_impulse, "radius");
	impulse_spread = cgGetNamedParameter(m_frag_impulse, "spread");
	
	m_frag_vorticity = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "vorticity", 0);
	
	if (m_frag_vorticity == NULL) cgCheckError("ERROR create vorticity");
	
	cgGLLoadProgram(m_frag_vorticity);
	vorticity_u = cgGetNamedParameter(m_frag_vorticity, "u");
	
	m_frag_swirl = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "swirl", 0);
	
	if (m_frag_swirl == NULL) cgCheckError("ERROR create swirl");
	
	cgGLLoadProgram(m_frag_swirl);
	swirl_u = cgGetNamedParameter(m_frag_swirl, "u");
	swirl_v = cgGetNamedParameter(m_frag_swirl, "v");
	swirl_Ks = cgGetNamedParameter(m_frag_swirl, "Ks");
	
	m_frag_boundary = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "boundary", 0);
	
	if (m_frag_boundary == NULL) cgCheckError("ERROR create boundary");
	
	cgGLLoadProgram(m_frag_boundary);
	boundary_u = cgGetNamedParameter(m_frag_boundary, "u");
	boundary_alpha = cgGetNamedParameter(m_frag_boundary, "alpha");
	
	m_frag_offset = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "offset", 0);
	
	if (m_frag_offset == NULL) cgCheckError("ERROR create offset");
	
	cgGLLoadProgram(m_frag_offset);
	offset_o = cgGetNamedParameter(m_frag_offset, "o");
	
	m_frag_abc = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "abc", 0);
	
	if (m_frag_abc == NULL) cgCheckError("ERROR create abc");
	
	cgGLLoadProgram(m_frag_abc);
	abc_u = cgGetNamedParameter(m_frag_abc, "u");
	abc_o = cgGetNamedParameter(m_frag_abc, "o");
	abc_dimensions = cgGetNamedParameter(m_frag_abc, "dimensions");
	/*
	m_frag_addTemperature = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "addTemperature", 0);
	
	if (m_frag_addTemperature == NULL) cgCheckError("ERROR create addTemperature");
	
	cgGLLoadProgram(m_frag_addTemperature);
	addTemperature_u = cgGetNamedParameter(m_frag_addTemperature, "u");
	addTemperature_T = cgGetNamedParameter(m_frag_addTemperature, "T");
	addTemperature_p = cgGetNamedParameter(m_frag_addTemperature, "p");
	addTemperature_radius = cgGetNamedParameter(m_frag_addTemperature, "radius");
	*/
	m_frag_buoyancy = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "buoyancy", 0);
	
	if (m_frag_buoyancy == NULL) cgCheckError("ERROR create buoyancy");
	
	cgGLLoadProgram(m_frag_buoyancy);
	buoyancy_u = cgGetNamedParameter(m_frag_buoyancy, "u");
	buoyancy_t = cgGetNamedParameter(m_frag_buoyancy, "t");
	buoyancy_Kb = cgGetNamedParameter(m_frag_buoyancy, "Kb");
	
	m_frag_flat = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "flat", 0);
	
	if (m_frag_flat == NULL) cgCheckError("ERROR create flat");
	
	cgGLLoadProgram(m_frag_flat);
	
	m_frag_convex = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "convex", 0);
	
	if (m_frag_convex == NULL) cgCheckError("ERROR create convex");
	
	cgGLLoadProgram(m_frag_convex);
	convex_x = cgGetNamedParameter(m_frag_convex, "x");
	convex_y = cgGetNamedParameter(m_frag_convex, "y");
	convex_z = cgGetNamedParameter(m_frag_convex, "z");
	
	m_frag_addTemperature = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "addTemperature", 0);
	
	if (m_frag_addTemperature == NULL) cgCheckError("ERROR create addTemperature");
	
	cgGLLoadProgram(m_frag_addTemperature);
	addTemperature_u = cgGetNamedParameter(m_frag_addTemperature, "u");
	addTemperature_impulse = cgGetNamedParameter(m_frag_addTemperature, "impulse");
	
	m_frag_addVelocity = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "addVelocity", 0);
	
	if (m_frag_addVelocity == NULL) cgCheckError("ERROR create addVelocity");
	
	cgGLLoadProgram(m_frag_addVelocity);
	addVelocity_u = cgGetNamedParameter(m_frag_addVelocity, "u");
	addVelocity_impulse = cgGetNamedParameter(m_frag_addVelocity, "impulse");
	
	m_frag_particle = cgCreateProgram(m_context, CG_SOURCE, programSource, m_frag_profile, "particle", 0);
	
	if (m_frag_particle == NULL) cgCheckError("ERROR create particle");
	cgGLLoadProgram(m_frag_particle);
	
}

CFluidProgram::~CFluidProgram(void)
{
	if(m_context!= NULL) cgDestroyContext(m_context);
}

void CFluidProgram::begin()
{
	cgGLEnableProfile(m_vert_profile);
	cgGLBindProgram(m_vert_program);
		
	// update projection matrix
	cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	cgGLEnableProfile(m_frag_profile);
}

void CFluidProgram::end()
{
	cgGLDisableProfile(m_frag_profile);
	cgGLDisableProfile(m_vert_profile);
}

void CFluidProgram::advectBegin(GLuint iU, GLuint iQ, float x, float y, float z, float w, float alpha)
{
	cgGLBindProgram(m_frag_advect);
	
	cgGLSetTextureParameter(advect_u, iU);
	cgGLEnableTextureParameter(advect_u);
	
	cgGLSetTextureParameter(advect_q, iQ);
	cgGLEnableTextureParameter(advect_q);
	
	cgGLSetParameter1f(advect_timeStep, 1.0f);
	cgGLSetParameter1f(advect_alpha, alpha);
	cgGLSetParameter4f(advect_dimensions, x, y, z, w);
}

void CFluidProgram::advectEnd()
{
	cgGLDisableTextureParameter(advect_u);
	cgGLDisableTextureParameter(advect_q);
}

void CFluidProgram::divergenceBegin(GLuint i_texture)
{
	cgGLBindProgram(m_frag_divergence);
	
	cgGLSetTextureParameter(divergence_q, i_texture);
	cgGLEnableTextureParameter(divergence_q);
}

void CFluidProgram::divergenceEnd()
{
	cgGLDisableTextureParameter(divergence_q);
}

void CFluidProgram::jacobiBegin(GLuint i_textureX, GLuint i_textureB)
{
	cgGLBindProgram(m_frag_jacobi);
	
	cgGLSetTextureParameter(jacobi_x, i_textureX);
	cgGLSetTextureParameter(jacobi_b, i_textureB);
	cgGLEnableTextureParameter(jacobi_x);
	cgGLEnableTextureParameter(jacobi_b);
}

void CFluidProgram::jacobiEnd()
{
	cgGLDisableTextureParameter(jacobi_x);
	cgGLDisableTextureParameter(jacobi_b);
}

void CFluidProgram::displayVectorBegin(GLuint i_texture)
{
	cgGLBindProgram(m_frag_displayVector);
	
	cgGLSetTextureParameter(displayVector_x, i_texture);
	cgGLEnableTextureParameter(displayVector_x);
}

void CFluidProgram::displayVectorEnd()
{
	cgGLDisableTextureParameter(displayVector_x);
}

void CFluidProgram::gradientBegin(GLuint i_textureP, GLuint i_textureU)
{
	cgGLBindProgram(m_frag_gradient);
	
	cgGLSetTextureParameter(gradient_p, i_textureP);
	cgGLEnableTextureParameter(gradient_p);
	
	cgGLSetTextureParameter(gradient_u, i_textureU);
	cgGLEnableTextureParameter(gradient_u);
}

void CFluidProgram::gradientEnd()
{
	cgGLDisableTextureParameter(gradient_p);
	cgGLDisableTextureParameter(gradient_u);
}

void CFluidProgram::impulseBegin(GLuint i_textureU, int type, float radius, float spread, float px, float py, float pz, float vx, float vy, float vz)
{
	cgGLBindProgram(m_frag_impulse);
	
	cgGLSetTextureParameter(impulse_u, i_textureU);
	cgGLEnableTextureParameter(impulse_u);
	
	cgGLSetParameter1f(impulse_radius, radius/0.5f);
	cgGLSetParameter1f(impulse_spread, spread);
	cgGLSetParameter3f(impulse_c, vx, vy, vz);
	cgGLSetParameter3f(impulse_p, px, py, pz);
}

void CFluidProgram::impulseEnd()
{
	cgGLDisableTextureParameter(impulse_u);
}

void CFluidProgram::vorticityBegin(GLuint i_textureU)
{
	cgGLBindProgram(m_frag_vorticity);
	
	cgGLSetTextureParameter(vorticity_u, i_textureU);
	cgGLEnableTextureParameter(vorticity_u);
}

void CFluidProgram::vorticityEnd()
{
	cgGLDisableTextureParameter(vorticity_u);
}

void CFluidProgram::swirlBegin(GLuint i_textureV, GLuint i_textureU, float Ks)
{
	cgGLBindProgram(m_frag_swirl);
	
	cgGLSetTextureParameter(swirl_v, i_textureV);
	cgGLEnableTextureParameter(swirl_v);
	
	cgGLSetTextureParameter(swirl_u, i_textureU);
	cgGLEnableTextureParameter(swirl_u);
	
	cgGLSetParameter1f(swirl_Ks, Ks);
}

void CFluidProgram::swirlEnd()
{
	cgGLDisableTextureParameter(swirl_v);
	cgGLDisableTextureParameter(swirl_u);
}

void CFluidProgram::boundaryBegin(GLuint i_textureU, float alpha)
{
	cgGLBindProgram(m_frag_boundary);
	
	cgGLSetTextureParameter(boundary_u, i_textureU);
	cgGLEnableTextureParameter(boundary_u);
	
	cgGLSetParameter1f(boundary_alpha, alpha);
}

void CFluidProgram::boundaryEnd()
{
	cgGLDisableTextureParameter(boundary_u);
}

void CFluidProgram::offsetBegin(GLuint i_textureO)
{
	cgGLBindProgram(m_frag_offset);
	
	cgGLSetTextureParameter(offset_o, i_textureO);
	cgGLEnableTextureParameter(offset_o);
}

void CFluidProgram::offsetEnd()
{
	cgGLDisableTextureParameter(offset_o);
}

void CFluidProgram::abcBegin(GLuint i_textureU, GLuint i_textureO, float x, float y, float z, float w)
{
	cgGLBindProgram(m_frag_abc);
	
	cgGLSetTextureParameter(abc_u, i_textureU);
	cgGLEnableTextureParameter(abc_u);
	
	cgGLSetTextureParameter(abc_o, i_textureO);
	cgGLEnableTextureParameter(abc_o);
	
	cgGLSetParameter4f(abc_dimensions, x, y, z, w);
}

void CFluidProgram::abcEnd()
{
	cgGLDisableTextureParameter(abc_u);
	cgGLDisableTextureParameter(abc_o);
}
/*
void CFluidProgram::addTemperatureBegin(GLuint i_textureU, float radius, float T, float px, float py, float pz)
{
	cgGLBindProgram(m_frag_addTemperature);
	
	cgGLSetTextureParameter(addTemperature_u, i_textureU);
	cgGLEnableTextureParameter(addTemperature_u);
	
	cgGLSetParameter1f(addTemperature_radius, radius/0.5f);
	cgGLSetParameter1f(addTemperature_T, T);
	cgGLSetParameter3f(addTemperature_p, px, py, pz);
}

void CFluidProgram::addTemperatureEnd()
{
	cgGLDisableTextureParameter(addTemperature_u);
}
*/
void CFluidProgram::buoyancyBegin(GLuint iU, GLuint iT, float Kb)
{
	cgGLBindProgram(m_frag_buoyancy);
	
	cgGLSetTextureParameter(buoyancy_u, iU);
	cgGLEnableTextureParameter(buoyancy_u);
	
	cgGLSetTextureParameter(buoyancy_t, iT);
	cgGLEnableTextureParameter(buoyancy_t);
	
	cgGLSetParameter1f(buoyancy_Kb, Kb);
}

void CFluidProgram::buoyancyEnd()
{
	cgGLDisableTextureParameter(buoyancy_u);
	cgGLDisableTextureParameter(buoyancy_t);
}

void CFluidProgram::flatBegin()
{
	cgGLBindProgram(m_frag_flat);
}

void CFluidProgram::flatEnd()
{
	
}

void CFluidProgram::convexBegin(GLuint iX, GLuint iY, GLuint iZ)
{
	cgGLBindProgram(m_frag_convex);
	
	cgGLSetTextureParameter(convex_x, iX);
	cgGLEnableTextureParameter(convex_x);
	
	cgGLSetTextureParameter(convex_y, iY);
	cgGLEnableTextureParameter(convex_y);
	
	cgGLSetTextureParameter(convex_z, iZ);
	cgGLEnableTextureParameter(convex_z);
}

void CFluidProgram::convexEnd()
{
	cgGLDisableTextureParameter(convex_x);
	cgGLDisableTextureParameter(convex_y);
	cgGLDisableTextureParameter(convex_z);
}

void CFluidProgram::addTemperatureBegin(GLuint i_textureU, GLuint i_textureImpulse)
{
	//cgGLEnableProfile(m_vert_profile);
	//cgGLBindProgram(m_vert_program);
	//cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	cgGLEnableProfile(m_frag_profile);
	cgGLBindProgram(m_frag_addTemperature);
	
	cgGLSetTextureParameter(addTemperature_u, i_textureU);
	cgGLEnableTextureParameter(addTemperature_u);
	
	cgGLSetTextureParameter(addTemperature_impulse, i_textureImpulse);
	cgGLEnableTextureParameter(addTemperature_impulse);
}

void CFluidProgram::addTemperatureEnd()
{
	cgGLDisableTextureParameter(addTemperature_u);
	cgGLDisableTextureParameter(addTemperature_impulse);
	//cgGLDisableProfile(m_frag_profile);
	//cgGLDisableProfile(m_vert_profile);
}

void CFluidProgram::addVelocityBegin(GLuint i_textureU, GLuint i_textureImpulse)
{
	cgGLBindProgram(m_frag_addVelocity);
	
	cgGLSetTextureParameter(addVelocity_u, i_textureU);
	cgGLEnableTextureParameter(addVelocity_u);
	
	cgGLSetTextureParameter(addVelocity_impulse, i_textureImpulse);
	cgGLEnableTextureParameter(addVelocity_impulse);
}

void CFluidProgram::addVelocityEnd()
{
	cgGLDisableTextureParameter(addVelocity_u);
	cgGLDisableTextureParameter(addVelocity_impulse);
}

void CFluidProgram::particleBegin()
{
	cgGLEnableProfile(m_vert_profile);
	cgGLBindProgram(m_vert_particle);
		
	// update projection matrix
	cgGLSetStateMatrixParameter(particle_modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

	cgGLEnableProfile(m_frag_profile);
	cgGLBindProgram(m_frag_particle);
}

void CFluidProgram::particleEnd()
{
	cgGLDisableProfile(m_frag_profile);
	cgGLDisableProfile(m_vert_profile);
}
	