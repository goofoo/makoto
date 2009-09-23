CC = cc
FLAG = -DREQUIRE_IOSTREAM  -O3 -Wall -fPIC -D_BOOL
INC = -I../rmandevkit/include -I$(MAYA_LOCATION)/include -I$(RATTREE)/devkit/include -I$(RATTREE)/devkit/examples/ribgen -I/usr/include/libxml2
LIB = -L../rmandevkit/lib -L$(MAYA_LOCATION)/lib
SRC = rmsMeshGen.cpp RIBContextUtil.cpp ../shared/zWorks.cpp ../shared/zGlobal.cpp

all:
	$(CC) $(FLAG) -o ../module/ribgen/meshRibgen.so -shared $(SRC) $(INC) $(LIB) -lOpenMaya -lFoundation -lOpenMayaAnim -lOpenMayaUI  

