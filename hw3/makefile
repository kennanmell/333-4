SRC=CrushView.c Array2D.c  # change these to your source file names
HEADERS=CrushView.h Array2D.h  # change these to your header file names
CFLAGS = -Wall -Wpedantic -std=c11 
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw3

hw3: $(SRC) $(HEADERS) $(LIBS)
	gcc $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw3 $(SRC) $(LDFLAGS) $(GTKLDFLAGS)


clean:
	rm -f *.o *~ hw3
