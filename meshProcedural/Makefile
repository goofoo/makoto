CC = cc
FLAG = -DREQUIRE_IOSTREAM  -O3 -Wall -fPIC -D_BOOL
INC = -I../rmandevkit/include -I$(RATTREE)/devkit/include -I$(RATTREE)/devkit/examples/ribgen -I/usr/include/libxml2
LIB = -L../rmandevkit/lib
SRC = meshProcedural.cpp meshData.cpp ../shared/zGlobal.cpp ../shared/FColorSet.cpp ../shared/FFloatSet.cpp ../shared/FUVSet.cpp ../shared/FXMLMesh.cpp ../shared/zXMLDoc.cpp ../sh_lighting/FTriangle.cpp

all:
	$(CC) $(FLAG) -o ../module/procedural/meshCacheProcedural.so -shared $(SRC) $(INC) $(LIB) -lxml2 

