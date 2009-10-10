#pragma once

#include <Cg/cg.h>
#include <Cg/cgGL.h>

static const char *programSource =
"	static const half EPSILON = 2.4414e-4; \n"
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
"	half left, right, bottom, top, back, front;"
"	h1gridneighbourst(x, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	half bC = h1texRECT(b, IN.TexCoord);"
"	return (left+right+bottom+top+back+front - bC)/6;"
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
"	return h4texRECT(u, IN.TexCoord) + h1texRECT(t, IN.TexCoord)*half4(0,Kb,0,0);"
"}"
"\n"
"half4 vorticity(vfconn IN, uniform samplerRECT u) : COLOR \n"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(u, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	return 0.5 * half4((top.z - bottom.z) - (front.y - back.y), ((front.x - back.x) - (right.z - left.z)), ((right.y - left.y) - (top.x - bottom.x)), 0);"
"}"
"\n"
"half4 swirl(vfconn IN, uniform samplerRECT v, uniform samplerRECT u, uniform half Ks) : COLOR \n"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(v, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	half3 force = 0.5 * half3(abs(right.x) - abs(left.x), abs(top.y) - abs(bottom.y), abs(front.z) - abs(back.z));\n" 
"	half magSqr = max(EPSILON, dot(force, force));"
"	force = force * rsqrt(magSqr);" 
"	half3 vC = h4texRECT(v, IN.TexCoord);" 
"	force = Ks * cross(force, vC);"
"	return h4texRECT(u, IN.TexCoord) + half4(force,0);"
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
//"	return h4texRECT(z, IN.coord3D.xy);"
"}"
"\n"
"half4 addTemperature(vfconn IN, uniform samplerRECT u, uniform samplerRECT impulse) : COLOR"
"{"
"	half4 left, right, bottom, top, back, front;"
"	h4gridneighbourst(impulse, IN.TexCoord, IN.coordZ, left, right, bottom, top, back, front);"
"	return h4texRECT(u, IN.TexCoord) + (left.w+right.w+bottom.w+top.w+back.w+front.w)/120;"
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

class CFluidProgram
{
public:
	CFluidProgram(void);
	~CFluidProgram(void);
	
	void begin();
	void end();
	
	void advectBegin(GLuint iU, GLuint iQ, float x, float y, float z, float w, float alpha);
	void advectEnd();
	void divergenceBegin(GLuint i_texture);
	void divergenceEnd();
	void jacobiBegin(GLuint i_textureX, GLuint i_textureB);
	void jacobiEnd();
	void displayVectorBegin(GLuint i_texture);
	void displayVectorEnd();
	void gradientBegin(GLuint i_textureP, GLuint i_textureU);
	void gradientEnd();
	void impulseBegin(GLuint i_textureU, int type, float radius, float spread, float px, float py, float pz, float vx, float vy, float vz);
	void impulseEnd();
	void vorticityBegin(GLuint i_textureU);
	void vorticityEnd();
	void swirlBegin(GLuint i_textureV, GLuint i_textureU, float Ks);
	void swirlEnd();
	void boundaryBegin(GLuint i_textureU, float alpha);
	void boundaryEnd();
	void offsetBegin(GLuint i_textureO);
	void offsetEnd();
	void abcBegin(GLuint i_textureU, GLuint i_textureO, float x, float y, float z, float w);
	void abcEnd();
	//void addTemperatureBegin(GLuint i_textureU, float radius, float T, float px, float py, float pz);
	//void addTemperatureEnd();
	void buoyancyBegin(GLuint iU, GLuint iT, float Kb);
	void buoyancyEnd();
	void flatBegin();
	void flatEnd();
	void convexBegin(GLuint iX, GLuint iY, GLuint iZ);
	void convexEnd();
	void addTemperatureBegin(GLuint i_textureU, GLuint i_textureImpulse);
	void addTemperatureEnd();
	void addVelocityBegin(GLuint i_textureU, GLuint i_textureImpulse);
	void addVelocityEnd();
	void particleBegin();
	void particleEnd();
	
private:
	CGcontext m_context;
	
	CGprofile m_vert_profile;
	CGprofile m_frag_profile;
	
	CGprogram m_vert_program;
	CGprogram m_vert_particle;
	
	CGprogram m_frag_advect;
	CGprogram m_frag_divergence;
	CGprogram m_frag_jacobi;
	CGprogram m_frag_gradient;
	CGprogram m_frag_impulse;
	CGprogram m_frag_displayVector;
	CGprogram m_frag_vorticity;
	CGprogram m_frag_swirl;
	CGprogram m_frag_boundary;
	CGprogram m_frag_offset;
	CGprogram m_frag_abc;
	//CGprogram m_frag_addTemperature;
	CGprogram m_frag_buoyancy;
	CGprogram m_frag_flat;
	CGprogram m_frag_convex;
	CGprogram m_frag_addTemperature;
	CGprogram m_frag_addVelocity;
	CGprogram m_frag_particle;
	
	CGparameter modelViewMatrix;
	CGparameter advect_timeStep, advect_u, advect_q, advect_dimensions, advect_alpha;
	CGparameter divergence_q;
	CGparameter jacobi_x, jacobi_b;
	CGparameter displayVector_x;
	CGparameter gradient_p, gradient_u;
	CGparameter impulse_u, impulse_radius, impulse_c, impulse_p, impulse_spread;
	CGparameter vorticity_u;
	CGparameter swirl_v, swirl_u, swirl_Ks;
	CGparameter boundary_u, boundary_alpha;
	CGparameter offset_o;
	CGparameter abc_u, abc_o, abc_dimensions;
	//CGparameter addTemperature_u, addTemperature_T, addTemperature_radius, addTemperature_p;
	CGparameter buoyancy_u, buoyancy_t, buoyancy_Kb;
	CGparameter convex_x, convex_y, convex_z;
	CGparameter addTemperature_u, addTemperature_impulse;
	CGparameter addVelocity_u, addVelocity_impulse;
	CGparameter particle_modelViewMatrix;
};
