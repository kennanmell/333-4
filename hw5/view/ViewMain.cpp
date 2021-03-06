#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>

using namespace std;

#include "../shared/ServerSocket.h"
#include "../shared/ClientSocket.h"
#include "../model/CrushMain.h"

// An arbitrarily defined initial buffer size for reading buffers.
const int BUFSIZE = 1024;
// The socket used to read and write to the model across the internet.
hw5_net::ClientSocket* peerSocket;

// Yells at the client to enter the correct command line arguements.
void usage(const char *exeName) {
  cout << "Usage: " << exeName << " json-file" << " [port]" << endl;
  cout << "  Creates a server socket on port, if given," << endl
       << "  or on random port, if not." << endl;
  cout << "  Then displays the game with json-file." << endl;
  exit(1);
}

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

// Uses the internet to ask the model for a new state by passing a move action request and returning the response as a json_t.
// (x1, y1) is the tile the player tried to move.
// (x2, y2) is the tile the player tried to swap it with.
json_t* newStateMaker(int x1, int y1, int x2, int y2) {
   // Send update message.
   json_t* sendJson = json_object();
   json_t* xIntJson = json_integer(y1);
   json_t* yIntJson = json_integer(x1);

   json_t* moveStringJson = json_string("move");

   json_object_set(sendJson, "action", moveStringJson);
   json_object_set(sendJson, "row", xIntJson);
   json_object_set(sendJson, "column", yIntJson);

   json_t* directionJson;
   if (x1 == x2 + 1) {
     // left
     directionJson = json_integer(0);
   } else if (x1 == x2 - 1) {
     // right
     directionJson = json_integer(1);
   } else if (y1 == y2 - 1) {
     // up
     directionJson = json_integer(2);
   } else { // y1 == y2 + 1
     // down
     directionJson = json_integer(3);
   }
   json_object_set(sendJson, "direction", directionJson);

   char* moveChars = json_dumps(sendJson, 0);
   string moveMessage = string(moveChars);

   cout << moveMessage << endl;

   peerSocket->WrappedWrite(moveMessage.c_str(), moveMessage.length());
   json_decref(xIntJson);
   json_decref(yIntJson);
   json_decref(sendJson);
   json_decref(moveStringJson);
   json_decref(directionJson);
   free(moveChars);

   // Wait for move message.
   json_t* resJson;

   if (getModelResponse(peerSocket, &resJson)) {
     printf("failed to get json");
     return NULL;
   }

   json_t* updateJson = json_object_get(resJson, "action");
   if (!json_is_string(updateJson) || strcmp("update", json_string_value(updateJson))) {
     printf("Expected update message, didn't get it.");
     return NULL;
   }
   
   json_t* result = json_deep_copy(json_object_get(resJson, "gameinstance"));
   json_decref(resJson);
   return result;
}

// Main method plays the candy crush game. Pass the initial game state json as the first argument and optionally specify a port number as the second argument.
int main(int argc, char *argv[]) {

  // Make sure arguments are correct.
  if ( argc != 2 && argc != 3 ) usage(argv[0]);
  
  int port = 0;
  try {
    if ( argc == 3 ) port = stoi(argv[2]);
  } catch (...) {
    usage(argv[0]);
  }

  try {
    int socketFd;
    hw5_net::ServerSocket sock(port);
    sock.BindAndListen(AF_INET, &socketFd);
    cout << "Created bound socket. port = " << sock.port() << endl;

    while (true) {
    // wait for client connection
    int acceptedFd;
    string clientAddr;
    uint16_t clientPort;
    string clientDNSName;
    string serverAddress;
    string serverDNSName;

    sock.Accept( &acceptedFd,
		 &clientAddr,
		 &clientPort,
		 &clientDNSName,
		 &serverAddress,
		 &serverDNSName );

    cout << "Got connection from: " << endl
	 << "\tacceptedFd\t" << acceptedFd << endl
	 << "\tclientPort\t" << clientPort << endl
	 << "\tclientDNSName\t" << clientDNSName << endl
	 << "\tserverAddress\t" << serverAddress << endl
	 << "\tserverDNSName\t" << serverDNSName << endl;

    // wrap connection to peer with a CientSocket
    peerSocket = new hw5_net::ClientSocket(acceptedFd);


    // Look for hello response.
    json_t* resJson;

    if (getModelResponse(peerSocket, &resJson)) {
      printf("failed to get json");
      return 1;
    }


    json_t* helloJson = json_object_get(resJson, "action");
    if (!json_is_string(helloJson) || strcmp("hello", json_string_value(helloJson))) {
      printf("Expected hello message, didn't get it.");
      return 1;
    }

    json_decref(resJson);

    // Send helloack message.
    json_t* helloackJson = json_object();
    json_error_t error;
    json_t* gameInstanceJson = json_load_file(argv[1], 0, &error);
    if (gameInstanceJson == NULL) {
      printf("Json file invalid");
      return 1;
    }
    json_object_set_new(helloackJson, "action", json_string("helloack"));
    json_object_set_new(helloackJson, "gameinstance", gameInstanceJson);
    
    char* helloackMessageChars = json_dumps(helloackJson, 0);
    string helloackMessage = string(helloackMessageChars);

    cout << helloackMessage << endl;

    peerSocket->WrappedWrite(helloackMessageChars, helloackMessage.length());

    json_decref(helloackJson);
    free(helloackMessageChars);

    // Look for update message.

    if (getModelResponse(peerSocket, &resJson)) {
      printf("failed to get json");
      return 1;
    }


    json_t* updateJson = json_object_get(resJson, "action");
    if (!json_is_string(updateJson) || strcmp("update", json_string_value(updateJson))) {
      printf("Expected update message, didn't get it.");
      return 1;
    }
    printf("through update call");

    // Display game
    playWithSerializedBoard(argc, argv, json_deep_copy(json_object_get(resJson, "gameinstance")), &newStateMaker);
     
    json_decref(resJson);

    // Send goodbye message.
    json_t* byeJson = json_object();
    json_t* byeStringJson = json_string("bye");
    json_object_set(byeJson, "action", byeStringJson);
    
    char* byeMessageChars = json_dumps(byeJson, 0);
    string byeMessage = string(byeMessageChars);

    cout << byeMessage << endl;

    peerSocket->WrappedWrite(byeMessageChars, byeMessage.length());

    json_decref(byeJson);
    json_decref(byeStringJson);
    free(byeMessageChars);
    }
  } catch(string errString) {
    cout << errString << endl;
    return 1;
  }
  
  cout << "Closing" << endl;
  
  return EXIT_SUCCESS;
}
