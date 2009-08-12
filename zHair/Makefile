CC =  g++
CFLAGS = -dynamiclib
INCS = -I${DELIGHT}/include/ -I/usr/local/include/OpenEXR/
LIBS = -L${DELIGHT}/lib/ -L/usr/local/lib/ -l3delight -lHalf -lIlmImf
SRC = hairProcedural.cpp RHair.cpp HairCache.cpp ../shared/DiceTriangle.cpp   BindTriangle.cpp ../shared/FNoise.cpp ../shared/QuickSort.cpp  ../shared/zGlobal.cpp ../shared/zFnEXR.cpp BoundHair.cpp

all:
	$(CC) $(CFLAGS) -o hairProcedural.so $(SRC) $(INCS) $(LIBS)
	renderdl foo.rib

test:
	renderdl foo.rib