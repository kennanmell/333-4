#include "CrushModel.h"
#include <gtk/gtk.h>    //for printing
#include <stdlib.h>
#include <stdio.h>

int swapCandies(ModelData* md, int x1, int y1, char dir){
  if (md->moves == 0){
    return 3;
  }
  int x2 = x1;
  int y2 = y1;
  if (dir == 'l'){
    x2--;
  } else if (dir == 'r'){
    x2++;
  } else if (dir == 'u'){
    y2--;
  } else { // dir == 'd'
    y2++;
  }
  int code = swapArray2D(md->arr, x1, y1, x2, y2);
  if (code == 0){ //swap success
    md->moves--;
  }
  return code;
}
