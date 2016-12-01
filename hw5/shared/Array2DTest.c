#include "Array2D.h"
#include <stdlib.h>

int main(int argc, char** argv){
  Array2D arr = allocateArray2D(3, 2);
  int* num1 = (int*) malloc(sizeof(int));
  *num1 = 1;
  setArray2D(arr, num1, 0, 0);
  int* num2 = (int*) malloc(sizeof(int));
  *num2 = 2;
  setArray2D(arr, num2, 0, 1);
  int* num3 = (int*) malloc(sizeof(int));
  *num3 = 3;
  setArray2D(arr, num3, 0, 2);
  int* num4 = (int*) malloc(sizeof(int));
  *num4 = 4;
  setArray2D(arr, num4, 1, 0);
  int* num5 = (int*) malloc(sizeof(int));
  *num5 = 5;
  setArray2D(arr, num5, 1, 1);
  int* num6 = (int*) malloc(sizeof(int));
  *num6 = 6;
  setArray2D(arr, num6, 1, 2);
  printArray(arr);
  freeArray2D(arr, &free);
  printf("success\n");
}
