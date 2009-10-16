CC = g++
FLAG = -fPIC
INC = -I../rmandevkit/include -I/usr/include/libxml2
LIB = -L../rmandevkit/lib
SRC = prtRslPlugin.cpp CPRTData.cpp ../sh_lighting/SHWorks.cpp ../sh_lighting/SHMath.cpp ../sh_lighting/zSphericalHarmonics.cpp ../sh_lighting/VisibilityBuffer.cpp ../sh_lighting/FQSPLAT.cpp\
../shared/FXMLMesh.cpp ../shared/FUVSet.cpp ../shared/FColorSet.cpp ../shared/FFloatSet.cpp ../sh_lighting/FTriangle.cpp ../shared/zXMLDoc.cpp\
../shared/zGlobal.cpp ../shared/hdrtexture.cpp "../sh_lighting/SH Rotation/ApplyMatrix.cpp" "../sh_lighting/SH Rotation/GetX90DegreeRotationMatrix.cpp" "../sh_lighting/SH Rotation/GetZRotationMatrix.cpp" "../sh_lighting/SH Rotation/RotateSHCoefficients.cpp"\
../shared/gBase.cpp

DLSRC = dl_prtmain.cpp RPRT.cpp ../shared/hdrtexture.cpp ../sh_lighting/SphericalHarmonics.cpp\
../shared/FNoise.cpp ../sh_lighting/SHMath.cpp

all:
	$(CC) $(FLAG) -o zPRT.so -shared $(SRC) $(INC) $(LIB) -lxml2 -lGL
	shader myconstant.sl
	cp zPRT.so ../module/rms
	
mac:
	$(CC) -dynamiclib -o zPRT.so $(DLSRC) -I$(DELIGHT)/include -framework OpenGL
	sudo cp zPRT.so $(DELIGHT)/shaders/
	rm zPRT.so
	
linux:
	$(CC) $(FLAG) -o zPRT.so -shared $(DLSRC) -I$(DELIGHT)/include -lGL
	cp zPRT.so $(DELIGHT)/shaders/
	rm zPRT.so
