CC      = g++
FLAGS   = -g -Wno-deprecated -DLINUX -pthread
INCLUDES = -I/usr/X11R6/include -I/usr/include/libxml2
LIBS = -lGL -lGLU -lxml2
SRC = photosynth.cpp ../shared/FColorSet.cpp ../shared/FFloatSet.cpp ../shared/FUVSet.cpp ../shared/FXMLMesh.cpp ../shared/FXMLScene.cpp  ../shared/XMLUtil.cpp ../shared/zXMLDoc.cpp ../sh_lighting/FQSPLAT.cpp ../sh_lighting/FTriangle.cpp ../sh_lighting/VisibilityBuffer.cpp ../shared/gBase.cpp ../shared/zGlobal.cpp ../sh_lighting/SHWorks.cpp ../sh_lighting/SHMath.cpp ../sh_lighting/zSphericalHarmonics.cpp ../shared/hdrtexture.cpp "../sh_lighting/SH Rotation/ApplyMatrix.cpp"   "../sh_lighting/SH Rotation/GetX90DegreeRotationMatrix.cpp" "../sh_lighting/SH Rotation/GetZRotationMatrix.cpp" "../sh_lighting/SH Rotation/RotateSHCoefficients.cpp"

all: 
	$(CC) $(FLAGS) -o photosynth $(SRC) $(INCLUDES) $(LIBS)

