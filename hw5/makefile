SRC=CrushMain.cpp ClientSocket.cc Verify333.c Array2D.c CSE333Socket.cc CrushModel.c CrushView.c ViewMain.cpp ServerSocket.cc  # change these to your source file names
HEADERS=CrushView.h Array2D.h CrushModel.h CSE333Socket.h CrushMain.h ClientSocket.h ServerSocket.h CSE333.h # c 
CFLAGS = -Wall -Wpedantic -std=c++11
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw4

hw4: $(SRC) $(HEADERS) $(LIBS)
	g++ $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw5 $(SRC) $(LDFLAGS) $(GTKLDFLAGS)

clean:
	rm -f *.o *~ hw5