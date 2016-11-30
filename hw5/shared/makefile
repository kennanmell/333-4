SRC=Array2D.c Array2DTest.c
HEADER=Array2D.h

hw2:  ${SRC} ${HEADER}
	gcc -std=c11 -Wall -g -Wno-pointer-to-int-cast -I ../../jansson/include ${SRC} -L ../../jansson/lib -l:libjansson.a -o test

run: hw2
	valgrind --leak-check=full ./hw2 test.json

clean:
	rm -f *.o *~ hw2 json.out
