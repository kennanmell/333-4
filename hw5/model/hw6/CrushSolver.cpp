#include <jansson.h>
#include "../../shared/Array2D.h"
#include <stdlib.h>
#include "../../view/CrushView.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <thread>
#include "../../shared/ClientSocket.h"
#include "../../shared/ServerSocket.h"
#include "../CrushMain.h"
#include "string.h"

using namespace std;

// Default read buffer size.
#define BUFSIZE 1024
// Depth cutoff for parallel solver algorithm.
#define CUTOFF = 2;
// Socket for reading and writing to server.
hw5_net::ClientSocket* clientSocket;
// Represents the current game state. Should match the state displayed on the view server.
CrushMain* gameInstance;
// Lock for the parallel solver algorithm.
mutex mtx;

// Get a JSON response from the model, going through the internet using peerSocket.
// Sets responseJson to represent the model's JSON response.
// Returns 0 if successful, 1 otherwise. responseJson is undefined if 1 is returned.
int getModelResponse(hw5_net::ClientSocket* peerSocket, json_t** responseJson) {
    // Read and print input until EOF.

    cout << "Reading" << endl;

    int curlyBraceCount = 0;
    int stringJsonAllocatedSize = sizeof(char) * BUFSIZE;
    char* stringJson = (char*) malloc(stringJsonAllocatedSize);
    int size = 0; //how much is actually being used
    do {
      printf("in do-loop");
      char buf[BUFSIZE];
      int readCount = peerSocket->WrappedRead(buf, BUFSIZE - 1);
      printf("%d\n", readCount);
      //if allocated space isn't big enough, reallocate
      if (size + readCount > stringJsonAllocatedSize){
	stringJson = (char*) realloc(stringJson, stringJsonAllocatedSize + BUFSIZE * sizeof(char));
	stringJsonAllocatedSize += BUFSIZE * sizeof(char);
      }
      //move chars into allocated space and count curly braces to see if we're done or not
      for (int i = 0; i < readCount; i++){
	stringJson[size] = buf[i];
	size++;
	if (buf[i] == '{'){
	  curlyBraceCount++;
	} else if (buf[i] == '}'){
	  curlyBraceCount--;
	}
      }
    } while (curlyBraceCount != 0);

    stringJson[size] = '\0';
    cout.flush();
    cout << "done" << endl;

    printf("stringJson: %s\n", stringJson);
    json_error_t error;
    json_t* resJson = json_loads(stringJson, 0, &error);

    free(stringJson);

    if (!resJson) {
      printf("Response is not json value.");
      return 1;
    } else {
      *responseJson = resJson;
      return 0;
    }
}

// Represents a move that can be made on a CrushMain. Swaps (x, y) with (x2, y2).
class Move {
  public:
  	// x-coordinate of first position to swap.
	int x;
	// y-coordinate of first position to swap.
	int y;
	// x-coordinate of second position to swap.
	int x2;
	// y-coordinate of second position to swap.
	int y2;
	// the direction of the swap (0 = left, 1 = right, 2 = down, 3 = up)
	int direction;
	// not set by default. If set, indicates the "best" game score that can result from making the move.
	int score;

	// Constructor to make a Move representing the swap between (x, y) and (x2, y2)
	Move(int x, int y, int x2, int y2) {
		this->x = x;
		this->y = y;
		this->x2 = x2;
		this->y2 = y2;
		if (x + 1 == x2) {
			// Right
			this->direction = 1;
		} else if (x - 1 == x2) {
			// Left
			this->direction = 0;
		} else if (y + 1 == y2) {
			// Down
			this->direction = 2;
		} else {
			// Up
			this->direction = 3;
		}
	}
};

// Returns a queue of all valid moves that can be made on model. May be empty.
// Treats each swap as one valid move as opposed to 2 (since swapping 2 tiles up or down
// is really the same thing.)
queue<Move*>* findAllValidMoves(CrushMain* model) {
	queue<Move*>* result = new queue<Move*>();
    for (int i = 0; i <= model->boardState->rows; i++) {
      int j = (i % 2 == 0) ? 0 : 1;
  	  for (; j <= model->boardState->columns; j += 2) {
  		int valid = model->checkIfValidMove(j, i, j - 1, i);
  		if (valid) {
  			result->push(new Move(j, i, j - 1, i));
   		}
  		valid = model->checkIfValidMove(j, i, j + 1, i);
  		if (valid) {
  			result->push(new Move(j, i, j + 1, i));
  		}
  		valid = model->checkIfValidMove(j, i, j, i + 1);
  		if (valid) {
  			result->push(new Move(j, i, j, i + 1));
  		}
  		valid = model->checkIfValidMove(j, i, j, i - 1);
  		if (valid) {
  			result->push(new Move(j, i, j, i - 1));
  		}
  	  }
    }
    return result;
}

