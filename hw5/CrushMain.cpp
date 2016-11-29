#include <jansson.h>
#include "Array2D.h"
#include <stdlib.h>
#include "CrushView.h"
#include <iostream>

#include "ClientSocket.h"
#include "ServerSocket.h"

using namespace std;

  int zero = 0; // used to represent the default candy type

//The class used to store data about a game
class CrushMain {
  public:
  int neg = -1; // used to represent a template that needs to be fired
  int gameid; //unique identifier
  Array2D extensionColor; //the extension grid
  Array2D boardInitialState; //how many times each square must fire
  int movesAllowed; //max moves allowed
  int colors; //max # of distinct colors in extension board

  Array2D boardCandies; //current candies on the board
  Array2D boardCandyTypes; //the type of each candy
  Array2D boardState; //how many times each square must fire
  Array2D boardType; //the type of each candy
  int movesMade; //how many moves have been made
  int currentScore; //total amount that state has decreased
  int* extensionOffset;

CrushMain(int gameid, Array2D extensionColor,
		       Array2D boardInitialState, int movesAllowed,
	               int colors, Array2D boardCandies, Array2D boardCandyTypes,
	               Array2D boardState, int movesMade, int currentScore, 
		       int extensionOffset[]){

  this->gameid = gameid;
  this->extensionColor = extensionColor;
  this->boardInitialState = boardInitialState;
  this->movesAllowed = movesAllowed;
  this->colors = colors;
  this->boardCandies = boardCandies;
  this->boardCandyTypes = boardCandyTypes;
  this->boardState = boardState;
  this->movesMade = movesMade;
  this->currentScore = currentScore;
  this->extensionOffset = extensionOffset;
}

~CrushMain() {
  freeArray2D(extensionColor, NULL);
  freeArray2D(boardInitialState, NULL);
  freeArray2D(boardCandies, NULL);
  freeArray2D(boardCandyTypes, NULL);
  freeArray2D(boardState, NULL);
  free(extensionOffset);
  //freeArray2D(boardType, NULL);
}

// Repeats the process of finding and firing templates until no templates are found. Updates moves remaining, etc. accordingly. Returns the number of templates found.
int updateWithMove(int x1, int y1, int x2, int y2) {
  if (movesMade >= movesAllowed) {
    // Too many moves made, can't do anything.
    return 0;
  }

  swapArray2D(this->boardCandies, x1, y1, x2, y2);

  int found = 0;
  int arbitraryMagicNumber = 1000; // max template firings per move
  while (found < arbitraryMagicNumber && this->findTemplates()) {
    found += 1;
    this->fireTemplates();
  }
  if (found) {
    // Move made successfully, increase count.
    movesMade++;
  } else {
    // No templates made so move is invalid.
    swapArray2D(this->boardCandies, x1, y1, x2, y2);
  }

  return found;
}

// Removes all tiles on the board marked as neg, and applies gravity using the extension board. Also updates the score.
void fireTemplates() {
  for (int i = this->boardCandies->rows; i >= 0; i--) {
    for (int j = 0; j <= this->boardCandies->columns; j++) {
      if (*((int *) getArray2D(this->boardCandies, j, i)) == neg) {
        int k = i;
        for (; k < this->boardCandies->rows; k++) {
          swapArray2D(this->boardCandies, j, k, j, k + 1);
        }

        setArray2D(this->boardCandies, getArray2D(extensionColor, j, extensionOffset[j] % (extensionColor->rows + 1)), j, k);

        // Update score.
        int* remaining = (int*) getArray2D(this->boardState, j, i);
        if (*remaining != 0) {
          *remaining = *remaining - 1;
          currentScore += 1;
        }

        extensionOffset[j] += 1;
      }
    }
  }
}

// Finds all existing templates on the board. Returns 1 if it finds at least one template, 0 if none are found. Sets all tiles on the board included in a template to neg, indicating that they are ready to fire using fireTemplates.
int findTemplates() {
  int result = 0;

  // Vertical 4-in-a-row
  for (int i = 0; i <= this->boardCandies->rows - 3; i++) {
    for (int j = 0; j <= this->boardCandies->columns; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j, i + 1)) && curr == *((int*)getArray2D(this->boardCandies, j, i + 2)) && curr == *((int*)getArray2D(this->boardCandies, j, i + 3))) {
        result = 1;
        setArray2D(this->boardCandies, &neg, j, i);
        setArray2D(this->boardCandies, &neg, j, i + 1);
        setArray2D(this->boardCandies, &neg, j, i + 2);
        setArray2D(this->boardCandies, &neg, j, i + 3);
      }
    }
  }

  // Horizontal 4-in-a-row
  for (int i = 0; i <= this->boardCandies->rows; i++) {
    for (int j = 0; j <= this->boardCandies->columns - 3; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j + 1, i)) && curr == *((int*)getArray2D(this->boardCandies, j + 2, i)) && curr == *((int*)getArray2D(this->boardCandies, j + 3, i))) {
        result = 1;
        setArray2D(this->boardCandies, &neg, j, i);
        setArray2D(this->boardCandies, &neg, j + 1, i);
        setArray2D(this->boardCandies, &neg, j + 2, i);
        setArray2D(this->boardCandies, &neg, j + 3, i);
      }
    }
  }

  // Vertical 3-in-a-row
  for (int i = 0; i <= this->boardCandies->rows - 2; i++) {
    for (int j = 0; j <= this->boardCandies->columns; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j, i + 1))&& curr == *((int*)getArray2D(this->boardCandies, j, i + 2))) {
        result = 1;
        setArray2D(this->boardCandies, &neg, j, i);
        setArray2D(this->boardCandies, &neg, j, i + 1);
        setArray2D(this->boardCandies, &neg, j, i + 2);
      }
    }
  }
 
  // Horizontal 3-in-a-row
  for (int i = 0; i <= this->boardCandies->rows; i++) {
    for (int j = 0; j <= this->boardCandies->columns - 2; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j + 1, i)) && curr == *((int*)getArray2D(this->boardCandies, j + 2, i))) {
        result = 1;
        setArray2D(this->boardCandies, &neg, j, i);
        setArray2D(this->boardCandies, &neg, j + 1, i);
        setArray2D(this->boardCandies, &neg, j + 2, i);
      }
    }
  }

  return result;
}
};

