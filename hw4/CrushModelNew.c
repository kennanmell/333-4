#include <jansson.h>
#include "Array2D.h"
#include <stdlib.h>

/*
class CrushModel {
  int gameid; //unique identifier
  Array2D extensionColor; //the extension grid
  Array2D boardInitialState; //how many times each square must fire
  int movesAllowed; //max moves allowed
  int colors; //max # of distinct colors in extension board

  Array2D boardCandies; //current candies on the board
  Array2D boardState; //how many times each square must fire
  int movesMade; //how many moves have been made
  int currentScore; //total amount that state has decreased
  int extensionOffset[];

  public:
    CrushModel();

};

CrushModel::CrushModel(int gameid, Array2D extensionColor,
		       Array2D boardInitialState, int movesAllowed,
		       int colors, Array2D boardCandies, Array2D boardState,
		       int movesMade, int currentScore, 
		       int extensionOffset[]){

  this.gameid = gameid;
  this.extensionColor = extensionColor;
  this.boardInitialState = boardInitialState;
  this.movesAllowed = movesAllowed;
  this.colors = boardCandies;
  this.boardState = boardState;
  this.movesMade = movesMade;
  this.currentScore = currentScore;
  this.extensionOffset = extensionOffset;
}
*/

Array2D deserializeInt2DArrayFromJsonObject(json_t* json) {
   json_t* jRows = json_object_get(json, "rows");

   if (!json_is_integer(jRows)) {
      return NULL;
   }
   int rows = json_integer_value(jRows);

   json_t* jColumns = json_object_get(json, "columns");
   if (!json_is_integer(jColumns)) {
      return NULL;
   }
   int columns = json_integer_value(jColumns);

   Array2D array = allocateArray2D(rows, columns);   

   json_t* jData = json_object_get(json, "data");
   if (!json_is_array(jData)) {
      return NULL;
   }
   int arraySize = json_array_size(jData);
   int* arr = (int*) malloc(sizeof(int) * arraySize);

   for (int i = 0; i < arraySize; i++) {
      arr[i] = json_integer_value(json_array_get(jData, i));
      setArray2D(array, &arr[i], i % columns, i / columns);
   }
   
   json_decref(json);
   return array;
}

Array2D deserializeBoardCandiesFromJsonObject(json_t* json) {
   json_t* jRows = json_object_get(json, "rows");

   if (!json_is_integer(jRows)) {
      return NULL;
   }
   int rows = json_integer_value(jRows);

   json_t* jColumns = json_object_get(json, "columns");
   if (!json_is_integer(jColumns)) {
      return NULL;
   }
   int columns = json_integer_value(jColumns);

   Array2D array = allocateArray2D(rows, columns);   

   json_t* jData = json_object_get(json, "data");
   if (!json_is_array(jData)) {
      return NULL;
   }
   int arraySize = json_array_size(jData);
   int* arr = (int*) malloc(sizeof(int) * arraySize);

   for (int i = 0; i < arraySize; i++) {
     arr[i] = json_integer_value(json_object_get(json_array_get(jData, i), "color"));
      setArray2D(array, &arr[i], i % columns, i / columns);
   }
   
   json_decref(json);
   return array;
}


void deserializeGameInstance(char* location){
  json_error_t error;
  json_t* json = json_load_file(location, 0, &error);
  if (!json){
    return NULL;
  }
  //game def
  json_t* gameDef = json_object_get(json, "gamedef");

  //game id
  json_t* jGameid = json_object_get(gameDef, "gameid");
  int gameid = json_integer_value(jGameid);
  printf("gameID: %d\n",gameid);

  //extension color
  json_t* jExtensionColor = json_object_get(gameDef, "extensioncolor");
  Array2D extensionColor = deserializeInt2DArrayFromJsonObject(jExtensionColor);
  printf("extension grid:\n");
  printArray(extensionColor);

  //board Initial State
  json_t* jBoardInitialState = json_object_get(gameDef, "boardstate");
  Array2D boardInitialState = deserializeInt2DArrayFromJsonObject(jBoardInitialState);
  printf("board initial state:\n");
  printArray(boardInitialState);

  //moves allowed
  json_t* jMovesAllowed = json_object_get(gameDef, "movesallowed");
  int movesAllowed = json_integer_value(jMovesAllowed);
  printf("moves allowed: %d\n", movesAllowed);

  //colors
  json_t* jColors = json_object_get(gameDef, "colors");
  int colors = json_integer_value(jColors);
  printf("colors: %d\n", colors);

  //game state
  json_t* gameState = json_object_get(json, "gamestate");
  Array2D boardCandies, boardState;
  int movesMade, currentScore, *extensionOffset;
  if (gameState != NULL){
    printf("Not null!\n");
    
    //boardCandies
    json_t* jBoardCandies = json_object_get(gameState, "boardcandies");
    boardCandies = deserializeBoardCandiesFromJsonObject(jBoardCandies);
    printf("board candies:\n");
    printArray(boardCandies);

    //board state
    json_t* jBoardState = json_object_get(gameState, "boardstate");
    boardState = deserializeInt2DArrayFromJsonObject(jBoardState);
    printf("board state:\n");
    printArray(boardState);

    //moves made
    json_t* jMovesMade = json_object_get(gameState, "movesmade");
    movesMade = json_integer_value(jMovesMade);
    printf("moves made: %d\n", movesMade);
    
    //current score
    json_t* jCurrentScore = json_object_get(gameState, "currentscore");
    currentScore = json_integer_value(jCurrentScore);
    printf("current score: %d\n", currentScore);

    //extension offest
    json_t* jExtensionOffset = json_object_get(gameState, "extensionoffset");
    extensionOffset = (int*) malloc(sizeof(int) * json_array_size(jExtensionOffset));
    for (int i = 0; i < json_array_size(jExtensionOffset); i++){
      extensionOffset[i] = json_integer_value(json_array_get(jExtensionOffset, i));
    }
    printf("extension offset:\n");
    for (int i = 0; i < json_array_size(jExtensionOffset); i++){
      printf("%d ", extensionOffset[i]);
    }
    printf("\n");

  } else {
    printf("null!\n");

    //board Candies
    int boardCandiesRows = boardInitialState->rows + 1;
    int boardCandiesColumns = boardInitialState->columns + 1;
    boardCandies = allocateArray2D(boardCandiesColumns, boardCandiesRows);
    for (int i = 0; i < boardCandiesRows; i++){
      for (int j = 0; j < boardCandiesColumns; j++){
	setArray2D(boardCandies, getArray2D(extensionColor, j, i), j, i);
      } 
    }
    printf("board candies:\n");
    printArray(boardCandies);
  

    //board state
    boardState = boardInitialState;
    printf("board state:\n");
    printArray(boardState);

    //moves made
    movesMade = 0;
    printf("moves made: %d\n", movesMade);

    //current score
    currentScore = 0;
    printf("currentScore %d\n", currentScore);

    //extension offset
    extensionOffset = (int*) malloc(sizeof(int) * boardCandiesColumns);
    for (int i = 0; i < boardCandiesColumns; i++){
      extensionOffset[i] = boardCandiesColumns + 1;
    }
    printf("extension offset:\n");
    for (int i = 0; i < boardCandiesColumns; i++){
      printf("%d ", extensionOffset[i]);
    }
    printf("\n");
  }
}



int main(int argc, char** argv){
  deserializeGameInstance(argv[1]);
}
