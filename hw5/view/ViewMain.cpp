#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>

using namespace std;

#include "../shared/ServerSocket.h"
#include "../shared/ClientSocket.h"
#include "../model/CrushMain.h"

const int BUFSIZE = 1024;

void usage(const char *exeName) {
  cout << "Usage: " << exeName << " json-file" << " [port]" << endl;
  cout << "  Creates a server socket on port, if given," << endl
       << "  or on random port, if not." << endl;
  cout << "  Then displays the game with json-file." << endl;
  exit(1);
}

int getModelResponse(hw5_net::ClientSocket peerSocket, json_t** responseJson) {
    // Read and print input until EOF.

    cout << "Reading" << endl;

    int curlyBraceCount = 0;
    int stringJsonAllocatedSize = sizeof(char) * BUFSIZE;
    char* stringJson = (char*) malloc(stringJsonAllocatedSize);
    int size = 0; //how much is actually being used
    do {
      printf("in do-loop");
      char buf[BUFSIZE];
      int readCount = peerSocket.WrappedRead(buf, BUFSIZE - 1);

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

    if (!resJson) {
      printf("Response is not json value.");
      return 1;
    } else {
      *responseJson = resJson;
      return 0;
    }
}

int main(int argc, char *argv[]) {

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
    hw5_net::ClientSocket peerSocket(acceptedFd);

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

    json_t* helloackJson = json_object();
    json_error_t error;
    json_t* gameInstanceJson = json_load_file(argv[1], 0, &error);
    if (gameInstanceJson == NULL) {
      printf("Json file invalid");
      return 1;
    }
    json_object_set_new(helloackJson, "action", json_string("helloack"));
    json_object_set_new(helloackJson, "gameinstance", gameInstanceJson);
    string helloackMessage = string(json_dumps(helloackJson, 0));

    cout << helloackMessage << endl;

    peerSocket.WrappedWrite(helloackMessage.c_str(), helloackMessage.length());

    // Display game
    playWithSerializedBoard(argc, argv);
  } catch(string errString) {
    cout << errString << endl;
    return 1;
  }
  printf("exiting\n");
  return 0;
  
  cout << "Closing" << endl;
  
  return EXIT_SUCCESS;
}