// Returns 1 if all necessary template firings have been made, 0 otherwise.
int gameIsWon(CrushMain* state) {
	for (int i = 0; i <= state->boardState->rows; i++) {
		for (int j = 0; j <= state->boardState->columns; j++) {
			if (*((int*) getArray2D(state->boardState, j, i)) != 0) {
				return 0;
			}
		}
	}
	return 1;
}

// Function for each thread of the parallel algorithm to execute. Tests
// possible states resulting from a move recursively to find the best score
// that can result from that move.
void threadExec(CrushMain* old, Move* topMove, Move* moveToMake, int depth) {
	CrushMain* state = new CrushMain(*old);
	state->updateWithMove(moveToMake->x, moveToMake->y, moveToMake->x2, moveToMake->y2);
	queue<thread*>* threads = new queue<thread*>();
	if (depth == CUTOFF || gameIsWon(state)) {
		mtx.lock();
		topMove->score = state->currentScore;
		mtx.unlock();
	} else {
		queue<Move*>* allMoves = findAllValidMoves(gameInstance);
		while (!allMoves->empty()) {
			Move* currentMove = allMoves->front();
			allMoves->pop();
			thread t = thread(&threadExec, state, topMove, currentMove, depth + 1);
			threads->push(&t);
		}
		while (!threads->empty()) {
			thread* t = threads->front();
			t->join();
			threads->pop();
		}
	}
}

// Finds and returns the best possible move that can be made on gameInstance.
Move* findBestMove() {
	queue<Move*>* allMoves = findAllValidMoves(gameInstance);
	queue<Move*>* allMoves2 = new queue<Move*>();
	queue<thread*>* threads = new queue<thread*>();
	Move* bestMove = NULL;
	while (!allMoves->empty()) {
		Move* currentMove = allMoves->front();
		allMoves2->push(allMoves->front());
		allMoves->pop();
		thread t = thread(&threadExec, gameInstance, currentMove, currentMove, 0);
		threads->push(&t);
	}
	while (!threads->empty()) {
		thread* t = threads->front();
		t->join();
		threads->pop();
	}
	while (!allMoves2->empty()) {
		Move* currentMove = allMoves->front();
		if (bestMove == NULL || bestMove->score < currentMove->score) {
			bestMove = currentMove;
		}
	}
	return bestMove;
}

// Main method to solve a candy crush game.
int main(int argc, char** argv) {
  if (argc != 3){
    printf("Usage: ./hw6-model hostname port");
    return 1;
  }
  int serverPort;
  try {
    serverPort = stoi(argv[2]);
  } catch (...){
  }

  try {
    string serverName(argv[1]);
    clientSocket = new hw5_net::ClientSocket(serverName, serverPort);
    string helloMessage = "{\"action\": \"hello\", \"teamname\": \"1934820\"}";
    clientSocket->WrappedWrite(helloMessage.c_str(), helloMessage.length());
    json_t* responseJson;
    printf("1");
    getModelResponse(clientSocket, &responseJson);
    printf("2");
    // Set up initial board.
    gameInstance = deserializeServerGameInstanceMessage(json_dumps(responseJson, 0));
    printf("3");
    int found = gameInstance->updateWithMove(0, 0, 0, 0);
    if (found){
      gameInstance->movesMade -= 1;
    }
    while (true) {
    	getModelResponse(clientSocket, &responseJson);
  		json_t* act = json_object_get(responseJson, "action");
  		printf("z\n");
  		if (!strcmp("bye", json_string_value(act))) {
  			printf("exiting");
  			break;
  		}
  		printf("a\n");
  		// Otherwise request move.
  		json_t* response = json_object();
  		json_object_set(response, "action", json_string("mymove"));
  		json_object_set(response, "teamname", json_string("1934820"));
  		  		printf("b\n");
  		Move* bestMove = findBestMove();
  		printf("t\n");
  		json_t* moveJson = json_object();
  	    json_object_set(moveJson, "row", json_integer(bestMove->x));
  	    json_object_set(moveJson, "column", json_integer(bestMove->y));
  	    json_object_set(moveJson, "direction", json_integer(bestMove->direction));
  	    json_object_set(response, "move", moveJson);
  		json_object_set(response, "movesevaluated", json_integer(1));
  		  		printf("c\n");
  		json_object_set(response, "gameinstance", serializeGameInstance(gameInstance, NULL));
  				printf("d\n");
  		char* stringJson = json_dumps(response, 0);
  		string cppStringJson = string(stringJson);
  		clientSocket->WrappedWrite(stringJson, cppStringJson.length());
  				printf("e\n");
    }
  } catch(string errString) {
    cout << errString << endl;
    printf("error\n");
    return 1;
  }

  return 0;
}