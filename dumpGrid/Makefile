CC =  c++
CFLAGS = -dynamiclib
INCS = -I${DELIGHT}/include/
LIBS = -L${DELIGHT}/lib/ -l3delight
SRC = dumpGrid.cpp

all:
	$(CC) $(CFLAGS) -o dumpGrid.so $(SRC) $(INCS)
	shaderdl surfdump.sl
	renderdl -p:1 foo.rib
	