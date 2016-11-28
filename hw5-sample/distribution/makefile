# define the commands we will use for compilation
CC = gcc
CXX = g++

# where the socket source code lives
SOCKDIR = ../socketCode

# define useful flags to cc/ld/etc.
CFLAGS = -g -Wall -Wpedantic -O0 -I$(SOCKDIR)
LDFLAGS =

# define common dependencies
HEADERS = $(SOCKDIR)/CSE333.h $(SOCKDIR)/CSE333Socket.h
OBJS = $(SOCKDIR)/CSE333Socket.o $(SOCKDIR)/Verify333.o $(SOCKDIR)/ClientSocket.o

all: clientSocketTest serverSocketTest

serverSocketTest:  mainServerSocketTest.o $(OBJS) $(SOCKDIR)/ServerSocket.o
	cd $(SOCKDIR) && make
	$(CXX) $(CFLAGS) -o serverSocketTest $(SOCKDIR)/ServerSocket.o mainServerSocketTest.o $(OBJS)

clientSocketTest: mainClientSocketTest.o $(OBJS)
	cd $(SOCKDIR) && make
	$(CXX) $(CFLAGS) -o clientSocketTest mainClientSocketTest.o $(OBJS)

%.o: %.cc $(HEADERS)
	$(CXX) -std=c++11 $(CFLAGS) -c $<

%.o: %.c $(HEADERS)
	$(CC) -std=c11 $(CFLAGS) -c -std=c11 $<

clean: FORCE
	/bin/rm -f *.o *~ serverSocketTest clientSocketTest

FORCE:
