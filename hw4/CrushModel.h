#include "Array2D.h"
#ifndef _CModel_h
#define _CModel_h

//a struct to hold the data for the model
typedef struct model_holder {
    Array2D arr;
    Array2D points;
    int moves;
} ModelData;

//Attempts a candy swap given a ModelData struct, an x and a y,
//and a direction is which to push that candy. Returns 0 if the
//swap is successful. If the swap is successful, the number of
//remaining moves in the given ModelData will be decremented by one.
int swapCandies(ModelData* md, int x, int y, char dir);

#endif
