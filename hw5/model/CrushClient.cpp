#include <jansson.h>
#include "../shared/Array2D.h"
#include <stdlib.h>
#include "../view/CrushView.h"
#include <iostream>

#include "../shared/ClientSocket.h"
#include "../shared/ServerSocket.h"
#include "CrushMain.h"
#include "string.h"
#define BUFSIZE 1024

using namespace std;


int recieveSendLoop(hw5_net::ClientSocket* clientSocket, CrushMain* gameInstance);


//begins the client/model side of the program
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
    //hw5_net::ClientSocket clientSocket(serverName, serverPort);
    hw5_net::ClientSocket* clientSocket = new hw5_net::ClientSocket(serverName, serverPort);
    string helloMessage = "{\"action\": \"hello\"}";
    clientSocket->WrappedWrite(helloMessage.c_str(), helloMessage.length());
    int curlyBraceCount = 0;
    char* stringJson = (char*) malloc(sizeof(char) * BUFSIZE);
    int stringJsonAllocatedSize = sizeof(char) * BUFSIZE;
    int size = 0; //how much is actually being used
    do {
      char buf[BUFSIZE];
      int readCount = clientSocket->WrappedRead(buf, BUFSIZE - 1);

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
    
    //deserialize and settle
    CrushMain* gameInstance = deserializeServerGameInstanceMessage(stringJson);
    int found = gameInstance->updateWithMove(0, 0, 0, 0);
    if (found){
      gameInstance->movesMade -= 1;
    }

    //serialize and send back
    const char* serialized = (serializeServerMessage(gameInstance));
    string toSend(serialized, strlen(serialized));
    clientSocket->WrappedWrite(toSend.c_str(), toSend.length());
    recieveSendLoop(clientSocket, gameInstance);
    delete(gameInstance);
    delete(clientSocket);
    free(stringJson);
    free((void *) serialized);
  } catch(string errString) {
    cout << errString << endl;
    printf("error\n");
    return 1;
  }
  printf("exiting\n");
  return 0;
}

//main loop which recieves an update, fires/settles, sends a gameinstance, and
//repeats until the bye message is sent.
int recieveSendLoop(hw5_net::ClientSocket* clientSocket, CrushMain* gameInstance){
  while(true){

    int curlyBraceCount = 0;
    char* stringJson = (char*) malloc(sizeof(char) * BUFSIZE);
    int stringJsonAllocatedSize = sizeof(char) * BUFSIZE;
    int size = 0; //how much is actually being used
    do {
      char buf[BUFSIZE];
      int readCount = clientSocket->WrappedRead(buf, BUFSIZE - 1);

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
    cout << stringJson << endl;
    json_error_t error;
    json_t* updateJson = json_loads(stringJson, 0, &error);
    const char* action = json_string_value(json_object_get(updateJson, "action"));
    if (action[0] == 'b'){ //bye message
      printf("BYE\n");
      json_decref(updateJson);
      free(stringJson);
      return 0;
    }
    int row1 = json_integer_value(json_object_get(updateJson, "row"));
    int column1 = json_integer_value(json_object_get(updateJson, "column"));
    int direction = json_integer_value(json_object_get(updateJson, "direction"));
    int row2 = row1;
    int column2 = column1;
    if (direction == 0){ //left
      column2--;
    } else if (direction == 1){ //right
      column2++;
    } else if (direction == 2){
      row2++;
    } else {
      row2--;
    }
    cout << "(" << row1 << ", " << column1 << ") (" << row2 << ", " << column2 << ")" << endl;
    gameInstance->updateWithMove(column1, row1, column2, row2);
    printf("Moves made:%d\n", gameInstance->movesMade);
    printf("updated\n");
    const char* serialized = (serializeServerMessage(gameInstance));
    cout << "TOSEND" << serialized << endl;
    string toSend(serialized, strlen(serialized));
    clientSocket->WrappedWrite(toSend.c_str(), toSend.length());
    free((void *) serialized);
    json_decref(updateJson);
    //free(updateJson);
    free(stringJson);
  }
  return 0;
}
