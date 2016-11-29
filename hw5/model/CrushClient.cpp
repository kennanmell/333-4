#include <jansson.h>
#include "../shared/Array2D.h"
#include <stdlib.h>
#include "../view/CrushView.h"
#include <iostream>

#include "../shared/ClientSocket.h"
#include "../shared/ServerSocket.h"
#include "CrushMain.h"
#define BUFSIZE 1024

using namespace std;

//int BUFSIZE = 1024;

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
    string helloMessage = "{\"action\": \"hello\"}";
    clientSocket.WrappedWrite(helloMessage.c_str(), helloMessage.length());
    int curlyBraceCount = 0;
    char* stringJson = (char*) malloc(sizeof(char) * BUFSIZE);
    int stringJsonAllocatedSize = sizeof(char) * BUFSIZE;
    int size = 0; //how much is actually being used
    do {
      char buf[BUFSIZE];
      int readCount = clientSocket.WrappedRead(buf, BUFSIZE - 1);

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
    CrushMain* gameInstance = deserializeServerMessage(stringJson);
    if (gameInstance != NULL){
      printf("success!\n");
    } else {
      printf("error!\n");
    }
  } catch(string errString) {
    cout << errString << endl;
    return 1;
  }
  printf("exiting\n");
  return 0;
}
