#include <stdlib.h>
#include <iostream>

#include "../shared/ClientSocket.h"
#include "../shared/ServerSocket.h"
#include "CrushMain.h"

using namespace std;

int zero = 0; // used to represent the default candy type


// The "global" CrushMain. Used when working with a CrushView, since c classes can't deal with instances of C++ objects.
CrushMain *m;

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

   for (int i = 0; i < arraySize; i++) {
     //arr[i] = json_integer_value(json_array_get(jData, i));
     // setArray2D(array, &arr[i], i % columns, i / columns);
     int* element = (int*) malloc(sizeof(int));
     *element = json_integer_value(json_array_get(jData, i));
     setArray2D(array, element, i % columns, i / columns);
   }
   return array;
}

int deserializeBoardCandiesFromJsonObject(json_t* json, Array2D* colors, Array2D* types, int** thingToFree) {
   json_t* jRows = json_object_get(json, "rows");

   if (!json_is_integer(jRows)) {
     return 1;
   }
   int rows = json_integer_value(jRows);

   json_t* jColumns = json_object_get(json, "columns");
   if (!json_is_integer(jColumns)) {
     return 1;
   }
   int columns = json_integer_value(jColumns);

   *colors = allocateArray2D(rows, columns);
   *types = allocateArray2D(rows, columns);

   json_t* jData = json_object_get(json, "data");
   if (!json_is_array(jData)) {
     return 1;
   }
   int arraySize = json_array_size(jData);

   int* arr = (int*) malloc(sizeof(int) * arraySize);
   *thingToFree = arr;
   for (int i = 0; i < arraySize; i++) {
     int* z2 = (int*) malloc(sizeof(int));
     arr[i] = json_integer_value(json_object_get(json_array_get(jData, i), "color"));
     *z2 = json_integer_value(json_object_get(json_array_get(jData, i), "type"));
     setArray2D(*colors, &arr[i], i % columns, i / columns);
     setArray2D(*types, z2, i % columns, i / columns);
   }
   
   return 0;
}

CrushMain* deserializeHelper(json_t* json){
  int* thingToFree = NULL;

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
  Array2D boardCandies, boardState, boardCandyTypes;
  int movesMade, currentScore, *extensionOffset;
  if (gameState != nullptr){
    printf("Not null!\n");
    
    //boardCandies and candy types
    json_t* jBoardCandies = json_object_get(gameState, "boardcandies");
    deserializeBoardCandiesFromJsonObject(jBoardCandies, &boardCandies, &boardCandyTypes, &thingToFree);
    printf("board candies:\n");
    printArray(boardCandies);
    printf("board types:\n");
    printArray(boardCandyTypes);

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
    for (int i = 0; i < (int) json_array_size(jExtensionOffset); i++){
      extensionOffset[i] = json_integer_value(json_array_get(jExtensionOffset, i));
    }
    printf("extension offset:\n");
    for (int i = 0; i < (int) json_array_size(jExtensionOffset); i++){
      printf("%d ", extensionOffset[i]);
    }
    printf("\n");

  } else {
    printf("null!\n");

    //board Candies and candy types
    int boardCandiesRows = boardInitialState->rows + 1;
    int boardCandiesColumns = boardInitialState->columns + 1;
    boardCandies = allocateArray2D(boardCandiesRows, boardCandiesColumns);
    boardCandyTypes = allocateArray2D(boardCandiesRows, boardCandiesColumns);
    for (int i = 0; i < boardCandiesRows; i++){
      for (int j = 0; j < boardCandiesColumns; j++){
	setArray2D(boardCandies, getArray2D(extensionColor, j, i), j, i);
	setArray2D(boardCandyTypes, &zero, j, i);
      } 
    }
    printf("board candies:\n");
    printArray(boardCandies);
    printf("board candy types:\n");
    printArray(boardCandyTypes);

    //board state
    boardState = allocateArray2D(boardCandiesRows, boardCandiesColumns);
    for (int i = 0; i < boardCandiesRows; i++) {
      for (int j = 0; j < boardCandiesColumns; j++) {
        setArray2D(boardState, getArray2D(boardInitialState, j, i), j, i);
      }
    }
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
      extensionOffset[i] = boardCandiesRows;
    }
    printf("extension offset:\n");
    for (int i = 0; i < boardCandiesColumns; i++){
      printf("%d ", extensionOffset[i]);
    }
    printf("\n");
  }
  CrushMain* result = new CrushMain(gameid, extensionColor, boardInitialState, movesAllowed, colors, boardCandies, boardCandyTypes,  boardState, movesMade, currentScore, extensionOffset);
  result->thingToFree = thingToFree;
  json_decref(json);
  return result;
}

