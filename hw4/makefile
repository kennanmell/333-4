SRC=CrushModelNew.c Array2D.c CrushModel.c CrushView.c  # change these to your source file names
HEADERS=CrushView.h Array2D.h CrushModel.h  # c 
CFLAGS = -Wall -Wpedantic -std=c++11
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw4

hw4: $(SRC) $(HEADERS) $(LIBS)
	g++ $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw4 $(SRC) $(LDFLAGS) $(GTKLDFLAGS)

clean:
	rm -f *.o *~ hw3
