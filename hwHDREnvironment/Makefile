include ../shared/buildconfig

SRC = pluginMain.cpp hdrEnvNode.cpp ../shared/zWorks.cpp ../shared/hdrtexture.cpp ../shared/gBase.cpp hdrProgram.cpp ../shared/FnCg.cpp "../sh_lighting/SH Rotation/ApplyMatrix.cpp"   "../sh_lighting/SH Rotation/GetX90DegreeRotationMatrix.cpp" "../sh_lighting/SH Rotation/GetZRotationMatrix.cpp" "../sh_lighting/SH Rotation/RotateSHCoefficients.cpp" ../sh_lighting/SHMath.cpp  ../sh_lighting/zSphericalHarmonics.cpp 

all:
	$(LD) -o ../module/plug-ins/HDREnvironmentViewer.so $(SRC) $(INCLUDES) $(LIBS) -lOpenMayaUI -lCg -lCgGL 
