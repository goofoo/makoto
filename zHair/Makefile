include ../shared/buildconfig

SRC = pluginMain.cpp zHairNode.cpp HairMap.cpp BindTriangle.cpp ../shared/QuickSort.cpp ../shared/zWorks.cpp ../shared/FNoise.cpp ../shared/zGlobal.cpp HairDguideNode.cpp guideCommand.cpp hairDeformer.cpp curvePatchNode.cpp combCurveTool.cpp

all:
	$(LD) -o ../module/plug-ins/ZHair.so $(SRC) $(INCLUDES) $(LIBS) -lOpenMayaUI -lOpenMayaAnim
