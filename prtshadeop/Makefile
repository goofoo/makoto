CC = g++
FLAG = -DREQUIRE_IOSTREAM  -O3 -Wall -fPIC -D_BOOL
INC = -I../rmandevkit/include
LIB = -L../rmandevkit/lib
SRC = prtRslPlugin.cpp

all:
	$(CC) $(FLAG) -o zPRT.so -shared $(SRC) $(INC) $(LIB)
	shader myconstant.sl
	cp zPRT.so ../module/rslPlugin
