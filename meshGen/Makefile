CC = g++
FLAG = -DREQUIRE_IOSTREAM  -O3 -Wall -D_BOOL -DLINUX -fPIC
INC = -I../rmandevkit/include -I$(MAYA_LOCATION)/include -I$(RATTREE)/devkit/include -I$(RATTREE)/devkit/examples/ribgen -I/usr/include/libxml2
LIB = -L../rmandevkit/lib -L$(MAYA_LOCATION)/lib
SRC = meshGen.cpp ../shared/zWorks.cpp ../shared/RIBContextUtil.cpp ../shared/zGlobal.cpp ../shared/zXMLDoc.cpp ../shared/XMLUtil.cpp

all:
	$(CC) $(FLAG) -o ../module/ribgen/meshRibgen.so -shared $(SRC) $(INC) $(LIB) -lOpenMayaAnim -lOpenMayaUI -lxml2
