include ../shared/buildconfig

SRC = pluginMain.cpp fluidDescData.cpp fluidField.cpp fluidSolver.cpp\
 fluidViz.cpp fluidProgram.cpp ../shared/FnCg.cpp ../shared/zFrameBuffer.cpp

all:
	$(LD) -o ../module/plug-ins/ZFluid.so $(SRC) $(INCLUDES) $(LIBS) -lOpenMayaUI
