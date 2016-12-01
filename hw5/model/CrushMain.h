#ifndef _CrushMain_h
#define _CrushMain_h

#include "../shared/Array2D.h"
#include <jansson.h>

//The class used to store data about a game
class CrushMain {
  public:
  int* thingToFree = NULL; // thing to free
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
  freeArray2D(extensionColor, &free);
  freeArray2D(boardInitialState, &free);
  freeArray2D(boardCandies, NULL);
  freeArray2D(boardCandyTypes, &free);
  freeArray2D(boardState, &free);
  free(extensionOffset);
  if (thingToFree != NULL) {
    free(thingToFree);
  }
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

        hsetArray2D(this->boardCandies, getArray2D(extensionColor, j, extensionOffset[j] % (extensionColor->rows + 1)), j, k);

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
        hsetArray2D(this->boardCandies, &neg, j, i);
        hsetArray2D(this->boardCandies, &neg, j, i + 1);
        hsetArray2D(this->boardCandies, &neg, j, i + 2);
        hsetArray2D(this->boardCandies, &neg, j, i + 3);
      }
    }
  }

  // Horizontal 4-in-a-row
  for (int i = 0; i <= this->boardCandies->rows; i++) {
    for (int j = 0; j <= this->boardCandies->columns - 3; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j + 1, i)) && curr == *((int*)getArray2D(this->boardCandies, j + 2, i)) && curr == *((int*)getArray2D(this->boardCandies, j + 3, i))) {
        result = 1;
        hsetArray2D(this->boardCandies, &neg, j, i);
        hsetArray2D(this->boardCandies, &neg, j + 1, i);
        hsetArray2D(this->boardCandies, &neg, j + 2, i);
        hsetArray2D(this->boardCandies, &neg, j + 3, i);
      }
    }
  }

  // Vertical 3-in-a-row
  for (int i = 0; i <= this->boardCandies->rows - 2; i++) {
    for (int j = 0; j <= this->boardCandies->columns; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j, i + 1))&& curr == *((int*)getArray2D(this->boardCandies, j, i + 2))) {
        result = 1;
        hsetArray2D(this->boardCandies, &neg, j, i);
        hsetArray2D(this->boardCandies, &neg, j, i + 1);
        hsetArray2D(this->boardCandies, &neg, j, i + 2);
      }
    }
  }
 
  // Horizontal 3-in-a-row
  for (int i = 0; i <= this->boardCandies->rows; i++) {
    for (int j = 0; j <= this->boardCandies->columns - 2; j++) {
      int curr = *((int*)getArray2D(this->boardCandies, j, i));
      if (curr != neg && curr == *((int*)getArray2D(this->boardCandies, j + 1, i)) && curr == *((int*)getArray2D(this->boardCandies, j + 2, i))) {
        result = 1;
        hsetArray2D(this->boardCandies, &neg, j, i);
        hsetArray2D(this->boardCandies, &neg, j + 1, i);
        hsetArray2D(this->boardCandies, &neg, j + 2, i);
      }
    }
  }

  return result;
}

    void hsetArray2D(Array2D arr, Array2DPayload_t val, int j, int i) {
      // long* returnVal = (long*) arr->head + i * (arr->columns + 1) + j;
      // long *longPtr = getArray2D(arr, j, i);
      //*returnVal = (long) val;
      //int* intPtr = (int*) getArray2D(arr, j, i);
      //int* intPtr2 = (int*) val;
      
      //*intPtr = *intPtr2;
      setArray2D(arr, val, j, i);
    }
};

// Play the candy crush game with command line arguments argc, argv, the initial game state represented in JSON as stateJson, and the callback function to get new states (newStateMaker).
int playWithSerializedBoard(int argc, char** argv, json_t* stateJson, json_t* (*newStateMaker)(int, int, int, int));

//Given a json object which contains int fields "rows" and "columns",
//and an array field of ints "data", creates an Array2D representing that
//object and returns it
Array2D deserializeInt2DArrayFromJsonObject(json_t* json);

//Given a json object containing int fields "rows" and "columns", and an array field
//"data" containing json objects which store ints "color" and "type", creates two
//Array2Ds containing the color information and the type information. These two
//arrays are given as out parameters.
int deserializeBoardCandiesFromJsonObject(json_t* json, Array2D* colors, Array2D* types, int** thingToFree);

//deserializes the given json object into a CrushMain game
//instance and returns it
CrushMain* deserializeHelper(json_t* json);

//Given a char* to deserialize from, deserializes the given
//string and creates a CrushMain object to represent that
//game instance, and returns it. String must be in correct format
CrushMain* deserializeServerGameInstanceMessage(char* location);

//Given a file name to deserialize from, deserializes the given file
//and creates a CrushMain object to represent that game instance.
//pre: The file must be of the correct format
CrushMain* deserializeGameInstance(char* location);

//Given a Array2D of ints, creates and returns a json object representing that Array2D.
//The json object will have a "rows" field, a "columns" field, and a "data" array
//which will have the data of the Array2D.
json_t* serializeArray2DToJsonObject(Array2D array);

//Serializes two arrays representing board candy colors and board candy types
//into a single json object. The json object will have int field "rows", "columns",
//and an array called "data" which contains json objects which have the int fields
//"color" and "type". Returns this as a pointer
json_t* serializeBoardCandiesToJsonObject(Array2D array, Array2D array2);

//Given a location to write out to, serializes the current game instance.
const char* serializeGameInstance(char* location);

// Serializes model to JSON representation and returns that representation.
const char* serializeServerMessage(CrushMain* model);

// Used to call the global model's updateWithMove function, allowing for compatibility with C.
void instanceCaller(int x1, int y1, int x2, int y2);

#endif /* _CrushMain_h */
