include ../shared/buildconfig

SRC = pluginMain.cpp PTCDemViz.cpp PTCMapViz.cpp PTCMapCmd.cpp \
../shared/zWorks.cpp ../shared/zGlobal.cpp ../shared/gBase.cpp ../shared/QuickSort.cpp ../shared/FNoise.cpp \
../sh_lighting/SHMath.cpp ../sh_lighting/SphericalHarmonics.cpp \
../3dtexture/OcTree.cpp ../3dtexture/CubeRaster.cpp ../3dtexture/Z3DTexture.cpp

DLINCS = -I${DELIGHT}/include/
DLLIBS = -L${DELIGHT}/lib/ -l3delight

DSOSRC = dsoParticleCache.cpp RPTC.cpp RCloud.cpp \
../shared/zGlobal.cpp ../shared/gBase.cpp ../shared/QuickSort.cpp ../shared/FNoise.cpp \
../sh_lighting/SHMath.cpp ../sh_lighting/SphericalHarmonics.cpp \
../3dtexture/OcTree.cpp ../3dtexture/CubeRaster.cpp ../3dtexture/Z3DTexture.cpp

all:
	$(LD) -o ../module/plug-ins/ParticleMap.so $(SRC) $(INCLUDES) $(LIBS) -lOpenMayaUI -lOpenMayaAnim -lOpenMayaFX

linux:
	$(LD) -o particleCacheProcedural.so $(DSOSRC) $(DLINCS) $(DLLIBS)
	
mac:
	g++ -dynamiclib -o particleCacheProcedural.so -O3 $(DSOSRC) $(DLINCS) $(DLLIBS) -framework OpenGL
	renderdl foo.rib
