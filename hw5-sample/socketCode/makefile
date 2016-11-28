# define the commands we will use for compilation and library building
AR = ar
ARFLAGS = rcs
CC = gcc
CXX = g++

# define useful flags to cc/ld/etc.
CFLAGS = -g -Wall -Wpedantic -O0
LDFLAGS =

# define common dependencies
HEADERS = ClientSocket.h CSE333.h CSE333Socket.h ServerSocket.h
OBJS = ClientSocket.o CSE333Socket.o ServerSocket.o Verify333.o

all: $(OBJS)

%.o: %.cc $(HEADERS)
	$(CXX) -std=c++11 $(CFLAGS) -c $<

%.o: %.c $(HEADERS)
	$(CC) -std=c11 $(CFLAGS) -c -std=c11 $<

clean: FORCE
	/bin/rm -f *.o *~

FORCE:
