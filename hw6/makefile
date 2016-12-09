SRC=solution/Array2D.c solution/ClientSocket.cc solution/CrushMain.cpp solution/CrushSolver.cpp solution/CSE333Socket.cc solution/ServerSocket.cc solution/Verify333.c
HEADERS=solution/Array2D.h solution/ClientSocket.h solution/CrushMain.h solution/CSE333.h solution/CSE333Socket.h solution/ServerSocket.h
CFLAGS = -Wall -Wpedantic -std=c++11
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw6-model

hw6-model: $(SRC) $(HEADERS) $(LIBS)
	g++ $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw6-model $(SRC) $(LDFLAGS) $(GTKLDFLAGS)

clean:
	rm -f *.o *~ hw6-model