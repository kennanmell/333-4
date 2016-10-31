#include "Array2D.h"
#ifndef _CModel_h
#define _CModel_h

typedef struct model_holder {
    Array2D arr;
    int moves;
} ModelData;

int swapCandies(ModelData* md, int x, int y, char dir);

#endif