CrushMain* deserializeServerGameInstanceMessage(char* location){
  json_error_t error;
  json_t* json = json_loads(location, 0, &error);
  if (!json){
    return nullptr;
  }
  json_t* gameInstance = json_deep_copy(json_object_get(json, "gameinstance"));
  json_decref(json);
  return deserializeHelper(gameInstance);
}

CrushMain* deserializeGameInstance(char* location){
  json_error_t error;
  json_t* json = json_load_file(location, 0, &error);
  if (!json){
    return nullptr;
  }
  return deserializeHelper(json);
}

json_t* serializeArray2DToJsonObject(Array2D array){
  json_t* out = json_object();
  json_t* jArr = json_array();
  for (int i = 0; i <= array->rows; i++) {
     for (int j = 0; j <= array->columns; j++) {
        int* curr = (int*) getArray2D(array,j, i);
        json_array_append_new(jArr, json_integer(*curr));
     }
  }

  json_object_set_new(out, "rows", json_integer(array->rows + 1));
  json_object_set_new(out, "columns", json_integer(array->columns + 1));
  json_object_set_new(out, "data", jArr);
  return out;
}

json_t* serializeBoardCandiesToJsonObject(Array2D array, Array2D array2){
  json_t* out = json_object();
  json_t* jArr = json_array();
  for (int i = 0; i <= array->rows; i++) {
     for (int j = 0; j <= array->columns; j++) {
        int* curr = (int*) getArray2D(array, j, i);
	int* curr2 = (int*) getArray2D(array2, j, i);
	json_t* index = json_object();
	json_object_set_new(index, "color", json_integer(*curr));
	json_object_set_new(index, "type", json_integer(*curr2));
        json_array_append_new(jArr, index);
     }
  }

  json_object_set_new(out, "rows", json_integer(array->rows + 1));
  json_object_set_new(out, "columns", json_integer(array->columns + 1));
  json_object_set_new(out, "data", jArr);
  return out;
}

json_t* serializeGameInstance(CrushMain* m, char* location){
  CrushMain* model = m;
  json_t* out = json_object();

  //gameDef
  json_t* jGameDef = json_object();
  json_object_set_new(jGameDef, "gameid", json_integer(model->gameid));
  json_t* jExtensionColor = serializeArray2DToJsonObject(model->extensionColor);
  json_object_set_new(jGameDef, "extensioncolor", jExtensionColor);
  json_t* jBoardInitialState = serializeArray2DToJsonObject(model->boardInitialState);
  json_object_set_new(jGameDef, "boardstate", jBoardInitialState);
  json_object_set_new(jGameDef,"movesallowed", json_integer(model->movesAllowed));
  json_object_set_new(jGameDef, "colors", json_integer(model->colors));
  json_object_set_new(out, "gamedef", jGameDef);

  //gameState
  json_t* jGameState = json_object();
  json_t* jBoardCandies = serializeBoardCandiesToJsonObject(model->boardCandies, model->boardCandyTypes);
  json_object_set_new(jGameState, "boardcandies", jBoardCandies);
  json_t* jBoardState = serializeArray2DToJsonObject(model->boardState);
  json_object_set_new(jGameState, "boardstate", jBoardState);
  json_object_set_new(jGameState, "movesmade", json_integer(model->movesMade));
  json_object_set_new(jGameState, "currentscore", json_integer(model->currentScore));
  json_t* jExtensionOffset = json_array();
  for (int i = 0; i <= model->extensionColor->columns; i++){
    json_array_append_new(jExtensionOffset, json_integer(model->extensionOffset[i]));
  }
  json_object_set_new(jGameState, "extensionoffset", jExtensionOffset);
  json_object_set_new(out, "gamestate", jGameState);
  if (location != NULL) {
    json_dump_file(out, location, 0);
  }

  return out;
}