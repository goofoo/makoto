include ../shared/buildconfig

SRC = pluginMain.cpp bruiseMapNode.cpp bruiseMap.cpp ../shared/DiceTriangle.cpp ../shared/zWorks.cpp ../shared/zFnEXR.cpp

all:
	$(LD) -o ../module/plug-ins/BruiseMap.so $(SRC) $(INCLUDES) $(LIBS) -lOpenMayaUI -lHalf -lIlmImf