CrushMain *m;
int* arr;

//Given a json object which contains int fields "rows" and "columns",
//and an array field of ints "data", creates an Array2D representing that
//object and returns it
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
   arr = (int*) malloc(sizeof(int) * arraySize);

   for (int i = 0; i < arraySize; i++) {
     arr[i] = json_integer_value(json_array_get(jData, i));
      setArray2D(array, &arr[i], i % columns, i / columns);
   }
   return array;
}

//Given a json object containing int fields "rows" and "columns", and an array field
//"data" containing json objects which store ints "color" and "type", creates two
//Array2Ds containing the color information and the type information. These two
//arrays are given as out parameters.
int deserializeBoardCandiesFromJsonObject(json_t* json, Array2D* colors, Array2D* types) {
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
   int* arr2 = (int*) malloc(sizeof(int) * arraySize);

   for (int i = 0; i < arraySize; i++) {
     arr[i] = json_integer_value(json_object_get(json_array_get(jData, i), "color"));
     arr2[i] = json_integer_value(json_object_get(json_array_get(jData, i), "type"));
     setArray2D(*colors, &arr[i], i % columns, i / columns);
     setArray2D(*types, &arr2[i], i % columns, i / columns);
   }
   
   return 0;
}

//Given a file name to deserialize from, deserializes the given file
//and creates a CrushMain object to represent that game instance.
//pre: The file must be of the correct format
CrushMain* deserializeGameInstance(char* location){
  json_error_t error;
  json_t* json = json_load_file(location, 0, &error);
  if (!json){
    return nullptr;
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
  Array2D boardCandies, boardState, boardCandyTypes;
  int movesMade, currentScore, *extensionOffset;
  if (gameState != nullptr){
    printf("Not null!\n");
    
    //boardCandies and candy types
    json_t* jBoardCandies = json_object_get(gameState, "boardcandies");
    deserializeBoardCandiesFromJsonObject(jBoardCandies, &boardCandies, &boardCandyTypes);
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
  json_decref(json);
  return result;
}

//Given a Array2D of ints, creates and returns a json object representing that Array2D.
//The json object will have a "rows" field, a "columns" field, and a "data" array
//which will have the data of the Array2D.
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

//Serializes two arrays representing board candy colors and board candy types
//into a single json object. The json object will have int field "rows", "columns",
//and an array called "data" which contains json objects which have the int fields
//"color" and "type". Returns this as a pointer
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

//Given a location to write out to, serializes the current game instance.
void serializeGameInstance(char* location){
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
  json_dump_file(out, location, 0);
  json_decref(out);
  json_decref(jGameDef);
  json_decref(jGameState);
  json_decref(jExtensionOffset);
}

// Used to call the global model's updateWithMove function, allowing for compatibility with C.
void instanceCaller(int x1, int y1, int x2, int y2) {
  m->updateWithMove(x1, y1, x2, y2);
}

int main(int argc, char** argv){
  if (argc != 3){
    printf("Usage: ./CrushMain.cpp host port");
    return 1;
  }
  int serverPort;
  try {
    serverPort = stoi(argv[2]);
  } catch (...){
  }

  try {
    string serverName(argv[1]);
    hw5_net::ClientSocket clientSocket(serverName, serverPort);
    string helloMessage = "hello";
    clientSocket.WrappedWrite(helloMessage.c_str(), helloMessage.length());
  } catch(string errString) {
    cout << errString << endl;
    return 1;
  }
  printf("exiting\n");
  return 0;
}

int playWithSerializedBoard(char* argv){
  m = deserializeGameInstance(argv);
  int found = m->updateWithMove(0, 0, 0, 0); // Make sure the initial game state is settled.
  if (found) {
    m->movesMade -= 1;
  }
  int result = runner(m->boardCandies, m->boardState, &(m->movesMade), &(m->currentScore), &instanceCaller,
		      &serializeGameInstance, 1, &argv);
  delete m;
  free(arr);
  return result;
}
